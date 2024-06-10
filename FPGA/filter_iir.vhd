-- Mono IIR audio-filter
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

entity filter_iir is
    generic (
        bit_width : natural range 16 to 48 := 24
    );
    port (
		clk  : in std_logic := '0';

		iir_in : in signed (bit_width - 1 downto 0) := (others=>'0');
		sync_in  : in std_logic := '0';

		iir_out : out signed(bit_width - 1 downto 0) := (others=>'0');
		sync_out : out std_logic := '0';

		busy : out std_logic := '0';

		-- dynamic setting of filter-coefficients
		-- a0, a1, a2, b1, b2 must be multiplied with 2^30 before
		a0	:	in signed(31 downto 0);
		a1 :	in signed(31 downto 0);
		a2 :	in signed(31 downto 0);
		b1	:	in signed(31 downto 0);
		b2 :	in signed(31 downto 0)
	);
end filter_iir;

architecture Behavioral of filter_iir is
	signal state : natural range 0 to 7 := 0;

	--signals for multiplier
	signal mult_in_a : signed(bit_width - 1 downto 0) := (others=>'0'); -- input signal
	signal mult_in_b : signed(31 downto 0) := (others=>'0'); -- coefficient
	signal mult_out : signed(bit_width + 32 - 1 downto 0):= (others=>'0');

	--temp regs and delay regs
	signal temp : signed(bit_width + 8 - 1 downto 0):= (others=>'0');
	signal temp_in, in_z1, in_z2, out_z1, out_z2 : signed(bit_width - 1 downto 0):= (others=>'0');
begin
	-- multiplier
	process(mult_in_a, mult_in_b)
	begin
		mult_out <= mult_in_a * mult_in_b;
	end process;

	process (clk)
	begin
		if (rising_edge(clk)) then    
			-- start process when valid sample arrived
			if (sync_in = '1' and state = 0) then
				-- load multiplier with samplein * a0
				mult_in_a <= shift_left(iir_in, 8);
				temp_in <= shift_left(iir_in, 8);
				mult_in_b <= a0;
				busy <= '1';

				state <= 1;

			elsif (state = 1) then
				--save result of (samplein*a0) to temp and apply right-shift of 30
				--and load multiplier with in_z1 and a1
				temp <= resize(shift_right(mult_out, 30), bit_width + 8);
				mult_in_a <= in_z1;
				mult_in_b <= a1;
				state <= 2;

			elsif (state = 2) then
				--save and sum up result of (in_z1*a1) to temp and apply right-shift of 30
				--and load multiplier with in_z2 and a2
				temp <= temp + resize(shift_right(mult_out, 30), bit_width + 8);
				mult_in_a <= in_z2;
				mult_in_b <= a2;
				state <= 3;
					
			elsif (state = 3) then
				--save and sum up result of (in_z2*a2) to temp and apply right-shift of 30
				-- and load multiplier with out_z1 and b1
				temp <= temp + resize(shift_right(mult_out, 30), bit_width + 8);
				mult_in_a <= out_z1;
				mult_in_b <= b1;
				state <= 4;

			elsif (state = 4) then
				--save and sum up (negative) result of (out_z1*b1) and apply right-shift of 30
				--and load multiplier with out_z2 and b2
				temp <= temp - resize(shift_right(mult_out, 30), bit_width + 8);
				mult_in_a <= out_z2;
				mult_in_b <= b2;
				state <= 5;

			elsif (state = 5) then
				--save and sum up (negative) result of (out_z2*b2) and apply right-shift of 30
				temp <= temp - resize(shift_right(mult_out, 30), bit_width + 8);
				state <= 6;
				  
			elsif (state = 6) then
				--save result to output, save all delay registers, apply ouput-valid signal
				iir_out <= resize(shift_right(temp, 8), bit_width);
				out_z1 <= resize(temp, bit_width);
				out_z2 <= out_z1;       
				in_z2 <= in_z1;
				in_z1 <= temp_in;
				sync_out <= '1';
				state <= 7;
				  
			elsif (state = 7) then
				sync_out <= '0';
				state <= 0;
				busy <= '0';
			end if;      
		end if;
	end process;
end Behavioral;