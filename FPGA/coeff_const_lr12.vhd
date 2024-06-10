-- Coeff-Constant-Block for Linkwitz-Riley Filter with 12dB/oct
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains coefficients for 12dB/oct Linkwitz-Riley-Filter
-- It will output a sync-signal with duration of one single main-clk based
-- on clk_sample as trigger for the low-pass-filter

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity coeff_const_lr12 is
	port (
		clk			:	in std_logic := '0';
		clk_sample	:	in std_logic := '0';

		a0_out		:	out signed(26 downto 0) := (others=>'0');
		a1_out		:	out signed(26 downto 0) := (others=>'0');
		a2_out		:	out signed(26 downto 0) := (others=>'0');

		b1_out		:	out signed(26 downto 0) := (others=>'0');
		b2_out		:	out signed(26 downto 0) := (others=>'0');

		sync_out		:	out std_logic := '0'
	);
end coeff_const_lr12;

architecture Behavioral of coeff_const_lr12 is
	signal zclk_sample 	: std_logic;
begin
	detect_clk_sample_edge : process(clk)
	begin
		if rising_edge(clk) then
			zclk_sample <= clk_sample;
			if clk_sample = '1' and zclk_sample = '0' then
				sync_out <= '1';
			else
				sync_out <= '0';
			end if;
		end if;
	end process;

	process(clk)
	begin
		if rising_edge(clk) then
			-- coefficients for 27bit: 1bit sign + 1bit integer + 25bit fraction
			-- 5MHz sample-frequency, 20kHz cut-off
			a0_out <= signed'("000000000000001010000110001"); -- 5169
			a1_out <= signed'("000000000000010100001100001"); -- 10337
			a2_out <= signed'("000000000000001010000110001"); -- 5169
			
			b1_out <= signed'("100000110010110101011111001"); -- -65443079
			b2_out <= signed'("001111001101110010111001010"); -- 31909322
		end if;
	end process;
end Behavioral;
