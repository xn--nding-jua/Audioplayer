-- Receiver for stereo I2S in I2S_PHILIPS_MODE for PCM1822 in Slave-Mode or comparable devices
-- (c) 2024 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains an I2S-receiver
-- I2S sends two 16-, 24- or 32-bit audio-samples (left and right) right after each other
-- Most devices sends in I2S_PHILIPS_MODE which means with 1 bit-shift lagging compared to Wordclock

-- individual samples are identified by lrclk


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity i2s_master_rx is
	generic (
		i2s_bit_width		: natural range 16 to 32 := 32
	);
	port (
			clk 				: in std_logic; -- clock
			bclk_in			: in std_logic; -- bit-clock (3.072 MHz for 48kHz Stereo with each 32 bit per message
			sdata_in			: in std_logic; -- I2S Serial-data
			
			bclk_out			: out std_logic; -- I2S Bitclock
			lrclk_out		: out std_logic; -- I2S Wordclock (negated = left-channel, asserted = right-channel)
			sample_l_out	: out std_logic_vector(23 downto 0) := (others=>'0'); -- received audio-sample for left-channel
			sample_r_out	: out std_logic_vector(23 downto 0) := (others=>'0'); -- received audio-sample for right-channel
			sync_out			: out std_logic -- new data received successfully
        );
end i2s_master_rx;

architecture rtl of i2s_master_rx is
	signal zbclk, zzbclk, zzzbclk 	: std_logic;
	signal b_pos_edge, b_neg_edge		: std_logic;
	signal b_count 					: natural range 0 to i2s_bit_width := 0;
	signal lrclk							: std_logic;

	signal signal_shift	: std_logic_vector(i2s_bit_width - 1 downto 0); -- space for one single sample
	signal signal_l 		: std_logic_vector(23 downto 0);
	signal sync				: std_logic;
begin
	detect_bclk_edge : process(clk)
	begin
		if rising_edge(clk) then
			zbclk <= bclk_in;
			zzbclk <= zbclk;
			zzzbclk <= zzbclk;
			if zzbclk = '1' and zzzbclk = '0' then
				b_pos_edge <= '1';
			elsif zzbclk = '0' and zzzbclk = '1' then
				b_neg_edge <= '1';
			else
				b_pos_edge <= '0';
				b_neg_edge <= '0';
			end if;
		end if;
	end process;
	
	process(clk)
	begin
		if rising_edge(clk) then
			if b_pos_edge = '1' then
				-- fill shift-register with new data
				signal_shift <= signal_shift(signal_shift'high - 1 downto 0) & sdata_in;
			end if;

			if b_neg_edge  = '1' then
				if b_count = 0 then
					if lrclk = '1' then
						-- read left channel
						if i2s_bit_width = 16 then
							signal_l <= signal_shift(15 downto 0) & "00000000"; -- fill missing bits with zeros from right
						elsif i2s_bit_width = 24 then
							signal_l <= signal_shift(23 downto 0); -- take all bits
						elsif i2s_bit_width = 32 then
							signal_l <= signal_shift(31) & signal_shift(30 downto 8); -- take only the leftmost 24 bits to keep 0dBfs
						end if;
					else
						-- output both channel left and right
						sample_l_out <= signal_l;
						
						if i2s_bit_width = 16 then
							sample_r_out <= signal_shift(15 downto 0) & "00000000"; -- fill missing bits with zeros from right
						elsif i2s_bit_width = 24 then
							sample_r_out <= signal_shift(23 downto 0); -- take all bits
						elsif i2s_bit_width = 32 then
							sample_r_out <= signal_shift(31) & signal_shift(30 downto 8); -- take only the leftmost 24 bits to keep 0dBfs
						end if;

						sync <= '1';
					end if;
				end if;

				-- clock-divider for lrclk
				if b_count = i2s_bit_width - 1 then -- ((3072000 / 2) / 32) = 48000
					lrclk <= not lrclk;
					b_count <= 0;
				else
					b_count <= b_count + 1;
				end if;
			end if;			

			if sync = '1' then
				sync <= '0';
			end if;
		end if;
	end process;
	
	-- output the signals
	bclk_out <= bclk_in;
	lrclk_out <= lrclk;
	sync_out <= sync;
end rtl;
