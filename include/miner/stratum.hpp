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

#ifndef MINER_STRATUM_HPP
#define MINER_STRATUM_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace miner {

    class stratum
    {
        public:

            /**
             * Constructor
             */
            stratum();
        
            /**
             * The singleton accessor.
             */
            static stratum & instance();
        
            /**
             * Set's the username.
             * @param val The value.
             */
            void set_username(const std::string & val);
        
            /**
             * The username.
             */
            const std::string & username() const;
        
            /**
             * Set's the password.
             * @param val The value.
             */
            void set_password(const std::string & val);
        
            /**
             * The password.
             */
            const std::string & password() const;
        
            /**
             * Set's the host.
             * @param val The value.
             */
            void set_host(const std::string & val);
        
            /**
             * The host.
             */
            const std::string & host() const;
        
            /**
             * Set's the port.
             * @param val The value.
             */
            void set_port(const std::uint16_t & val);
        
            /**
             * The port.
             */
            const std::uint16_t & port() const;
        
            /**
             * Sets the shares accepted.
             * @param val The value.
             */
            void set_shares_accepted(const std::uint32_t & val);
        
            /**
             * The shares accepted.
             */
            const std::uint32_t & shares_accepted() const;
        
            /**
             * Sets the shares rejected.
             * @param val The value.
             */
            void set_shares_rejected(const std::uint32_t & val);
        
            /**
             * The shares rejected.
             */
            const std::uint32_t & shares_rejected() const;
        
        private:
        
            /**
             * The username.
             */
            std::string m_username;
        
            /**
             * The password.
             */
            std::string m_password;
        
            /**
             * The host.
             */
            std::string m_host;
        
            /**
             * The port.
             */
            std::uint16_t m_port;
        
            /**
             * The shares accepted.
             */
            std::uint32_t m_shares_accepted;
        
            /**
             * The shares rejected.
             */
            std::uint32_t m_shares_rejected;
        
        protected:
        
            // ...
    };
    
} // namespace miner

#endif // MINER_STRATUM_HPP
