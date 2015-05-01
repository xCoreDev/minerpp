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

#ifndef MINER_CPU_MANAGER_HPP
#define MINER_CPU_MANAGER_HPP

#include <cstdint>
#include <memory>
#include <vector>

namespace miner {

    class cpu;
    class stack_impl;
    class stratum_work;
    
    /**
     * Implements a CPU manager.
     */
    class cpu_manager
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit cpu_manager(stack_impl & owner);
        
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
             * The CPU's.
             */
            const std::vector< std::shared_ptr<cpu> > & cpus() const;
        
        private:
        
            /**
             * The work.
             */
            std::shared_ptr<stratum_work> m_work;
        
            /**
             * The CPU's.
             */
            std::vector< std::shared_ptr<cpu> > m_cpus;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace miner

#endif // MINER_CPU_MANAGER_HPP
