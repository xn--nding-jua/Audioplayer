-- Coeff-Constant-Block for Linkwitz-Riley Filter with 24dB/oct
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

entity coeff_const_lr24 is
	port (
		clk			:	in std_logic := '0';
		clk_sample	:	in std_logic := '0';

		a0_out		:	out signed(39 downto 0) := (others=>'0');
		a1_out		:	out signed(39 downto 0) := (others=>'0');
		a2_out		:	out signed(39 downto 0) := (others=>'0');
		a3_out		:	out signed(39 downto 0) := (others=>'0');
		a4_out		:	out signed(39 downto 0) := (others=>'0');

		b1_out		:	out signed(39 downto 0) := (others=>'0');
		b2_out		:	out signed(39 downto 0) := (others=>'0');
		b3_out		:	out signed(39 downto 0) := (others=>'0');
		b4_out		:	out signed(39 downto 0) := (others=>'0');

		sync_out		:	out std_logic := '0'
	);
end coeff_const_lr24;

architecture Behavioral of coeff_const_lr24 is
	signal zclk_sample 	: std_logic;
begin
	-- process for creating sync-signal with PDM-frequency
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

	-- static filter-constants for Low-Pass-Filter with 24dB/oct
	process(clk)
	begin
		if rising_edge(clk) then
			-- coefficients for 40bit: 1bit sign + 3bit integer + 36bit fraction
			-- 5MHz sample-frequency, 20kHz cut-off
			a0_out <= signed'("0000000000000000000000000000011001110110"); -- 1654
			a1_out <= signed'("0000000000000000000000000001100111011001"); -- 6617
			a2_out <= signed'("0000000000000000000000000010011011000101"); -- 9925
			a3_out <= signed'("0000000000000000000000000001100111011001"); -- 6617
			a4_out <= signed'("0000000000000000000000000000011001110110"); -- 1654
			
			b1_out <= signed'("1100000100100011001000111001000010111000"); -- -269993406280
			b2_out <= signed'("0101110010100000110101110011000000001101"); -- 397835448333
			b3_out <= signed'("1100001101010101000101010010111010111010"); -- -260565553478
			b4_out <= signed'("0000111011100110111100000111011111100100"); -- 64004061156

			
			-- To save some logic, we could use a LowPass filter with fc=24kHz and
			-- reduced bits for the coefficients (36 instead of 40bit):
			
			-- coefficients for 36bit: 1bit sign + 3bit integer + 32bit fraction
			-- 5MHz sample-frequency, 24kHz cut-off
			--a0_out <= signed'("000000000000000000000000000011010101"); -- 213
			--a1_out <= signed'("000000000000000000000000001101010100"); -- 852
			--a2_out <= signed'("000000000000000000000000010011111101"); -- 1277
			--a3_out <= signed'("000000000000000000000000001101010100"); -- 852
			--a4_out <= signed'("000000000000000000000000000011010101"); -- 213
			
			--b1_out <= signed'("110000010101110101011001111010011101"); -- -16813547875
			--b2_out <= signed'("010110111111011010110000110011001101"); -- 24686300365
			--b3_out <= signed'("110000111111101011100000010011011000"); -- -16111500072
			--b4_out <= signed'("000011101011000100010101110100001100"); -- 3943783692
		end if;
	end process;
end Behavioral;
