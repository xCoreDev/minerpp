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

#ifndef MINER_CONFIGURATION_HPP
#define MINER_CONFIGURATION_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace miner {

    /**
     * Implements a configuration.
     */
    class configuration
    {
        public:

            /**
             * Constructor
             */
            configuration();
        
            /**
             * The singleton accessor.
             */
            static configuration & instance();

            /**
             * The device types.
             */
            typedef enum device_type_s
            {
                device_type_none,
                device_type_cpu,
                device_type_gpu,
                device_type_serial,
            } device_type_t;
        
            /**
             * The Proof-of-Work types.
             */
            typedef enum proof_of_work_type_s
            {
                proof_of_work_type_none,
                proof_of_work_type_whirlpool_xor,
            } proof_of_work_type_t;
        
            /**
             * Set the device type.
             * @param val The value.
             */
            void set_device_type(const device_type_t & val);
        
            /**
             * The device type.
             */
            const device_type_t & device_type() const;
        
            /**
             * Set the Proof-of-Work type.
             * @param val The value.
             */
            void set_proof_of_work_type(const proof_of_work_type_t & val);
        
            /**
             * The the Proof-of-Work type.
             */
            const proof_of_work_type_t & proof_of_work_type() const;
        
            /**
             * The serial ports.
             */
            std::vector<std::string> & serial_ports();
        
            /**
             * The work hosts.
             */
            std::vector<
                std::pair< std::pair<std::string, std::string>,
                std::pair<std::string, std::uint16_t> >
            > & work_hosts();
        
            /**
             * Sets the device cores.
             * @param val The value.
             */
            void set_device_cores(const std::uint32_t & val);
        
            /**
             * The number of device cores.
             */
            const std::uint32_t & device_cores() const;
        
        private:
        
            /**
             * The device type.
             */
            device_type_t m_device_type;
        
            /**
             * The Proof-of-Work type.
             */
            proof_of_work_type_t m_proof_of_work_type;
        
            /**
             * The serial ports.
             */
            std::vector<std::string> m_serial_ports;
        
            /**
             * The work hosts.
             */
            std::vector<
                std::pair< std::pair<std::string, std::string>,
                std::pair<std::string, std::uint16_t> >
            > m_work_hosts;
        
            /**
             * The number of device cores.
             */
            std::uint32_t m_device_cores;
        
        protected:
        
            // ...
    };
    
} // namespace miner

#endif // MINER_CONFIGURATION_HPP
