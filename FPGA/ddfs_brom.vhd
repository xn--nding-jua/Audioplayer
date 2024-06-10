-- Source: http://www.lothar-miller.de/s9y/categories/31-DDFS

library IEEE;
  use IEEE.std_logic_1164.all;
  use IEEE.numeric_std.all;
--------------------------------------------------
Entity ddfs_brom is
--------------------------------------------------
   Port ( CLK       : in  std_logic;
          Freq_Data : in  std_logic_vector (7 downto 0);
          Dout      : out std_logic_vector (7 downto 0)
        );
   end ddfs_brom;
--------------------------------------------------
Architecture RTL of ddfs_brom is
--------------------------------------------------
  signal Result   : signed (7 downto 0);
  signal Accum    : unsigned (20 downto 0) := (others=>'0'); 
  signal Address  : integer range 0 to 63;
  signal RomAddr  : integer range 0 to 63;
  signal Quadrant : std_logic; 
  signal Sign     : std_logic; 

  type Rom64x8 is array (0 to 63) of signed (7 downto 0); 
  -- siehe http://www.mikrocontroller.net/articles/Digitale_Sinusfunktion
  constant Sinus_Rom : Rom64x8 := (
    x"02",  x"05",  x"08",  x"0b",  x"0e",  x"11",  x"14",  x"17",
    x"1a",  x"1d",  x"20",  x"23",  x"26",  x"29",  x"2c",  x"2f",
    x"32",  x"36",  x"39",  x"3c",  x"3e",  x"40",  x"43",  x"46",
    x"48",  x"4b",  x"4d",  x"50",  x"52",  x"54",  x"57",  x"59",
    x"5b",  x"5d",  x"5f",  x"62",  x"64",  x"65",  x"67",  x"69",
    x"6b",  x"6d",  x"6e",  x"70",  x"71",  x"73",  x"74",  x"75",
    x"76",  x"77",  x"79",  x"79",  x"7a",  x"7b",  x"7c",  x"7d",
    x"7d",  x"7e",  x"7e",  x"7f",  x"7f",  x"7f",  x"7f",  x"7f");
begin
   -- Phasenakkumulator   
   process begin
     wait until rising_edge(CLK);
     Accum <= Accum + unsigned(Freq_Data);
   end process;

   -- BROM      
   process begin
     wait until rising_edge(CLK);
     RomAddr <= Address;          -- getaktete Adresse --> BRAM
   end process;
   Result   <= signed(Sinus_Rom(RomAddr));

   Quadrant <= Accum(Accum'left-1);
   Address  <=    to_integer(Accum(Accum'high-2 downto Accum'high-7))  when (Quadrant='0') else 
               63-to_integer(Accum(Accum'high-2 downto Accum'high-7));
 
   -- 1 Takt Latency wegen BROM
   process begin
     wait until rising_edge(CLK);
     Sign    <= Accum(Accum'left);
   end process;
   Dout     <= std_logic_vector(  Result)   when (Sign='1') else  
               std_logic_vector(0-Result);
end RTL;