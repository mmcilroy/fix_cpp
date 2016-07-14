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

    //
    fix::session* get_session( const std::string& );

    // convert message to a user then return get_session( user )
    fix::session* get_session( const fix::message& );

private:
    typedef std::map< std::string, fix::session* > session_map;
    session_map sessions_;
};

}



// ----------------------------------------------------------------------------
fix::session* fix::session_factory::get_session( const fix::user& user )
{
    fix::session* sess;
    session_map::iterator it = sessions_.find( user.sender_ );
    if( it == sessions_.end() )
    {
        sess = new fix::session( user );
        sessions_[ user.sender_ ] = sess;
    }
    else
    {
        sess = it->second;
    }

    return sess;
}

fix::session* fix::session_factory::get_session( const std::string& sender )
{
    return get_session( fix::user( "", sender, "" ) );
}

fix::session* fix::session_factory::get_session( const fix::message& msg )
{
    std::string protocol;
    std::string sender;
    std::string target;

    msg.parse( [&]( fix::tag tag, const std::string& val ) {
        if( tag == 8 ) {
            protocol = val;
        } else if( tag == 49 ) {
            sender = val;
        } else if( tag == 56 ) {
            target = val;
        }
    } );

    return get_session( fix::user( protocol, sender, target ) );
}
