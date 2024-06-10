library ieee; 
use ieee.std_logic_1164.all;

entity lfsr is 
	generic(
		bit_width	:	integer := 8;
		G_POLY		:	std_logic_vector := "11000000" -- x^7+x^6+1
	);
	port (
		i_clk		:	in  std_logic;
		i_rstb		:	in  std_logic;
		i_sync_reset:	in  std_logic;
		i_seed		:	in  std_logic_vector(bit_width - 1 downto 0);
		i_en		:	in  std_logic;
		o_lsfr		:	out std_logic_vector(bit_width - 1 downto 0)
	);
end lfsr;

architecture rtl of lfsr is  
	signal r_lfsr	:	std_logic_vector(bit_width downto 1);
	signal w_mask	:	std_logic_vector(bit_width downto 1);
	signal w_poly	:	std_logic_vector(bit_width downto 1);
begin  
	o_lsfr <= r_lfsr(bit_width downto 1);
	w_poly <= G_POLY;

	bit_width_mask : for k in bit_width downto 1 generate
		w_mask(k) <= w_poly(k) and r_lfsr(1);
	end generate bit_width_mask;

	p_lfsr : process (i_clk, i_rstb) begin 
		if (i_rstb = '0') then 
				r_lfsr <= (others=>'1');
			elsif rising_edge(i_clk) then 
				if(i_sync_reset='1') then
					r_lfsr <= i_seed;
				elsif (i_en = '1') then 
					r_lfsr <= '0'&r_lfsr(bit_width downto 2) xor w_mask;
			end if; 
		end if; 
	end process p_lfsr; 
end architecture rtl;