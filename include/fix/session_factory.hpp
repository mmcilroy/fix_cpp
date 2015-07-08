#pragma once

#include "fix/session.hpp"
#include "fix/message.hpp"
#include "fix/user.hpp"

namespace fix {

class session_factory
{
public:
    // return a new or existing session for user
    // will return null if an error occurred
    fix::session* get_session( const fix::user& );

    // convert message to a user then return get_session( user )
    fix::session* get_session( const fix::message& );
};

}



// ----------------------------------------------------------------------------
fix::session* fix::session_factory::get_session( const fix::user& user )
{
    return new fix::session( user );
}

fix::session* fix::session_factory::get_session( const fix::message& msg )
{
    std::string protocol;
    std::string sender;
    std::string target;

    msg.parse( [&]( fix::tag tag, const std::string& val ) {
        if( tag == 8 ) {
            protocol = val;
        } else if( tag == 8 ) {
            sender = val;
        } else if( tag == 8 ) {
            target = val;
        }
    } );

    return get_session( fix::user( protocol, sender, target ) );
}
