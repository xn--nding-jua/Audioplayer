-- This file contains a Linear Feedback Shift Register to generate random numbers
-- Author: Lothar Miller
-- Source: http://www.lothar-miller.de/s9y/categories/38-LFSR

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity lfsr is
	Port (
		clk     : in  std_logic;
		value32 : out  std_logic_vector(31 downto 0);
		value24 : out  std_logic_vector(23 downto 0);
		value16 : out  std_logic_vector(15 downto 0);
		value8  : out  std_logic_vector( 7 downto 0);
		sync_out	:	out std_logic := '0'
	);
end lfsr;

architecture Behavioral of lfsr is
	signal rnd32 : std_logic_vector(31 downto 0) := (others=>'0');
	signal rnd24 : std_logic_vector(23 downto 0) := (others=>'0');
	signal rnd16 : std_logic_vector(15 downto 0) := (others=>'0');
	signal rnd8  : std_logic_vector( 7 downto 0) := (others=>'0');
	signal zclk  : std_logic;
begin
	process(clk)
	begin
		if rising_edge(clk) then
			-- 8Bit
			rnd8(7 downto 1) <= rnd8(6 downto 0) ;
			rnd8(0) <= not(rnd8(7) XOR rnd8(5) XOR rnd8(4) XOR rnd8(3));
			-- 16Bit
			rnd16(15 downto 1) <= rnd16(14 downto 0) ;
			rnd16(0) <= not(rnd16(15) XOR rnd16(13) XOR rnd16(12) XOR rnd16(10));
			-- 24 Bit
			rnd24(23 downto 1) <= rnd24(22 downto 0) ;
			rnd24(0) <= not(rnd24(23) XOR rnd24(22) XOR rnd24(21) XOR rnd24(16));
			-- 32 Bit
			rnd32(31 downto 1) <= rnd32(30 downto 0) ;
			rnd32(0) <= not(rnd32(31) XOR rnd32(21) XOR rnd32(1) XOR rnd32(0));

			zclk <= clk;
			if clk = '1' and zclk = '0' then
				sync_out <= '1';
			else
				sync_out <= '0';
			end if;
		end if;
	end process;
	value32 <= rnd32;
	value24 <= rnd24;
	value16 <= rnd16;
	value8  <= rnd8;
end Behavioral;