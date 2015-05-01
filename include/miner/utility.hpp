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

#ifndef MINER_UTILITY_HPP
#define MINER_UTILITY_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace miner {

    /**
     * Implements utilities.
     */
    class utility
    {
        public:
        
            /**
             * Converts from hexidecimal format.
             * @param val The value.
             */
            static std::vector<std::uint8_t> from_hex(const std::string & val);
        
            /**
             * hex_string
             */
            template<typename T>
            static inline std::string to_hex(
                const T it_begin, const T it_end, const bool & spaces = false
                )
            {
                std::string ret;
                
                static const char hexmap[16] =
                {
                    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                    'a', 'b', 'c', 'd', 'e', 'f'
                };
                
                ret.reserve((it_end - it_begin) * 3);
                
                for (auto it = it_begin; it < it_end; ++it)
                {
                    auto val = static_cast<std::uint8_t> (*it);
                    
                    if (spaces && it != it_begin)
                    {
                        ret.push_back(' ');
                    }
                    
                    ret.push_back(hexmap[val >> 4]);
                    
                    ret.push_back(hexmap[val & 15]);
                }

                return ret;
            }

            /**
             * Converts an array of bytes into a hexidecimal string
             * representation.
             * @param bytes The bytes.
             * @param spaces If true the encoding will including spaces.
             */
            static std::string to_hex(
                const std::vector<std::uint8_t> & bytes,
                const bool & spaces = false
            );
            /**
             * Performs le32enc.
             * @param val The value.
             * @param x The x.
             */
            static void le32enc(void * val, std::uint32_t x);
        
            /**
             * Performs le32dec.
             * @param val The value.
             */
            static std::uint32_t le32dec(const void * val);

            /**
             * Performs be32enc.
             * @param val The value.
             * @param x The x.
             */
            static void be32enc(void * val, std::uint32_t x);

            /**
             * Performs be32dec.
             * @param val The value.
             */
            static std::uint32_t be32dec(const void * val);
        
            /**
             * Performs swab32.
             * @param val The value.
             */
            static std::uint32_t swab32(std::uint32_t val);

        private:
        
            // ...
        
        protected:
        
            // ...
    };
    
} // miner

#endif // MINER_UTILITY_HPP
