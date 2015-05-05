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

#include <miner/logger.hpp>
#include <miner/serial_port.hpp>
#include <miner/serial_handler_mojov3.hpp>
#include <miner/stratum_work.hpp>
#include <miner/utility.hpp>

using namespace miner;

serial_handler_mojov3::serial_handler_mojov3(
    std::shared_ptr<serial_port> owner
    )
    : serial_handler(owner)
{
    log_debug("Serial handler MojoV3 allocated.");
}

void serial_handler_mojov3::on_read(const char * buf, const std::size_t & len)
{
    log_debug(
        "Serial handler MojoV3 read  = " << buf << ", len = " << len << "."
    );
    
    // :TODO: Check the resulting hash:nonce.
}

void serial_handler_mojov3::set_has_new_work(const bool & val)
{
    log_debug("Serial handler MojoV3 sending test work.");
    
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
            // :TODO: Encode and send 80 bytes of endiandata.
            
            const char * test0007 = "hello\r";
            
            i->write(test0007, strlen(test0007));
        }
    }
}

void serial_handler_mojov3::set_needs_work_restart(const bool & val)
{
    log_debug("Serial handler MojoV3 sending test work.");
    
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
            // :TODO: Encode and send 80 bytes of endiandata.
            
            const char * test0007 = "hello\r";
            
            i->write(test0007, strlen(test0007));
        }
    }
}

bool serial_handler_mojov3::prepare_work(std::uint32_t * val)
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
