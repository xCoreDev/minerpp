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

#ifndef MINER_GPU_HPP
#define MINER_GPU_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace miner {

    class gpu_handler;
    class stack_impl;
    class stratum_work;
    
    /**
     * Implements a GPU.
     */
    class gpu : public std::enable_shared_from_this<gpu>
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             * @param id The id.
             * @param id_max The id.
             */
            explicit gpu(
                stack_impl & owner, const std::uint32_t & id,
                const std::uint32_t & id_max
            );
            
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
            
            /**
             * Sets the work.
             * @param val The value.
             */
            void set_work(const std::shared_ptr<stratum_work> & val);
        
            /**
             * The number of hashes per second.
             */
            const double & hashes_per_second() const;
        
        private:
        
            /**
             * The states.
             */
            typedef enum state_s
            {
                state_none,
                state_started,
                state_stopped
            } state_t;
        
            /**
             * The id.
             */
            std::uint32_t m_id;
        
            /**
             * The maximum id.
             */
            std::uint32_t m_id_maximum;
        
            /**
             * The gpu_handler.
             */
            std::shared_ptr<gpu_handler> m_gpu_handler;
        
            /**
             * The work.
             */
            std::shared_ptr<stratum_work> m_work;
        
            /**
             * The number of hashes per second.
             */
            double m_hashes_per_second;
        
            /**
             * The hash counter.
             */
            std::uint64_t m_hash_counter;
        
        protected:
        
            /**
             * The thread loop.
             */
            void loop();
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The state.
             */
            state_t state_;
    
            /**
             * The std::thread.
             */
            std::thread thread_;
        
            /**
             * The work std::mutex.
             */
            std::mutex mutex_work_;
        
            /**
             * If true we have new work.
             */
            bool has_new_work_;
        
            /**
             * If true we ned a work restart.
             */
            bool needs_work_restart_;
    };
    
} // namespace miner

#endif // MINER_GPU_HPP
