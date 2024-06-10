-- Three-way signal
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file creates a mono-audio-signal out of a stereo-audio-signal

library ieee;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all; --This allows the use of the signed variable types. The complete model using this approach is much more compact and is shown below:

entity subwoofer_sum is
   generic (
      -- Registers width, determines minimal baud speed of input AES3 at given master clock frequency
      bit_width	:	natural range 16 to 48 := 24
   );
	port (
		clk			:	in std_logic := '0';

		left_in 		:	in signed(bit_width - 1 downto 0) := (others=>'0');
		right_in 	:	in signed(bit_width - 1 downto 0) := (others=>'0');
		sync_in		:	in std_logic := '0';

		left_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
		right_out	:	out signed(bit_width - 1 downto 0) := (others=>'0');
		sub_out		:	out signed(bit_width - 1 downto 0) := (others=>'0');
		sync_out		:	out std_logic := '0'
		);
end entity subwoofer_sum;

architecture behavior of subwoofer_sum is
begin
	process(clk)
	begin
		if rising_edge(clk) then
			if (sync_in = '1') then
				left_out <= left_in;
				right_out <= right_in;
				
				-- just calc the sum and ignore overflow. We are using 36 or 48 bit, so we have at least 72dB headroom
				sub_out <= (left_in + right_in);
			end if;
		end if;

		sync_out <= sync_in;
	end process;
end architecture behavior;
