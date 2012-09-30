--
-- Helper functions for magic powers
--

__magic_powers = {}

function add_magic(m)
        local i, ret

        if type(m.spell_list) ~= "table" then
                error("add_magic called without a table")
        end

        -- Ok iterate over all the powers to add
        local index, p, max

        -- First, count them
        max = 0
        for index, p in m.spell_list do
                max = max + 1
        end

        -- Now register it
        ret = {}
        i = new_magic_power(max)
        ret.spells = i
        ret.max = max
        ret.fail_fct = m.fail

        -- And add each spells
        max = 0
        ret.info = {}
        ret.spell = {}
        for index, p in m.spell_list do
                assert(p.desc, "No desc for the spell!")
                assert(p.mana, "No mana for the spell!")
                assert(p.level, "No level for the spell!")
                assert(p.fail, "No fail for the spell!")
                assert(p.info, "No info for the spell!")
                assert(p.spell, "No info for the spell!")

                get_magic_power(i, max).name = index
                get_magic_power(i, max).desc = p.desc
                get_magic_power(i, max).mana_cost = p.mana
                get_magic_power(i, max).min_lev = p.level
                get_magic_power(i, max).fail = p.fail
                ret.info[max] = p.info
                ret.spell[max] = p.spell

                max = max + 1
        end

        return ret
end

function __get_magic_info(power)
        return __current_magic_power_info[power]()
end

function execute_magic(m)
        local sn, ret

        -- Ask for a spell
        __current_magic_power_info = m.info
        ret, sn = select_magic_power(0, m.spells, m.max, "__get_magic_info")
        if (ret == FALSE) then return end
                
        -- Verify mana needs
        if (get_magic_power(m.spells, sn).mana_cost > player.csp) then msg_print("Not enough mana!") return end

        -- Verify failure(second parameter is optional)
        if m.fail then
                __current_magic_power_fail = m.fail_fct
                if (magic_power_sucess(get_magic_power(m.spells, sn), "__current_magic_power_fail") == FALSE) then return end
        else
                if (magic_power_sucess(get_magic_power(m.spells, sn)) == FALSE) then return end
        end

        -- Actually cast the spells
        m.spell[sn]()

        -- use up some mana
        increase_mana(-get_magic_power(m.spells, sn).mana_cost)
end
