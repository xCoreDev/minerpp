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

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <miner/logger.hpp>
#include <miner/sha256.hpp>
#include <miner/stack_impl.hpp>
#include <miner/statistics.hpp>
#include <miner/stratum.hpp>
#include <miner/stratum_connection.hpp>
#include <miner/stratum_work.hpp>
#include <miner/utility.hpp>
#include <miner/work_manager.hpp>

using namespace miner;

stratum_connection::stratum_connection(stack_impl & owner)
    : m_next_difficulty(1.0f)
    , stack_impl_(owner)
    , strand_(stack_impl_.io_service())
    , timeout_timer_(stack_impl_.io_service())
{
    // ...
}

void stratum_connection::start()
{
    response_.reset(new boost::asio::streambuf());

    /**
     * Allocate the socket.
     */
    socket_.reset(new boost::asio::ip::tcp::socket(stack_impl_.io_service()));
    
    auto self(shared_from_this());
    
    timeout_timer_.expires_from_now(std::chrono::seconds(8));
    timeout_timer_.async_wait(
        strand_.wrap(
            [this, self](boost::system::error_code ec)
            {
                if (ec)
                {
                    // ...
                }
                else
                {
                    if (socket_)
                    {
                        socket_->close();
                    }
                    
                    log_info("Stratum connection timed out.");
                }
            }
        )
    );
    
    try
    {
        boost::asio::ip::tcp::resolver resolver(stack_impl_.io_service());
        boost::asio::ip::tcp::resolver::query query(
            stratum::instance().host(),
            std::to_string(stratum::instance().port())
        );
        
        do_connect(resolver.resolve(query));
    }
    catch (std::exception & e)
    {
        log_debug(
            "Http transport resolve failed, what = " << e.what() << "."
        );

        timeout_timer_.cancel();
        
        if (socket_)
        {
            socket_->close();
        }
    }
}

void stratum_connection::stop()
{
    timeout_timer_.cancel();
    
    if (socket_)
    {
        socket_->close();
    }
}

void stratum_connection::write(const std::string & buf)
{
    auto self(shared_from_this());

    stack_impl_.io_service().post(strand_.wrap(
        [this, self, buf] ()
        {
            bool write_in_progress = write_queue_.size() > 0;

            write_queue_.push_back(buf);
          
            if (write_in_progress == false)
            {
                do_write(write_queue_.front());
            }
        })
    );
}

void stratum_connection::do_connect(
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator
    )
{
    auto self(shared_from_this());
    
    log_debug(
        "Stratum connection is connecting to " << endpoint_iterator->endpoint()
    );
    
    boost::asio::async_connect(*socket_, endpoint_iterator,
        [this, self, endpoint_iterator](boost::system::error_code ec,
        boost::asio::ip::tcp::resolver::iterator)
        {
            if (ec)
            {
                log_error(
                    "Stratum connection to " <<
                    endpoint_iterator->endpoint() << " failed, message = " <<
                    ec.message() << "."
                );
                
                /**
                 * Cancel the timeout timer.
                 */
                timeout_timer_.cancel();
                
                /**
                 * Close the socket.
                 */
                socket_->close();
            }
            else
            {
                log_debug("HTTP transport connected, sending request.");
                
                /**
                 * Cancel the timeout timer.
                 */
                timeout_timer_.cancel();
                
                /**
                 * Start reading.
                 */
                do_read();
                
                const auto & username = stratum::instance().username();
                const auto & password = stratum::instance().password();
                
                /**
                 * Write the request.
                 */
                write(
                    "{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": "
                    "[]}\n"
                    "{\"params\": [\"" + username + "\", \"" + password + "\"]"
                    ", \"id\": 2, \"method\": \"mining.authorize\"}\n"
                );
            }
        }
    );
}

void stratum_connection::do_read()
{
    auto self(shared_from_this());
    
    boost::asio::async_read_until(*socket_, *response_.get(), "\n",
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (ec)
            {
                log_error("Stratum connection read failed.");
                
                /**
                 * Close the socket.
                 */
                socket_->close();
            }
            else
            {
                std::istream response_istream(response_.get());

                std::string json_line;
                
                std::getline(response_istream, json_line);

                try
                {
                    /**
                     * Handles the JSON line.
                     */
                    handle_json_line(json_line);
                }
                catch (std::exception & e)
                {
                    log_error(
                        "Stratum connection handle JSON line failed, "
                        "what = " << e.what() << ""
                    );
                }

                /**
                 * Continue reading.
                 */
                do_read();
            }
        }
    );
}

void stratum_connection::do_write(const std::string & buf)
{
    auto self(shared_from_this());

    boost::asio::async_write(*socket_, boost::asio::buffer(buf),
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (ec)
            {
                log_error("Stratum connection write failed.");
            }
            else
            {
                if (write_queue_.size() > 0)
                {
                    write_queue_.pop_front();
                }
                
                if (write_queue_.size() == 0)
                {
                    // ...
                }
                else
                {
                    do_write(write_queue_.front());
                }
            }
        }
    );
}

void stratum_connection::handle_json_line(const std::string & json_line)
{
    std::stringstream ss;

    ss << json_line;

    boost::property_tree::ptree pt;

    try
    {
        read_json(ss, pt);

        std::string method;
        
        try
        {
            method = pt.get_child("method").get<std::string> ("");
        }
        catch (...)
        {
            // ...
        }
        
        boost::property_tree::ptree params;
        
        try
        {
            params = pt.get_child("params");
        }
        catch (...)
        {
            // ...
        }
        
        boost::property_tree::ptree result;
        
        try
        {
            result = pt.get_child("result");
        }
        catch (...)
        {
            // ...
        }
        
        boost::property_tree::ptree error;
        
        try
        {
            error = pt.get_child("error");
        }
        catch (...)
        {
            // ...
        }

        auto id = pt.get_child("id").get<std::string> ("");

        if (method.size() > 0)
        {
            log_debug(
                "Stratum connection got method = " << method <<
                ", params = " << params.size() << ", id = " << id << "."
            );
            
            /**
             * Handle the JSON-RPC error.
             */
            handle_json_rpc_method(id, method, params);
        }
        else if (result.size() > 0)
        {
            log_debug(
                "Stratum connection got result = " <<
                result.size() << ", id = " << id << "."
            );
            
            /**
             * Handle the JSON-RPC result.
             */
            handle_json_rpc_result(id, result);
        }
        else if (error.size() > 0)
        {
            log_debug(
                "Stratum connection got error = " << error.size() <<
                ", id = " << id << "."
            );
            
            /**
             * Handle the JSON-RPC error.
             */
            handle_json_rpc_error(id, error);
        }
        else if (params.size() > 0)
        {
            assert(0);
        }
        else
        {
            /**
             * Check for the mining.authorize or mining.submit id.
             */
            if (id == "2")
            {
                /**
                 * Most likely a result code.
                 */
                if (result.empty() && result.data().empty() == false)
                {
                    if (result.get<bool> (""))
                    {
                        log_info("Stratum connection authorization success.");
                    }
                    else
                    {
                        log_error("Stratum connection authorization failure.");
                    }
                }
            }
            else if (id == "4")
            {
                log_debug("Stratum connection mining.submit result (true).");
                
                handle_mining_submit_result(true);
            }
        }
    }
    catch (std::exception & e)
    {
        log_error(
            "Stratum connection failed to parse JSON-RPC line, what = " <<
            e.what() << "."
        );
    }
}

void stratum_connection::handle_json_rpc_method(
    const std::string & id, const std::string & method,
    const boost::property_tree::ptree & params
    )
{
    try
    {
        if (method == "mining.notify")
        {
            log_info("Stratum connection got mining.notify.");
            
            /**
             * Handle the mining.notify.
             */
            handle_mining_notify(params);
        }
        else if (method == "mining.set_difficulty")
        {
            log_info("Stratum connection got mining.set_difficulty.");
            
            if (params.size() == 1)
            {
                auto diff = params.front().second.get<double> ("");

                if (diff > 0.0f)
                {
                    log_info(
                        "Stratum connection is setting (next) "
                        "difficulty to " << diff << "."
                    );
                    
                    /**
                     * Set the next difficulty.
                     */
                    m_next_difficulty = diff;
                }
            }
        }
    }
    catch (std::exception & e)
    {
        log_error(
            "Stratum connection failed to handle JSON RPC method = " <<
            method << ", what = " << e.what() << "."
        );
    }
}

void stratum_connection::handle_json_rpc_result(
    const std::string & id, const boost::property_tree::ptree & result
    )
{
    if (id == "1")
    {
        auto index = 0;
        
        boost::property_tree::ptree ptree_subscriptions_details;
        boost::property_tree::ptree ptree_extranonce1;
        boost::property_tree::ptree ptree_extranonce2_size;
        
        for (auto & i : result)
        {
            if (index == 0)
            {
                ptree_subscriptions_details = i.second;
            }
            else if (index == 1)
            {
                ptree_extranonce1 = i.second;
            }
            else if (index == 2)
            {
                ptree_extranonce2_size = i.second;
            }
            
            index++;
        }
        
        boost::property_tree::ptree ptree_mining_set_difficulty;
        boost::property_tree::ptree ptree_mining_notify;

        auto index1 = 0;
        
        for (auto & i : ptree_subscriptions_details)
        {
            auto index2 = 0;
            
            for (auto & j : i.second)
            {
                if (index1 == 0 && index2 == 1)
                {
                    ptree_mining_set_difficulty = j.second;
                }
                else if (index1 == 1 && index2 == 1)
                {
                    ptree_mining_notify = j.second;
                }
                
                index2++;
            }
            
            index1++;
        }
        
        /**
         * Set the session id.
         */
        m_session_id =
            ptree_mining_set_difficulty.get<std::string> ("")
        ;
        
        /**
         * :TODO:
         */
        auto mining_notify = ptree_mining_notify.get<std::string> ("");
        
        /**
         * Get the extranonce1.
         */
        auto extranonce1 = ptree_extranonce1.get<std::string> ("");

        /**
         * Set the extranonce1.
         */
        m_extranonce1 = utility::from_hex(extranonce1);

        /**
         * Set the extranonce1 size.
         */
        m_extranonce1_size = m_extranonce1.size();

        /**
         * The extranonce2 size.
         */
        m_extranonce2_size = ptree_extranonce2_size.get<std::size_t> ("");
        
        /**
         * Allocate the extranonce2.
         */
        m_extranonce2.resize(m_extranonce2_size, 0);
    }
    else
    {
        log_error("Stratum connection failed, invalid initial id.");
    }
}

void stratum_connection::handle_json_rpc_error(
    const std::string & id, const boost::property_tree::ptree & error
    )
{
    try
    {
        /**
         * The error code.
         */
        std::int32_t error_code = 0;
        
        /**
         * The error message.
         */
        std::string error_message;
        
        auto index = 0;
        
        for (auto & i : error)
        {
            if (index == 0)
            {
                error_code = i.second.get<std::int32_t> ("");
            }
            else if (index == 1)
            {
                error_message = i.second.get<std::string> ("");
            }
            
            index++;
        }
        
        if (id == "4")
        {
            log_debug(
                "Stratum connection mining.submit result (false), "
                "message = " << error_message << "."
            );
            
            handle_mining_submit_result(false);
        }
    }
    catch (std::exception & e)
    {
        log_error(
            "Stratum connection " << this << " handle JSON RPC failed, "
            "what = " << e.what() << "."
        );
    }
}

void stratum_connection::handle_mining_notify(
    const boost::property_tree::ptree & params
    )
{
    auto index = 0;
    
    std::string job_id;
    std::string previous_hash;
    std::string coinb1;
    std::string coinb2;
    std::vector<std::string> merkles;
    std::string version;
    std::string bits;
    std::string time;
    
    bool clean_jobs = false;
    
    for (auto & i : params)
    {
        switch (index)
        {
            case 0:
            {
                job_id = i.second.get<std::string> ("");
            }
            break;
            case 1:
            {
                previous_hash = i.second.get<std::string> ("");
            }
            break;
            case 2:
            {
                coinb1 = i.second.get<std::string> ("");
            }
            break;
            case 3:
            {
                coinb2 = i.second.get<std::string> ("");
            }
            break;
            case 4:
            {
                /**
                 * Parse the merkle.
                 */
                if (i.second.size() > 0)
                {
                    for (auto & j : i.second)
                    {
                        auto merkle = j.second.get<std::string> ("");
                        
                        merkles.push_back(merkle);
                    }
                }
            }
            break;
            case 5:
            {
                version = i.second.get<std::string> ("");
            }
            break;
            case 6:
            {
                bits = i.second.get<std::string> ("");
            }
            break;
            case 7:
            {
                time = i.second.get<std::string> ("");
            }
            break;
            case 8:
            {
                clean_jobs = i.second.get<bool> ("");
            }
            break;
            default:
            {
                assert(0);
            }
            break;
        }
        
        index++;
    }
    
    if (clean_jobs)
    {
        log_info("Stratum connection got work restart.");
        
        /**
         * Inform the work manager of null work.
         */
        stack_impl_.get_work_manager()->set_work(0);
    }
    
    /**
     * Generate new work.
     */
    auto work = generate_work(
        job_id, previous_hash, coinb1, coinb2, merkles, version, bits, time
    );
    
    if (work)
    {
        log_debug(
            "Stratum connection generated work, difficulty = " <<
            m_next_difficulty << "."
        );
        
        /**
         * Inform the work manager of the new work.
         */
        stack_impl_.get_work_manager()->set_work(work);
    }
}

std::shared_ptr<stratum_work> stratum_connection::generate_work(
    const std::string & job_id, const std::string & previous_hash,
    const std::string & coinb1, const std::string & coinb2,
    const std::vector<std::string> & merkles, const std::string & version,
    const std::string & bits, const std::string & time
    )
{
    /**
     * Convert the difficulty to the target.
     * @note If this were the Scrypt algorithm you would use
     * m_next_difficulty / 65536.0
     */
    std::uint32_t target[8];

	std::uint64_t m;

    std::int32_t k;
    
    /**
     * Set the difficulty.
     */
    auto difficulty = m_next_difficulty;
    
	for (k = 6; k > 0 && difficulty > 1.0; k--)
    {
		difficulty /= 4294967296.0;
	}
    
    m = 4294901760.0 / difficulty;
	
    if (m == 0 && k == 6)
	{
    	std::memset(target, 0xff, 32);
	}
    else
    {
		std::memset(target, 0, 32);
		
        target[k] = static_cast<std::uint32_t> (m);
		target[k + 1] = static_cast<std::uint32_t> (m >> 32);
	}
    
    /**
     * Allocate the coinbase.
     */
    std::vector<std::uint8_t> coinbase;
    
    /**
     * Convert previous_hash from hexidecimal.
     */
    auto previous_hash_bytes = utility::from_hex(previous_hash);
    
    /**
     * Convert coinb1 from hexidecimal.
     */
    auto coinb1_bytes = utility::from_hex(coinb1);
    
    /**
     * Convert coinb2 from hexidecimal.
     */
    auto coinb2_bytes = utility::from_hex(coinb2);
    
    /**
     * Convert version from hexidecimal.
     */
    auto version_bytes = utility::from_hex(version);
    
    /**
     * Convert bits from hexidecimal.
     */
    auto bits_bytes = utility::from_hex(bits);
    
    /**
     * Convert time from hexidecimal.
     */
    auto time_bytes = utility::from_hex(time);

    auto ret = std::make_shared<stratum_work> (
        stratum::instance().username(), job_id, m_extranonce1, m_extranonce2,
        previous_hash_bytes, coinb1_bytes, coinb2_bytes, merkles,
        version_bytes, bits_bytes, time_bytes, target
    );
    
    return ret;
}

void stratum_connection::handle_mining_submit_result(const bool & result)
{
    if (result)
    {
        stratum::instance().set_shares_accepted(
            stratum::instance().shares_accepted() + 1
        );
    }
    else
    {
        stratum::instance().set_shares_rejected(
            stratum::instance().shares_rejected() + 1
        );
    }
    
    const auto & shares_accepted = stratum::instance().shares_accepted();
    const auto & shares_rejected = stratum::instance().shares_rejected();
    
    log_info(
        "Stratum connection handled mining.submit result, " <<
        shares_accepted << "/" << shares_accepted +
        shares_rejected << " (" << 100.0f * shares_accepted /
        (shares_accepted + shares_rejected) << "%) " <<
        (result ? "accepted" : "rejected") << " at " <<
        std::fixed << std::setprecision(2) <<
        statistics::instance().hashes_per_second() / 1000.0f << " KH/s."
    );
}
