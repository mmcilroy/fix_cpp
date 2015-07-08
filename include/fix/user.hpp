#pragma once

#include <iostream>

namespace fix {

struct user
{
    user( const std::string& protocol,
          const std::string& sender,
          const std::string& target );

    std::string protocol_;
    std::string sender_;
    std::string target_;
};

inline user::user(
    const std::string& protocol,
    const std::string& sender,
    const std::string& target ) :
    protocol_( protocol ),
    sender_( sender ),
    target_( target )
{
}

inline std::ostream& operator<<( std::ostream& out, const fix::user& user )
{
    out << "protocol=" << user.protocol_
        << ", sender=" << user.sender_
        << ", target=" << user.target_;

    return out;
}

}
