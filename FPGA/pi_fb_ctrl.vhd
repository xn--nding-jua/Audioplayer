-- PI Feedback-Controller with Feedforward for Class-D-Amplifier
-- (c) 2024 Dr.-Ing. Christian Noeding
-- christian.noeding@uni-kassel.de
-- https://www.LE.uni-kassel.de
--
-- This file contains a PI-controller to control the feedback-loop
-- of a Class-D-Amplifier. The feedback is the measured output-voltage
-- of a specific output-channel. The input is the input-audio-sample.
-- The controller will adjust any non-linearities of the output-filter
-- and the connected speaker or load.
-- 
-- The basic calculation is:
-- e = target - actual
-- esum = esum + e
-- y = Kp * e + Ki * Ta * esum
--
-- target_val is signed audiosample with audio-bitwidth
-- actual_val is output-voltage converted to audio-bitwidth

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity pi_fb_ctrl is
	port (
		clk				:	in std_logic := '0';
		target_val		:	in signed(23 downto 0) := (others=>'0');
		actual_val		:	in signed(23 downto 0) := (others=>'0');
		pre_gain			:	in signed(15 downto 0); -- in Q15.0
		p_gain			:	in signed(31 downto 0); -- in Q11.20, just kp
		i_gain_ts		:	in signed(31 downto 0); -- in Q11.20, contains ki * Ts
		sync_in			:	in std_logic := '0';
		
		output			:	out signed(23 downto 0) := (others=>'0');
		sync_out			:	out std_logic := '0'
	);
end pi_fb_ctrl;

architecture Behavioral of pi_fb_ctrl is
	signal state		:	natural range 0 to 6 := 0;

	signal error		:	signed(23 + 1 downto 0); -- 1 bit extra headroom for maximum error
	signal ki_Ts_esum	:	signed(44 downto 0); -- Q24.20
	signal kp_e			:	signed(44 downto 0); -- Q24.20
	signal out_tmp		:	signed(23 + 1 downto 0); -- Q24.0

	signal max_pos_value	: signed(23 + 1 downto 0) := signed'("0011111111111111111111111"); --to_signed((2**(output_bit_width - 1)) - 1, input_bit_width); -- 2^(output_bit_width - 1) - 1 = 2^23-1 = 8388607 for 24bit
	signal max_neg_value	: signed(23 + 1 downto 0) := signed'("1100000000000000000000000"); --to_signed(-((2**(output_bit_width - 1)) - 1), input_bit_width); -- 2^(output_bit_width - 1) - 1 = 2^23-1 = 8388607 for 24bit --signed'("100000000000000000000000000000000000");
	
	-- signals for multiplier
	signal mult_in_a	:	signed(44 downto 0) := (others=>'0'); -- Q24.20, to use 5x 9-bit DSP-slices
	signal mult_in_b	:	signed(23 + 1 downto 0) := (others=>'0'); -- Q24.0
	signal mult_out	:	signed(44 downto 0) := (others=>'0'); -- Q24.20
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= resize(mult_in_a * mult_in_b, 45); -- convert Q49.20 to Q24.20
	end process;

	process(clk)
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and state = 0) then
				-- calculate error = target_val - actual_val
				error <= resize(target_val, error'length) - resize(actual_val, error'length);

				state <= 1;
				
			elsif (state = 1) then
				-- calculate kp * error
				-- load multiplicator for calculating proportional part
				mult_in_a <= resize(p_gain, mult_in_a'length); -- convert Q11.20 to Q24.20
				mult_in_b <= resize(error, mult_in_b'length); -- Q24.0
				
				state <= 2;
				
			elsif (state = 2) then
				-- get output for proportional part
				kp_e <= mult_out; -- Q24.20
				
				-- calculate ki * Ts * error
				-- load multiplicator for calculating esum
				mult_in_a <= resize(i_gain_ts, mult_in_a'length); -- convert Q11.20 to Q24.20
				mult_in_b <= resize(error, mult_in_b'length); -- Q24.0

				state <= 3;

			elsif (state = 3) then
				-- integrate ki*Ts*error
				ki_Ts_esum <= ki_Ts_esum + mult_out; -- Q24.20
				
				-- load multiplicator for calculating feedforward signal
				mult_in_a <= resize(pre_gain, mult_in_a'length); -- convert Q15.0 to Q44.0
				mult_in_b <= resize(target_val, mult_in_b'length); -- Q24.0

				state <= 4;
			
			elsif (state = 4) then
				-- calculate output = feedforward + (kp * e + ki * Ts * esum)
				out_tmp <= resize(shift_right(mult_out, 15), 25) + resize(shift_right(kp_e, 20), 25) + resize(shift_right(ki_Ts_esum, 20), 25); -- convert to Q24.0
				
				state <= 5;
			
			elsif (state = 5) then
				-- clip value to maximum/minimum and output values
				if (out_tmp > max_pos_value) then
					output <= resize(max_pos_value, output'length); -- set to positive fullscale
				elsif (out_tmp < max_neg_value) then
					output <= resize(max_neg_value, output'length); -- set to negative fullscale
				else
					output <= resize(out_tmp, output'length);
				end if;
				
				sync_out <= '1';
				
				state <= 6;
			
			elsif (state = 6) then
				sync_out <= '0';
			
				state <= 0;
			end if;
		end if;
	end process;
end Behavioral;