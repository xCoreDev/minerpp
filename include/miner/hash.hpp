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

#ifndef MINER_HASH_HPP
#define MINER_HASH_HPP

#include <cstdint>

#include <miner/configuration.hpp>

namespace miner {

    /**
     * Implements various scanning algorithms.
     */
    class hash
    {
        public:

            /**
             * Scans a hash for a solution.
             * @param type The type.
             * @param ptr_data The data.
             * @param ptr_target The target.
             * @param max_nonce The nonce.
             * @param hashes_done The hashes done.
             * @param nonce The nonce.
             * @param restart If true a restart will occur.
             * @param has_new_work If true a restart will occur.
             */
            static bool scan(
                const configuration::proof_of_work_type_t & type,
                std::uint32_t * ptr_data, const std::uint32_t * ptr_target,
                const std::uint32_t & max_nonce, std::uint64_t & hashes_done,
                std::uint32_t & nonce, bool & restart, bool & has_new_work
            );

        private:
        
            /**
             * Scans a hash for a solution.
             * @param ptr_data The data.
             * @param ptr_target The target.
             * @param max_nonce The nonce.
             * @param hashes_done The hashes done.
             * @param nonce The nonce.
             * @param restart If true a restart will occur.
             * @param has_new_work If true a restart will occur.
             */
            static bool scan_whirlpool_xor(
                std::uint32_t * ptr_data, const std::uint32_t * ptr_target,
                const std::uint32_t & max_nonce, std::uint64_t & hashes_done,
                std::uint32_t & nonce, bool & restart, bool & has_new_work
            );
        
        protected:
        
            // ...
    };
    
} // miner

#endif // MINER_HASH_HPP
