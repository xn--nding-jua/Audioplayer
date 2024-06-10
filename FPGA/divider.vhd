-- Source: http://www.lothar-miller.de/s9y/categories/24-Rechnen

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity divider is
	generic (
		bit_width	:	natural range 4 to 48 := 32
	);
	port (
		clk			:	in   std_logic;
		start			:	in   std_logic;
		dividend		:	in   unsigned(bit_width - 1 downto 0);
		divisor		:	in   unsigned(bit_width - 1 downto 0);
		quotient		:	out  unsigned(bit_width - 1 downto 0);
		remainder	:	out  unsigned(bit_width - 1 downto 0);
		busy			:	out  std_logic
	);
end divider;

architecture Behave_Unsigned of divider is
	signal dd		:	unsigned(bit_width - 1 downto 0); -- dividend
	signal dr		:	unsigned(bit_width - 1 downto 0); -- divisor
	signal q			:	unsigned(bit_width - 1 downto 0); -- quotient
	signal r			:	unsigned(bit_width - 1 downto 0); -- remainder
	signal bits		:	integer range bit_width downto 0;
	type t_SM_state is (idle, prepare, shift, sub, done);
	signal s_SM_Main : t_SM_state;
begin

   process 
		variable diff : unsigned(bit_width - 1 downto 0);
   begin
      wait until rising_edge(clk);
      case s_SM_Main is 
         when idle => 
            if (start='1') then 
               s_SM_Main <= prepare; 
               busy <= '1';
            end if;
            dd <= dividend;
            dr <= divisor;

         when prepare =>
            q <= (others=>'0');
            r <= (others=>'0');
            s_SM_Main <= shift;            
            bits <= bit_width;
				
            -- Special case 1: Division by Zero
            if (dr = 0) then  
               q <= (others=>'1');
               r <= (others=>'1');
               s_SM_Main <= done;
					
            -- Special case 2: Divisor larger than Dividend
            elsif (dr > dd) then 
               r <= dd;
               s_SM_Main <= done;
					
            -- Special case 3: Divisor equals Dividend
            elsif (dr = dd) then
               q <= to_unsigned(1, bit_width);
               s_SM_Main <= done;
            end if;

         when shift =>
				-- first, move the two operands for the subtraction into place
            if ( (r(bit_width - 2 downto 0) & dd(bit_width - 1)) < dr) then
               bits <= bits - 1;
               r    <= r(bit_width - 2 downto 0) & dd(bit_width - 1);
               dd   <= dd(bit_width - 2 downto 0) & '0';
            else
               s_SM_Main <= sub;
            end if;

         when sub =>
            if (bits > 0) then
               r    <= r(bit_width - 2 downto 0) & dd(bit_width - 1);
               dd   <= dd(bit_width - 2 downto 0) & '0';
					
               -- remainder minus divisor
               diff := (r(bit_width - 2 downto 0) & dd(bit_width -1)) - dr;  
               if (diff(bit_width - 1)='0') then                 
                  -- if no underrun
                  --> divisor is fitting
                  --> MSB=0 --> 1 shift into result
                  q <= q(bit_width - 2 downto 0) & '1';
                  r <= diff;
               else
                  -- if underrund
                  --> shift 0, calculate with old value
                  q <= q(bit_width - 2 downto 0) & '0';
               end if;
               bits <= bits - 1;
            else
               s_SM_Main <= done;
            end if;
            
         when done =>
            busy <= '0';

				-- wait, until start signal is set to 0
            if (start = '0') then 
               s_SM_Main <= idle; 
            end if;
      end case;
   end process;
   
   quotient  <= q;
   remainder <= r;
end;