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

#include <boost/algorithm/string.hpp>

#include <miner/configuration.hpp>
#include <miner/cpu.hpp>
#include <miner/cpu_manager.hpp>
#include <miner/gpu.hpp>
#include <miner/gpu_manager.hpp>
#include <miner/logger.hpp>
#include <miner/serial_manager.hpp>
#include <miner/serial_port.hpp>
#include <miner/stack.hpp>
#include <miner/stack_impl.hpp>
#include <miner/statistics.hpp>
#include <miner/stratum.hpp>
#include <miner/stratum_work.hpp>
#include <miner/work_manager.hpp>

using namespace miner;

stack_impl::stack_impl(miner::stack & owner)
    : stack_(owner)
    , strand_(m_io_service)
{
    // ...
}

void stack_impl::start(const std::map<std::string, std::string> & args)
{
    /**
     * Parse the command line arguments.
     */
    parse_command_line_args(args);
    
    /**
     * Reset the boost::asio::io_service.
     */
    m_io_service.reset();
    
    /**
     * Allocate the boost::asio::io_service::work.
     */
    work_.reset(new boost::asio::io_service::work(m_io_service));

    /**
     * Allocate the thread.
     */
    auto thread = std::make_shared<std::thread> (
        std::bind(&stack_impl::loop, this)
    );
    
    /**
     * Retain the thread.
     */
    threads_.push_back(thread);

    /**
     * Check the device type.
     */
    if (
        configuration::instance().device_type() ==
        configuration::device_type_cpu
        )
    {
        /**
         * Allocate the cpu_manager.
         */
        m_cpu_manager = std::make_shared<cpu_manager>(*this);
        
        /**
         * Start the serial_manager.
         */
        m_cpu_manager->start();
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_gpu
        )
    {
        /**
         * Allocate the gpu_manager.
         */
        m_gpu_manager = std::make_shared<gpu_manager>(*this);
        
        /**
         * Start the serial_manager.
         */
        m_gpu_manager->start();
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_serial
        )
    {
        /**
         * Allocate the serial_manager.
         */
        m_serial_manager = std::make_shared<serial_manager>(*this);
        
        /**
         * Start the serial_manager.
         */
        m_serial_manager->start();
    }
    
    /**
     * Allocate the work manager.
     */
    m_work_manager = std::make_shared<work_manager>(*this);
    
    /**
     * Start the work_manager.
     */
    m_work_manager->start();
}

void stack_impl::stop()
{
    if (m_cpu_manager)
    {
        m_cpu_manager->stop();
    }
    
    if (m_gpu_manager)
    {
        m_gpu_manager->stop();
    }
    
    if (m_serial_manager)
    {
        m_serial_manager->stop();
    }
    
    if (m_work_manager)
    {
        m_work_manager->stop();
    }
    
    /**
     * Reset the work.
     */
    work_.reset();

    /**
     * Stop the boost::asio::io_service.
     */
    m_io_service.stop();
    
    /**
     * Join the threads.
     */
    for (auto & i : threads_)
    {
        try
        {
            if (i->joinable())
            {
                i->join();
            }
        }
        catch (std::exception & e)
        {
            // ...
        }
    }
    
    /**
     * Clear the threads.
     */
    threads_.clear();
    
    m_cpu_manager.reset();
    m_serial_manager.reset();
    m_work_manager.reset();
}

void stack_impl::parse_command_line_args(
    const std::map<std::string, std::string> & args
    )
{
    for (auto & i : args)
    {
        if (i.first == "device-cores")
        {
            auto device_cores = std::stoi(i.second);
            
            log_info("Stack got device cores = " << device_cores << ".");
            
            configuration::instance().set_device_cores(device_cores);
        }
        else if (i.first == "device-type")
        {
            log_info("Stack got device type = " << i.second << ".");
            
            /**
             * Set the device type.
             */
            if (i.second == "cpu")
            {
                miner::configuration::instance().set_device_type(
                    miner::configuration::device_type_cpu
                );
            }
            else if (i.second == "gpu")
            {
                miner::configuration::instance().set_device_type(
                    miner::configuration::device_type_gpu
                );
            }
            else if (i.second == "serial")
            {
                miner::configuration::instance().set_device_type(
                    miner::configuration::device_type_serial
                );
            }
            else
            {
                throw std::runtime_error("invalid device type");
            }
        }
        else if (i.first == "serial-ports")
        {
            std::vector<std::string> parts;
            
            boost::split(parts, i.second, boost::is_any_of(","));
            
            for (auto & i : parts)
            {
                log_info("Stack added serial port = " << i << ".");
            
                configuration::instance().serial_ports().push_back(i);
            }
        }
        else if (i.first == "work-algorithm")
        {
            if (i.second == "whirlpoolx" || i.second == "whirlpoolxor")
            {
                /**
                 * Set the Proof-of-Work type.
                 */
                miner::configuration::instance().set_proof_of_work_type(
                    miner::configuration::proof_of_work_type_whirlpool_xor
                );
            }
            else
            {
                throw std::runtime_error("invalid work algorithm");
            }
        }
        else if (i.first == "work-hosts")
        {
            std::vector<std::string> parts;
            
            boost::split(parts, i.second, boost::is_any_of(","));
            
            for (auto & i : parts)
            {
                auto pos = i.find("stratum+tcp://");
                
                /**
                 * We only support stratum (for now).
                 */
                if (pos == std::string::npos)
                {
                    log_info("Stack got invalid work host = " << i << ".");
                }
                else
                {
                    pos = i.find_first_of("//");
                    
                    auto uri = i.substr(pos + strlen("//"));

                    std::vector<std::string> parts1, parts2;
                    
                    boost::split(parts1, uri, boost::is_any_of("@"));

                    auto index1 = 0;
                    
                    std::string username, password;
                    std::string host;
                    std::uint16_t port = 0;
                    
                    for (auto & i : parts1)
                    {
                        boost::split(parts2, i, boost::is_any_of(":"));

                        auto index2 = 0;
                        
                        for (auto & j : parts2)
                        {
                            if (index1 == 0 && index2 == 0)
                            {
                                username = j;
                            }
                            else if (index1 == 0 && index2 == 1)
                            {
                                password = j;
                            }
                            else if (index1 == 1 && index2 == 0)
                            {
                                host = j;
                            }
                            else if (index1 == 1 && index2 == 1)
                            {
                                port = std::stoul(j);
                            }
                            
                            index2++;
                        }
                        
                        index1++;
                    }
                    
                    std::pair<
                        std::pair<std::string, std::string>,
                        std::pair<std::string, std::uint16_t> >
                    work_host;
                    
                    work_host.first.first = username;
                    work_host.first.second = password;
                    work_host.second.first = host;
                    work_host.second.second = port;
                    
                    configuration::instance().work_hosts().push_back(
                        work_host
                    );
                }
            }
            
            for (auto & i :configuration::instance().work_hosts())
            {
                log_info(
                    "Stack configured work host = " <<
                    i.first.first << ":" << i.first.second << "@" <<
                    i.second.first << ":" << i.second.second  << "."
                );
            }
        }
    }
    
    if (
        configuration::instance().proof_of_work_type() ==
        configuration::proof_of_work_type_none
        )
    {
        throw std::runtime_error("null work algorithm");
    }
}

void stack_impl::loop()
{
    while (work_)
    {
        try
        {
            m_io_service.run();
            
            if (work_ == 0)
            {
                break;
            }
        }
        catch (std::exception & e)
        {
            // ...
        }
    }
}

boost::asio::io_service & stack_impl::io_service()
{
    return m_io_service;
}

std::shared_ptr<work_manager> & stack_impl::get_work_manager()
{
    return m_work_manager;
}

void stack_impl::handle_work(const std::shared_ptr<stratum_work> & work)
{
    log_debug("Stack handle work.");
    
    /**
     * Check the device type.
     */
    if (
        configuration::instance().device_type() ==
        configuration::device_type_cpu
        )
    {
        if (m_cpu_manager)
        {
            m_cpu_manager->set_work(work);
        }
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_gpu
        )
    {
        if (m_gpu_manager)
        {
            m_gpu_manager->set_work(work);
        }
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_serial
        )
    {
        if (m_serial_manager)
        {
            m_serial_manager->set_work(work);
        }
    }
}

void stack_impl::update_statistics()
{
    log_debug("Stack update statistics.");
    
    /**
     * Check the device type.
     */
    if (
        configuration::instance().device_type() ==
        configuration::device_type_cpu
        )
    {
        if (m_cpu_manager)
        {
            double hashes_per_second = 0.0;
            
            const auto & cpus = m_cpu_manager->cpus();
            
            for (auto & i : cpus)
            {
                hashes_per_second += i->hashes_per_second();
            }
            
            /**
             * Set the statistics hashes_per_second.
             */
            statistics::instance().set_hashes_per_second(hashes_per_second);
        }
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_gpu
        )
    {
        if (m_gpu_manager)
        {
            double hashes_per_second = 0.0;
            
            const auto & gpus = m_gpu_manager->gpus();
            
            for (auto & i : gpus)
            {
                hashes_per_second += i->hashes_per_second();
            }
            
            /**
             * Set the statistics hashes_per_second.
             */
            statistics::instance().set_hashes_per_second(hashes_per_second);
        }
    }
    else if (
        configuration::instance().device_type() ==
        configuration::device_type_serial
        )
    {
        if (m_serial_manager)
        {
            double hashes_per_second = 0.0;
            
            const auto & ports = m_serial_manager->serial_ports();
            
            for (auto & i : ports)
            {
                if (auto j = i.lock())
                {
                    hashes_per_second += j->hashes_per_second();
                }
            }
            
            /**
             * Set the statistics hashes_per_second.
             */
            statistics::instance().set_hashes_per_second(hashes_per_second);
        }
    }
}
