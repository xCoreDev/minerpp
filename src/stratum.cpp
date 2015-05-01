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

#include <miner/stratum.hpp>

using namespace miner;

stratum::stratum()
    : m_port(0)
    , m_shares_accepted(0)
    , m_shares_rejected(0)
{
    // ...
}

stratum & stratum::instance()
{
    static stratum g_stratum;
    
    return g_stratum;
}

void stratum::set_username(const std::string & val)
{
    m_username = val;
}

const std::string & stratum::username() const
{
    return m_username;
}

void stratum::set_password(const std::string & val)
{
    m_password = val;
}

const std::string & stratum::password() const
{
    return m_password;
}

void stratum::set_host(const std::string & val)
{
    m_host = val;
}

const std::string & stratum::host() const
{
    return m_host;
}

void stratum::set_port(const std::uint16_t & val)
{
    m_port = val;
}

const std::uint16_t & stratum::port() const
{
    return m_port;
}

void stratum::set_shares_accepted(const std::uint32_t & val)
{
    m_shares_accepted = val;
}

const std::uint32_t & stratum::shares_accepted() const
{
    return m_shares_accepted;
}

void stratum::set_shares_rejected(const std::uint32_t & val)
{
    m_shares_rejected = val;
}

const std::uint32_t & stratum::shares_rejected() const
{
    return m_shares_rejected;
}
