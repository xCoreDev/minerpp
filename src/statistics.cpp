/*
 * Copyright (c) 2013-2015 John Connor (BM-NC49AxAjcqVcF5jNPu85Rb8MJ2d9JqZt)
 *
 * This file is part of MinerPP.
 *
 * MinerPP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <miner/statistics.hpp>

using namespace miner;

statistics::statistics()
    : m_hashes_per_second(0.0)
{
    // ...
}

statistics & statistics::instance()
{
    static statistics g_statistics;
            
    return g_statistics;
}

void statistics::set_hashes_per_second(const double & val)
{
    m_hashes_per_second = val;
}

const double & statistics::hashes_per_second() const
{
    return m_hashes_per_second;
}
