-- This file contains a first-order sigma-delta-converter
-- Source by Mike Field (https://github.com/hamsternz/second_order_sigma_delta_DAC)
--
-- MIT License
-- 
-- Copyright (c) 2020 Mike Field
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sigma_delta_first is
	generic(
		bit_width	: natural range 16 to 32 := 24
	);
	Port (
		clk   		: in  STD_LOGIC;
		sample_in	: in  STD_LOGIC_VECTOR (23 downto 0);
		output		: out STD_LOGIC
	);
end sigma_delta_first;

architecture Behavioral of sigma_delta_first is
	signal dac_out          : std_logic           := '0';
	signal dac_accum        : signed(bit_width + 8 - 1 downto 0) := (others => '0');
	signal dac_feedback     : signed(bit_width + 8 - 1 downto 0) := (others => '0');
begin
	output <= dac_out;

		with dac_out select dac_feedback <= to_signed(-2**(bit_width - 1), bit_width + 8) when '1',
														to_signed( 2**(bit_width - 1), bit_width + 8) when others;     
	dac_proc: process(clk)
		variable new_val : signed(bit_width + 8 - 1 downto 0);
	begin
		if rising_edge(clk) then
			-- calculate output
			new_val := dac_accum + signed(sample_in) + dac_feedback;

			-- quantizing to 1-bit data and calculate feedback-value
			if new_val < 0 then
				dac_out <= '0';
			else
				dac_out <= '1';
			end if;
			
			-- store values
			dac_accum <= new_val;
		end if;
	end process;
end Behavioral;