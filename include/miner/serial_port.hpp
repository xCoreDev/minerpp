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

#ifndef MINER_SERIAL_PORT_HPP
#define MINER_SERIAL_PORT_HPP

#include <boost/asio.hpp>

#include <cstdint>
#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

#include <miner/serial.hpp>

namespace miner {

    class serial_handler;
    class stack_impl;
    class stratum_work;
    
    /**
     * Implements a serial port.
     */
    class serial_port : public std::enable_shared_from_this<serial_port>
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit serial_port(stack_impl & owner);
        
            /**
             * Destructor
             */
            ~serial_port();
        
            /**
             * Opens
             * @param device_name The device.
             */
            bool open(const std::string & device);
        
            /**
             * Closes
             */
            void close();
        
            /**
             * Writes the given buffer.
             * @param buf The buffer.
             * @param len The length.
             */
            void write(const char * buf, const std::size_t & len);
        
            /**
             * Sets the device model.
             * @param val The value.
             */
            void set_device_model(const serial::device_model_t & val);
        
            /**
             * Sets the work.
             * @param val The value.
             */
            void set_work(const std::shared_ptr<stratum_work> & val);
        
            /**
             * Returns a copy of thw work.
             */
            std::shared_ptr<stratum_work> work();
        
            /**
             * The number of hashes per second.
             */
            const double & hashes_per_second() const;
            
        private:
        
            /**
             * Performs a read operation.
             */
            void do_read();
        
            /**
             * Writes the given buffer.
             * @param buf The buffer.
             * @param len The length.
             */
            void do_write(const char * buf, const std::size_t & len);

            /**
             * The timer handler.
             * @param ec The boost::system::error_code.
             */
            void tick(const boost::system::error_code & ec);
        
            /**
             * The boost::asio::serial_port.
             */
            boost::asio::serial_port m_serial_port;
        
            /**
             * The device.
             */
            std::string m_device;
        
            /**
             * The device model.
             */
            serial::device_model_t m_device_model;

            /**
             * The serial_handler.
             */
            std::shared_ptr<serial_handler> m_serial_handler;
        
            /**
             * The work.
             */
            std::shared_ptr<stratum_work> m_work;
        
            /**
             * The number of hashes per second.
             */
            double m_hashes_per_second;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The strand.
             */
            boost::asio::strand strand_;
        
            /**
             * The timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_;
        
            /**
             * The read buffer.
             */
            char read_buffer_[512];
        
            /**
             * The write queue.
             */
            std::deque< std::vector<char> > write_queue_;
        
            /**
             * The work std::mutex.
             */
            std::mutex mutex_work_;
    };
    
} // namespace miner

#endif // MINER_SERIAL_PORT_HPP
