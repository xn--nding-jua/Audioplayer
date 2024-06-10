-- Main-fader section
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a main-fader-section for audio-mixer

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity mainfadersection is
  generic (
    -- Registers width, determines minimal baud speed of input AES3 at given master clock frequency
    bit_width		:	natural range 16 to 48 := 24
  );
  port (
	clk				:	in std_logic := '0';
			
	main_l_in		:	in signed(bit_width - 1 downto 0) := (others=>'0');
	main_r_in		:	in signed(bit_width - 1 downto 0) := (others=>'0');
	main_sub_in		:	in signed(bit_width - 1 downto 0) := (others=>'0');
	sync_in			:	in std_logic := '0';

	vol_main_l		:	in std_logic_vector(7 downto 0) := (others=>'0');
	vol_main_r		:	in std_logic_vector(7 downto 0) := (others=>'0');
	vol_main_sub	:	in std_logic_vector(7 downto 0) := (others=>'0');
	
	main_l_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
	main_r_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
	main_sub_out	:	out signed(bit_width - 1 downto 0) := (others=>'0');
	sync_out			:	out std_logic := '0'
	);
end mainfadersection;

architecture Behavioral of mainfadersection is
	signal state		:	natural range 0 to 5 := 0;

	--signals for multiplier
	signal mult_in_a	:	signed(bit_width - 1 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(8 downto 0) := (others=>'0');
	signal mult_out	:	signed((bit_width + 9) - 1 downto 0) := (others=>'0');
	
	-- signals for audio-samples
	signal main_l 		: signed(bit_width - 1 downto 0);
	signal main_r 		: signed(bit_width - 1 downto 0);
	signal main_sub	: signed(bit_width - 1 downto 0);
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
				mult_in_a <= main_l_in;
				mult_in_b <= signed("0" & vol_main_l);
				state <= 1;
				
			elsif (state = 1) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				main_l <= resize(shift_right(mult_out, 7), bit_width);

				-- load multiplicator with ch1 (right)
				mult_in_a <= main_r_in;
				mult_in_b <= signed("0" & vol_main_r);
				state <= state + 1;
	
			elsif (state = 2) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				main_r <= resize(shift_right(mult_out, 7), bit_width);

				-- load multiplicator with ch1 (right)
				mult_in_a <= main_sub_in;
				mult_in_b <= signed("0" & vol_main_sub);
				state <= state + 1;

			elsif (state = 3) then
				-- now we have to divide the multiplicated signal by 128.
				-- We can bitshift this by 7 to the right
				main_sub <= resize(shift_right(mult_out, 7), bit_width);
				state <= state + 1;

			elsif (state = 4) then
				main_l_out <= main_l;
				main_r_out <= main_r;
				main_sub_out <= main_sub;

				sync_out <= '1';
				state <= state + 1;
			elsif (state = 5) then
				sync_out <= '0';
				state <= 0;
			end if;
		end if;
	end process;
end Behavioral;