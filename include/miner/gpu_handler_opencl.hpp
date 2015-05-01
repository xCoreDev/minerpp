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

#ifndef MINER_GPU_HANDLER_OPENCL_HPP
#define MINER_GPU_HANDLER_OPENCL_HPP

#include <memory>

#define USE_OPENCL 0

#if (defined USE_OPENCL && USE_OPENCL)
#include <OpenCL/opencl.h>
#endif // USE_OPENCL

#include <miner/gpu_handler.hpp>

namespace miner {

    class gpu;
    
    /**
     * Implements a GPU handler for OpenCL.
     */
    class gpu_handler_opencl : public gpu_handler
    {
        public:
        
            /**
             * Constructor
             * @param owner The gpu.
             */
            explicit gpu_handler_opencl(std::shared_ptr<gpu> owner)
                : gpu_handler(owner)
            {
#if (defined USE_OPENCL && USE_OPENCL)
                if (
                    clGetDeviceIDs(0, CL_DEVICE_TYPE_GPU, 1,
                    &cl_device_id_, 0) != CL_SUCCESS
                    )
                {
                    throw std::runtime_error("clGetDeviceIDs failed");
                }
                
                auto error = 0;
                
                cl_context_ = clCreateContext(
                    0, 1, &cl_device_id_, 0, 0, &error
                );
    
                if (cl_context_ == 0)
                {
                    throw std::runtime_error("clCreateContext failed");

                }

                cl_command_queue_ = clCreateCommandQueue(
                    cl_context_, cl_device_id_, 0, &error
                );
    
                if (cl_command_queue_ == 0)
                {
                    throw std::runtime_error("clCreateCommandQueue failed");

                }
#endif // USE_OPENCL
            }
        
            /**
             * The read handler.
             * @param buf The buffer.
             * @param len The length.
             */
            virtual void on_read(const char * buf, const std::size_t & len)
            {
                assert(0);
            }
        
            /**
             * Sets that new work is available.
             * @param val The value.
             */
            virtual void set_has_new_work(const bool & val)
            {
                assert(0);
            }
        
            /**
             * Sets that new work is available.
             * @param val The value.
             */
            virtual void set_needs_work_restart(const bool & val)
            {
                assert(0);
            }
        
        private:
        
            // ...
        
        protected:
#if (defined USE_OPENCL && USE_OPENCL)
            /**
             * The cl_device_id.
             */
            cl_device_id cl_device_id_;
        
            /**
             * The cl_context.
             */
            cl_context cl_context_;
        
            /**
             * The cl_command_queue.
             */
            cl_command_queue cl_command_queue_;
#endif // USE_OPENCL
    };
    
} // namespace miner

#endif // MINER_GPU_HANDLER_OPENCL_HPP
