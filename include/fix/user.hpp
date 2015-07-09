#pragma once

#include <iostream>

namespace fix {

struct user
{
    user( const std::string& protocol,
          const std::string& sender,
          const std::string& target );

    user( const fix::message& msg );

    std::string protocol_;
    std::string sender_;
    std::string target_;
};

}

inline fix::user::user(
    const std::string& protocol,
    const std::string& sender,
    const std::string& target ) :
    protocol_( protocol ),
    sender_( sender ),
    target_( target )
{
}

inline fix::user::user( const fix::message& msg )
{
    msg.parse( [&]( fix::tag tag, const std::string& val ) {
        if( tag == 8 ) {
            protocol_ = val;
        } else if( tag == 8 ) {
            sender_ = val;
        } else if( tag == 8 ) {
            target_ = val;
        }
    } );
}

inline std::ostream& operator<<( std::ostream& out, const fix::user& user )
{
    out << "protocol=" << user.protocol_
        << ", sender=" << user.sender_
        << ", target=" << user.target_;

    return out;
}
