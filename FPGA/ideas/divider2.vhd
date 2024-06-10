// Source: https://github.com/jwhitham/spdif-bit-exactness-tools/blob/master/fpga/lib/divider.vhdl

-- Divider with configurable width:
--
--   result = top / bottom
--
-- The divider does not report an error if bottom = 0, but the result is undefined.


library work;
use work.all;

library ieee;
use ieee.std_logic_1164.all;

entity divider is
    generic (
        top_width              : Natural;
        bottom_width           : Natural;
        subtractor_slice_width : Natural := 8);
    port (
        top_value_in        : in std_logic_vector (top_width - 1 downto 0);
        bottom_value_in     : in std_logic_vector (bottom_width - 1 downto 0);
        start_in            : in std_logic;
        reset_in            : in std_logic;
        finish_out          : out std_logic := '0';
        ready_out           : out std_logic := '0';
        result_out          : out std_logic_vector (top_width - 1 downto 0) := (others => '0');
        clock_in            : in std_logic
    );
end divider;

architecture structural of divider is

    subtype t_wide is std_logic_vector (top_width + bottom_width - 2 downto 0);
    subtype t_steps_to_do is Natural range 0 to top_width - 1; 
    type t_state is (IDLE, START_SUBTRACT, AWAIT_RESULT);

    -- Registers
    signal top          : t_wide := (others => '0');
    signal bottom       : t_wide := (others => '0');
    signal result       : std_logic_vector (top_width - 1 downto 0) := (others => '0');
    signal state        : t_state := IDLE;
    signal steps_to_do  : t_steps_to_do := top_width - 1;

    -- Signals
    signal subtract_result   : t_wide := (others => '0');
    signal subtract_start    : std_logic := '0';
    signal subtract_finish   : std_logic := '0';
    signal subtract_overflow : std_logic := '0';

begin
    top (top'Left downto top_width) <= (others => '0');
    result_out <= result;
    subtract_start <= '1' when state = START_SUBTRACT else '0';
    ready_out <= '1' when state = IDLE else '0';


    sub : entity subtractor
        generic map (value_width => t_wide'Length, slice_width => subtractor_slice_width)
        port map (
            top_value_in => top,
            bottom_value_in => bottom,
            start_in => subtract_start,
            reset_in => reset_in,
            finish_out => subtract_finish,
            result_out => subtract_result,
            overflow_out => subtract_overflow,
            clock_in => clock_in);

    process (clock_in)
    begin
        if clock_in'event and clock_in = '1' then
            finish_out <= '0';

            case state is
                when IDLE =>
                    -- Load new inputs
                    top (top_width - 1 downto 0) <= top_value_in;
                    bottom (bottom'Left downto top_width - 1) <= bottom_value_in;

                    steps_to_do <= top_width - 1;

                    if start_in = '1' then
                        state <= START_SUBTRACT;
                    end if;

                when START_SUBTRACT =>
                    -- Begin subtraction
                    state <= AWAIT_RESULT;

                when AWAIT_RESULT =>
                    -- Perform unsigned division
                    if subtract_finish = '1' then
                        result (result'Left downto 1) <= result (result'Left - 1 downto 0);
                        bottom (bottom'Left - 1 downto 0) <= bottom (bottom'Left downto 1);
                        bottom (bottom'Left) <= '0';

                        if subtract_overflow = '0' then
                            -- subtraction did not result in overflow
                            top (top_width - 1 downto 0) <= subtract_result (top_width - 1 downto 0);
                            result (0) <= '1';
                        else
                            -- subtraction resulted in overflow
                            result (0) <= '0';
                        end if;

                        if steps_to_do = 0 then
                            -- Finished?
                            finish_out <= '1';
                            state <= IDLE;
                        else
                            steps_to_do <= steps_to_do - 1;
                            state <= START_SUBTRACT;
                        end if;
                    end if;
            end case;

            if reset_in = '1' then
                state <= IDLE;
            end if;
        end if;
    end process;

end structural;