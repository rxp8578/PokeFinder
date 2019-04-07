/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2019 by Admiral_Fish, bumba, and EzPzStreamz
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

#include "EggSearcher4.hpp"

EggSearcher4::EggSearcher4(const Egg4 &generator, const FrameCompare &compare, u32 minDelay, u32 maxDelay)
{
    this->generator = generator;
    this->compare = compare;
    this->minDelay = minDelay;
    this->maxDelay = maxDelay;
    cancel = false;
    progress = 0;

    connect(this, &EggSearcher4::finished, this, &EggSearcher4::deleteLater);
}

void EggSearcher4::run()
{
    int total = 0;

    for (u16 ab = 0; ab < 256; ab++)
    {
        for (u8 cd = 0; cd < 24; cd++)
        {
            for (u32 efgh = minDelay; efgh <= maxDelay; efgh++)
            {
                if (cancel)
                {
                    return;
                }

                if (total > 10000)
                {
                    progress = static_cast<int>(256 * 24 * (maxDelay - minDelay + 1));
                    break;
                }

                u32 seed = ((ab << 24) | (cd << 16)) + efgh;
                generator.setSeed(seed);

                auto frames = generator.generate(compare);
                progress++;
                total += frames.size();

                QMutexLocker locker(&mutex);
                results.append(frames);
            }
        }
    }
}

int EggSearcher4::currentProgress() const
{
    return progress;
}

QVector<Frame4> EggSearcher4::getResults()
{
    QMutexLocker lock(&mutex);
    auto data(results);
    results.clear();
    return data;
}

void EggSearcher4::cancelSearch()
{
    cancel = true;
}
