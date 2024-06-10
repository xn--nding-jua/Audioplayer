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

entity syncselector is
	port (
		selection	:	in integer range 0 to 255;

		sync_in1			:	in std_logic := '0';
		sync_in2			:	in std_logic := '0';
		sync_in3			:	in std_logic := '0';
		sync_in4			:	in std_logic := '0';
		sync_in5			:	in std_logic := '0';
		sync_in6			:	in std_logic := '0';
		
		sync_out			:	out std_logic := '0'
	);
end syncselector;

architecture Behavioral of syncselector is
begin
	process(sync_in1, sync_in2, sync_in3, sync_in4, sync_in5, sync_in6)
	begin
		case selection is
			when 0 =>
				sync_out <= sync_in1; -- ADC
			when 1 =>
				sync_out <= sync_in2; -- SDCard
			when 2 =>
				sync_out <= sync_in3; -- Bluetooth
			when 3 =>
				sync_out <= sync_in4; -- AES/SPDIF
			when 4 =>
				sync_out <= sync_in5; -- AES/Ultranet 1-8
			when 5 =>
				sync_out <= sync_in6; -- AES/Ultranet 9-16
			when others =>
				sync_out <= sync_in2; -- SDCard as standard
		end case;
	end process;
end Behavioral;
