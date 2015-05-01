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

#include <miner/configuration.hpp>
#include <miner/logger.hpp>
#include <miner/serial_handler.hpp>
#include <miner/serial_handler_mojov3.hpp>
#include <miner/serial_port.hpp>
#include <miner/stack_impl.hpp>
#include <miner/stratum_work.hpp>

using namespace miner;

serial_port::serial_port(stack_impl & owner)
    : m_serial_port(owner.io_service())
    , m_device_model(serial::device_model_none)
    , m_hashes_per_second(0.0)
    , stack_impl_(owner)
    , strand_(owner.io_service())
    , timer_(owner.io_service())
{
    // ...
}

serial_port::~serial_port()
{
    log_debug("~serial_port")
}

bool serial_port::open(const std::string & device)
{
    /**
     * Set the device.
     */
    m_device = device;

    assert(
        configuration::instance().device_type() ==
        configuration::device_type_serial
    );
    
    assert(m_device_model != serial::device_model_none);
    
    try
    {
        assert(m_device.size());
    
        m_serial_port.open(m_device);
    
        m_serial_port.set_option(
            boost::asio::serial_port_base::baud_rate(115200)
        );
        m_serial_port.set_option(
            boost::asio::serial_port_base::character_size(8)
        );
        m_serial_port.set_option(
            boost::asio::serial_port_base::flow_control(
            boost::asio::serial_port_base::flow_control::none)
        );
        m_serial_port.set_option(
            boost::asio::serial_port_base::parity(
            boost::asio::serial_port_base::parity::none)
        );
        m_serial_port.set_option(
            boost::asio::serial_port_base::stop_bits(
            boost::asio::serial_port_base::stop_bits::one)
        );
        
        /**
         * Configure (the default) handler.
         */
        if (m_device_model == serial::device_model_mojov3)
        {
            /**
             * Allocate the handler per the model of device.
             */
            m_serial_handler = std::make_shared<serial_handler_mojov3> (
                shared_from_this()
            );
        }
        else
        {
            /**
             * :TODO: Add handlers for other FPGA device models.
             */
        }
        
        /**
         * Start reading.
         */
        do_read();
    }
    catch (std::exception & e)
    {
        log_error("Serial port open failed, what = " << e.what() << ".");
        
        return false;
    }
    
    timer_.expires_from_now(std::chrono::seconds(8));
    timer_.async_wait(std::bind(
        &serial_port::tick, shared_from_this(), std::placeholders::_1)
    );
    
    return true;
}

void serial_port::close()
{
    m_serial_port.close();
    
    timer_.cancel();
}

void serial_port::write(const char * buf, const std::size_t & len)
{
    auto self(shared_from_this());
    
    std::vector<char> buffer(buf, buf + len);
    
    stack_impl_.io_service().post(strand_.wrap(
        [this, self, buffer] ()
        {
            bool write_in_progress = write_queue_.size() > 0;

            write_queue_.push_back(buffer);
          
            if (write_in_progress == false)
            {
                do_write(
                    &write_queue_.front()[0], write_queue_.front().size()
                );
            }
        })
    );
}

void serial_port::set_device_model(const serial::device_model_t & val)
{
    m_device_model = val;
}

void serial_port::set_work(const std::shared_ptr<stratum_work> & val)
{
    log_info(
        "Serial port " << this << " got new work, size = " <<
        val->data().size() << "."
    );

    /**
     * If true we have new stratum_work.
     */
    auto has_new_work = false;
    
    /**
     * If true we need to restart.
     */
    auto needs_work_restart = false;
    
    /**
     * If the work is null we need to restart.
     */
    if (val == 0)
    {
        needs_work_restart = true;

        mutex_work_.lock();
        
        m_work = val;
        
        mutex_work_.unlock();
    }
    else
    {
        mutex_work_.lock();
        
        if (m_work == 0 || m_work->job_id() != val->job_id())
        {
            /**
             * Set the work it will then be picked up in the next hashing loop.
             */
            m_work = val;
            
            has_new_work = true;
        }
        else
        {
            /**
             * Set the work it will then be picked up in the next hashing loop.
             */
            m_work = val;
        }
        
        mutex_work_.unlock();
    }
    
    if (has_new_work)
    {
        if (m_serial_handler)
        {
            m_serial_handler->set_has_new_work(true);
        }
    }
    else if (needs_work_restart)
    {
        if (m_serial_handler)
        {
            m_serial_handler->set_needs_work_restart(true);
        }
    }
}

std::shared_ptr<stratum_work> serial_port::work()
{
    std::lock_guard<std::mutex> l1(mutex_work_);
    
    return m_work ? std::make_shared<stratum_work> (*m_work) : 0;
}

const double & serial_port::hashes_per_second() const
{
    return m_hashes_per_second;
}

void serial_port::do_read()
{
    auto self(shared_from_this());

    m_serial_port.async_read_some(boost::asio::buffer(read_buffer_),
        [this, self](boost::system::error_code ec, std::size_t len)
        {
            if (ec)
            {
                log_error(
                    "Serial port read failed, message = " << ec.message() << "."
                );
                
                /**
                 * Close
                 */
                m_serial_port.close();
            }
            else
            {
                if (m_serial_handler)
                {
                    m_serial_handler->on_read(read_buffer_, len);
                }
                
                /**
                 * Start reading.
                 */
                do_read();
            }
        }
    );
}

void serial_port::do_write(const char * buf, const std::size_t & len)
{
    auto self(shared_from_this());
    
    m_serial_port.async_write_some(
        boost::asio::buffer(buf, len),
        [this, self](boost::system::error_code ec,
        std::size_t bytes_transferred)
        {
            if (ec)
            {
                log_error(
                    "Serial port write failed, message = " << ec.message() << "."
                );
                
                /**
                 * Close
                 */
                m_serial_port.close();
            }
            else
            {
                write_queue_.pop_front();
                
                if (write_queue_.size() == 0)
                {
                    // ...
                }
                else
                {
                    do_write(
                        &write_queue_.front()[0], write_queue_.front().size()
                    );
                }
            }
        }
    );
}

void serial_port::tick(const boost::system::error_code & ec)
{
    if (ec)
    {
        // ...
    }
    else
    {
        log_debug("serial_port is_open = " << m_serial_port.is_open());
    
        if (m_serial_port.is_open() == false)
        {
            log_debug("Serial port closed, closing.");
        
            close();
        }
        else
        {
            timer_.expires_from_now(std::chrono::seconds(8));
            timer_.async_wait(std::bind(
                &serial_port::tick, shared_from_this(), std::placeholders::_1)
            );
        }
    }
}
