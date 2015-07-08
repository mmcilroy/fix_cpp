#pragma once

#include "fix/user.hpp"
#include "fix/message.hpp"
#include "fix/session.hpp"
#include "fix/session_factory.hpp"

class tcp_socket;

namespace fix {

class tcp
{
friend tcp_socket;

public:
    tcp( fix::session_factory&, fix::receive_function );

    void accept( const std::string& );

    void connect( const std::string&, const fix::user& );

    void start();

private:
    fix::session_factory& factory_;
    fix::receive_function recv_;
};

}



// ----------------------------------------------------------------------------
class tcp_socket : public fix::responder
{
public:
    tcp_socket( fix::tcp& tcp ) :
        tcp_( tcp )
    {
    }

    ~tcp_socket()
    {
        if( session_ ) {
            session_->set_responder( nullptr );
        }
    }

    void receive()
    {
        session_ = tcp_.factory_.get_session( fix::user( "p", "s", "t" ) );
        session_->set_responder( this );

        tcp_.recv_( *session_, fix::message( "55=VOD.L|" ) );
    }

    virtual void respond( const fix::message& msg )
    {
        std::cout << ">>> " << msg << std::endl;
    }

private:
    fix::tcp& tcp_;
    fix::session* session_;
};



// ----------------------------------------------------------------------------
inline fix::tcp::tcp( fix::session_factory& factory, fix::receive_function recv ) :
    factory_( factory ),
    recv_( recv )
{
}

inline void fix::tcp::accept( const std::string& )
{
}

inline void fix::tcp::connect( const std::string&, const fix::user& )
{
}

inline void fix::tcp::start()
{
    tcp_socket sess( *this );
    sess.receive();
}
