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

#include <memory>

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
             * Prepares work (80 bytes work of big endian data) for the device.
             */
            bool prepare_work(std::uint32_t * val);
        
        protected:
        
            // ...
    };
    
} // namespace miner

#endif // MINER_SERIAL_HANDLER_MOJOV3_HPP
