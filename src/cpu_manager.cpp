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

#include <algorithm>
#include <cstdint>
#include <thread>

#include <miner/configuration.hpp>
#include <miner/cpu.hpp>
#include <miner/cpu_manager.hpp>
#include <miner/logger.hpp>
#include <miner/stratum_work.hpp>

using namespace miner;

cpu_manager::cpu_manager(stack_impl & owner)
    : stack_impl_(owner)
{
    // ...
}

void cpu_manager::start()
{
    auto device_cores = configuration::instance().device_cores();
    
    /**
     * If the supplied number of device cores is zero, determine the number.
     */
    if (device_cores == 0)
    {
        /**
         * Calculate the number of cores.
         */
        device_cores = (std::max)(
            static_cast<std::uint32_t> (1),
            std::thread::hardware_concurrency() - 1
        );
    }
    
    log_info(
        "CPU manager is starting with " << device_cores << " device cores."
    );

    /**
     * Allocate the CPU's.
     */
    for (auto i = 0; i < device_cores; i++)
    {
        auto c = std::make_shared<cpu>(stack_impl_, i, device_cores);
        
        m_cpus.push_back(c);
        
        c->start();
    }
}

void cpu_manager::stop()
{
    log_debug("CPU manager is starting.");
    
    /**
     * Stop the CPU's.
     */
    for (auto & i : m_cpus)
    {
        i->stop();
    }
}

void cpu_manager::set_work(const std::shared_ptr<stratum_work> & val)
{
    log_debug("CPU manager is setting work.");
    
    if (val == 0)
    {
        m_work = val;
        
        /**
         * Inform all CPU's of the new work.
         */
        for (auto & i : m_cpus)
        {
            i->set_work(m_work);
        }
    }
    else
    {
        m_work = val;
        
        /**
         * Inform all CPU's of the new work.
         */
        for (auto & i : m_cpus)
        {
            i->set_work(m_work);
        }
    }
}

const std::vector< std::shared_ptr<cpu> > & cpu_manager::cpus() const
{
    return m_cpus;
}
