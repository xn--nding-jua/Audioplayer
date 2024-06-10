library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity audiodithering is
	generic (
		data_width : integer := 24;
		noise_width : integer :=  4
	); 
	port (
		clk		:	in std_logic;
		data_in	:	in std_logic_vector(data_width - 1 downto 0);
      data_out :	out std_logic_vector(data_width - 1 downto 0)
	);
end audiodithering;

architecture Behavioral of audiodithering is
	signal random_num : std_logic_vector(noise_width - 1 downto 0);
begin
	process(clk)
		variable random_num : std_logic_vector(noise_width - 1 downto 0) := (noise_width - 1 => '1', others => '0');
		variable temp : std_logic := '0';
	begin
		if(rising_edge(clk)) then
			temp := random_num(noise_width - 1) xor random_num(noise_width - 2);
			random_num(noise_width - 1 downto 1) := random_num(noise_width - 2 downto 0);
			random_num(0) := temp;
		end if;
		
		data_out <= data_in(data_width - 1 downto noise_width) & random_num(noise_width - 1 downto 0);
	end process;
end Behavioral;