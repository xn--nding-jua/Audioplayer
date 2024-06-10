-------------------------------------------------------------------------------
-- Receiver for Behringer UltraNet or other 2- or Multi-Channel AES/EBU-encoded audio-streams
-- based on OpenCore's i2s_interface by Geir Drange (https://opencores.org/projects/i2s_interface)
--
-- UltraNet sends eight 24-bit audio-samples in AES/EBU-format.
-- this file expects I2S-data converted from AES/EBU-block
--
-- bsync (Z subframe in AES/EBU) will signal channel 1 followed by up to seven more samples
-- individual samples are identified by lrclk
------------------------------------------------------------------------------- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity i2s_aes_rx is
   generic (
      -- number of expected audio-channels to receive
      audio_channels	:	natural range 2 to 8 := 8
   );
	port (
		clk 		: in std_logic; -- Master clock
		bsync		: in std_logic; -- Block start (asserted when Z subframe is being transmitted)
		bclk 		: in std_logic; -- output serial data clock (AES3-clock)
		sdata		: in std_logic; -- output serial data
		lrclk		: in std_logic; -- Frame sync (asserted for channel A, negated for B)
		
		ch1_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch2_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch3_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch4_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch5_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch6_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch7_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		ch8_out	: out std_logic_vector(23 downto 0); -- received audio-sample
		sync_out	: out std_logic -- new data received successfully
	  );
end i2s_aes_rx;

architecture rtl of i2s_aes_rx is
	signal sample_data			: std_logic_vector(23 downto 0);
	signal bsync_pos_edge		: std_logic;
	signal neg_edge, pos_edge 	: std_logic;
	signal lr_edge					: std_logic;
	signal rx_sampledata 		: std_logic;

	signal zbsync, zzbsync, zzzbsync	: std_logic;
	signal zbclk, zzbclk, zzzbclk 	: std_logic;
	signal zlrclk, zzlrclk, zzzlrclk	: std_logic;

	signal bit_cnt					: integer range 0 to 31 := 0;
	signal chn_cnt					: integer range 0 to 7 := 0;
begin
	detect_bsync_pos_edge : process(clk)
	begin
		if rising_edge(clk) then
			zbsync <= bsync;
			zzbsync <= zbsync;
			zzzbsync <= zzbsync;
			if zzbsync = '1' and zzzbsync = '0' then
				bsync_pos_edge <= '1';
			else
				bsync_pos_edge <= '0';
			end if;
		end if;
	end process;

	detect_edge : process(clk)
	begin
		if rising_edge(clk) then
			zbclk <= bclk;
			zzbclk <= zbclk;
			zzzbclk <= zzbclk;
			if zzbclk = '1' and zzzbclk = '0' then
				pos_edge <= '1';
			elsif zzbclk = '0' and zzzbclk = '1' then
				neg_edge <= '1';
			else
				pos_edge <= '0';
				neg_edge <= '0';
			end if;
		end if;
	end process;
 
   detect_lr_edge : process(clk)
	begin
		if rising_edge(clk) then
			zlrclk <= lrclk;
			zzlrclk <= zlrclk;
			zzzlrclk <= zzlrclk;
			if zzlrclk /= zzzlrclk then
				lr_edge <= '1';
			else
				lr_edge <= '0';
			end if;
		end if;
	end process;
 
	detect_sample : process(clk)
	begin
		if rising_edge(clk) then
			if bsync_pos_edge = '1' then -- begin of new block (means channel 1 on Ultranet)
				bit_cnt <= 0;
				chn_cnt <= 0; -- reset channel-counter to 0
				sync_out <= '0';
			else
				if lr_edge = '1' then -- rising or falling edge on LRCLK detected
					bit_cnt <= 0;

					if chn_cnt < (audio_channels-1) then
						chn_cnt <= chn_cnt + 1; -- increase channel-counter on each LRCLK-edge
					else
						chn_cnt <= 0; -- reset to 0 as we are receiving a bsync only every 192 frames
					end if;
				end if;
				
				if pos_edge = '1' then
					if bit_cnt < 31 then
						bit_cnt <= bit_cnt + 1; -- increment bit_cnt until bit 31
					end if;
				end if;
				
				if neg_edge = '1' then  	
					if bit_cnt = 4 then -- data is 4 cycles after the word strobe
						rx_sampledata <= '1';
					elsif bit_cnt >= 24+4 then -- reached end of sampledata
						rx_sampledata <= '0';
					end if;
				end if;
				
				if bit_cnt = 30 and neg_edge = '1' then
					-- we have reached the unused 4bits at the end of the frame

					-- set output and raise sync-signal when receiving the last channel
					case chn_cnt is
						when 0 =>
							ch1_out <= sample_data;
						when 1 =>
							ch2_out <= sample_data;
						when 2 =>
							ch3_out <= sample_data;
						when 3 =>
							ch4_out <= sample_data;
						when 4 =>
							ch5_out <= sample_data;
						when 5 =>
							ch6_out <= sample_data;
						when 6 =>
							ch7_out <= sample_data;
						when 7 =>
							ch8_out <= sample_data;
					end case;
					
					if (chn_cnt = audio_channels-1) then
						-- set output
						sync_out <= '1';
					end if;
				else
					sync_out <= '0';
				end if;
			end if;
		end if;
	end process;

	get_data : process(clk)
	begin
		if rising_edge(clk) then
			-- receive individual bits for audio-data (24 bits)
			if pos_edge = '1' and rx_sampledata = '1' then
				--sample_data <= sample_data(sample_data'high - 1 downto 0) & sdata;
				sample_data <= sdata & sample_data(sample_data'high downto 1); -- in AES3/EBU the first bit after preamble is LSB, so we have to shift from the left to the right
			end if;
		end if;
	end process;
end rtl;
        