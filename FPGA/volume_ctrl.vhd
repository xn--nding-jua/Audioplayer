-- Volume Control for single single audio-stream
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a simple volume controller for mono audio

library ieee;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all; --This allows the use of the signed variable types. The complete model using this approach is much more compact and is shown below:

entity volume_ctrl is
	generic(top : natural := 23);
port (
	signal_in 	: in signed(top downto 0);
	volume 		: in signed(top downto 0); -- value is between 0 and 256
	signal_out 	: out signed(top downto 0)
	);
end entity volume_ctrl;

architecture behavior of volume_ctrl is
	signal signal_x_volume : signed((2*top)+1 downto 0); -- we need double the bits for the multiplication
begin
	volume_control : process (signal_in, volume)
	begin
		signal_x_volume <= signal_in * volume; -- do multiplication signal x volume[0-256]
	end process volume_control;
	
	-- now we have to divide the multiplicated signal by 256.
	-- We can bitshift this by 8 to the right
	signal_out <= resize(shift_right(signal_x_volume, 8), top+1);
end architecture behavior;
