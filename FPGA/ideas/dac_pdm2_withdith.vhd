-- This file contains a two-stage sigma-delta-converter with noise-shaping
-- Original source by Mike Field, 06.07.2020, New Zealand (https://github.com/hamsternz/second_order_sigma_delta_DAC/blob/master/sigma_delta_second.vhd)
-- General information about delta-sigma-converters can be found here: https://www.beis.de/Elektronik/DeltaSigma/DeltaSigma.html

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity dac_pdm2_withdith is
	generic(
		bit_width	: natural range 16 to 32 := 24
	);
	port(
		clk			: in  std_logic;
		sample_in	: in  signed(bit_width - 1 downto 0);
		output		: out std_logic
	);
end dac_pdm2_withdith;

architecture Behavioral of dac_pdm2_withdith is
    signal dac_out          : std_logic := '0';
    signal dac_feedback     : signed(bit_width + 3 downto 0) := (others => '0');
    signal dac_feedback_zz  : signed(bit_width + 3 downto 0) := (others => '0');
begin
	output <= dac_out;
	
	-- digital feedback
	with dac_out select dac_feedback <= to_signed(-2**(bit_width - 1), bit_width + 4) when '1',
                                        to_signed( 2**(bit_width - 1), bit_width + 4) when others;     

	dac2_proc: process(clk)
		variable new_val1 : signed(bit_width + 3 downto 0); -- variables will be updated immediately
		variable new_val2 : signed(bit_width + 3 downto 0); -- variables will be updated immediately
		variable temp : std_logic := '0';
	begin
		if rising_edge(clk) then
			-- calculate integrator-outputs
			new_val1 := sample_in - dac_feedback_zz;
			new_val2 := new_val1 + shift_left(dac_feedback + new_val2, 1);
			
			-- quantizing to 1-bit data
			if (new_val2 < 0) then
				dac_out <= '0';
			else
				dac_out <= '1';
			end if;
			
			-- store value for feedback
			dac_feedback_zz <= dac_feedback + new_val2;
       end if;
	end process;
end Behavioral;