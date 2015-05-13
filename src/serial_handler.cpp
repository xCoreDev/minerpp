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

#include <miner/serial.hpp>
#include <miner/serial_handler.hpp>
#include <miner/serial_port.hpp>
#include <miner/stratum_work.hpp>
#include <miner/utility.hpp>

using namespace miner;

serial_handler::serial_handler(std::shared_ptr<serial_port> & owner)
    : serial_port_(owner)
{
    // ...
}

void serial_handler::on_read(const char * buf, const std::size_t & len)
{
    assert(0);
}

void serial_handler::set_has_new_work(const bool & val)
{
    if (auto i = serial_port_.lock())
    {
        /**
         * Allocate the big endian data.
         */
        std::uint32_t endiandata[32];
    
        assert(sizeof(endiandata) == 128);

        /**
         * Prepare the work.
         */
        if (prepare_work(endiandata))
        {
			/**
			 * The work length.
			 */
			enum { work_length = 80 };

			/**
			 * Send 80 bytes of endiandata.
			 */
            serial::message_t msg;
            
            msg.type = serial::message_type_new_work;
            msg.length = work_length;
			
            std::vector<std::uint8_t> buffer(2 + work_length);
				
            buffer[0] = msg.type;
            buffer[1] = msg.length;

			std::memcpy(&buffer[2], endiandata, work_length);

            i->write(
                reinterpret_cast<const char *> (&buffer[0]), buffer.size()
            );
        }
    }
}

void serial_handler::set_needs_work_restart(const bool & val)
{
    if (auto i = serial_port_.lock())
    {
        /**
         * Allocate the big endian data.
         */
        std::uint32_t endiandata[32];
    
        assert(sizeof(endiandata) == 128);

        /**
         * Prepare the work.
         */
        if (prepare_work(endiandata))
        {
            serial::message_t msg;
            
            msg.type = serial::message_type_restart;
            msg.length = 0;
            
            std::vector<std::uint8_t> buffer(2);
            
            buffer[0] = msg.type;
            buffer[1] = msg.length;

            i->write(
                reinterpret_cast<const char *> (&buffer[0]), buffer.size()
            );
        }
	}
}

bool serial_handler::prepare_work(std::uint32_t * val)
{
    if (auto i = serial_port_.lock())
    {
        /**
         * Get the work.
         */
        const auto & work = i->work();
        
		/**
		 * Generate the work.
		 */
		if (work && work->generate())
		{
			/**
			 * Prepare the work.
			 */
			if (work->data().size() > 0)
			{
				auto ptr_data = &work->data()[0];

				for (auto kk = 0; kk < 32; kk++)
				{
					utility::be32enc(&val[kk], ((std::uint32_t *)ptr_data)[kk]);
				}

				ptr_data[19] = ++ptr_data[19];

				utility::be32enc(&val[19], ptr_data[19]);
            
				return true;
			}
		}
    }

    return false;
}

void serial_handler::send_test_work()
{
    if (auto i = serial_port_.lock())
    {
		/**
		 * The work length.
		 */
		enum { work_length = 80 };

        serial::message_t msg;
        
        msg.type = serial::message_type_test_work;
        msg.length = work_length;
            
        std::vector<std::uint8_t> buffer(2 + work_length, 0);
            
        buffer[0] = msg.type;
        buffer[1] = msg.length;

		/**
		 * Test primes.
		 */
		buffer[7] = 7;
		buffer[11] = 11;
		buffer[13] = 13;
		buffer[17] = 17;
		buffer[19] = 19;
		buffer[23] = 23;
		buffer[29] = 29;
		buffer[31] = 31;
		buffer[37] = 37;
		buffer[41] = 41;
		buffer[43] = 43;
		buffer[47] = 47;
		buffer[53] = 53;
		buffer[59] = 59;
		buffer[61] = 61;
		buffer[67] = 67;
		buffer[71] = 71;
		buffer[73] = 73;
		buffer[79] = 79;

        i->write(
            reinterpret_cast<const char *> (&buffer[0]), buffer.size()
        );
	}
}
