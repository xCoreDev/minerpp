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

#include <miner/hash.hpp>
#include <miner/logger.hpp>
#include <miner/utility.hpp>
#include <miner/whirlpool.hpp>

using namespace miner;

bool check_hash(const std::uint32_t * hash, const std::uint32_t * target)
{
	bool ret = true;
	
    for (auto i = 7; i >= 0; i--)
    {
        if (hash[i] > target[i])
        {
            ret = false;
            
            break;
        }
        
        if (hash[i] < target[i])
        {
            ret = true;
            
            break;
        }
    }

	if (true)
    {
		std::uint32_t hash_be[8];
        
        std::uint32_t target_be[8];

		for (auto i = 0; i < 8; i++)
        {
			utility::be32enc(&hash_be[i], hash[7 - i]);
			utility::be32enc(&target_be[i], target[7 - i]);
		}
        
        log_info(
            "Hash check: \n\thash: " <<
            utility::to_hex(hash_be, hash_be + 32) << "\n\t" << "target: " <<
            utility::to_hex(target_be, target_be + 32) <<
            "\n\tresult: " << (ret ? "good" : "bad")
        );
	}

	return ret;
}

bool hash::scan(
    const configuration::proof_of_work_type_t & type,
    std::uint32_t * ptr_data, const std::uint32_t * ptr_target,
    const std::uint32_t & max_nonce, std::uint64_t & hashes_done,
    std::uint32_t & nonce, bool & restart, bool & has_new_work
    )
{
    if (type == configuration::proof_of_work_type_whirlpool_xor)
    {
        return scan_whirlpool_xor(
            ptr_data, ptr_target, max_nonce, hashes_done, nonce, restart,
            has_new_work
        );
    }

    log_error("Hash got invalid Proof-of-Work type = " << type << ".");
    
    return false;
}

bool hash::scan_whirlpool_xor(
    std::uint32_t * ptr_data, const std::uint32_t * ptr_target,
    const std::uint32_t & max_nonce, std::uint64_t & hashes_done,
    std::uint32_t & nonce, bool & restart, bool & has_new_work
    )
{
    nonce = ptr_data[19] - 1;
    
    const std::uint32_t first_nonce = ptr_data[19];

    std::uint32_t hash64[16];
    
    std::uint32_t endiandata[32];

    assert(sizeof(endiandata) == 128);
    
    for (auto kk = 0; kk < 32; kk++)
    {
        utility::be32enc(&endiandata[kk], ((std::uint32_t *)ptr_data)[kk]);
    }

    do
    {
        ptr_data[19] = ++nonce;
 
        utility::be32enc(&endiandata[19], ptr_data[19]);
        
        whirlpool_xor(
            reinterpret_cast<std::uint8_t *> (&endiandata[0]), 80,
            reinterpret_cast<std::uint8_t *> (&hash64[0])
        );
 
        /**
         * First check.
         */
        if ((hash64[7] & 0xFFFFFF00) == 0 && check_hash(hash64, ptr_target))
        {
            hashes_done = nonce - first_nonce + 1;

            return true;
        }
        
    } while (nonce < max_nonce && restart == false && has_new_work == false);
    
    hashes_done = nonce - first_nonce + 1;
    
    ptr_data[19] = nonce;
    
    return false;
}
