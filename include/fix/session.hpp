#pragma once

#include "fix/message.hpp"
#include "fix/responder.hpp"
#include "fix/user.hpp"
#include <functional>

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
void set_utc_time( std::string& s )
{
    time_t t = time( 0 );
    struct tm* utc = gmtime( &t );

    char buf[32];
    sprintf( buf, "%04d%02d%02d-%02d:%02d:%02d",
        utc->tm_year+1900,
        utc->tm_mon+1,
        utc->tm_mday,
        utc->tm_hour,
        utc->tm_min,
        utc->tm_sec );

    s = buf;
}

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
    if( responder_ )
    {
        std::string send_time;
        set_utc_time( send_time );

        fix::message msg;
        fix::message hdr;
        hdr.add( msg_type, type );
        hdr.add( msg_seq_num, 1 );
        hdr.add( sender_comp_id, user_.sender_ );
        hdr.add( target_comp_id, user_.target_ );
        hdr.add( sending_time, send_time );

        msg.add( begin_string, user_.protocol_ );
        msg.add( body_length, hdr.size() );
        msg.add( hdr );
        msg.add( body );

        int checksum = 0;
        for( int i=0; i<msg.size(); i++ ) {
            checksum += (int)msg[i];
        }

        char buf[4];
        sprintf( buf, "%03d", checksum % 256 );

        msg.add( check_sum, buf );

        responder_->respond( msg );
    }
}

const fix::user& fix::session::get_user() const
{
    return user_;
}
