-- Source: https://github.com/jwhitham/spdif-bit-exactness-tools/blob/master/fpga/app/vu_meter.vhdl

library ieee;
use ieee.std_logic_1164.all;

entity vu_meter is
    port (
        data_in         : in std_logic_vector (8 downto 0);
        strobe_in       : in std_logic;
        sync_in         : in std_logic;
        meter_out       : out std_logic_vector (7 downto 0) := "00000000";
        clock           : in std_logic);
end vu_meter;

architecture structural of vu_meter is
    signal data : std_logic_vector (8 downto 0) := (others => '0');
begin
    process (clock)
    begin
        if clock'event and clock = '1' then
            if sync_in = '0' then
                data <= (others => '0');
            elsif strobe_in = '1' then
                data <= data_in;
            end if;

            meter_out <= (others => '1');
            for i in 7 downto 0 loop
                if data (8) /= data (i) then
                    -- sign bit matches bit i - stop
                    exit;
                else
                    meter_out (7 - i) <= '0';
                end if;
            end loop;
        end if;
    end process;

end structural;