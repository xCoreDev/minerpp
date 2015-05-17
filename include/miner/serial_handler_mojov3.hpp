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

#ifndef MINER_SERIAL_HANDLER_MOJOV3_HPP
#define MINER_SERIAL_HANDLER_MOJOV3_HPP

#include <chrono>
#include <memory>

#include <miner/serial.hpp>
#include <miner/serial_handler.hpp>

namespace miner {

    class serial_port;
    
    /**
     * Implements a serial handler for the Mojo V3.
     */
    class serial_handler_mojov3 : public serial_handler
    {
        public:
        
            /**
             * Constructor
             * @param owner The serial_port.
             */
            explicit serial_handler_mojov3(
                std::shared_ptr<serial_port> owner
            );
        
            /**
             * Starts
             */
            virtual void start();
        
            /**
             * Stops
             */
            virtual void stop();
        
            /**
             * The read handler.
             * @param buf The buffer.
             * @param len The length.
             */
            virtual void on_read(const char * buf, const std::size_t & len);
        
            /**
             * Sets that new work is available.
             * @param val The value.
             */
            virtual void set_has_new_work(const bool & val);
        
            /**
             * Sets that new work is available.
             * @param val The value.
             */
            virtual void set_needs_work_restart(const bool & val);
            
        private:

			/**
			 * Handles an info message.
			 * @param msg The serial::message.
			 */
			bool handle_info(const serial::message_t & msg);

        protected:
        
            /**
             * The state.
             */
            enum
            {
                state_none,
                state_starting,
                state_started,
                state_stopping,
                state_stopped
            } state_;
        
			/**
			 * The read buffer.
			 */
			std::vector<std::uint8_t> read_buffer_;
        
            /**
             * The timeout timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_timeout_;
    };
    
} // namespace miner

#endif // MINER_SERIAL_HANDLER_MOJOV3_HPP
