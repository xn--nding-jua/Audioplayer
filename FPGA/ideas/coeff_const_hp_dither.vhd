-- Coeff-Constant-Block
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains coefficients for 24dB/oct Linkwitz-Riley-Filter
-- It will output a sync-signal with duration of one single main-clk based
-- on clk_sample as trigger for the low-pass-filter

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity coeff_const_hp_dither is
	port (
		clk			:	in std_logic := '0';

		a0_out		:	out signed(15 downto 0) := (others=>'0');
		a1_out		:	out signed(15 downto 0) := (others=>'0');
		a2_out		:	out signed(15 downto 0) := (others=>'0');

		b1_out		:	out signed(15 downto 0) := (others=>'0');
		b2_out		:	out signed(15 downto 0) := (others=>'0')
	);
end coeff_const_hp_dither;

architecture Behavioral of coeff_const_hp_dither is
begin
	process(clk)
	begin
		if rising_edge(clk) then
			-- coefficients for 16bit: 1bit sign + 1bit integer + 14bit fraction
			-- 5MHz sample-frequency, 20kHz cut-off as HighPass
			a0_out <= signed'("0011111001101100"); -- 15980
			a1_out <= signed'("1000001100101000"); -- -31960
			a2_out <= signed'("0011111001101100"); -- 15980
			
			b1_out <= signed'("1000001100101101"); -- -31955
			b2_out <= signed'("0011110011011101"); -- 15581
		end if;
	end process;
end Behavioral;
