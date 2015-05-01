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

#ifndef MINER_SERIAL_MANAGER_HPP
#define MINER_SERIAL_MANAGER_HPP

#include <cstdint>
#include <memory>
#include <vector>

namespace miner {

    class serial_port;
    class stack_impl;
    class stratum_work;
    
    class serial_manager
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit serial_manager(stack_impl & owner);
        
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
        
            /**
             * Sets the work.
             * @param val The value.
             */
            void set_work(const std::shared_ptr<stratum_work> & val);
        
            /**
             * The serial_port object's.
             */
            const std::vector< std::weak_ptr<serial_port> > &
                serial_ports() const
            ;
        
        private:
        
            /**
             * The serial_port object's.
             */
            std::vector< std::weak_ptr<serial_port> > m_serial_ports;
        
            /**
             * The work.
             */
            std::shared_ptr<stratum_work> m_work;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace miner

#endif // MINER_SERIAL_MANAGER_HPP
