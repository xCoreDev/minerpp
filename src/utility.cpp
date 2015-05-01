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

#include <cctype>

#include <miner/utility.hpp>

using namespace miner;

std::vector<std::uint8_t> utility::from_hex(const std::string & val)
{
    std::vector<std::uint8_t> ret;
    
    static const std::int8_t g_hex_digit[256] =
    {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
        -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    };

    const char * ptr = val.c_str();

    for (;;)
    {
        while (std::isspace(*ptr))
        {
            ptr++;
        }
        
        std::int8_t c = g_hex_digit[static_cast<std::uint8_t> (*ptr++)];
        
        if (c == (std::int8_t)-1)
        {
            break;
        }
        
        std::uint8_t n = (c << 4);
        
        c = g_hex_digit[static_cast<std::uint8_t> (*ptr++)];
        
        if (c == static_cast<std::int8_t> (-1))
        {
            break;
        }
        
        n |= c;
    
        ret.push_back(n);
    }
    
    return ret;
}

std::string utility::to_hex(
    const std::vector<std::uint8_t> & bytes, const bool & spaces
    )
{
    return to_hex(bytes.begin(), bytes.end(), spaces);
}

void utility::le32enc(void * val, std::uint32_t x)
{
    std::uint8_t * p = (std::uint8_t *)val;
    
    p[0] = x & 0xff;
    p[1] = (x >> 8) & 0xff;
    p[2] = (x >> 16) & 0xff;
    p[3] = (x >> 24) & 0xff;
}

std::uint32_t utility::le32dec(const void * val)
{
    const std::uint8_t * p = (const std::uint8_t *)val;
    
    return (
        (std::uint32_t)(p[0]) + ((std::uint32_t)(p[1]) << 8) +
        ((std::uint32_t)(p[2]) << 16) + ((std::uint32_t)(p[3]) << 24)
    );
}

void utility::be32enc(void * val, std::uint32_t x)
{
    std::uint8_t * p = (std::uint8_t *)val;
    
    p[3] = x & 0xff;
    p[2] = (x >> 8) & 0xff;
    p[1] = (x >> 16) & 0xff;
    p[0] = (x >> 24) & 0xff;
}

std::uint32_t utility::be32dec(const void * val)
{
	const std::uint8_t * p = (std::uint8_t const *)val;
    
	return
        ((std::uint32_t)(p[3]) + ((std::uint32_t)(p[2]) << 8) +
	    ((std::uint32_t)(p[1]) << 16) + ((std::uint32_t)(p[0]) << 24))
    ;
}

std::uint32_t utility::swab32(std::uint32_t val)
{
    return
        ((((val) << 24) & 0xff000000u) | (((val) << 8) & 0x00ff0000u) |
        (((val) >> 8) & 0x0000ff00u) | (((val) >> 24) & 0x000000ffu))
    ;
}
