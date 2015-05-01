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

#include <miner/logger.hpp>
#include <miner/serial_port.hpp>
#include <miner/serial_handler_mojov3.hpp>

using namespace miner;

serial_handler_mojov3::serial_handler_mojov3(
    std::shared_ptr<serial_port> owner
    )
    : serial_handler(owner)
{
    log_debug("Serial handler MojoV3 allocated.");
}

void serial_handler_mojov3::on_read(const char * buf, const std::size_t & len)
{
    log_debug("Serial handler MojoV3 read len = " << len << ".");
}

void serial_handler_mojov3::set_has_new_work(const bool & val)
{
    log_debug("Serial handler MojoV3 sending test work.");
    
    if (auto i = serial_port_.lock())
    {
        const char * test0007 = "0007";
        
        i->write(test0007, strlen(test0007));
    }
}

void serial_handler_mojov3::set_needs_work_restart(const bool & val)
{
    log_debug("Serial handler MojoV3 sending test work.");
    
    if (auto i = serial_port_.lock())
    {
        const char * test0007 = "0007";
        
        i->write(test0007, strlen(test0007));
    }
}
