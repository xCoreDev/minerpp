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

#include <map>
#include <string>

#include <boost/asio.hpp>

#include <miner/configuration.hpp>
#include <miner/stack.hpp>
#include <miner/stratum.hpp>

int main(int argc, const char * argv[])
{
    std::map<std::string, std::string> args;
    
    for (auto i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        {
            std::string arg = std::string(argv[i]).substr(2, strlen(argv[i]));
            
            std::string key, value;
            
            auto i = arg.find("=");

            if (i != std::string::npos)
            {
                key = arg.substr(0, i);
                
                i = arg.find("=");
                
                if (i != std::string::npos)
                {
                    value = arg.substr(i + 1, arg.length());
                    
                    args[key] = value;
                }
            }
        }
    }
    
    boost::asio::io_service ios;
    
    if (args.size() == 0)
    {
        auto help =
            "Copyright (c) 2015 John Connor"
            "\n\tUsage:"
            "\n\t--work-hosts=stratum+tcp://user:pass@host1:port,stratum+tcp://user:pass@host2:port"
            "\n\t--work-algorithm=whirlpoolxor"
            "\n\t--device-cores=0"
            "\n\t--device-type=cpu"
            "\n\t--serial-ports=COM1,COM2,COM3,COM4\n"
        ;
    
        printf("%s\n", help);
    
        return 0;
    }

    /**
     * Allocate the miner::stack.
     */
    miner::stack s;
    
    /**
     * Start the miner::stack.
     */
    s.start(args);
    
    boost::asio::signal_set signals(ios, SIGINT, SIGTERM);
    signals.async_wait(std::bind(&boost::asio::io_service::stop, &ios));
    ios.run();

    /**
     * Stop the miner::stack.
     */
    s.stop();
    
    return 0;
}

