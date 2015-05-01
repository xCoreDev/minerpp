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

#include <cassert>

#include <miner/sha256.hpp>
#include <miner/stratum_work.hpp>
#include <miner/utility.hpp>

using namespace miner;

stratum_work::stratum_work(
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
    )
    : m_worker_name(worker_name)
    , m_job_id(job_id)
    , m_extranonce1(extranonce1)
    , m_extranonce2(extranonce2)
    , m_previous_hash_bytes(previous_hash_bytes)
    , m_coinb1_bytes(coinb1_bytes)
    , m_coinb2_bytes(coinb2_bytes)
    , m_merkles(merkles)
    , m_version_bytes(version_bytes)
    , m_bits_bytes(bits_bytes)
    , m_time(time_bytes)
{
    m_target.insert(
        m_target.begin(), target, target + sizeof(std::uint32_t) * 8
    );
    
    assert(m_target.size() == 32);
}

bool stratum_work::generate()
{
    /**
     * Allocate the coinbase.
     */
    std::vector<std::uint8_t> coinbase;

    /**
     * Concatenate coinb1 + m_extranonce1 + m_extranonce2 + coinb2.
     */
    coinbase.insert(
        coinbase.end(), m_coinb1_bytes.begin(), m_coinb1_bytes.end()
    );
    coinbase.insert(
        coinbase.end(), m_extranonce1.begin(), m_extranonce1.end()
    );
    coinbase.insert(
        coinbase.end(), m_extranonce2.begin(), m_extranonce2.end()
    );
    coinbase.insert(
        coinbase.end(), m_coinb2_bytes.begin(), m_coinb2_bytes.end()
    );

    /**
     * Generate merkle root.
     */
    std::vector<std::uint8_t> digest_merkle_root(64, 0);

    sha256d(&coinbase[0], coinbase.size(), &digest_merkle_root[0]);

    for (auto & i : m_merkles)
    {
        auto merkle_bytes = utility::from_hex(i);
        
        assert(merkle_bytes.size() == 32);
        
        std::memcpy(
            &digest_merkle_root[0] + 32, merkle_bytes.data(),
            merkle_bytes.size()
        );
        
        sha256d(&digest_merkle_root[0], 64, &digest_merkle_root[0]);
    }

    /**
     * Assemble the block header.
     */
    m_data.resize(32, 0);

    m_data[0] = utility::le32dec(&m_version_bytes[0]);

    for (auto i = 0; i < 8; i++)
    {
        m_data[1 + i] =
            utility::le32dec((std::uint32_t *)&m_previous_hash_bytes[0] + i)
        ;
    }

    for (auto i = 0; i < 8; i++)
    {
        m_data[9 + i] =
            utility::be32dec((std::uint32_t *)&digest_merkle_root[0] + i)
        ;
    }

    m_data[17] = utility::le32dec(&m_time[0]);
    
    m_data[18] = utility::le32dec(&m_bits_bytes[0]);
    
    m_data[20] = 0x80000000;
    m_data[31] = 0x00000280;

#if 0
    /**
     * Increment extranonce2.
     */
    for (auto i = 0; i < m_extranonce2.size(); i++)
    {
        if (++m_extranonce2[i])
        {
            break;
        }
    }
#endif

    return true;
}

const std::string & stratum_work::worker_name() const
{
    return m_worker_name;
}

const std::string & stratum_work::job_id() const
{
    return m_job_id;
}

const std::vector<std::uint8_t> & stratum_work::extranonce2() const
{
    return m_extranonce2;
}

const std::vector<std::uint8_t> & stratum_work::time() const
{
    return m_time;
}

std::vector<std::uint32_t> & stratum_work::data()
{
    return m_data;
}

const std::uint32_t * stratum_work::target() const
{
    return &m_target[0];
}
