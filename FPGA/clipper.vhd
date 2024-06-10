-- Audio-clipper
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains an audio-clipper, that will prevent the audio-signal to flip all bits on exceeding the maximum audio-bits
-- Furthermore, it will indicate which channel is clipping. You can use an external OR-gate to combine all clip-signals
-- and use it to show the user a clip-LED

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity clipper is
   generic (
      -- Registers width, determines minimal baud speed of input AES3 at given master clock frequency
      input_bit_width :	natural range 16 to 48 := 32;
		output_bit_width	: natural range 16 to 48 := 24
   );
	port (
		clk				:	in std_logic := '0';
				
		main_l_in		:	in signed(input_bit_width - 1 downto 0) := (others=>'0');
		main_r_in		:	in signed(input_bit_width - 1 downto 0) := (others=>'0');
		main_sub_in		:	in signed(input_bit_width - 1 downto 0) := (others=>'0');
		sync_in			:	in std_logic := '0';
		clip_rst			:	in std_logic := '0';
		
		main_l_out		:	out signed(output_bit_width - 1 downto 0) := (others=>'0');
		main_r_out		:	out signed(output_bit_width - 1 downto 0) := (others=>'0');
		main_sub_out	:	out signed(output_bit_width - 1 downto 0) := (others=>'0');
		sync_out			:	out std_logic := '0';
		clip_l_out		:	out std_logic := '0';
		clip_r_out		:	out std_logic := '0';
		clip_sub_out	:	out std_logic := '0'
		);
end clipper;

architecture Behavioral of clipper is
	signal max_pos_value	: signed(input_bit_width - 1 downto 0) := signed'("000000000000011000000000000000000000"); --to_signed((2**(output_bit_width - 1)) - 1, input_bit_width); -- 2^(output_bit_width - 1) - 1 = 2^23-1 = 8388607 for 24bit
	signal max_neg_value	: signed(input_bit_width - 1 downto 0) := signed'("111111111111100111111111111111111111"); --to_signed(-((2**(output_bit_width - 1)) - 1), input_bit_width); -- 2^(output_bit_width - 1) - 1 = 2^23-1 = 8388607 for 24bit --signed'("100000000000000000000000000000000000");
	signal s_clip_rst : std_logic;
begin
	process(clk)
	begin
		if rising_edge(clk) then
			-- saturate input-signals to output-bit-width
			if (clip_rst = '1') then
				s_clip_rst <= '1';
			end if;
		
			if (sync_in = '1') then
				if (main_l_in > max_pos_value) then
					main_l_out <= resize(max_pos_value, main_l_out'length); -- set to positive fullscale
					clip_l_out <= '1';
				elsif (main_l_in < max_neg_value) then
					main_l_out <= resize(max_neg_value, main_l_out'length); -- set to negative fullscale
					clip_l_out <= '1';
				else
					main_l_out <= resize(main_l_in, main_l_out'length);
					if (s_clip_rst = '1') then
						clip_l_out <= '0';
					end if;
				end if;
			
				if (main_r_in > max_pos_value) then
					main_r_out <= resize(max_pos_value, main_r_out'length); -- set to positive fullscale
					clip_r_out <= '1';
				elsif (main_r_in < max_neg_value) then
					main_r_out <= resize(max_neg_value, main_r_out'length); -- set to negative fullscale
					clip_r_out <= '1';
				else
					main_r_out <= resize(main_r_in, main_r_out'length);
					if (s_clip_rst = '1') then
						clip_r_out <= '0';
					end if;
				end if;
			
				if (main_sub_in > max_pos_value) then
					main_sub_out <= resize(max_pos_value, main_sub_out'length); -- set to positive fullscale
					clip_sub_out <= '1';
				elsif (main_sub_in < max_neg_value) then
					main_sub_out <= resize(max_neg_value, main_sub_out'length); -- set to negative fullscale
					clip_sub_out <= '1';
				else
					main_sub_out <= resize(main_sub_in, main_sub_out'length);
					if (s_clip_rst = '1') then
						clip_sub_out <= '0';
					end if;
				end if;
				
				s_clip_rst <= '0';
			end if;
			
			sync_out <= sync_in;
		end if;
	end process;
end Behavioral;
