#pragma once

#include "fix/message.hpp"
#include "fix/responder.hpp"
#include "fix/user.hpp"

namespace fix {

class session
{
public:
    // the session protocol, sender and target must be known
    session( const fix::user& );

    // associate a responder with this session
    // if the session has a responder it will be used to send responses
    // if the responder closes this method should be called nullptr
    void set_responder( fix::responder* );

    // session has received a request
    // this will validate the message and update the sessions internal state
    // throws if the message is invalid
    void request( const fix::message& req );

    // ask session to encode and send a response
    // the response will only be sent if a responder is available
    void response( const std::string& type, const fix::message& body );

    // inspect session state
    const fix::user& get_user() const;

private:
    fix::user user_;
    fix::responder* responder_;
};

typedef std::function< void( fix::session&, const fix::message& ) > receive_function;

}



// ----------------------------------------------------------------------------
fix::session::session( const fix::user& user ) :
    user_( user ),
    responder_( nullptr )
{
}

void fix::session::set_responder( fix::responder* res )
{
    responder_ = res;
}

void fix::session::request( const fix::message& req )
{
}

void fix::session::response( const std::string& type, const fix::message& body )
{
    if( responder_ ) {
        responder_->respond( body );
    }
}

const fix::user& fix::session::get_user() const
{
    return user_;
}
