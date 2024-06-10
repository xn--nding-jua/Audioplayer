-- 6-channel Audiomixer
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a multi-channel audiomixer

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity audiomixer is
   generic (
      bit_width		:	natural range 16 to 48 := 24
   );
	port (
		clk				:	in std_logic := '0';
		ch1_in			:	in signed(23 downto 0) := (others=>'0');
		ch2_in			:	in signed(23 downto 0) := (others=>'0');
		ch3_in			:	in signed(23 downto 0) := (others=>'0');
		ch4_in			:	in signed(23 downto 0) := (others=>'0');
		ch5_in			:	in signed(23 downto 0) := (others=>'0');
		ch6_in			:	in signed(23 downto 0) := (others=>'0');
		sync_in			:	in std_logic := '0';

		vol_ch1_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch1_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch2_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch2_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch3_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch3_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch4_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch4_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch5_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch5_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch6_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
		vol_ch6_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
		
		main_l_out		:	out signed(bit_width - 1 downto 0) := (others=>'0'); -- use a specific number of additional bits to prevent signal-clipping
		main_r_out		:	out signed(bit_width - 1 downto 0) := (others=>'0'); -- use a specific number of additional bits to prevent signal-clipping
		sync_out			:	out std_logic := '0'
		);
end audiomixer;

architecture Behavioral of audiomixer is
	signal state		:	natural range 0 to 15 := 0;

	--signals for multiplier
	signal mult_in_a	:	signed(23 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(8 downto 0) := (others=>'0');
	signal mult_out	:	signed(32 downto 0) := (others=>'0');
	
	-- signals for audio-samples. We allow a signal-increase by factor 2, so we will use 25-bit here
	signal ch1_l 		: signed(24 downto 0);
	signal ch1_r 		: signed(24 downto 0);
	signal ch2_l 		: signed(24 downto 0);
	signal ch2_r 		: signed(24 downto 0);
	signal ch3_l 		: signed(24 downto 0);
	signal ch3_r 		: signed(24 downto 0);
	signal ch4_l 		: signed(24 downto 0);
	signal ch4_r 		: signed(24 downto 0);
	signal ch5_l 		: signed(24 downto 0);
	signal ch5_r 		: signed(24 downto 0);
	signal ch6_l 		: signed(24 downto 0);
	signal ch6_r 		: signed(24 downto 0);
	
	signal main_l 		: signed(bit_width - 1 downto 0);
	signal main_r 		: signed(bit_width - 1 downto 0);
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		--mult_out <= resize(shift_right(mult_in_a * resize(mult_in_b, 48), 40), 48);
		mult_out <= mult_in_a * mult_in_b;
	end process;

	process(clk)
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and state = 0) then
				-- load multiplicator with ch1 (left)
				mult_in_a <= ch1_in;
				mult_in_b <= signed("0" & vol_ch1_l);
				state <= 1;
			elsif (state = 1) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch1_l <= resize(shift_right(mult_out, 7), ch1_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch1_in;
				mult_in_b <= signed("0" & vol_ch1_r);
				state <= state + 1;
			elsif (state = 2) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch1_r <= resize(shift_right(mult_out, 7), ch1_r'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch2_in;
				mult_in_b <= signed("0" & vol_ch2_l);
				state <= state + 1;
			elsif (state = 3) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch2_l <= resize(shift_right(mult_out, 7), ch2_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch2_in;
				mult_in_b <= signed("0" & vol_ch2_r);
				state <= state + 1;
			elsif (state = 4) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch2_r <= resize(shift_right(mult_out, 7), ch2_r'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch3_in;
				mult_in_b <= signed("0" & vol_ch3_l);
				state <= state + 1;
			elsif (state = 5) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch3_l <= resize(shift_right(mult_out, 7), ch3_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch3_in;
				mult_in_b <= signed("0" & vol_ch3_r);
				state <= state + 1;
			elsif (state = 6) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch3_r <= resize(shift_right(mult_out, 7), ch3_r'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch4_in;
				mult_in_b <= signed("0" & vol_ch4_l);
				state <= state + 1;
			elsif (state = 7) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch4_l <= resize(shift_right(mult_out, 7), ch4_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch4_in;
				mult_in_b <= signed("0" & vol_ch4_r);
				state <= state + 1;
			elsif (state = 8) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch4_r <= resize(shift_right(mult_out, 7), ch4_r'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch5_in;
				mult_in_b <= signed("0" & vol_ch5_l);
				state <= state + 1;
			elsif (state = 9) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch5_l <= resize(shift_right(mult_out, 7), ch5_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch5_in;
				mult_in_b <= signed("0" & vol_ch5_r);
				state <= state + 1;
			elsif (state = 10) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch5_r <= resize(shift_right(mult_out, 7), ch5_r'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch6_in;
				mult_in_b <= signed("0" & vol_ch6_l);
				state <= state + 1;
			elsif (state = 11) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch6_l <= resize(shift_right(mult_out, 7), ch6_l'length);

				-- load multiplicator with ch1 (right)
				mult_in_a <= ch6_in;
				mult_in_b <= signed("0" & vol_ch6_r);
				state <= state + 1;
			elsif (state = 12) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				ch6_r <= resize(shift_right(mult_out, 7), ch6_r'length);
				
				state <= state + 1;
			elsif (state = 13) then
				-- create stereo-output
				main_l <= resize(ch1_l, bit_width) + resize(ch2_l, bit_width) + resize(ch3_l, bit_width) + resize(ch4_l, bit_width) + resize(ch5_l, bit_width) + resize(ch6_l, bit_width);
				main_r <= resize(ch1_r, bit_width) + resize(ch2_r, bit_width) + resize(ch3_r, bit_width) + resize(ch4_r, bit_width) + resize(ch5_r, bit_width) + resize(ch6_r, bit_width);

				state <= state + 1;
			elsif (state = 14) then
				main_l_out <= main_l;
				main_r_out <= main_r;
				
				sync_out <= '1';
				state <= state + 1;
			elsif (state = 15) then
				sync_out <= '0';
				state <= 0;
			end if;
		end if;
	end process;
end Behavioral;