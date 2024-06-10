-- Receiver for stereo I2S in I2S_PHILIPS_MODE with data 1 bit delayed compared to Wordclock
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains an I2S-receiver
-- I2S sends two 16-, 24- or 32-bit audio-samples (left and right) right after each other
-- Most devices sends in I2S_PHILIPS_MODE which means with 1 bit-shift lagging compared to Wordclock

-- With the gain-input you can increase the gain of the received signal. This makes sense
-- for 32-bit Audio-ADCs where you can use the first 8 bits.
--
-- individual samples are identified by lrclk


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity i2s_rx is
	generic (
		i2s_bit_width		: natural range 16 to 32 := 16;
		bit_delay			: integer := 1 -- 0 = STANDARD_MODE, +1 = I2S_PHILIPS_MODE
	);
	port (
			clk 				: in std_logic; -- Masterclock
			bclk 				: in std_logic; -- I2S Bitclock
			sdata				: in std_logic; -- I2S Serial-data
			lrclk				: in std_logic; -- I2S Wordclock (negated = left-channel, asserted = right-channel)
			gain				: in unsigned(7 downto 0) := (others=>'0'); -- 48dB gain for increasing signal
			
			sample_l_out	: out std_logic_vector(23 downto 0) := (others=>'0'); -- received audio-sample for left-channel
			sample_r_out	: out std_logic_vector(23 downto 0) := (others=>'0'); -- received audio-sample for right-channel
			sync_out			: out std_logic -- new data received successfully
        );
end i2s_rx;

architecture rtl of i2s_rx is
	signal signal_shift	: std_logic_vector((2 * i2s_bit_width) - 1 downto 0); -- space for two samples
	signal signal_l 		: std_logic_vector(23 downto 0);
	
	signal zbclk, zzbclk, zzzbclk 	: std_logic;
	signal zlrclk, zzlrclk, zzzlrclk	: std_logic;

	signal lr_neg_edge, lr_pos_edge	: std_logic;
	signal b_pos_edge 					: std_logic;
begin
	detect_edge : process(clk)
	begin
		if rising_edge(clk) then
			zbclk <= bclk;
			zzbclk <= zbclk;
			zzzbclk <= zzbclk;
			if zzbclk = '1' and zzzbclk = '0' then
				b_pos_edge <= '1';
			else
				b_pos_edge <= '0';
			end if;
		end if;
	end process;
 
	detect_lr_edge : process(clk)
	begin
		if rising_edge(clk) then
			zlrclk <= lrclk;
			zzlrclk <= zlrclk;
			zzzlrclk <= zzlrclk;
			
			if zzlrclk = '1' and zzzlrclk = '0' then
				lr_pos_edge <= '1';
			elsif zzlrclk = '0' and zzzlrclk = '1' then
				lr_neg_edge <= '1';
			else
				lr_pos_edge <= '0';
				lr_neg_edge <= '0';
			end if;
		end if;
	end process;
 
	detect_sample : process(clk)
	begin
		if rising_edge(clk) then
			-- we have to copy data from previous sample, that we could receive completely
			-- current sample has still a missing bit as we I2S_PHILIPS_MODE has serial-data
			-- with one single bit delay

			if lr_neg_edge = '1' then
				-- we are receiving sample for channel left. store it to signal
				if i2s_bit_width = 16 then
					signal_l <= std_logic_vector(shift_left(signed(signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay downto i2s_bit_width - bit_delay) & "00000000"), to_integer(gain))); -- fill missing bits with zeros from right
				elsif i2s_bit_width = 24 then
					signal_l <= std_logic_vector(shift_left(signed(signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay downto i2s_bit_width - bit_delay)), to_integer(gain))); -- take all 24 bits
				elsif i2s_bit_width = 32 then
					signal_l <= signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay - to_integer(gain) downto i2s_bit_width - bit_delay + 8 - to_integer(gain)); -- take only the leftmost 24 bits to keep 0dBfs
				end if;

				sync_out <= '0';
			elsif lr_pos_edge = '1' then
				sample_l_out <= signal_l; -- output sample for channel left

				-- output sample for channel right
				if i2s_bit_width = 16 then
					sample_r_out <= std_logic_vector(shift_left(signed(signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay downto i2s_bit_width - bit_delay) & "00000000"), to_integer(gain))); -- fill missing bits with zeros from right
				elsif i2s_bit_width = 24 then
					sample_r_out <= std_logic_vector(shift_left(signed(signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay downto i2s_bit_width - bit_delay)), to_integer(gain))); -- take all 24 bits
				elsif i2s_bit_width = 32 then
					sample_r_out <= signal_shift((2 * i2s_bit_width) - 1 - bit_delay) & signal_shift((2 * i2s_bit_width) - 2 - bit_delay - to_integer(gain) downto i2s_bit_width - bit_delay + 8 - to_integer(gain)); -- take only the leftmost 24 bits to keep 0dBfs
				end if;

				sync_out <= '1'; -- set output flag
			else
				sync_out <= '0';
			end if;
		end if;
	end process;

	get_data : process(clk)
	begin
		if rising_edge(clk) then
			if b_pos_edge = '1' then
				-- fill shift-register with new data
				signal_shift <= signal_shift(signal_shift'high - 1 downto 0) & sdata;
			end if;
		end if;
	end process;
end rtl;
