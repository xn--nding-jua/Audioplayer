-- This file contains a two-stage sigma-delta-converter with noise-shaping
-- (c) 2024 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
-- General information about delta-sigma-converters can be found here: https://www.beis.de/Elektronik/DeltaSigma/DeltaSigma.html

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sigma_delta_twostage is
	generic(
		bit_width	: natural range 16 to 32 := 24
	);
	port(
		clk			: in  std_logic;
		sample_in	: in  signed(bit_width - 1 downto 0);
		output		: out std_logic
	);
end sigma_delta_twostage;

architecture Behavioral of sigma_delta_twostage is
	signal dac_out          : std_logic := '0';
	signal feedback_z			: signed(bit_width + 30 - 1 downto 0) := (others => '0');
	signal feedback_zz		: signed(bit_width + 30 - 1 downto 0) := (others => '0');
begin
	output <= dac_out;
	
	dac_proc: process(clk)
		variable new_value : signed(bit_width + 30 - 1 downto 0); -- variables will be updated immediately
	begin
		if rising_edge(clk) then
			-- calculate output
			new_value := resize(sample_in, new_value'length) + feedback_z + feedback_z - feedback_zz;
			
			-- quantizing to 1-bit data and calculate feedback-value
			if (new_value < 0) then
				dac_out <= '0'; -- digital output
				feedback_z <= new_value - to_signed(-2**(bit_width - 1), feedback_z'length); -- feedback_z = new_value - dac_feedback
			else
				dac_out <= '1'; -- digital output
				feedback_z <= new_value - to_signed(2**(bit_width - 1), feedback_z'length); -- feedback_z = new_value - dac_feedback
			end if;
			
			-- store second delay
			feedback_zz <= feedback_z;
       end if;
	end process;
end Behavioral;