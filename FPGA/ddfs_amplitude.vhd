LIBRARY IEEE;    
USE IEEE.STD_LOGIC_1164.ALL;   
use ieee.numeric_std.all;

entity ddfs_amplitude is
	port (
		clk			:	in std_logic := '0';
		ddfs_in		:	in signed(7 downto 0) := (others=>'0');
		ddfs_out		:	out signed(23 downto 0) := (others=>'0')
    );
end ddfs_amplitude;

architecture Behavioral of ddfs_amplitude is
begin
	process(clk)     
    begin
		if(rising_edge(clk)) then
			ddfs_out	<= shift_left(resize(ddfs_in, 24), 15);
		end if;
    end process;
end;
