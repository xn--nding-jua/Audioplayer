-- RS232 command decoder
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a RS232 command-decoder with error-check.
-- It is like a signal demultiplexer, that will convert serial-data to parallel-data

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity rs232_decoder is 
	port
	(
		clk				: in std_logic;
	
		RX_DataReady	: in std_logic;
		RX_Data			: in std_logic_vector(7 downto 0);

		-- main-volumes
		main_volume_l	: out std_logic_vector(7 downto 0);
		main_volume_r	: out std_logic_vector(7 downto 0);
		main_volume_sub: out std_logic_vector(7 downto 0);

		-- channel-volumes
		ch1_volume_l	: out std_logic_vector(7 downto 0);
		ch1_volume_r	: out std_logic_vector(7 downto 0);
		ch2_volume_l	: out std_logic_vector(7 downto 0);
		ch2_volume_r	: out std_logic_vector(7 downto 0);
		ch3_volume_l	: out std_logic_vector(7 downto 0);
		ch3_volume_r	: out std_logic_vector(7 downto 0);
		ch4_volume_l	: out std_logic_vector(7 downto 0);
		ch4_volume_r	: out std_logic_vector(7 downto 0);
		ch5_volume_l	: out std_logic_vector(7 downto 0);
		ch5_volume_r	: out std_logic_vector(7 downto 0);
		ch6_volume_l	: out std_logic_vector(7 downto 0);
		ch6_volume_r	: out std_logic_vector(7 downto 0);
		ch7_volume_l	: out std_logic_vector(7 downto 0);
		ch7_volume_r	: out std_logic_vector(7 downto 0);
		ch8_volume_l	: out std_logic_vector(7 downto 0);
		ch8_volume_r	: out std_logic_vector(7 downto 0);
		ch9_volume_l	: out std_logic_vector(7 downto 0);
		ch9_volume_r	: out std_logic_vector(7 downto 0);
		ch10_volume_l	: out std_logic_vector(7 downto 0);
		ch10_volume_r	: out std_logic_vector(7 downto 0);
		ch11_volume_l	: out std_logic_vector(7 downto 0);
		ch11_volume_r	: out std_logic_vector(7 downto 0);
		ch12_volume_l	: out std_logic_vector(7 downto 0);
		ch12_volume_r	: out std_logic_vector(7 downto 0);
		ch13_volume_l	: out std_logic_vector(7 downto 0);
		ch13_volume_r	: out std_logic_vector(7 downto 0);
		ch14_volume_l	: out std_logic_vector(7 downto 0);
		ch14_volume_r	: out std_logic_vector(7 downto 0);
		ch15_volume_l	: out std_logic_vector(7 downto 0);
		ch15_volume_r	: out std_logic_vector(7 downto 0);
		ch16_volume_l	: out std_logic_vector(7 downto 0);
		ch16_volume_r	: out std_logic_vector(7 downto 0);
		ch17_volume_l	: out std_logic_vector(7 downto 0);
		ch17_volume_r	: out std_logic_vector(7 downto 0);
		ch18_volume_l	: out std_logic_vector(7 downto 0);
		ch18_volume_r	: out std_logic_vector(7 downto 0);
		ch19_volume_l	: out std_logic_vector(7 downto 0);
		ch19_volume_r	: out std_logic_vector(7 downto 0);
		ch20_volume_l	: out std_logic_vector(7 downto 0);
		ch20_volume_r	: out std_logic_vector(7 downto 0);
		ch21_volume_l	: out std_logic_vector(7 downto 0);
		ch21_volume_r	: out std_logic_vector(7 downto 0);
		ch22_volume_l	: out std_logic_vector(7 downto 0);
		ch22_volume_r	: out std_logic_vector(7 downto 0);

		-- LR24 Highpass
		c_a0_hp	: out std_logic_vector(39 downto 0);
		c_a1_hp	: out std_logic_vector(39 downto 0);
		c_a2_hp	: out std_logic_vector(39 downto 0);
		c_a3_hp	: out std_logic_vector(39 downto 0);
		c_a4_hp	: out std_logic_vector(39 downto 0);
		c_b1_hp	: out std_logic_vector(39 downto 0);
		c_b2_hp	: out std_logic_vector(39 downto 0);
		c_b3_hp	: out std_logic_vector(39 downto 0);
		c_b4_hp	: out std_logic_vector(39 downto 0);

		-- LR24 Lowpass
		c_a0_lp	: out std_logic_vector(39 downto 0);
		c_a1_lp	: out std_logic_vector(39 downto 0);
		c_a2_lp	: out std_logic_vector(39 downto 0);
		c_a3_lp	: out std_logic_vector(39 downto 0);
		c_a4_lp	: out std_logic_vector(39 downto 0);
		c_b1_lp	: out std_logic_vector(39 downto 0);
		c_b2_lp	: out std_logic_vector(39 downto 0);
		c_b3_lp	: out std_logic_vector(39 downto 0);
		c_b4_lp	: out std_logic_vector(39 downto 0);
		
		-- Parametric EQ1
		c_a0_peq1 : out std_logic_vector(31 downto 0);
		c_a1_peq1 : out std_logic_vector(31 downto 0);
		c_a2_peq1 : out std_logic_vector(31 downto 0);
		c_b1_peq1 : out std_logic_vector(31 downto 0);
		c_b2_peq1 : out std_logic_vector(31 downto 0);
		
		-- Parametric EQ2
		c_a0_peq2 : out std_logic_vector(31 downto 0);
		c_a1_peq2 : out std_logic_vector(31 downto 0);
		c_a2_peq2 : out std_logic_vector(31 downto 0);
		c_b1_peq2 : out std_logic_vector(31 downto 0);
		c_b2_peq2 : out std_logic_vector(31 downto 0);
		
		-- Parametric EQ3
		c_a0_peq3 : out std_logic_vector(31 downto 0);
		c_a1_peq3 : out std_logic_vector(31 downto 0);
		c_a2_peq3 : out std_logic_vector(31 downto 0);
		c_b1_peq3 : out std_logic_vector(31 downto 0);
		c_b2_peq3 : out std_logic_vector(31 downto 0);

		-- Parametric EQ4
		c_a0_peq4 : out std_logic_vector(31 downto 0);
		c_a1_peq4 : out std_logic_vector(31 downto 0);
		c_a2_peq4 : out std_logic_vector(31 downto 0);
		c_b1_peq4 : out std_logic_vector(31 downto 0);
		c_b2_peq4 : out std_logic_vector(31 downto 0);
		
		-- Parametric EQ5
		c_a0_peq5 : out std_logic_vector(31 downto 0);
		c_a1_peq5 : out std_logic_vector(31 downto 0);
		c_a2_peq5 : out std_logic_vector(31 downto 0);
		c_b1_peq5 : out std_logic_vector(31 downto 0);
		c_b2_peq5 : out std_logic_vector(31 downto 0);
		
		-- Noisegate ADC
		gate1_thrsh	: out std_logic_vector(23 downto 0);
		gate1_gainmin: out std_logic_vector(7 downto 0);
		gate1_c_att	: out std_logic_vector(15 downto 0);
		gate1_hold	: out std_logic_vector(15 downto 0);
		gate1_c_rel	: out std_logic_vector(15 downto 0);

		-- Compressor L/R (Limiter)
		comp1_thrsh	: out std_logic_vector(23 downto 0);
		comp1_ratio	: out std_logic_vector(7 downto 0);
		comp1_makeup	: out std_logic_vector(7 downto 0);
		comp1_c_att	: out std_logic_vector(15 downto 0);
		comp1_hold	: out std_logic_vector(15 downto 0);
		comp1_c_rel	: out std_logic_vector(15 downto 0);

		-- Compressor Sub
		comp2_thrsh	: out std_logic_vector(23 downto 0);
		comp2_ratio	: out std_logic_vector(7 downto 0);
		comp2_makeup	: out std_logic_vector(7 downto 0);
		comp2_c_att	: out std_logic_vector(15 downto 0);
		comp2_hold	: out std_logic_vector(15 downto 0);
		comp2_c_rel	: out std_logic_vector(15 downto 0);

		-- gains for ADCs
		adc1_gain	: out std_logic_vector(7 downto 0);

		-- some auxiliary stuff
		filter_rst	: out std_logic;
		sync_select	: out std_logic_vector(7 downto 0);
		fsclk_select	: out std_logic_vector(7 downto 0)
	);
end entity;

architecture Behavioral of rs232_decoder is
	type t_SM_Decoder is (s_Idle, s_CalcSum, s_Check, s_Read);
	signal s_SM_Decoder 	: t_SM_Decoder := s_Idle;
	
	signal ErrorCheckWord	: unsigned(15 downto 0);
	signal PayloadSum			: unsigned(15 downto 0);
begin
	process (clk)
		variable b1 : std_logic_vector(7 downto 0);	-- "A" = 0x41
		variable b2 : std_logic_vector(7 downto 0);	-- C
		variable b3 : std_logic_vector(7 downto 0);	-- V1 = MSB of payload with signed-bit
		variable b4 : std_logic_vector(7 downto 0);	-- V2
		variable b5 : std_logic_vector(7 downto 0);	-- V3
		variable b6 : std_logic_vector(7 downto 0);	-- V4
		variable b7 : std_logic_vector(7 downto 0);	-- V5
		variable b8 : std_logic_vector(7 downto 0);	-- V6 = LSB of payload
		variable b9 : std_logic_vector(7 downto 0);	-- ErrorCheckWord_MSB
		variable b10 : std_logic_vector(7 downto 0);	-- ErrorCheckWord_LSB
		variable b11 : std_logic_vector(7 downto 0);	-- "E" = 0x45
		
		variable selector: integer range 0 to 255;
	begin
		if (rising_edge(clk)) then
			if (RX_DataReady = '1' and s_SM_Decoder = s_Idle) then
				-- state 0 -> collect data
			
				-- move all bytes forward by one byte and put recent byte at b11
				b1 := b2;
				b2 := b3;
				b3 := b4;
				b4 := b5;
				b5 := b6;
				b6 := b7;
				b7 := b8;
				b8 := b9;
				b9 := b10;
				b10 := b11;
				b11 := RX_Data;

				s_SM_Decoder <= s_CalcSum;
			elsif s_SM_Decoder = s_CalcSum then
				-- build sum of payload and create ErrorCheckWord
				PayloadSum <= unsigned("00000000" & b3) + unsigned("00000000" & b4) + unsigned("00000000" & b5) + unsigned("00000000" & b6) + unsigned("00000000" & b7) + unsigned("00000000" & b8);

				ErrorCheckWord <= unsigned(b9 & b10);

				s_SM_Decoder <= s_Check;
			elsif s_SM_Decoder = s_Check then
				-- check if we have valid payload
				
				if ((unsigned(b1) = 65) and (unsigned(b11) = 69) and (PayloadSum = ErrorCheckWord)) then
					-- we have valid payload-data -> go into next state
					s_SM_Decoder <= s_Read;
				else
					-- unexpected values -> return to idle
					s_SM_Decoder <= s_Idle;
				end if;
			elsif s_SM_Decoder = s_Read then
				-- write data to output

				selector := to_integer(unsigned("0" & b2));
				case selector is
					when 0 =>
						main_volume_l(7 downto 0) <= b8;
					when 1 =>
						main_volume_r(7 downto 0) <= b8;
					when 2 =>
						main_volume_sub(7 downto 0) <= b8;
					when 3 =>
						ch1_volume_l(7 downto 0) <= b8;
					when 4 =>
						ch1_volume_r(7 downto 0) <= b8;
					when 5 =>
						ch2_volume_l(7 downto 0) <= b8;
					when 6 =>
						ch2_volume_r(7 downto 0) <= b8;
					when 7 =>
						ch3_volume_l(7 downto 0) <= b8;
					when 8 =>
						ch3_volume_r(7 downto 0) <= b8;
					when 9 =>
						ch4_volume_l(7 downto 0) <= b8;
					when 10 =>
						ch4_volume_r(7 downto 0) <= b8;
					when 11 =>
						ch5_volume_l(7 downto 0) <= b8;
					when 12 =>
						ch5_volume_r(7 downto 0) <= b8;
					when 13 =>
						ch6_volume_l(7 downto 0) <= b8;
					when 14 =>
						ch6_volume_r(7 downto 0) <= b8;

					when 15 =>
						ch7_volume_l(7 downto 0) <= b8;
					when 16 =>
						ch7_volume_r(7 downto 0) <= b8;
					when 17 =>
						ch8_volume_l(7 downto 0) <= b8;
					when 18 =>
						ch8_volume_r(7 downto 0) <= b8;
					when 19 =>
						ch9_volume_l(7 downto 0) <= b8;
					when 20 =>
						ch9_volume_r(7 downto 0) <= b8;
					when 21 =>
						ch10_volume_l(7 downto 0) <= b8;
					when 22 =>
						ch10_volume_r(7 downto 0) <= b8;
					when 23 =>
						ch11_volume_l(7 downto 0) <= b8;
					when 24 =>
						ch11_volume_r(7 downto 0) <= b8;
					when 25 =>
						ch12_volume_l(7 downto 0) <= b8;
					when 26 =>
						ch12_volume_r(7 downto 0) <= b8;
					when 27 =>
						ch13_volume_l(7 downto 0) <= b8;
					when 28 =>
						ch13_volume_r(7 downto 0) <= b8;
					when 29 =>
						ch14_volume_l(7 downto 0) <= b8;
					when 30 =>
						ch14_volume_r(7 downto 0) <= b8;
					when 31 =>
						ch15_volume_l(7 downto 0) <= b8;
					when 32 =>
						ch15_volume_r(7 downto 0) <= b8;
					when 33 =>
						ch16_volume_l(7 downto 0) <= b8;
					when 34 =>
						ch16_volume_r(7 downto 0) <= b8;
					when 35 =>
						ch17_volume_l(7 downto 0) <= b8;
					when 36 =>
						ch17_volume_r(7 downto 0) <= b8;
					when 37 =>
						ch18_volume_l(7 downto 0) <= b8;
					when 38 =>
						ch18_volume_r(7 downto 0) <= b8;
					when 39 =>
						ch19_volume_l(7 downto 0) <= b8;
					when 40 =>
						ch19_volume_r(7 downto 0) <= b8;
					when 41 =>
						ch20_volume_l(7 downto 0) <= b8;
					when 42 =>
						ch20_volume_r(7 downto 0) <= b8;
					when 43 =>
						ch21_volume_l(7 downto 0) <= b8;
					when 44 =>
						ch21_volume_r(7 downto 0) <= b8;
					when 45 =>
						ch22_volume_l(7 downto 0) <= b8;
					when 46 =>
						ch22_volume_r(7 downto 0) <= b8;

					-- reserve some commands for an optional 32-channel-mixer
					--when 47 =>
					--	ch23_volume_l(7 downto 0) <= b8;
					--when 48 =>
					--	ch23_volume_r(7 downto 0) <= b8;
					--when 49 =>
					--	ch24_volume_l(7 downto 0) <= b8;
					--when 50 =>
					--	ch24_volume_r(7 downto 0) <= b8;
					--when 51 =>
					--	ch25_volume_l(7 downto 0) <= b8;
					--when 52 =>
					--	ch25_volume_r(7 downto 0) <= b8;
					--when 53 =>
					--	ch26_volume_l(7 downto 0) <= b8;
					--when 54 =>
					--	ch26_volume_r(7 downto 0) <= b8;
					--when 55 =>
					--	ch27_volume_l(7 downto 0) <= b8;
					--when 56 =>
					--	ch27_volume_r(7 downto 0) <= b8;
					--when 57 =>
					--	ch28_volume_l(7 downto 0) <= b8;
					--when 58 =>
					--	ch28_volume_r(7 downto 0) <= b8;
					--when 59 =>
					--	ch29_volume_l(7 downto 0) <= b8;
					--when 60 =>
					--	ch29_volume_r(7 downto 0) <= b8;
					--when 61 =>
					--	ch30_volume_l(7 downto 0) <= b8;
					--when 62 =>
					--	ch30_volume_r(7 downto 0) <= b8;
					--when 63 =>
					--	ch31_volume_l(7 downto 0) <= b8;
					--when 64 =>
					--	ch31_volume_r(7 downto 0) <= b8;
					--when 65 =>
					--	ch32_volume_l(7 downto 0) <= b8;
					--when 66 =>
					--	ch32_volume_r(7 downto 0) <= b8;
						
					-- commands from 70 are reserved for coefficients
					when 70 =>
						--c_a0_hp(47 downto 40) <= b3;
						c_a0_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a0_hp(38 downto 32) <= b4(6 downto 0);
						c_a0_hp(31 downto 24) <= b5;
						c_a0_hp(23 downto 16) <= b6;
						c_a0_hp(15 downto 8) <= b7;
						c_a0_hp(7 downto 0) <= b8;
					when 71 =>
						--c_a1_hp(47 downto 40) <= b3;
						c_a1_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a1_hp(38 downto 32) <= b4(6 downto 0);
						c_a1_hp(31 downto 24) <= b5;
						c_a1_hp(23 downto 16) <= b6;
						c_a1_hp(15 downto 8) <= b7;
						c_a1_hp(7 downto 0) <= b8;
					when 72 =>
						--c_a2_hp(47 downto 40) <= b3;
						c_a2_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a2_hp(38 downto 32) <= b4(6 downto 0);
						c_a2_hp(31 downto 24) <= b5;
						c_a2_hp(23 downto 16) <= b6;
						c_a2_hp(15 downto 8) <= b7;
						c_a2_hp(7 downto 0) <= b8;
					when 73 =>
						--c_a3_hp(47 downto 40) <= b3;
						c_a3_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a3_hp(38 downto 32) <= b4(6 downto 0);
						c_a3_hp(31 downto 24) <= b5;
						c_a3_hp(23 downto 16) <= b6;
						c_a3_hp(15 downto 8) <= b7;
						c_a3_hp(7 downto 0) <= b8;
					when 74 =>
						--c_a4_hp(47 downto 40) <= b3;
						c_a4_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a4_hp(38 downto 32) <= b4(6 downto 0);
						c_a4_hp(31 downto 24) <= b5;
						c_a4_hp(23 downto 16) <= b6;
						c_a4_hp(15 downto 8) <= b7;
						c_a4_hp(7 downto 0) <= b8;
					when 75 =>
						--c_b1_hp(47 downto 40) <= b3;
						c_b1_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b1_hp(38 downto 32) <= b4(6 downto 0);
						c_b1_hp(31 downto 24) <= b5;
						c_b1_hp(23 downto 16) <= b6;
						c_b1_hp(15 downto 8) <= b7;
						c_b1_hp(7 downto 0) <= b8;
					when 76 =>
						--c_b2_hp(47 downto 40) <= b3;
						c_b2_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b2_hp(38 downto 32) <= b4(6 downto 0);
						c_b2_hp(31 downto 24) <= b5;
						c_b2_hp(23 downto 16) <= b6;
						c_b2_hp(15 downto 8) <= b7;
						c_b2_hp(7 downto 0) <= b8;
					when 77 =>
						--c_b3_hp(47 downto 40) <= b3;
						c_b3_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b3_hp(38 downto 32) <= b4(6 downto 0);
						c_b3_hp(31 downto 24) <= b5;
						c_b3_hp(23 downto 16) <= b6;
						c_b3_hp(15 downto 8) <= b7;
						c_b3_hp(7 downto 0) <= b8;
					when 78 =>
						--c_b4_hp(47 downto 40) <= b3;
						c_b4_hp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b4_hp(38 downto 32) <= b4(6 downto 0);
						c_b4_hp(31 downto 24) <= b5;
						c_b4_hp(23 downto 16) <= b6;
						c_b4_hp(15 downto 8) <= b7;
						c_b4_hp(7 downto 0) <= b8;
					when 79 =>
						--c_a0_lp(47 downto 40) <= b3;
						c_a0_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a0_lp(38 downto 32) <= b4(6 downto 0);
						c_a0_lp(31 downto 24) <= b5;
						c_a0_lp(23 downto 16) <= b6;
						c_a0_lp(15 downto 8) <= b7;
						c_a0_lp(7 downto 0) <= b8;
					when 80 =>
						--c_a1_lp(47 downto 40) <= b3;
						c_a1_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a1_lp(38 downto 32) <= b4(6 downto 0);
						c_a1_lp(31 downto 24) <= b5;
						c_a1_lp(23 downto 16) <= b6;
						c_a1_lp(15 downto 8) <= b7;
						c_a1_lp(7 downto 0) <= b8;
					when 81 =>
						--c_a2_lp(47 downto 40) <= b3;
						c_a2_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a2_lp(38 downto 32) <= b4(6 downto 0);
						c_a2_lp(31 downto 24) <= b5;
						c_a2_lp(23 downto 16) <= b6;
						c_a2_lp(15 downto 8) <= b7;
						c_a2_lp(7 downto 0) <= b8;
					when 82 =>
						--c_a3_lp(47 downto 40) <= b3;
						c_a3_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a3_lp(38 downto 32) <= b4(6 downto 0);
						c_a3_lp(31 downto 24) <= b5;
						c_a3_lp(23 downto 16) <= b6;
						c_a3_lp(15 downto 8) <= b7;
						c_a3_lp(7 downto 0) <= b8;
					when 83 =>
						--c_a4_lp(47 downto 40) <= b3;
						c_a4_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_a4_lp(38 downto 32) <= b4(6 downto 0);
						c_a4_lp(31 downto 24) <= b5;
						c_a4_lp(23 downto 16) <= b6;
						c_a4_lp(15 downto 8) <= b7;
						c_a4_lp(7 downto 0) <= b8;
					when 84 =>
						--c_b1_lp(47 downto 40) <= b3;
						c_b1_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b1_lp(38 downto 32) <= b4(6 downto 0);
						c_b1_lp(31 downto 24) <= b5;
						c_b1_lp(23 downto 16) <= b6;
						c_b1_lp(15 downto 8) <= b7;
						c_b1_lp(7 downto 0) <= b8;
					when 85 =>
						--c_b2_lp(47 downto 40) <= b3;
						c_b2_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b2_lp(38 downto 32) <= b4(6 downto 0);
						c_b2_lp(31 downto 24) <= b5;
						c_b2_lp(23 downto 16) <= b6;
						c_b2_lp(15 downto 8) <= b7;
						c_b2_lp(7 downto 0) <= b8;
					when 86 =>
						--c_b3_lp(47 downto 40) <= b3;
						c_b3_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b3_lp(38 downto 32) <= b4(6 downto 0);
						c_b3_lp(31 downto 24) <= b5;
						c_b3_lp(23 downto 16) <= b6;
						c_b3_lp(15 downto 8) <= b7;
						c_b3_lp(7 downto 0) <= b8;
					when 87 =>
						--c_b4_lp(47 downto 40) <= b3;
						c_b4_lp(39) <= b3(7); -- copy signed-bit from 48-bit-value
						c_b4_lp(38 downto 32) <= b4(6 downto 0);
						c_b4_lp(31 downto 24) <= b5;
						c_b4_lp(23 downto 16) <= b6;
						c_b4_lp(15 downto 8) <= b7;
						c_b4_lp(7 downto 0) <= b8;
					when 88 =>
						c_a0_peq1(31 downto 24) <= b3;
						c_a0_peq1(23 downto 16) <= b4;
						c_a0_peq1(15 downto 8) <= b5;
						c_a0_peq1(7 downto 0) <= b6;
					when 89 =>
						c_a1_peq1(31 downto 24) <= b3;
						c_a1_peq1(23 downto 16) <= b4;
						c_a1_peq1(15 downto 8) <= b5;
						c_a1_peq1(7 downto 0) <= b6;
					when 90 =>
						c_a2_peq1(31 downto 24) <= b3;
						c_a2_peq1(23 downto 16) <= b4;
						c_a2_peq1(15 downto 8) <= b5;
						c_a2_peq1(7 downto 0) <= b6;
					when 91 =>
						c_b1_peq1(31 downto 24) <= b3;
						c_b1_peq1(23 downto 16) <= b4;
						c_b1_peq1(15 downto 8) <= b5;
						c_b1_peq1(7 downto 0) <= b6;
					when 92 =>
						c_b2_peq1(31 downto 24) <= b3;
						c_b2_peq1(23 downto 16) <= b4;
						c_b2_peq1(15 downto 8) <= b5;
						c_b2_peq1(7 downto 0) <= b6;
					when 93 =>
						c_a0_peq2(31 downto 24) <= b3;
						c_a0_peq2(23 downto 16) <= b4;
						c_a0_peq2(15 downto 8) <= b5;
						c_a0_peq2(7 downto 0) <= b6;
					when 94 =>
						c_a1_peq2(31 downto 24) <= b3;
						c_a1_peq2(23 downto 16) <= b4;
						c_a1_peq2(15 downto 8) <= b5;
						c_a1_peq2(7 downto 0) <= b6;
					when 95 =>
						c_a2_peq2(31 downto 24) <= b3;
						c_a2_peq2(23 downto 16) <= b4;
						c_a2_peq2(15 downto 8) <= b5;
						c_a2_peq2(7 downto 0) <= b6;
					when 96 =>
						c_b1_peq2(31 downto 24) <= b3;
						c_b1_peq2(23 downto 16) <= b4;
						c_b1_peq2(15 downto 8) <= b5;
						c_b1_peq2(7 downto 0) <= b6;
					when 97 =>
						c_b2_peq2(31 downto 24) <= b3;
						c_b2_peq2(23 downto 16) <= b4;
						c_b2_peq2(15 downto 8) <= b5;
						c_b2_peq2(7 downto 0) <= b6;
					when 98 =>
						c_a0_peq3(31 downto 24) <= b3;
						c_a0_peq3(23 downto 16) <= b4;
						c_a0_peq3(15 downto 8) <= b5;
						c_a0_peq3(7 downto 0) <= b6;
					when 99 =>
						c_a1_peq3(31 downto 24) <= b3;
						c_a1_peq3(23 downto 16) <= b4;
						c_a1_peq3(15 downto 8) <= b5;
						c_a1_peq3(7 downto 0) <= b6;
					when 100 =>
						c_a2_peq3(31 downto 24) <= b3;
						c_a2_peq3(23 downto 16) <= b4;
						c_a2_peq3(15 downto 8) <= b5;
						c_a2_peq3(7 downto 0) <= b6;
					when 101 =>
						c_b1_peq3(31 downto 24) <= b3;
						c_b1_peq3(23 downto 16) <= b4;
						c_b1_peq3(15 downto 8) <= b5;
						c_b1_peq3(7 downto 0) <= b6;
					when 102 =>
						c_b2_peq3(31 downto 24) <= b3;
						c_b2_peq3(23 downto 16) <= b4;
						c_b2_peq3(15 downto 8) <= b5;
						c_b2_peq3(7 downto 0) <= b6;
					when 103 =>
						c_a0_peq4(31 downto 24) <= b3;
						c_a0_peq4(23 downto 16) <= b4;
						c_a0_peq4(15 downto 8) <= b5;
						c_a0_peq4(7 downto 0) <= b6;
					when 104 =>
						c_a1_peq4(31 downto 24) <= b3;
						c_a1_peq4(23 downto 16) <= b4;
						c_a1_peq4(15 downto 8) <= b5;
						c_a1_peq4(7 downto 0) <= b6;
					when 105 =>
						c_a2_peq4(31 downto 24) <= b3;
						c_a2_peq4(23 downto 16) <= b4;
						c_a2_peq4(15 downto 8) <= b5;
						c_a2_peq4(7 downto 0) <= b6;
					when 106 =>
						c_b1_peq4(31 downto 24) <= b3;
						c_b1_peq4(23 downto 16) <= b4;
						c_b1_peq4(15 downto 8) <= b5;
						c_b1_peq4(7 downto 0) <= b6;
					when 107 =>
						c_b2_peq4(31 downto 24) <= b3;
						c_b2_peq4(23 downto 16) <= b4;
						c_b2_peq4(15 downto 8) <= b5;
						c_b2_peq4(7 downto 0) <= b6;
					when 108 =>
						c_a0_peq5(31 downto 24) <= b3;
						c_a0_peq5(23 downto 16) <= b4;
						c_a0_peq5(15 downto 8) <= b5;
						c_a0_peq5(7 downto 0) <= b6;
					when 109 =>
						c_a1_peq5(31 downto 24) <= b3;
						c_a1_peq5(23 downto 16) <= b4;
						c_a1_peq5(15 downto 8) <= b5;
						c_a1_peq5(7 downto 0) <= b6;
					when 110 =>
						c_a2_peq5(31 downto 24) <= b3;
						c_a2_peq5(23 downto 16) <= b4;
						c_a2_peq5(15 downto 8) <= b5;
						c_a2_peq5(7 downto 0) <= b6;
					when 111 =>
						c_b1_peq5(31 downto 24) <= b3;
						c_b1_peq5(23 downto 16) <= b4;
						c_b1_peq5(15 downto 8) <= b5;
						c_b1_peq5(7 downto 0) <= b6;
					when 112 =>
						c_b2_peq5(31 downto 24) <= b3;
						c_b2_peq5(23 downto 16) <= b4;
						c_b2_peq5(15 downto 8) <= b5;
						c_b2_peq5(7 downto 0) <= b6;

					when 150 =>
						--gate1_thrsh(31 downto 24) <= b3;
						gate1_thrsh(23 downto 16) <= b4;
						gate1_thrsh(15 downto 8) <= b5;
						gate1_thrsh(7 downto 0) <= b6;
					when 151 =>
						--gate1_gainmin(15 downto 8) <= b3;
						gate1_gainmin(7 downto 0) <= b4;
					when 152 =>
						gate1_c_att(15 downto 8) <= b3;
						gate1_c_att(7 downto 0) <= b4;
					when 153 =>
						gate1_hold(15 downto 8) <= b3;
						gate1_hold(7 downto 0) <= b4;
					when 154 =>
						gate1_c_rel(15 downto 8) <= b3;
						gate1_c_rel(7 downto 0) <= b4;
						
					when 175 =>
						--comp1_thrsh(31 downto 24) <= b3;
						comp1_thrsh(23 downto 16) <= b4;
						comp1_thrsh(15 downto 8) <= b5;
						comp1_thrsh(7 downto 0) <= b6;
					when 176 =>
						--comp1_ratio(15 downto 8) <= b3;
						comp1_ratio(7 downto 0) <= b4;
					when 177 =>
						--comp1_makeup(15 downto 8) <= b3;
						comp1_makeup(7 downto 0) <= b4;
					when 178 =>
						comp1_c_att(15 downto 8) <= b3;
						comp1_c_att(7 downto 0) <= b4;
					when 179 =>
						comp1_hold(15 downto 8) <= b3;
						comp1_hold(7 downto 0) <= b4;
					when 180 =>
						comp1_c_rel(15 downto 8) <= b3;
						comp1_c_rel(7 downto 0) <= b4;
					when 181 =>
						--comp2_thrsh(31 downto 24) <= b3;
						comp2_thrsh(23 downto 16) <= b4;
						comp2_thrsh(15 downto 8) <= b5;
						comp2_thrsh(7 downto 0) <= b6;
					when 182 =>
						--comp2_ratio(15 downto 8) <= b3;
						comp2_ratio(7 downto 0) <= b4;
					when 183 =>
						--comp2_makeup(15 downto 8) <= b3;
						comp2_makeup(7 downto 0) <= b4;
					when 184 =>
						comp2_c_att(15 downto 8) <= b3;
						comp2_c_att(7 downto 0) <= b4;
					when 185 =>
						comp2_hold(15 downto 8) <= b3;
						comp2_hold(7 downto 0) <= b4;
					when 186 =>
						comp2_c_rel(15 downto 8) <= b3;
						comp2_c_rel(7 downto 0) <= b4;
					
					when 200 =>
						filter_rst <= '1';
					when 201 =>
						filter_rst <= '0';
					when 202 =>
						--sync_select(15 downto 8) <= b3;
						sync_select(7 downto 0) <= b4;
					when 203 =>
						--fs_select(15 downto 8) <= b3;
						fsclk_select(7 downto 0) <= b4;

					when 220 =>
						--adc1_gain(15 downto 8) <= b3;
						adc1_gain(7 downto 0) <= b4;
					when others =>
				end case;

				-- we are done -> return to state 0
				s_SM_Decoder <= s_Idle;
			end if;
		end if;
	end process;
end Behavioral;