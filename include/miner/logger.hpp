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


#ifndef MINER_LOGGER_HPP
#define MINER_LOGGER_HPP

#include <boost/asio.hpp>

#if (defined _MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _MSC_VER

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <mutex>

namespace miner {

    /**
     * Implements a logger.
     */
    class logger
    {
        public:
            
			typedef enum severity
			{
				severity_none,
				severity_debug,
				severity_error,
				severity_info,
				severity_warning,
                severity_test,
			} severity_t;
			
            /**
             * Singleton accessor.
             */
			static logger & instance()
			{
			    static logger g_logger;
                
			    return g_logger;
			}
            
            /**
             * operator <<
             */
            template <class T>
            logger & operator << (T const & val)
            {
                std::stringstream ss;
                
                ss << val;
                
                log(ss);
                
                ss.str(std::string());
                
                return logger::instance();
            }

            /**
             * Perform the actual logging.
             * @param val
             */
			void log(std::stringstream & val)
			{
			    static bool use_cout = true;

                static std::recursive_mutex g_mutex;
                
                std::lock_guard<std::recursive_mutex> l1(g_mutex);
                
			    if (use_cout)
			    {
#if (defined _MSC_VER)
#if defined(_UNICODE)
			        DWORD len = MultiByteToWideChar(
			            CP_ACP, 0, val.str().c_str(), -1, NULL, 0
			        );

			        std::unique_ptr<wchar_t> buf(new wchar_t[len]);

			        MultiByteToWideChar(
			            CP_ACP, 0, val.str().c_str(), -1, buf.get(), len
			        );

			        OutputDebugString(buf.get());
			        OutputDebugString(L"\n");

			        std::cerr << val.str() << std::endl;
#else
			        OutputDebugString(val.str().c_str());
			        OutputDebugString(L"\n");

			        std::cerr << val.str() << std::endl;
#endif // _UNICODE
#else
			        std::cerr << val.str() << std::endl;
#endif // _MSC_VER
			    }
			}
            
        private:
        
			// ...
            
        protected:
        
            /**
             * The std::ofstream.
             */
            std::ofstream ofstream_;
    };
    
    #define log_xx(severity, strm) \
    { \
        std::stringstream __ss; \
        switch (severity) \
        { \
            case miner::logger::severity_debug: \
                __ss << "[DEBUG] - "; \
            break; \
            case miner::logger::severity_error: \
                __ss << "[ERROR] - "; \
            break; \
            case miner::logger::severity_info: \
                __ss << "[INFO] - "; \
            break; \
            case miner::logger::severity_warning: \
                __ss << "[WARNING] - "; \
            break; \
            case miner::logger::severity_test: \
                __ss << "[TEST] - "; \
            break; \
            default: \
                __ss << "[UNKNOWN] - "; \
        } \
		__ss << __FUNCTION__ << ": "; \
        __ss << strm; \
        miner::logger::instance() << __ss.str(); \
        __ss.str(std::string()); \
    } \
	
#define log_none(strm) /** */
#if (defined NDEBUG)
#define log_debug(strm) log_none(strm)
#else
#define log_debug(strm) log_xx(miner::logger::severity_debug, strm)
#endif
#define log_error(strm) log_xx(miner::logger::severity_error, strm)
#define log_info(strm) log_xx(miner::logger::severity_info, strm)
#define log_warn(strm) log_xx(miner::logger::severity_warning, strm)
#define log_test(strm) log_xx(miner::logger::severity_test, strm)

} // namespace miner

#endif // MINER_LOGGER_HPP
