-- Stereo IIR audio-filter
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a stereo IIR audio-filter for low-/high-pass-filter,
-- peak-filter (PEQ), low-/high-shelf-, bandpass- and notch-filter
-- The type of filter can be adjusted by the filter-coefficients
-- It uses Fixed-Point-Multiplication for the coefficients in Qx-format.
-- The audio-bit-depth can be adjusted using generic setup

-- Original Source: https://github.com/YetAnotherElectronicsChannel/FPGA-Audio-IIR
-- Online-Calculation of Filter-Coefficients: https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity filter_iir_stereo is
   generic (
      bit_width 	: natural range 16 to 48 := 24;
      coeff_bits	:	natural range 16 to 48 := 32; -- signed-bit + integer-bits + fraction-bits
      fract_bits	:	natural range 16 to 48 := 30
   );
	port (
		clk  : in std_logic := '0';

		iir_in_l : in signed (bit_width - 1 downto 0) := (others=>'0');
		iir_in_r : in signed (bit_width - 1 downto 0) := (others=>'0');
		sync_in  : in std_logic := '0';
		rst		: in std_logic := '0';

		iir_out_l : out signed(bit_width - 1 downto 0) := (others=>'0');
		iir_out_r : out signed(bit_width - 1 downto 0) := (others=>'0');
		sync_out : out std_logic := '0';

		-- dynamic setting of filter-coefficients
		-- a0, a1, a2, b1, b2 must be multiplied with 2^fract_bits before
		a0	:	in signed(coeff_bits - 1 downto 0);
		a1 :	in signed(coeff_bits - 1 downto 0);
		a2 :	in signed(coeff_bits - 1 downto 0);
		b1	:	in signed(coeff_bits - 1 downto 0);
		b2 :	in signed(coeff_bits - 1 downto 0)
	);
end filter_iir_stereo;

architecture Behavioral of filter_iir_stereo is
	signal state : natural range 0 to 13 := 0;

	--signals for multiplier
	signal mult_in_a : signed(bit_width - 1 downto 0) := (others=>'0'); -- input signal
	signal mult_in_b : signed(coeff_bits - 1 downto 0) := (others=>'0'); -- coefficient
	signal mult_out : signed(bit_width + coeff_bits - 1 downto 0):= (others=>'0');

	--temp regs and delay regs
	signal temp : signed(bit_width + 8 - 1 downto 0):= (others=>'0'); -- keep 8 additional bits for storing fractions
	signal temp_in_l, in_z1_l, in_z2_l, out_z1_l, out_z2_l : signed(bit_width - 1 downto 0):= (others=>'0');
	signal temp_in_r, in_z1_r, in_z2_r, out_z1_r, out_z2_r : signed(bit_width - 1 downto 0):= (others=>'0');
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;

	process (clk, rst)
	begin
		if (rst = '1') then
			-- reset internal signals
			temp_in_l <= ( others => '0');
			in_z1_l <= ( others => '0');
			in_z2_l <= ( others => '0');
			out_z1_l <= ( others => '0');
			out_z2_l <= ( others => '0');
			temp_in_r <= ( others => '0');
			in_z1_r <= ( others => '0');
			in_z2_r <= ( others => '0');
			out_z1_r <= ( others => '0');
			out_z2_r <= ( others => '0');

			-- set output to zero
			iir_out_l <= ( others => '0');
			iir_out_r <= ( others => '0');
			sync_out <= '1';

			-- call last state to reset filter-states for next calculation
			state <= 13;
		else
			if (rising_edge(clk)) then    
				-- start process when valid sample arrived

				if (sync_in = '1' and state = 0) then
					-- load multiplier with samplein * a0
					mult_in_a <= shift_left(iir_in_l, 8); -- shift 8 to left to keep some bits for fraction-results lateron
					mult_in_b <= a0;
					temp_in_l <= shift_left(iir_in_l, 8); -- shift 8 to left to keep some bits for fraction-results lateron
					temp_in_r <= shift_left(iir_in_r, 8); -- shift 8 to left to keep some bits for fraction-results lateron
					
					state <= 1;

				elsif (state = 1) then
					--save result of (samplein*a0) to temp and apply right-shift of fract_bits
					--and load multiplier with in_z1 and a1
					temp <= resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= in_z1_l;
					mult_in_b <= a1;
					
					state <= 2;

				elsif (state = 2) then
					--save and sum up result of (in_z1*a1) to temp and apply right-shift of fract_bits
					--and load multiplier with in_z2 and a2
					temp <= temp + resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= in_z2_l;
					mult_in_b <= a2;
					
					state <= 3;
						
				elsif (state = 3) then
					--save and sum up result of (in_z2*a2) to temp and apply right-shift of fract_bits
					-- and load multiplier with out_z1 and b1
					temp <= temp + resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= out_z1_l;
					mult_in_b <= b1;
					
					state <= 4;

				elsif (state = 4) then
					--save and sum up (negative) result of (out_z1*b1) and apply right-shift of fract_bits
					--and load multiplier with out_z2 and b2
					temp <= temp - resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= out_z2_l;
					mult_in_b <= b2;
					
					state <= 5;

				elsif (state = 5) then
					--save and sum up (negative) result of (out_z2*b2) and apply right-shift of fract_bits
					temp <= temp - resize(shift_right(mult_out, fract_bits), bit_width + 8);

					state <= 6;
					  
				elsif (state = 6) then
					--save result to output, save all delay registers, apply ouput-valid signal
					iir_out_l <= resize(shift_right(temp, 8), bit_width);
					out_z1_l <= resize(temp, bit_width);
					out_z2_l <= out_z1_l;       
					in_z2_l <= in_z1_l;
					in_z1_l <= temp_in_l;

					-- load multiplier with samplein * a0
					mult_in_a <= temp_in_r;
					mult_in_b <= a0;

					state <= 7;
					  
				elsif (state = 7) then
					--save result of (samplein*a0) to temp and apply right-shift of fract_bits
					--and load multiplier with in_z1 and a1
					temp <= resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= in_z1_r;
					mult_in_b <= a1;

					state <= 8;
					  
				elsif (state = 8) then
					--save and sum up result of (in_z1*a1) to temp and apply right-shift of fract_bits
					--and load multiplier with in_z2 and a2
					temp <= temp + resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= in_z2_r;
					mult_in_b <= a2;
					
					state <= 9;
						
				elsif (state = 9) then
					--save and sum up result of (in_z2*a2) to temp and apply right-shift of fract_bits
					-- and load multiplier with out_z1 and b1
					temp <= temp + resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= out_z1_r;
					mult_in_b <= b1;
					
					state <= 10;

				elsif (state = 10) then
					--save and sum up (negative) result of (out_z1*b1) and apply right-shift of fract_bits
					--and load multiplier with out_z2 and b2
					temp <= temp - resize(shift_right(mult_out, fract_bits), bit_width + 8);
					mult_in_a <= out_z2_r;
					mult_in_b <= b2;
					
					state <= 11;

				elsif (state = 11) then
					--save and sum up (negative) result of (out_z2*b2) and apply right-shift of fract_bits
					temp <= temp - resize(shift_right(mult_out, fract_bits), bit_width + 8);
					
					state <= 12;
					  
				elsif (state = 12) then
					--save result to output, save all delay registers, apply ouput-valid signal
					iir_out_r <= resize(shift_right(temp, 8), bit_width);
					out_z1_r <= resize(temp, bit_width);
					out_z2_r <= out_z1_r;       
					in_z2_r <= in_z1_r;
					in_z1_r <= temp_in_r;
					sync_out <= '1';
					
					state <= 13;
					  
				elsif (state = 13) then
					sync_out <= '0';

					state <= 0;
				end if;
			end if;
		end if;
	end process;
end Behavioral;