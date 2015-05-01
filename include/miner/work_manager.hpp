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

#ifndef MINER_WORK_MANAGER_HPP
#define MINER_WORK_MANAGER_HPP

#include <cstdint>
#include <vector>

namespace miner {

    class stack_impl;
    class stratum_connection;
    class stratum_work;
    
    /**
     * Implements a work manager (gathers work via stratum and other mining
     * protocols).
     */
    class work_manager
        : public std::enable_shared_from_this<work_manager>
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit work_manager(stack_impl & owner);
        
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
        
            /**
             * Sets the stratum_work
             * @param val The value.
             */
            void set_work(const std::shared_ptr<stratum_work> & val);
        
            /**
             * Submits work with a solution.
             * @param val The work.
             */
            void submit_work(const std::shared_ptr<stratum_work> & val);
        
        private:
        
            /**
             * Connects to the work server.
             */
            void connect();
        
            /**
             * The timer handler.
             * @param ec The boost::system::error_code.
             */
            void tick(const boost::system::error_code & ec);
        
            /**
             * The check work hosts handler.
             * @param ec The boost::system::error_code.
             */
            void tick_check_work_hosts(const boost::system::error_code & ec);
        
            /**
             * The work.
             */
            std::shared_ptr<stratum_work> m_work;

        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand strand_;
            
            /**
             * The timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_;
        
            /**
             * The stratum connections.
             */
            std::vector<
                std::weak_ptr<stratum_connection>
            > stratum_connections_;
        
            /**
             * The currently selected work host index.
             */
            std::uint32_t work_host_index_;
        
            /**
             * The check work hosts timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_check_work_hosts_;
    };
    
} // namespace miner

#endif // MINER_WORK_MANAGER_HPP
