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

#include <cassert>

#include <miner/gpu.hpp>
#include <miner/gpu_handler.hpp>

using namespace miner;

gpu_handler::gpu_handler(std::shared_ptr<gpu> owner)
    : gpu_(owner)
{
    // ...
}

void gpu_handler::on_read(const char * buf, const std::size_t & len)
{
    assert(0);
}

void gpu_handler::set_has_new_work(const bool & val)
{
    assert(0);
}

void gpu_handler::set_needs_work_restart(const bool & val)
{
    assert(0);
}
