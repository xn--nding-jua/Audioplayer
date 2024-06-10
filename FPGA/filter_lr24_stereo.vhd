-- Stereo Linkwitz-Riley-Filter with 24dB/oct
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a stereo high-pass and a mono low-pass filter for audio
-- It consists of a steep 4th-order IIR Butterworth-Filter (Linkwitz-Riley)
-- that uses Fixed-Point-Multiplication for the coefficients.
-- Furthermore, the feedback-values are stored keeping the fractions, to
-- increase accuracy during the calculation. This will increase the number
-- of used DSP-multipliers for the sake of audio-quality.

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

entity filter_lr24_stereo is
  generic(
    coeff_bits	:	natural range 16 to 48 := 40; -- for a Qfract_bits-coefficient: signed-bit + integer-bits + Qfract_bits-bits = 1 + 4 + fract_bits = 40 bit
    fract_bits	:	natural range 16 to 48 := 35
    );
  port (
	clk			:	in std_logic := '0';
	input_l		:	in signed(23 downto 0) := (others=>'0');
	input_r		:	in signed(23 downto 0) := (others=>'0');
	sync_in		:	in std_logic := '0';
	rst			:	in std_logic := '0';

	-- coefficients have to be multiplied with 2^fract_bits before
	a0_lp 		:	in signed(coeff_bits - 1 downto 0);
	a1_lp 		:	in signed(coeff_bits - 1 downto 0);
	a2_lp 		:	in signed(coeff_bits - 1 downto 0);
	a3_lp 		:	in signed(coeff_bits - 1 downto 0);
	a4_lp 		:	in signed(coeff_bits - 1 downto 0);

	-- coefficients have to be multiplied with 2^fract_bits before
	b1_lp 		:	in signed(coeff_bits - 1 downto 0);
	b2_lp 		:	in signed(coeff_bits - 1 downto 0);
	b3_lp 		:	in signed(coeff_bits - 1 downto 0);
	b4_lp 		:	in signed(coeff_bits - 1 downto 0);

	output_l		:	out signed(23 downto 0) := (others=>'0');
	output_r		:	out signed(23 downto 0) := (others=>'0');
	sync_out		:	out std_logic := '0'
	);
end filter_lr24_stereo;

architecture Behavioral of filter_lr24_stereo is
	signal state		:	natural range 0 to 19 := 0;
	
	--signals for multiplier
	signal mult_in_a	:	signed(coeff_bits - 1 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(coeff_bits + 24 - 1 downto 0) := (others=>'0');
	signal mult_out	:	signed((coeff_bits + coeff_bits + 24 - 1) downto 0) := (others=>'0');

	--temp regs and delay regs
	signal temp_in_l, in_z1_l, in_z2_l, in_z3_l, in_z4_l				:	signed(23 downto 0):= (others=>'0');	
	signal temp_in_r, in_z1_r, in_z2_r, in_z3_r, in_z4_r				:	signed(23 downto 0):= (others=>'0');	
	signal out_z1_l, out_z2_l, out_z3_l, out_z4_l						:	signed(coeff_bits + 24 - 1 downto 0):= (others=>'0');	
	signal out_z1_r, out_z2_r, out_z3_r, out_z4_r						:	signed(coeff_bits + 24 - 1 downto 0):= (others=>'0');	
	signal temp			:	signed(coeff_bits + 24 - 1 downto 0):= (others=>'0');
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;

	--// calculate filter
	--float output = (LR24->a[0] * sampleData) + (LR24->a[1] * LR24->x[0]) + (LR24->a[2] * LR24->x[1]) + (LR24->a[3] * LR24->x[2]) + (LR24->a[4] * LR24->x[3]) - (LR24->b[1] * LR24->y[0]) - (LR24->b[2] * LR24->y[1]) - (LR24->b[3] * LR24->y[2]) - (LR24->b[4] * LR24->y[3]);
	process(clk, rst)
	begin
		if (rst = '1') then
			-- reset internal signals
			temp_in_l <= ( others => '0');
			temp_in_r <= ( others => '0');
			in_z1_l <= ( others => '0');
			in_z2_l <= ( others => '0');
			in_z3_l <= ( others => '0');
			in_z4_l <= ( others => '0');
			in_z1_r <= ( others => '0');
			in_z2_r <= ( others => '0');
			in_z3_r <= ( others => '0');
			in_z4_r <= ( others => '0');
			out_z1_l <= ( others => '0');
			out_z2_l <= ( others => '0');
			out_z3_l <= ( others => '0');
			out_z4_l <= ( others => '0');
			out_z1_r <= ( others => '0');
			out_z2_r <= ( others => '0');
			out_z3_r <= ( others => '0');
			out_z4_r <= ( others => '0');
			temp <= ( others => '0');
			
			-- set output to zero
			output_l <= ( others => '0');
			output_r <= ( others => '0');
			sync_out <= '1';
			
			-- call last state to reset filter-states for next calculation
			state <= 19;
		else
			if rising_edge(clk) then
				if (sync_in = '1' and state = 0) then
					-- load multiplier with a0 * input
					mult_in_a <= a0_lp;
					mult_in_b <= resize(input_l, coeff_bits + 24);
					temp_in_l <= input_l;
					temp_in_r <= input_r;
					state <= 1; -- start of state-machine
					
				elsif (state = 1) then
					-- save result of (samplein*a0) to temp and load multiplier with in_z1 and a1
					temp <= resize(mult_out, coeff_bits + 24);
					mult_in_a <= a1_lp;
					mult_in_b <= resize(in_z1_l, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 2) then
					-- save and sum up result of (in_z1*a1) to temp and load multiplier with in_z2 and a2
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a2_lp;
					mult_in_b <= resize(in_z2_l, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 3) then
					-- save and sum up result of (in_z2*a2) to temp and load multiplier with in_z3 and a3
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a3_lp;
					mult_in_b <= resize(in_z3_l, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 4) then
					-- save and sum up result of (in_z3*a3) to temp and load multiplier with in_z4 and a4
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a4_lp;
					mult_in_b <= resize(in_z4_l, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 5) then
					-- save and sum up result of (in_z4*a4) to temp and load multiplier with out_z1 and b1
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= b1_lp;
					mult_in_b <= out_z1_l;
					state <= state + 1;
					
				elsif (state = 6) then
					-- save and sum up (negative) result of (out_z1*b1) and load multiplier with out_z2 and b2
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b2_lp;
					mult_in_b <= out_z2_l;
					state <= state + 1;
					
				elsif (state = 7) then
					-- save and sum up (negative) result of (out_z2*b2) and load multiplier with out_z3 and b3
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b3_lp;
					mult_in_b <= out_z3_l;
					state <= state + 1;
					
				elsif (state = 8) then
					-- save and sum up (negative) result of (out_z3*b3) and load multiplier with out_z4 and b4
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b4_lp;
					mult_in_b <= out_z4_l;
					state <= state + 1;
					
				elsif (state = 9) then
					-- save and sum up (negative) result of (out_z4*b4)
					-- save all delay registers and save result to output
					in_z4_l <= in_z3_l;
					in_z3_l <= in_z2_l;
					in_z2_l <= in_z1_l;
					in_z1_l <= temp_in_l;

					out_z4_l <= out_z3_l;
					out_z3_l <= out_z2_l;
					out_z2_l <= out_z1_l;
					out_z1_l <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24); -- save value with fractions to gain higher resolution for this filter
					
					output_l <= resize(shift_right(temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24), fract_bits), 24); -- resize to 24-bit audio

					-- load multiplier with a0 * input
					mult_in_a <= a0_lp;
					mult_in_b <= resize(temp_in_r, coeff_bits + 24);

					state <= state + 1;
					
				elsif (state = 10) then
					-- save result of (samplein*a0) to temp and load multiplier with in_z1 and a1
					temp <= resize(mult_out, coeff_bits + 24);
					mult_in_a <= a1_lp;
					mult_in_b <= resize(in_z1_r, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 11) then
					-- save and sum up result of (in_z1*a1) to temp and load multiplier with in_z2 and a2
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a2_lp;
					mult_in_b <= resize(in_z2_r, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 12) then
					-- save and sum up result of (in_z2*a2) to temp and load multiplier with in_z3 and a3
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a3_lp;
					mult_in_b <= resize(in_z3_r, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 13) then
					-- save and sum up result of (in_z3*a3) to temp and load multiplier with in_z4 and a4
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= a4_lp;
					mult_in_b <= resize(in_z4_r, coeff_bits + 24);
					state <= state + 1;
					
				elsif (state = 14) then
					-- save and sum up result of (in_z4*a4) to temp and load multiplier with out_z1 and b1
					temp <= temp + resize(mult_out, coeff_bits + 24);
					mult_in_a <= b1_lp;
					mult_in_b <= out_z1_r;
					state <= state + 1;
					
				elsif (state = 15) then
					-- save and sum up (negative) result of (out_z1*b1) and load multiplier with out_z2 and b2
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b2_lp;
					mult_in_b <= out_z2_r;
					state <= state + 1;
					
				elsif (state = 16) then
					-- save and sum up (negative) result of (out_z2*b2) and load multiplier with out_z3 and b3
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b3_lp;
					mult_in_b <= out_z3_r;
					state <= state + 1;
					
				elsif (state = 17) then
					-- save and sum up (negative) result of (out_z3*b3) and load multiplier with out_z4 and b4
					temp <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24);
					mult_in_a <= b4_lp;
					mult_in_b <= out_z4_r;
					state <= state + 1;
					
				elsif (state = 18) then
					-- save and sum up (negative) result of (out_z4*b4)
					-- save all delay registers, save result to output and apply ouput-valid signal
					in_z4_r <= in_z3_r;
					in_z3_r <= in_z2_r;
					in_z2_r <= in_z1_r;
					in_z1_r <= temp_in_r;

					out_z4_r <= out_z3_r;
					out_z3_r <= out_z2_r;
					out_z2_r <= out_z1_r;
					out_z1_r <= temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24); -- save value with fractions to gain higher resolution for this filter
					
					output_r <= resize(shift_right(temp - resize(shift_right(mult_out, fract_bits), coeff_bits + 24), fract_bits), 24); -- resize to 24-bit audio

					sync_out <= '1';
					state <= state + 1;
					
				elsif (state = 19) then
					sync_out <= '0';
					state <= 0;
				end if;
			end if;
		end if;
	end process;
end Behavioral;