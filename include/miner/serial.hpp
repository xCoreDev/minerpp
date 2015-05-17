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

#ifndef MINER_SERIAL_HPP
#define MINER_SERIAL_HPP

#include <cstdint>
#include <vector>

namespace miner {

    namespace serial {

        /**
         * The device models.
         */
        typedef enum device_model_s
        {
            device_model_none,
            device_model_mojov3,
        } device_model_t;
        
        /**
         * The message types.
         */
        typedef enum message_type_s
        {
            message_type_none = 0,
            message_type_ack = 2,
            message_type_nack = 4,
            message_type_ping = 8,
            message_type_info = 18,
            message_type_new_work = 19,
            message_type_restart = 20,
            message_type_test_work = 21,
            message_type_result = 22,
            message_type_error = 0xfe,
        } message_type_t;
        
        /**
         * The message.
         */
        typedef struct message_s
        {
            std::uint8_t type;
            std::uint8_t length;
            std::vector<std::uint8_t> value;
        } message_t;

    } // namespace serial

} // namespace miner

#endif // MINER_SERIAL_HPP
