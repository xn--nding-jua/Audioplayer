-- This file contains a Linear Feedback Shift Register to generate random numbers
-- Author: Lothar Miller
-- Source: http://www.lothar-miller.de/s9y/categories/38-LFSR

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity lfsr_triangle_pdf is
	Port (
		clk     : in  std_logic;
		value24 : out  signed(23 downto 0);
		sync_out	:	out std_logic := '0'
	);
end lfsr_triangle_pdf;

architecture Behavioral of lfsr_triangle_pdf is
	signal rnd24_pos : std_logic_vector(23 downto 0) := (others=>'0');
	signal rnd24_neg : std_logic_vector(23 downto 0) := (others=>'0');
	signal zclk   : std_logic;
begin
	process(clk)
	begin
		if rising_edge(clk) then
			-- calculate two 24-bit random numbers using Linear Feedback Shift Register (LFSR)
			rnd24_pos(23 downto 1) <= rnd24_pos(22 downto 0) ;
			rnd24_pos(0) <= not(rnd24_pos(23) XOR rnd24_pos(22) XOR rnd24_pos(21) XOR rnd24_pos(16));

			rnd24_neg(23 downto 1) <= rnd24_neg(22 downto 0) ;
			rnd24_neg(0) <= not(rnd24_neg(23) XOR rnd24_neg(22) XOR rnd24_neg(21) XOR rnd24_neg(16));

			-- output synchronisation-clock
			zclk <= clk;
			if clk = '1' and zclk = '0' then
				sync_out <= '1';
			else
				sync_out <= '0';
			end if;
		end if;
	end process;
	
	-- output triangular shaped probability density function (PDF)
	value24 <= signed("0" & rnd24_pos(22 downto 0)) + signed("1" & rnd24_neg(22 downto 0));
end Behavioral;