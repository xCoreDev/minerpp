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

#include <miner/configuration.hpp>
#include <miner/logger.hpp>
#include <miner/serial_manager.hpp>
#include <miner/serial_port.hpp>
#include <miner/stack_impl.hpp>
#include <miner/stratum_work.hpp>

using namespace miner;

serial_manager::serial_manager(stack_impl & owner)
    : stack_impl_(owner)
{
    // ...
}

void serial_manager::start()
{
    for (auto & i : configuration::instance().serial_ports())
    {
        auto port = std::make_shared<serial_port> (stack_impl_);

        port->set_device_model(serial::device_model_mojov3);
        
        log_info("Serial manager is opening " << i << ".");

        if (port->open(i))
        {
            m_serial_ports.push_back(port);
        }
    }

    log_info("Serial manager opened " << m_serial_ports.size() << " ports.");
}

void serial_manager::stop()
{
    for (auto & i : m_serial_ports)
    {
        if (auto j = i.lock())
        {
            j->close();
        }
    }
    
    m_serial_ports.clear();
}

void serial_manager::set_work(const std::shared_ptr<stratum_work> & val)
{
    log_debug("Serial manager set work.");
    
    if (val == 0)
    {
        m_work = val;
        
        /**
         * Inform all serial ports of the new work.
         */
        for (auto & i : m_serial_ports)
        {
            if (auto j = i.lock())
            {
                j->set_work(m_work);
            }
        }
    }
    else
    {
        m_work = val;
        
        /**
         * Inform all serial ports of the new work.
         */
        for (auto & i : m_serial_ports)
        {
            if (auto j = i.lock())
            {
                j->set_work(m_work);
            }
        }
    }
}

const std::vector< std::weak_ptr<serial_port> > &
    serial_manager::serial_ports() const
{
    return m_serial_ports;
}
