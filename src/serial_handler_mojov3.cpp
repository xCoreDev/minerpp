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

using namespace miner;

serial_handler_mojov3::serial_handler_mojov3(
    std::shared_ptr<serial_port> owner
    )
    : serial_handler(owner)
    , state_(state_none)
    , timer_timeout_(owner->io_service())
{
    // ...
}

void serial_handler_mojov3::start()
{
    log_debug("Serial handler MojoV3 is starting.");

    state_ = state_starting;

	/**
	 * Get the info from the device to make sure it is a MojoV3.
	 */
    if (auto i = serial_port_.lock())
    {
        /**
         * Start the timeout timer before sending the serial::message_type_info.
         */
        timer_timeout_.expires_from_now(std::chrono::seconds(8));
        timer_timeout_.async_wait(
            strand_.wrap(
                [this](boost::system::error_code ec)
                {
                    if (ec)
                    {
                        // ...
                    }
                    else
                    {
                        log_info(
                            "Serial handler MojoV3 device failed to "
                            "acknowledge info request, calling stop."
                        );
                        
                        /**
                         * Call stop.
                         */
                        stop();
                    }
                }
            )
        );
        
		serial::message_t msg2;
            
		msg2.type = serial::message_type_info;
		msg2.length = 0;
            
		std::vector<std::uint8_t> buffer(2);
            
		buffer[0] = msg2.type;
		buffer[1] = msg2.length;

		i->write(
			reinterpret_cast<const char *> (&buffer[0]),
			buffer.size()
		);
	}
    
    state_ = state_started;
}

void serial_handler_mojov3::stop()
{
    if (state_ == state_starting || state_ == state_started)
    {
        log_debug("Serial handler MojoV3 is stopping.");
        
        state_ = state_stopping;
        
        /**
         * Cancel the timeout timer.
         */
        timer_timeout_.cancel();
        
        /**
         * Close the serial_port last.
         */
        if (auto i = serial_port_.lock())
        {
            i->close();
        }

        state_ = state_stopped;
    }
}

void serial_handler_mojov3::on_read(const char * buf, const std::size_t & len)
{
    log_debug(
        "Serial handler MojoV3 read  = " << buf << ", len = " << len << "."
    );

#define USE_READ_BUFFER 1

	if (len > 0)
	{
#if (defined USE_READ_BUFFER && USE_READ_BUFFER)
		read_buffer_.insert(read_buffer_.end(), buf, buf + len);

		if (read_buffer_.size() < 2)
		{
			return;
		}

		serial::message_t msg;
        
		std::memcpy(&msg, &read_buffer_[0], sizeof(std::uint8_t) * 2);

		auto remaining = read_buffer_.size() - sizeof(std::uint8_t) * 2;

		if (remaining >= msg.length)
		{
			if (msg.length > 0)
			{
				msg.value.resize(msg.length);

				read_buffer_.erase(read_buffer_.begin());
				read_buffer_.erase(read_buffer_.begin());

				std::memcpy(
					&msg.value[0], &read_buffer_[0], msg.length
				);

				read_buffer_.erase(
					read_buffer_.begin(), read_buffer_.begin() + msg.length
				);
			}
			else
			{
				read_buffer_.erase(read_buffer_.begin());
				read_buffer_.erase(read_buffer_.begin());
			}
		}
		else
		{
			log_error(
				"Serial handler MojoV3 got invalid length " <<
				(unsigned)msg.length << " in message, remaining = " <<
				remaining << "."
			);

			return;
		}
#else
		serial::message_t msg;
        
		std::memcpy(&msg, buf, sizeof(std::uint8_t) * 2);

		if (msg.length > 0)
		{
			msg.value.resize(msg.length);

			if (msg.length == len - sizeof(std::uint8_t) * 2)
			{
				std::memcpy(
					&msg.value[0], buf + sizeof(std::uint8_t) * 2,
					len - sizeof(std::uint8_t) * 2
				);
			}
			else
			{
				log_error(
					"Serial handler MojoV3 got invalid length in message."
				);
			}
		}
#endif // USE_READ_BUFFER
		switch (msg.type)
		{
			case serial::message_type_ack:
			{
				log_debug("got ack, msg.length = " << (unsigned)msg.length);
			}
			break;
			case serial::message_type_nack:
			{
				log_debug("got nack, msg.length = " << (unsigned)msg.length);
			}
			break;
			case serial::message_type_info:
			{
				log_debug(
					"got message_type_info, msg.length = " <<
					(unsigned)msg.length
				);

				if (handle_info(msg))
				{
					log_info("Serial handler MojoV3 confirmed device is MoV3.");
                    
                    /**
                     * Cancel the timeout timer.
                     */
                    timer_timeout_.cancel();
				}
				else
				{
					log_error(
                        "Serial handler MojoV3 handle info failed, "
                        "calling stop."
                    );

                    /**
                     * Call stop.
                     */
                    stop();
				}
			}
			break;
			case serial::message_type_test_work:
			{
				log_debug(
					"got message_type_test_work, msg.length = " <<
					(unsigned)msg.length
				);
			}
			break;
			case serial::message_type_result:
			{
				log_debug(
					"got message_type_result, msg.length = " <<
					(unsigned)msg.length
				);
			}
			break;
			case serial::message_type_error:
			{
				log_debug(
					"got message_type_error, msg.length = " <<
					(unsigned)msg.length
				);
			}
			break;
			default:
			{
				log_debug("got " <<  (int)msg.type);
			}
			break;
		}
	}
}

void serial_handler_mojov3::set_has_new_work(const bool & val)
{
	/**
	 * Inform the base class.
	 */
	serial_handler::set_has_new_work(val);

    log_debug("Serial handler MojoV3 has new work.");
}

void serial_handler_mojov3::set_needs_work_restart(const bool & val)
{
	/**
	 * Inform the base class.
	 */
	serial_handler::set_needs_work_restart(val);

    log_debug("Serial handler MojoV3 needs work restart.");
}

bool serial_handler_mojov3::handle_info(const serial::message_t & msg)
{
	return
		msg.value[0] == 'M' && msg.value[1] == 'o' &&
		msg.value[2] == 'V' && msg.value[3] == '3'
	;
}
