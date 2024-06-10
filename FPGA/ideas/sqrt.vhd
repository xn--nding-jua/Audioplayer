-- Source: http://www.lothar-miller.de/s9y/categories/24-Rechnen

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity SQRT is
    Generic ( b  : natural range 4 to 32 := 16 ); -- Breite 
    Port ( clk    : in   STD_LOGIC;
           start  : in   STD_LOGIC;
           value  : in   STD_LOGIC_VECTOR (15 downto 0);
           result : out  STD_LOGIC_VECTOR (7 downto 0);
           busy   : out  STD_LOGIC);
end SQRT;

architecture Behave of SQRT is
signal op  : unsigned(b-1 downto 0); -- 
signal res : unsigned(b-1 downto 0); -- 
signal one : unsigned(b-1 downto 0); -- 

signal bits : integer range b downto 0;
type zustaende is (idle, shift, calc, done);
signal z : zustaende;

begin
   process begin
      wait until rising_edge(clk);
      case z is 
         when idle => 
            if (start='1') then 
               z <= shift; 
               busy <= '1';
            end if;
            one <= to_unsigned(2**(b-2),b);
            op  <= unsigned(value);
            res <= (others=>'0');

         when shift =>
            if (one > op) then
               one <= one/4;
            else
               z   <= calc;
            end if;

         when calc =>
            if (one /= 0) then
               if (op >= res+one) then
                  op   <= op - (res+one);
                  res  <= res/2 + one;
               else
                  res  <= res/2;
               end if;
               one <= one/4;
            else
               z    <= done;
            end if;
            
         when done =>
            busy <= '0';
            -- Handshake: wenn nötig warten, bis start='0'
            if (start='0') then 
               z <= idle; 
            end if;
      end case;
   end process;
   
   result <= std_logic_vector(res(result'range));
end;