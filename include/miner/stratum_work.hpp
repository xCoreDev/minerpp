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

#ifndef MINER_STRATUM_WORK_HPP
#define MINER_STRATUM_WORK_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace miner {

    /**
     * Implements stratum work.
     */
    class stratum_work
    {
        public:
        
            /**
             * Constructor
             * @param worker_name The worker name.
             * @param job_id The job id.
             * @param extranonce1 The extranonce1.
             * @param extranonce2 The extranonce2.
             * @param previous_hash_bytes The previous hash.
             * @param coinb1_bytes The coinb1.
             * @param coinb2_bytes The coinb2.
             * @param merkles The merkles.
             * @param version_bytes The version.
             * @param bits_bytes The bits
             * @param time_bytes The time.
             * @param target The targert.
             */
            explicit stratum_work(
                const std::string & worker_name,
                const std::string & job_id,
                const std::vector<std::uint8_t> & extranonce1,
                const std::vector<std::uint8_t> & extranonce2,
                const std::vector<std::uint8_t> & previous_hash_bytes,
                const std::vector<std::uint8_t> & coinb1_bytes,
                const std::vector<std::uint8_t> & coinb2_bytes,
                const std::vector<std::string> & merkles,
                const std::vector<std::uint8_t> & version_bytes,
                const std::vector<std::uint8_t> & bits_bytes,
                const std::vector<std::uint8_t> & time_bytes,
                const std::uint32_t * target
            );
        
            /**
             * Generates the work.
             */
            bool generate();
        
            /**
             * The worker name
             */
            const std::string & worker_name() const;
        
            /**
             * The job id.
             */
            const std::string & job_id() const;
        
            /**
             * The extranonce2.
             */
            const std::vector<std::uint8_t> & extranonce2() const;
        
            /**
             * The time.
             */
            const std::vector<std::uint8_t> & time() const;
        
            /**
             * The data.
             */
            std::vector<std::uint32_t> & data();
        
            /**
             * The target.
             */
            const std::uint32_t * target() const;
        
        private:
        
            /**
             * The worker name
             */
            std::string m_worker_name;
        
            /**
             * The new job id.
             */
            std::string m_job_id;
        
            /**
             * The previous hash.
             */
            std::vector<std::uint8_t> m_previous_hash_bytes;
        
            /**
             * The coinb1.
             */
            std::vector<std::uint8_t> m_coinb1_bytes;
        
            /**
             * The coinb2.
             */
            std::vector<std::uint8_t> m_coinb2_bytes;
        
            /**
             * The merkles.
             */
            std::vector<std::string> m_merkles;
        
            /**
             * The version.
             */
            std::vector<std::uint8_t> m_version_bytes;
        
            /**
             * The bits.
             */
            std::vector<std::uint8_t> m_bits_bytes;
    
            /**
             * The extranonce1.
             */
            std::vector<std::uint8_t> m_extranonce1;
        
            /**
             * The extranonce2.
             */
            std::vector<std::uint8_t> m_extranonce2;
        
            /**
             * The time.
             */
            std::vector<std::uint8_t> m_time;
        
            /**
             * The data.
             */
            std::vector<std::uint32_t> m_data;
        
            /**
             * The target.
             */
            std::vector<std::uint32_t> m_target;
    
        protected:
        
            // ...
    };
    
} // namespace miner

#endif // MINER_STRATUM_WORK_HPP
