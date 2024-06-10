-- reduce 4 MHz clock to 1 Hz clock.    

LIBRARY IEEE;    
USE IEEE.STD_LOGIC_1164.ALL;    

entity reset_signal is
    generic (
        clk_in_hz : natural range 0 to 10000000 := 4000000;
        rst_hz : natural range 0 to 10000 := 1
    );
    port (
        clk_in : in std_logic;
        rst_out : out std_logic
    );
end reset_signal;

architecture Behavioral of reset_signal is
    signal count : natural range 0 to (clk_in_hz/rst_hz) := 0;
begin
    process(clk_in)     
    begin
        if(rising_edge(clk_in)) then
            count <=count+1;
            if (count = (clk_in_hz/rst_hz)-1) then --1999999
					rst_out <= '1';
					count <= 0;
				else
					rst_out <= '0';
            end if;
        end if;
    end process;
end;
