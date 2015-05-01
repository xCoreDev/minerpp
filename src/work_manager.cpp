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
#include <miner/stack_impl.hpp>
#include <miner/stratum.hpp>
#include <miner/stratum_connection.hpp>
#include <miner/stratum_work.hpp>
#include <miner/utility.hpp>
#include <miner/work_manager.hpp>

using namespace miner;

work_manager::work_manager(stack_impl & owner)
    : stack_impl_(owner)
    , strand_(owner.io_service())
    , timer_(owner.io_service())
    , timer_check_work_hosts_(owner.io_service())
    , work_host_index_(0)
{
    // ...
}

void work_manager::start()
{
    if (configuration::instance().work_hosts().size() > 0)
    {
        log_info(
            "Work manager is switching to (primary) work host " <<
            work_host_index_ << "."
        );
        
        stratum::instance().set_host(
            configuration::instance().work_hosts()[
            work_host_index_].second.first
        );
        stratum::instance().set_port(
            configuration::instance().work_hosts()[
            work_host_index_].second.second
        );
        stratum::instance().set_username(
            configuration::instance().work_hosts()[
            work_host_index_].first.first
        );
        stratum::instance().set_password(
            configuration::instance().work_hosts()[
            work_host_index_].first.second
        );
    }
    else
    {
        throw std::runtime_error("no work hosts");
    }
    
    timer_.expires_from_now(std::chrono::seconds(8));
    timer_.async_wait(std::bind(
        &work_manager::tick, this, std::placeholders::_1)
    );
    
    /**
     * Connect to a work server.
     */
    connect();
}

void work_manager::stop()
{
    timer_.cancel();
    timer_check_work_hosts_.cancel();
    
    for (auto & i : stratum_connections_)
    {
        if (auto j = i.lock())
        {
            j->stop();
        }
    }
}

void work_manager::set_work(const std::shared_ptr<stratum_work> & val)
{
    m_work = val;
    
    auto self(shared_from_this());
    
    stack_impl_.io_service().post(strand_.wrap(
        [this, self] ()
        {
            stack_impl_.handle_work(m_work);
        })
    );
}

void work_manager::submit_work(const std::shared_ptr<stratum_work> & val)
{
    /**
     * Update the statistics.
     */
    stack_impl_.update_statistics();
    
    auto time = utility::to_hex(val->time());
    
    std::uint32_t nonce_little = utility::le32dec(&val->data()[19]);

    auto nonce = utility::to_hex(
        reinterpret_cast<std::uint8_t *>(&nonce_little),
        reinterpret_cast<std::uint8_t *>(&nonce_little) + sizeof(std::uint32_t)
    );
    
    auto extranonce2 = utility::to_hex(val->extranonce2());

    std::string json_line =
        "{\"params\": [\"" + val->worker_name() +
        "\", \"" + val->job_id() + "\", \"" + extranonce2 + "\", \"" + time +
        "\", \"" + nonce + "\"], \"id\": 4, \"method\": \"mining.submit\"}\n"
    ;

    for (auto & i : stratum_connections_)
    {
        if (auto j = i.lock())
        {
            j->write(json_line);
        }
    }
}

void work_manager::connect()
{
    log_info("Work manager is connecting.");
    
    /**
     * Allocate a stratum_connection.
     */
    auto c = std::make_shared<stratum_connection> (stack_impl_);
    
    /**
     * Retain the stratum_connection.
     */
    stratum_connections_.push_back(c);
    
    /**
     * Start the stratum_connection.
     */
    c->start();
}

void work_manager::tick(const boost::system::error_code & ec)
{
    if (ec)
    {
        // ...
    }
    else
    {
        /**
         * The timer timeout in seconds.
         */
        auto timeout = 8;
        
        /**
         * If the connections are empty we need a reconnect.
         */
        bool needs_reconnect = stratum_connections_.size() == 0;
        
        auto connections = 0;
        
        for (auto & i : stratum_connections_)
        {
            if (auto j = i.lock())
            {
                connections++;
            }
        }
        
        log_debug("Work manager tick, connections = " << connections << ".");
        
        if (needs_reconnect || connections == 0)
        {
            log_error("Work manager disconnected.");

            /**
             * Abort work.
             */
            set_work(0);

            /**
             * Try to set a backup work hosts if possible.
             */
            if (
                configuration::instance().work_hosts().size() >
                work_host_index_ + 1
                )
            {
                /**
                 * Increment the work index.
                 */
                ++work_host_index_;
                
                log_info(
                    "Work manager is switching to (backup) work host " <<
                    work_host_index_ << "."
                );
                
                /**
                 * Start the check work hosts timer.
                 */
                timer_check_work_hosts_.expires_from_now(
                    std::chrono::seconds(60)
                );
                timer_check_work_hosts_.async_wait(std::bind(
                    &work_manager::tick_check_work_hosts, this,
                    std::placeholders::_1)
                );
                
                stratum::instance().set_host(
                    configuration::instance().work_hosts()[
                    work_host_index_].second.first
                );
                stratum::instance().set_port(
                    configuration::instance().work_hosts()[
                    work_host_index_].second.second
                );
                stratum::instance().set_username(
                    configuration::instance().work_hosts()[
                    work_host_index_].first.first
                );
                stratum::instance().set_password(
                    configuration::instance().work_hosts()[
                    work_host_index_].first.second
                );
        
                /**
                 * Connect to a work server.
                 */
                connect();
            }
            else
            {
                log_info(
                    "Work manager (primary) work host " <<
                    work_host_index_ << " is down, will retry."
                );
                
                /**
                 * Start the check work hosts timer.
                 */
                timer_check_work_hosts_.expires_from_now(
                    std::chrono::seconds(60)
                );
                timer_check_work_hosts_.async_wait(std::bind(
                    &work_manager::tick_check_work_hosts, this,
                    std::placeholders::_1)
                );
            }
            
            /**
             * Set the timeout to N to allow for work host check to complete.
             */
            timeout = 90;
            
            assert(timeout > 60);
        }

        /**
         * Update the statistics.
         */
        stack_impl_.update_statistics();
    
        timer_.expires_from_now(std::chrono::seconds(timeout));
        timer_.async_wait(std::bind(
            &work_manager::tick, this, std::placeholders::_1)
        );
    }
}

void work_manager::tick_check_work_hosts(const boost::system::error_code & ec)
{
    if (ec)
    {
        // ...
    }
    else
    {
        boost::asio::ip::tcp::iostream tcp_stream;
        
        /**
         * Check connectivity.
         */
        tcp_stream.connect(
            configuration::instance().work_hosts()[0].second.first,
            std::to_string(configuration::instance().work_hosts()[
            0].second.second)
        );
        
        if (!tcp_stream || tcp_stream.error())
        {
            log_info(
                "Work manager (primary) work host check failed, will retry."
            );
            
            /**
             * Start the check work hosts timer.
             */
            timer_check_work_hosts_.expires_from_now(
                std::chrono::seconds(60)
            );
            timer_check_work_hosts_.async_wait(std::bind(
                &work_manager::tick_check_work_hosts, this,
                std::placeholders::_1)
            );
        }
        else
        {
            log_info(
                "Work manager (primary) work host check success."
            );
            
            /**
             * Close the boost::asio::ip::tcp::iostream.
             */
            tcp_stream.close();
            
            /**
             * Disconnect all backup work hosts.
             */
            for (auto & i : stratum_connections_)
            {
                if (auto j = i.lock())
                {
                    j->stop();
                }
            }
            
            /**
             * Reset the work index.
             */
            work_host_index_ = 0;
        
            log_info(
                "Work manager is switching to (primary) work host " <<
                work_host_index_ << "."
            );

            stratum::instance().set_host(
                configuration::instance().work_hosts()[
                work_host_index_].second.first
            );
            stratum::instance().set_port(
                configuration::instance().work_hosts()[
                work_host_index_].second.second
            );
            stratum::instance().set_username(
                configuration::instance().work_hosts()[
                work_host_index_].first.first
            );
            stratum::instance().set_password(
                configuration::instance().work_hosts()[
                work_host_index_].first.second
            );
    
            /**
             * Connect to a work server.
             */
            connect();
        }
    }
}
