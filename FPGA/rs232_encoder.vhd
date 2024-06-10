-- RS232 data encoder
-- (c) 2023 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/Audioplayer
--
-- This file contains a RS232 data-encoder to transmit individual bytes via a rs232-transmitter
-- It is like a signal multiplexer, that will prepare multiple bytes for a serial-transmitter

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity rs232_encoder is 
	generic(
		clk_rate_hz    : integer := 48000000;
		txd_rate_hz   : integer := 20
	);
	port
	(
		clk		: in std_logic;
		byte1		: in unsigned(7 downto 0); -- Version
		byte2		: in unsigned(7 downto 0); -- info about noisegates
		byte3		: in unsigned(7 downto 0); -- info about dynamic compressors
		byte4		: in unsigned(7 downto 0); -- info about clipping
		byte5		: in unsigned(7 downto 0); -- VU-Meter Left
		byte6		: in unsigned(7 downto 0); -- VU-Meter Right
		byte7		: in unsigned(7 downto 0); -- VU-Meter Subwoofer
		TX_rdy	: in std_logic := '0'; -- transmitter is ready for next byte
		
		TX_data	:	out unsigned(7 downto 0); -- databyte to transmit
		TX_send	:	out std_logic := '0' -- send data via transmitter
	);
end entity;

architecture Behavioral of rs232_encoder is
	type t_SM_Encoder is (s_Idle, s_Send, s_Prepare, s_Wait);
	signal s_SM_Encoder 	: t_SM_Encoder := s_Idle;
	
	signal txd_rate_cnt	: natural range 0 to clk_rate_hz/(2*txd_rate_hz) := 0;
	signal byte_cnt 		: natural range 0 to 10 := 0;
	
	signal tx_next_byte	: std_logic := '0';
	
	signal b1, b2, b3, b4, b5, b6, b7	:	unsigned(7 downto 0);
	signal PayloadSum	: unsigned(15 downto 0);
begin
	process(clk)
	begin
		if (rising_edge(clk)) then
			case s_SM_Encoder is
				when s_Idle =>
					-- waiting for next send-interval
					TX_send <= '0';
					
					txd_rate_cnt <= txd_rate_cnt + 1;
					if (txd_rate_cnt = ((clk_rate_hz/(2*txd_rate_hz) - 1))) then -- 4MHz / 200000 = 20 Hz, but we are inverting on each clock-edge, so 10 Hz
						-- we reached the user-defined send-interval -> start new frame
						s_SM_Encoder <= s_Prepare;

						-- reset counter for next interval
						txd_rate_cnt <= 0;
					end if;

				when s_Prepare =>
					-- reset byte-counter
					byte_cnt <= 0;
				
					-- copy all bytes to internal signals as data will change during the transmission
					b1 <= byte1;
					b2 <= byte2;
					b3 <= byte3;
					b4 <= byte4;
					b5 <= byte5;
					b6 <= byte6;
					b7 <= byte7;
					
					-- calculate the checksum
					PayloadSum <= resize(byte1, 16) + resize(byte2, 16) + resize(byte3, 16) + resize(byte4, 16) + resize(byte5, 16) + resize(byte6, 16) + resize(byte7, 16);

					-- go into send-state
					s_SM_Encoder <= s_Send;
				when s_Send =>

					case byte_cnt is
						when 0 =>
							-- start of frame
							TX_data <= to_unsigned(65, 8); -- character "A"
						when 1 =>
							-- byte 1
							TX_data <= b1;
						when 2 =>
							-- byte 2
							TX_data <= b2;
						when 3 =>
							-- byte 3
							TX_data <= b3;
						when 4 =>
							-- byte 4
							TX_data <= b4;
						when 5 =>
							-- byte 5
							TX_data <= b5;
						when 6 =>
							-- byte 6
							TX_data <= b6;
						when 7 =>
							-- byte 7
							TX_data <= b7;
						when 8 =>
							-- payload part 1
							TX_data <= unsigned(std_logic_vector(PayloadSum)(15 downto 8));
						when 9 =>
							-- payload part 2
							TX_data <= unsigned(std_logic_vector(PayloadSum)(7 downto 0));
						when 10 =>
							-- end of frame
							TX_data <= to_unsigned(69, 8); -- character "E"
					end case;

					-- clear to send
					TX_send <= '1';

					if (byte_cnt < 10) then
						-- increase byte-counter
						byte_cnt <= byte_cnt + 1;
						
						-- go into wait-state and wait for transmitter to transmit next byte
						s_SM_Encoder <= s_Wait;
					else
						-- all data transmitted -> go into idle-state
						s_SM_Encoder <= s_Idle;
					end if;
					
				when s_Wait =>
					-- wait for transmitter

					-- reset clear-flag
					TX_send <= '0';
					
					-- check if we are ready for next byte
					if (TX_rdy = '1') then
						s_SM_Encoder <= s_Send;
					end if;
					
			end case;
		end if;
	end process;
end Behavioral;