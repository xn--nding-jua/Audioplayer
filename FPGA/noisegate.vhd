-- Noisegate Filter
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a noisegate that will set the output to a specific
-- gain-value when the input-signal falls below a specific threshold.
-- The gain-change will be filtered by a first-order low-pass filter
-- for which two coefficients (for attack and release) have to be set

-- Original Source: https://github.com/YetAnotherElectronicsChannel/FPGA-Audio-IIR
-- Online-Calculation of Filter-Coefficients: https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity noisegate is
	generic (
		bit_width		:	natural range 16 to 48 := 24
	);
	port (
		clk				:	in std_logic := '0';

		sample_in		:	in signed(bit_width - 1 downto 0) := (others=>'0');
		sync_in			:	in std_logic := '0';
		threshold		:	in unsigned(bit_width - 1 downto 0) := (others=>'0'); -- threshold with bitdepth of sample, that will be used to check the audio-level (between 0 and 8388607)
		gain_min			:	in unsigned(7 downto 0) := (others=>'0'); -- minimum gain the user will allow (48dB-range (6dB/bit * 8bit), between 0 and 255)
		coeff_attack	:	in signed(15 downto 0); -- Q15-format, 1 bit sign, 0 bit for integer-part, 15 bit for fraction-part
		hold_ticks		:	in unsigned(15 downto 0) := (others=>'0'); -- 16 bit value for hold_counter = 0...65535/48000 [0...1365ms]
		coeff_release	:	in signed(15 downto 0); -- Q15-format, 1 bit sign, 0 bit for integer-part, 15 bit for fraction-part
		
		sample_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
		sync_out			:	out std_logic := '0';
		gate_closed_out	:	out std_logic := '0'
	);
end noisegate;

architecture rtl of noisegate is
	-- signals for the state-machines
	signal s_SM_Main	:	natural range 0 to 6 := 0;
	type t_SM_Effect is (s_GateClosed, s_Attack, s_GateOpen, s_Hold, s_GateClosing);
	signal s_SM_Effect 	: t_SM_Effect := s_Hold; -- start in hold mode to get release in next cycle

	-- signals for processing
	signal sample		:	signed(bit_width - 1 downto 0) := (others=>'0');
	signal coeff		:	signed(15 downto 0); -- Q15-format, 1 bit for sign, 0 bit for integer-part, 15 bit for fraction-part
	
	signal gain_set	:	unsigned(7 downto 0) := (others=>'0'); -- desired value that is calculated
	signal gain			:	unsigned(22 downto 0) := (others=>'0'); -- unsigned Q15-format, that will be used for current gain and holds value as gain_z1 for next step
	
	signal hold_counter	:	unsigned(15 downto 0) := (others=>'0'); -- unsigned 16 bit counter for hold_counter = 0...1365ms

	--signals for multiplier
	signal mult_in_a	:	signed(bit_width - 1 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(15 downto 0) := (others=>'0');
	signal mult_out	:	signed((bit_width + 16) - 1 downto 0) := (others=>'0');
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;
	
	process(clk)
		variable gate_closed : std_logic := '0';
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and s_SM_Main = 0) then
				-- we are receiving a new sample
				
				-- copy sample to internal signal
				sample <= sample_in;

				-- check if the effect is triggered
				if (unsigned(abs(sample_in)) > threshold) then
					gate_closed := '0';
				else
					gate_closed := '1';
				end if;

				-- check state of Effect-Statemachine
				if (s_SM_Effect = s_GateClosed) then
					-- check level for entering attack-state
					if (gate_closed = '0') then
						s_SM_Effect <= s_Attack;
					end if;
				elsif (s_SM_Effect = s_Attack) then
					-- gate is opening
					
					-- set target gain-level to maximum
					-- set attack-coefficient for LP-filter
					-- gain itself will be set via low-pass-filter down below
					gain_set <= to_unsigned(255, gain_set'length);
					coeff <= coeff_attack;
					
					-- go directly into on-state
					-- we could add a attack-state-counter here, but the wait-time
					-- will depend on coefficients, so keep it simple
					s_SM_Effect <= s_GateOpen;
				elsif (s_SM_Effect = s_GateOpen) then
					if (gate_closed = '1') then
						-- we are above the threshold
						
						-- load counter and enter release-hold-state
						hold_counter <= hold_ticks;
						s_SM_Effect <= s_Hold;
					end if;
				elsif (s_SM_Effect = s_Hold) then
					-- gate is in hold-state before release
					
					if (gate_closed = '0') then
						-- value is below threshold again -> re-enter on-State
						s_SM_Effect <= s_GateOpen;
					else
						-- we are above the threshold
						
						-- check hold_counter
						if (hold_counter = 0) then
							-- we reached end of hold -> enter release-state
							s_SM_Effect <= s_GateClosing;
						else
							-- we still have to stay in hold
							-- decrement hold-counter
							hold_counter <= hold_counter - 1;
						end if;
					end if;
				elsif (s_SM_Effect = s_GateClosing) then
					-- set target gain-level to minimum
					-- set release-coefficient for LP-filter
					-- gain itself will be set via low-pass-filter down below
					gain_set <= gain_min;
					coeff <= coeff_release;
					
					-- go directly into idle-state (TODO: we could add a release-state-counter here, but the wait-time will depend on coefficients)
					s_SM_Effect <= s_GateClosed;
				end if;
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 1) then
				-- now calculate low-pass-filter: gain = coeff * gain_z1 - coeff * gain + gain

				-- first, calculate the front part of filter-equation: part1 = coeff * gain_z1

				-- load multiplier
				mult_in_a <= resize(signed("0" & std_logic_vector(gain)), mult_in_a'length); -- gain-value (from last sample = z^-1) is in unsigned Q8.15-format -> convert to signed Q8.15
				mult_in_b <= coeff; -- coefficient in Q0.15-format
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 2) then
				-- store first part and calculate next part of filter: coeff * gain
			
				-- convert resulting Q8.30 back to unsigned Q8.15 by removing/ignoring least-significant 15 bits
				gain <= unsigned(std_logic_vector(mult_out)(22+15 downto 0+15)); -- we ignore the signed-bit here, as gain is unsigned
				
				-- load multiplier
				mult_in_a <= resize(signed("0" & std_logic_vector(gain_set)), mult_in_a'length); -- desired gain is in unsigned Q8.0-format -> convert to signed Q8.0
				mult_in_b <= coeff; -- coefficient in Q0.15-format
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 3) then
				-- calculate full value: gain = coeff * gain_z1 - coeff * gain + gain 
				
				-- convert gain_set from unsigned Q8.0 to unsigned Q8.15
				-- convert multiplication-result (alpha*gain_set) from signed Q8.15 to unsigned Q8.15
				-- calc: (alpha*gain) + (gain_set - alpha*gain_set)
				
				gain <= gain + (unsigned(std_logic_vector(gain_set) & "000000000000000") - unsigned(std_logic_vector(mult_out)(22 downto 0)));
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 4) then
				-- calculate the output signal
				
				-- load multiplier
				mult_in_a <= sample; -- audio-sample
				mult_in_b <= signed("0" & std_logic_vector(gain)(22 downto 8)); -- convert current gain-value from Q8.15 into Q8.7-format
				
				s_SM_Main <= s_SM_Main + 1;
			elsif (s_SM_Main = 5) then
				-- convert resulting signed Q8.7 back to Q8.0 and set output, divide by 256 and output signal
				sample_out <= resize(shift_right(mult_out, 8 + 7) , bit_width);

				-- indicate if gate is closed
				if (shift_right(gain, 15) < 250) then
					gate_closed_out <= '1';
				else
					gate_closed_out <= '0';
				end if;

				sync_out <= '1';
				
				s_SM_Main <= s_SM_Main + 1;
			elsif (s_SM_Main = 6) then
				-- cleanup and return to state 0
				sync_out <= '0';
				
				s_SM_Main <= 0;
			end if;
		end if;
	end process;
end rtl;
