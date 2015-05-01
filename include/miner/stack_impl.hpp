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

#ifndef MINER_STACK_IMPL_HPP
#define MINER_STACK_IMPL_HPP

#include <cstdint>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace miner {
    
    class cpu_manager;
    class gpu_manager;
    class serial_manager;
    class stack;
    class stratum_work;
    class work_manager;
    
    /**
     * The stack implementation.
     */
    class stack_impl
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack.
             */
            stack_impl(miner::stack &);
            
            /**
             * Starts the stack.
             * @param args The arguments.
             */
            void start(
                const std::map<std::string, std::string> & args
            );
        
            /**
             * Stops the stack.
             */
            void stop();
        
            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service & io_service();
        
            /**
             * The work_manager.
             */
            std::shared_ptr<work_manager> & get_work_manager();
        
            /**
             * Handles work.
             * @param buffer The buffer.
             */
            void handle_work(const std::shared_ptr<stratum_work> & work);
        
            /**
             * Updates the statistics.
             */
            void update_statistics();
        
        private:
        
            /**
             * The cpu_manager.
             */
            std::shared_ptr<cpu_manager> m_cpu_manager;
        
            /**
             * The gpu_manager.
             */
            std::shared_ptr<gpu_manager> m_gpu_manager;
        
            /**
             * The serial_manager.
             */
            std::shared_ptr<serial_manager> m_serial_manager;
        
            /**
             * The work_manager.
             */
            std::shared_ptr<work_manager> m_work_manager;
        
            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service m_io_service;
        
        protected:

            /**
             * Parses the command line args into the configuration.
             * @param args The arguments.
             */
            void parse_command_line_args(
                const std::map<std::string, std::string> & args
            );
        
            /**
             * The main loop.
             */
            void loop();

            /**
             * The stack.
             */
            miner::stack & stack_;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand strand_;
        
            /**
             * The boost::asio::io_service::work.
             */
            std::shared_ptr<boost::asio::io_service::work> work_;
        
            /**
             * The thread.
             */
            std::vector< std::shared_ptr<std::thread> > threads_;
    };
    
} // namespace miner

#endif // MINER_STACK_IMPL_HPP
