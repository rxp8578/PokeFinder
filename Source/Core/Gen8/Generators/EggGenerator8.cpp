/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2021 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "EggGenerator8.hpp"
#include <Core/Enum/Game.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/RNG/Xoroshiro.hpp>
#include <Core/RNG/Xorshift.hpp>

EggGenerator8::EggGenerator8(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 genderRatio, const StateFilter &filter,
                             const Daycare &daycare, bool shinyCharm, u8 compatability) :
    EggGenerator(initialAdvances, maxAdvances, tid, sid, genderRatio, Method::Null, filter, daycare),
    shinyCharm(shinyCharm),
    compatability(compatability)
{
}

std::vector<EggState> EggGenerator8::generate(u64 seed0, u64 seed1) const
{
    Xorshift rng(seed0, seed1);
    rng.advance(initialAdvances + offset);

    std::vector<EggState> states;
    for (u32 cnt = 0; cnt <= maxAdvances; cnt++, rng.next())
    {
        Xorshift check(rng);
        if (check.next(100) < compatability)
        {
            EggState state(initialAdvances + cnt);
            XoroshiroBDSP gen(check.next());

            // Nidoran
            // gen.next(2)

            // Illumise/Volbeat
            // gen.next(2)

            // Indeedee
            // gen.next(2)

            if (genderRatio == 255)
            {
                state.setGender(2);
            }
            else if (genderRatio == 254)
            {
                state.setGender(1);
            }
            else if (genderRatio == 0)
            {
                state.setGender(0);
            }
            else
            {
                u8 gender = gen.nextUInt(252) + 1 < genderRatio;
                state.setGender(gender);
            }

            u8 nature = gen.nextUInt(25);
            if (daycare.getEverstoneCount(Game::BDSP) == 2)
            {
                nature = daycare.getParentNature(gen.nextUInt(2));
            }
            else if (daycare.getParentItem(0) == 1)
            {
                nature = daycare.getParentNature(0);
            }
            else if (daycare.getParentItem(1) == 1)
            {
                nature = daycare.getParentNature(1);
            }
            state.setNature(nature);

            u8 ability = gen.nextUInt(100);
            u8 parentAbility = daycare.getParentAbility(1);
            if (parentAbility == 2)
            {
                ability = ability < 20 ? 0 : ability < 40 ? 1 : 2;
            }
            else if (parentAbility == 1)
            {
                ability = ability < 20 ? 0 : 1;
            }
            else
            {
                ability = ability < 80 ? 0 : 1;
            }
            state.setAbility(ability);

            u8 inheritance = 3;
            if (daycare.getParentItem(0) == 8 || daycare.getParentItem(1) == 8)
            {
                inheritance = 5;
            }

            if (filter.comparePID(state) && filter.compareIV(state))
            {
                states.emplace_back(state);
            }
        }
    }

    return states;
}