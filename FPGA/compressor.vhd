-- Compressor/Limiter Filter
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a compressor/limiter that will set the output to a specific
-- maximum gain-value depending on a specific maximum threshold.
-- The gain-change will be filtered by a first-order low-pass filter
-- for which two coefficients (for attack and release) have to be set

-- Original Source: https://github.com/YetAnotherElectronicsChannel/FPGA-Audio-IIR
-- Online-Calculation of Filter-Coefficients: https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity compressor is
	generic (
		bit_width		:	natural range 16 to 48 := 24;
		threshold_bit_width	:	natural range 16 to 48 := 24
	);
	port (
		clk				:	in std_logic := '0';

		sample_in		:	in signed(bit_width - 1 downto 0) := (others=>'0');
		sync_in			:	in std_logic := '0';
		threshold		:	in unsigned(threshold_bit_width - 1 downto 0) := (others=>'0'); -- threshold with bitdepth of sample, that will be used to check the audio-level (between 0 and 8388608)
		ratio				:	in unsigned(7 downto 0) := (others=>'0'); -- ratio of gain-reduction (0=1:1, 1=2:1, 2=4:1, 3=8:1, 4=16:1, 5=32:1, 6=64:1, ...)
		makeup			:	in unsigned(7 downto 0) := (others=>'0'); -- makeup-gain. (0=x1, 1=x2, 2=x4, 3=x8, 4=x16, 5=x32, 6=x64, 7=x128, 8=x256, ...)
		coeff_attack	:	in signed(15 downto 0); -- Q15-format, 1 bit sign, 0 bit for integer-part, 15 bit for fraction-part
		hold_ticks		:	in unsigned(15 downto 0) := (others=>'0'); -- 16 bit value for hold_counter = 0...65535/48000 [0...1365ms]
		coeff_release	:	in signed(15 downto 0); -- Q15-format, 1 bit sign, 0 bit for integer-part, 15 bit for fraction-part
		
		sample_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
		sync_out			:	out std_logic := '0';
		comp_out			:	out std_logic := '0'
	);
end compressor;

architecture rtl of compressor is
	-- signals for the state-machines
	signal s_SM_Main	:	natural range 0 to 7 := 0;
	type t_SM_Effect is (s_Idle, s_Attack, s_Active, s_Hold, s_Release);
	signal s_SM_Effect 	: t_SM_Effect := s_Release; -- start in release mode

	-- signals for processing
	signal sample		:	signed(bit_width - 1 downto 0) := (others=>'0');
	signal coeff		:	signed(15 downto 0); -- Q15-format, 1 bit for sign, 0 bit for integer-part, 15 bit for fraction-part
	
	signal gain_set	:	unsigned(bit_width - 1 downto 0) := (others=>'0'); -- target-value that is calculated
	signal gain			:	unsigned(22 downto 0) := (others=>'0'); -- unsigned Q15-format, that will be used for current gain and holds value as gain_z1 for next step
	
	signal hold_counter	:	unsigned(15 downto 0) := (others=>'0'); -- unsigned 16 bit counter for hold_counter = 0...1365ms

	--signals for multiplier
	signal mult_in_a	:	signed(bit_width - 1 downto 0) := (others=>'0');
	signal mult_in_b	:	signed(15 downto 0) := (others=>'0');
	signal mult_out	:	signed((bit_width + 16) - 1 downto 0) := (others=>'0');

	-- signals for divider
	signal div_start		:	std_logic;
	signal div_dividend	:	unsigned(bit_width - 1 downto 0) := (others=>'0'); -- temporary value for gain_set
	signal div_busy		:	std_logic;
	signal div_quotient	:	unsigned(bit_width - 1 downto 0) := (others=>'0');
	-- copy inputs/outputs from Divider-entity
	component divider is
		generic (
			bit_width	: natural range 4 to 48 := 24
		);
		port (
			clk			: in   std_logic;
			start			: in   std_logic;
			dividend		: in   unsigned(bit_width - 1 downto 0);
			divisor		: in   unsigned(bit_width - 1 downto 0);
			quotient		: out  unsigned(bit_width - 1 downto 0);
			remainder	: out  unsigned(bit_width - 1 downto 0);
			busy			: out  std_logic
		);
	end component;
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;
	
	-- divider
	div : divider
		generic map(
			bit_width => bit_width
		)
		port map(
			clk => clk,
			start => div_start,
			dividend => div_dividend,
			divisor => unsigned(sample), -- sample contains only the abs()-value of sample. So its OK to cast to unsigned without additional abs()
			quotient => div_quotient,
			remainder => open,
			busy => div_busy
		);

	process(clk)
		variable effect_triggered : std_logic := '0';
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and s_SM_Main = 0) then
				-- we are receiving a new sample
				
				-- copy sample to internal signal
				sample <= sample_in;
				
				-- check if the effect is triggered
				if (unsigned(abs(sample_in)) > threshold) then
					effect_triggered := '1';
				else
					effect_triggered := '0';
				end if;

				-- precalculate a temporary value for gain_set
				-- overshoot = sample - threshold
				-- output = overshoot / ratio + threshold
				-- gain = (output / sample) * 256
				-- we will calculate the divisor including factor of 256:
				div_dividend <= shift_left(resize((shift_right(unsigned(abs(sample_in)) - threshold, to_integer(ratio)) + threshold), div_dividend'length), 8) - 5; -- division has limited precision, so we will keep a larger distance away from 256 to mitigate overflow
				-- sample is already mapped to divider as divisor
				
				-- start divider
				div_start <= '1';

				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 1 and div_busy = '0') then
				-- set divider-start to 0 for next cycle
				div_start <= '0';

				-- check state of Effect-Statemachine
				if (s_SM_Effect = s_Idle) then
					-- Effect is idle (off)
					
					-- check level for entering attack-state
					if (effect_triggered = '1') then
						s_SM_Effect <= s_Attack;
					end if;
				elsif (s_SM_Effect = s_Attack) then
					-- Effect entered attack-state
					
					-- we are limiting with a specific ratio: 0=1:1, 1=2:1, 2=4:1, 3=8:1, 4=16:1, 5=32:1, 6=64:1, ..., 24=oo:1=Limiter
					-- general equation is: output = (input - threshold)/ratio + threshold
					-- equation for desired gain is: gain = output / input

					--gain_set <= to_unsigned(32, gain_set'length); -- for debugging: set volume during compression to 12.5% (=32/256)
					gain_set <= div_quotient; -- take the desired gain from division
					coeff <= coeff_attack;
					
					-- go directly into on-state
					-- we could add a attack-state-counter here, but the wait-time
					-- will depend on coefficients, so keep it simple
					s_SM_Effect <= s_Active;
				elsif (s_SM_Effect = s_Active) then
					-- Effect is in on-state

					if (effect_triggered = '1') then
						-- value is still above threshold -> update gain if below last gain
						if (div_quotient < gain_set) then
							-- we have to reduce gain even more
							gain_set <= div_quotient; -- take the desired gain from division
						end if;
					else
						-- we are below the threshold
						
						-- load counter and enter release-hold-state
						hold_counter <= hold_ticks;
						s_SM_Effect <= s_Hold;
					end if;
				elsif (s_SM_Effect = s_Hold) then
					-- Effect is in hold-state before release
					if (effect_triggered = '1') then
						-- value is above threshold again -> re-enter on-State
						s_SM_Effect <= s_Active;
					else
						-- we are below the threshold
						
						-- check hold_counter
						if (hold_counter = 0) then
							-- we reached end of hold -> enter release-state
							s_SM_Effect <= s_Release;
						else
							-- we still have to stay in hold
							-- decrement hold-counter
							hold_counter <= hold_counter - 1;
						end if;
					end if;
				elsif (s_SM_Effect = s_Release) then
					-- Effect entered release-state
					
					-- set desired gain-level to 100%
					-- set release-coefficient for LP-filter
					-- gain itself will be set via low-pass-filter down below
					gain_set <= to_unsigned(255, gain_set'length);
					coeff <= coeff_release;
					
					-- go directly into idle-state (TODO: we could add a release-state-counter here, but the wait-time will depend on coefficients)
					s_SM_Effect <= s_Idle;
				end if;
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 2) then
				-- now calculate low-pass-filter: gain = coeff * gain_z1 - coeff * gain + gain

				-- first, calculate the front part of filter-equation: part1 = coeff * gain_z1

				-- load multiplier
				mult_in_a <= resize(signed("0" & std_logic_vector(gain)), mult_in_a'length); -- gain-value (from last sample = z^-1) is in unsigned Q8.15-format -> convert to signed Q8.15
				mult_in_b <= coeff; -- coefficient in Q0.15-format
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 3) then
				-- store first part and calculate next part of filter: coeff * gain
			
				-- convert resulting Q8.30 back to unsigned Q8.15 by removing/ignoring least-significant 15 bits
				gain <= unsigned(std_logic_vector(mult_out)(22+15 downto 0+15)); -- we ignore the signed-bit here, as gain is unsigned
				
				-- load multiplier
				mult_in_a <= resize(signed("0" & std_logic_vector(gain_set)), mult_in_a'length); -- desired gain is in unsigned Q8.0-format -> convert to signed Q8.0
				mult_in_b <= coeff; -- coefficient in Q0.15-format
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 4) then
				-- calculate full value: gain = coeff * gain_z1 - coeff * gain + gain 
				
				-- convert gain_set from unsigned Q8.0 to unsigned Q8.15
				-- convert multiplication-result (alpha*gain_set) from signed Q8.15 to unsigned Q8.15
				-- calc: (alpha*gain) + (gain_set - alpha*gain_set)
				
				gain <= gain + (resize(unsigned(std_logic_vector(gain_set) & "000000000000000"), gain'length) - unsigned(std_logic_vector(mult_out)(22 downto 0)));
				
				s_SM_Main <= s_SM_Main + 1; -- go into next state
			elsif (s_SM_Main = 5) then
				-- calculate the output signal
				
				-- load multiplier
				mult_in_a <= sample; -- audio-sample
				mult_in_b <= signed("0" & std_logic_vector(gain)(22 downto 8)); -- convert current gain-value from Q8.15 into Q8.7-format
				
				s_SM_Main <= s_SM_Main + 1;
			elsif (s_SM_Main = 6) then
				-- convert resulting signed Q8.7 back to Q8.0 and set output, divide by 256 and output signal
				-- applying makeup-gain by bitshift to the left again
				sample_out <= resize(shift_right(mult_out, 8 + 7 - to_integer(makeup)) , bit_width);

				-- indicate, if we are compressing
				if (shift_right(gain, 15) < 250) then
					comp_out <= '1';
				else
					comp_out <= '0';
				end if;

				sync_out <= '1';
				
				s_SM_Main <= s_SM_Main + 1;
			elsif (s_SM_Main = 7) then
				-- cleanup and return to state 0
				sync_out <= '0';
				
				s_SM_Main <= 0;
			end if;
		end if;
	end process;
end rtl;
