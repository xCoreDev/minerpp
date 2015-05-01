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
 
#ifndef MINING_STRATUM_CONNECTION_HPP
#define MINING_STRATUM_CONNECTION_HPP

#include <deque>
#include <string>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

namespace miner {

    class stack_impl;
    class stratum_work;
    
    /**
     * Implements a stratum connection.
     */
    class stratum_connection
        : public std::enable_shared_from_this<stratum_connection>
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit stratum_connection(stack_impl & owner);
        
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
        
            /**
             * Performs a write operation.
             * buf The buffer.
             */
            void write(const std::string & buf);
        
        private:
        
            /**
             * Perfoms a connect operation.
             * @param it_endpoint The iterator.
             */
            void do_connect(
                boost::asio::ip::tcp::resolver::iterator it_endpoint
            );
        
            /**
             * Performs a read operation.
             */
            void do_read();
        
            /**
             * Performs a write operation.
             * buf The buffer.
             */
            void do_write(const std::string & buf);
        
            /**
             * The next difficulty.
             */
            double m_next_difficulty;

            /**
             * The session id.
             */
            std::string m_session_id;
        
            /**
             * The xnonce1 size.
             */
            std::size_t m_extranonce1_size;
        
            /**
             * The extranonce1.
             */
            std::vector<std::uint8_t> m_extranonce1;
        
            /**
             * The extranonce2 size.
             */
            std::size_t m_extranonce2_size;

            /**
             * The extranonce2.
             */
            std::vector<std::uint8_t> m_extranonce2;
        
        protected:
        
            /**
             * Handles a JSON line.
             * @Param json_line The json_line.
             */
            void handle_json_line(const std::string & json_line);
        
            /**
             * Handles a JSON-RPC method.
             * @param id The id.
             * @param method The method.
             * @param params The params.
             */
            void handle_json_rpc_method(
                const std::string & id, const std::string & method,
                const boost::property_tree::ptree & params
            );
        
            /**
             * Handles a JSON-RPC result.
             * @param id The id.
             * @param result The result.
             */
            void handle_json_rpc_result(
                const std::string & id,
                const boost::property_tree::ptree & result
            );
        
            /**
             * Handles a JSON-RPC error.
             * @param id The id.
             * @param error The error.
             */
            void handle_json_rpc_error(
                const std::string & id,
                const boost::property_tree::ptree & error
            );
        
            /**
             * Handles a mining.notify method.
             * @param params The boost::property_tree::ptree.
             */
            void handle_mining_notify(
                const boost::property_tree::ptree & params
            );
        
            /**
             * Attempts to generate a work from stratum.
             * @param job_id The job id.
             * @param previous_hash The previous hash.
             * @param coinb1 The coinb1.
             * @param coinb2 The coinb2.
             * @param merkles The merkles.
             * @param version The version.
             * @param bits The bits.
             * @param time The time.
             */
            std::shared_ptr<stratum_work> generate_work(
                const std::string & job_id, const std::string & previous_hash,
                const std::string & coinb1, const std::string & coinb2,
                const std::vector<std::string> & merkles,
                const std::string & version, const std::string & bits,
                const std::string & time
            );
        
            /**
             * Handles a mining.submit result.
             * @param result The result.
             */
            void handle_mining_submit_result(const bool & result);
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand strand_;
        
            /**
             * The timeout timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timeout_timer_;
        
            /**
             * The socket.
             */
            std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
        
            /**
             * The response.
             */
            std::unique_ptr<boost::asio::streambuf> response_;
        
            /**
             * The write queue.
             */
            std::deque<std::string> write_queue_;
        
            /**
             * The last job id.
             */
            std::string last_job_id_;
    };
    
} // namespace miner

#endif // MINING_STRATUM_CONNECTION_HPP
