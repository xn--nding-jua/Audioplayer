-- vu-meter
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file converts 24-bit signed audio-data into a 8-bit unsigned VU-meter between -6dBfs and -78dBfs

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity vu_meter is
	port (
		main_l_in		:	in signed(23 downto 0) := (others=>'0');
		main_r_in		:	in signed(23 downto 0) := (others=>'0');
		main_sub_in		:	in signed(23 downto 0) := (others=>'0');
		sync_in			:	in std_logic := '0';
		
		vu_left_out		:	out std_logic_vector(7 downto 0) := (others=>'0');
		vu_right_out	:	out std_logic_vector(7 downto 0) := (others=>'0');
		vu_sub_out		:	out std_logic_vector(7 downto 0) := (others=>'0')
	);
end vu_meter;

architecture Behavioral of vu_meter is
begin
	process(sync_in)
		variable data_left, data_right, data_sub	:	unsigned(23 downto 0);
	begin
		if rising_edge(sync_in) then
			-- we received a new audio-sample
			
			-- calculate the absolute-value of the samples
			data_left := unsigned(abs(main_l_in));
			data_right := unsigned(abs(main_r_in));
			data_sub := unsigned(abs(main_sub_in));
			
			-- lets take some individual bits and put them together. We want a VU-meter between 0dBfs and -60dBfs. So we take bits 22, 21, 20, 19, 18, 16, 14 and 12 resulting 0dBfs, -6dBfs, -12dBfs, -18dBfs, -24dBfs, -36dBfs, -48dBfs and -60dBfs
			vu_left_out <= std_logic_vector(data_left)(22) & std_logic_vector(data_left)(21) & std_logic_vector(data_left)(20) & std_logic_vector(data_left)(18) & std_logic_vector(data_left)(16) & std_logic_vector(data_left)(14) & std_logic_vector(data_left)(12) & std_logic_vector(data_left)(10);
			vu_right_out <= std_logic_vector(data_right)(22) & std_logic_vector(data_right)(21) & std_logic_vector(data_right)(20) & std_logic_vector(data_right)(18) & std_logic_vector(data_right)(16) & std_logic_vector(data_right)(14) & std_logic_vector(data_right)(12) & std_logic_vector(data_right)(10);
			vu_sub_out <= std_logic_vector(data_sub)(22) & std_logic_vector(data_sub)(21) & std_logic_vector(data_sub)(20) & std_logic_vector(data_sub)(18) & std_logic_vector(data_sub)(16) & std_logic_vector(data_sub)(14) & std_logic_vector(data_sub)(12) & std_logic_vector(data_sub)(10);
		end if;
	end process;
end Behavioral;
