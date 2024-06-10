-- Mono Linkwitz-Riley-Filter with 12dB/oct
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a mono low-pass filter for audio
-- It consists of a 2nd-order IIR Butterworth-Filter (Linkwitz-Riley)
-- that uses Fixed-Point-Multiplication for the coefficients.
-- Furthermore, the feedback-values are stored keeping the fractions, to
-- increase accuracy during the calculation. This will increase the number
-- of used DSP-multipliers for the sake of audio-quality.

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

entity filter_lr12_mono is
  generic(
    coeff_bits	:	natural range 8 to 48 := 32; -- for a Qcoeff_bits - 5-coefficient: signed-bit + integer-bits + Qcoeff_bits - 5-bits = 1 + 4 + coeff_bits - 5 = 40 bit
    fract_bits	:	natural range 8 to 48 := 27
    );
  port (
	clk		:	in std_logic := '0';
	input		:	in signed(23 downto 0) := (others=>'0');
	sync_in	:	in std_logic := '0';
	rst		:	in std_logic := '0';

	-- coefficients have to be multiplied with 2^fract_bits before
	a0 		:	in signed(coeff_bits - 1 downto 0);
	a1 		:	in signed(coeff_bits - 1 downto 0);
	a2 		:	in signed(coeff_bits - 1 downto 0);

	-- coefficients have to be multiplied with 2^fract_bits before
	b1 		:	in signed(coeff_bits - 1 downto 0);
	b2 		:	in signed(coeff_bits - 1 downto 0);

	output	:	out signed(23 downto 0) := (others=>'0');
	sync_out	:	out std_logic := '0'
	);
end filter_lr12_mono;

architecture Behavioral of filter_lr12_mono is
	signal state		:	natural range 0 to 7 := 0;
	
	--signals for multiplier
	signal mult_in_a	:	signed(coeff_bits - 1 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(coeff_bits + 24 - 1 downto 0) := (others=>'0');
	signal mult_out	:	signed((coeff_bits + coeff_bits + 24 - 1) downto 0) := (others=>'0');

	--temp regs and delay regs
	signal temp_in, in_z1, in_z2				:	signed(23 downto 0):= (others=>'0');	
	signal out_z1, out_z2						:	signed(coeff_bits + 24 - 1 downto 0):= (others=>'0');	
	signal temp			:	signed(coeff_bits + 24 - 1 downto 0):= (others=>'0');
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;

	--// calculate filter
	--float output = (LR12->a[0] * sampleData) + (LR12->a[1] * LR12->x[0]) + (LR12->a[2] * LR12->x[1]) - (LR12->b[1] * LR12->y[0]) - (LR12->b[2] * LR12->y[1]);
	process(clk, rst)
	begin
		if (rst = '1') then
			-- reset internal signals
			temp_in <= ( others => '0');
			in_z1 <= ( others => '0');
			in_z2 <= ( others => '0');
			out_z1 <= ( others => '0');
			out_z2 <= ( others => '0');
			temp <= ( others => '0');
			
			-- set output to zero
			output <= ( others => '0');
			sync_out <= '1';
			
			-- call last state to reset filter-states for next calculation
			state <= 7;
		else
			if rising_edge(clk) then
				if (sync_in = '1' and state = 0) then
					-- load multiplier with a0 * input
					mult_in_a <= a0;
					mult_in_b <= resize(input, coeff_bits + 24);
					temp_in <= input;
					state <= 1; -- start of state-machine
					
				elsif (state = 1) then
					-- save result of (samplein*a0) to temp and load multiplier with in_z1 and a1
					temp <= resize(mult_out, coeff_bits + 24);
					mult_in_a <= a1;
					mult_in_b <= resize(in_z1, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 2) then
					-- save and sum up result of (in_z1*a1) to temp and load multiplier with in_z2 and a2
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a2;
					mult_in_b <= resize(in_z2, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 3) then
					-- save and sum up result of (in_z2*a2) to temp and load multiplier with out_z1 and b1
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= b1;
					mult_in_b <= out_z1;
					state <= state + 1;
					
				elsif (state = 4) then
					-- save and sum up (negative) result of (out_z1*b1) and load multiplier with out_z2 and b2
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b2;
					mult_in_b <= out_z2;
					state <= state + 1;
					
				elsif (state = 5) then
					-- save and sum up (negative) result of (out_z2*b2)
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 6) then
					-- save all delay registers and save result to output
					in_z2 <= in_z1;
					in_z1 <= temp_in;

					out_z2 <= out_z1;
					out_z1 <= temp; -- save value with fractions to gain higher resolution for this filter
					
					output <= resize(shift_right(temp, fract_bits), 24); -- resize to 24-bit audio

					sync_out <= '1';
					state <= state + 1;
					
				elsif (state = 7) then
					sync_out <= '0';
					state <= 0;
				end if;
			end if;
		end if;
	end process;
end Behavioral;