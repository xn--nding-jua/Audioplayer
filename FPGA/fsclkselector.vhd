-- SyncInput Selector
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a input-selector for digital audio-input-sync-sources

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity fsclkselector is
	port (
		fsclk_selection	:	in integer range 0 to 255;

		fsclk_in1			:	in std_logic := '0';
		fsclk_in2			:	in std_logic := '0';
		fsclk_in3			:	in std_logic := '0';
		
		fsclk_out			:	out std_logic := '0'
	);
end fsclkselector;

architecture rtl of fsclkselector is
begin
	process(fsclk_in1, fsclk_in2, fsclk_in3)
	begin
		case fsclk_selection is
			when 0 =>
				fsclk_out <= fsclk_in1; -- 44100
			when 1 =>
				fsclk_out <= fsclk_in2; -- 48000
			when 2 =>
				fsclk_out <= fsclk_in3; -- 96000
			when others =>
				fsclk_out <= fsclk_in2; -- 48kHz as standard
		end case;
	end process;
end rtl;
