#pragma once

#include "fix/session_factory.hpp"
#include "fix/decoder.hpp"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

class tcp_socket;

namespace fix {

class tcp
{
friend tcp_socket;

public:
    tcp( fix::session_factory&, fix::receive_function );

    void accept( const std::string& );

    fix::session* connect( const std::string&, const fix::user& );

    void start();

private:
    void accept( boost::asio::ip::tcp::acceptor* );

    void on_accept( boost::asio::ip::tcp::acceptor*, tcp_socket*, const boost::system::error_code& );

    fix::session_factory& factory_;
    fix::receive_function recv_;

    boost::asio::io_service io_;
};

}



// ----------------------------------------------------------------------------
const size_t buffer_size = 4*1024;

class tcp_socket : public fix::responder
{
public:
    tcp_socket( fix::tcp& tcp ) :
        tcp_( tcp ),
        socket_( tcp.io_ )
    {
    }

    tcp_socket( fix::tcp& tcp, const fix::user& user ) :
        tcp_( tcp ),
        session_( tcp.factory_.get_session( user ) ),
        socket_( tcp.io_ )
    {
        session_->set_responder( this );
    }

    ~tcp_socket()
    {
        if( session_ ) {
            session_->set_responder( nullptr );
        }
    }

    void receive()
    {
        socket_.async_read_some(
            boost::asio::buffer( buf_, buffer_size ),
            boost::bind(
                &tcp_socket::on_receive,
                this,
                boost::asio::placeholders::bytes_transferred,
                boost::asio::placeholders::error ) );
    }

    void on_receive( size_t len, const boost::system::error_code& err )
    {
        if( !err )
        {
            decoder_.decode( buf_, len, [&]( const fix::message& msg )
            {
                std::cout << "<<< (tcp) " << msg << std::endl;
                tcp_.recv_( *session_, msg );
            } );
            receive();
        }
        else
        {
            delete this;
        }
    }

    void respond( const fix::message& msg ) override
    {
        std::cout << ">>> (tcp) " << msg << std::endl;

        boost::asio::write(
            socket_,
            boost::asio::buffer(
                msg.str().c_str(),
                msg.size() ) );
    }

    fix::tcp& tcp_;
    fix::decoder decoder_;
    fix::session* session_;

    boost::asio::ip::tcp::socket socket_;

    char buf_[ buffer_size ];
};



// ----------------------------------------------------------------------------
inline void parse_conn( const std::string& conn, std::string& host, std::string& port )
{
    std::vector< std::string > strs;
    boost::split( strs, conn, boost::is_any_of( ":" ) );
    if( strs.size() == 2 ) {
        host = strs[0]; port = strs[1];
    } else {
        std::cerr << "invalid tcp connection: " << conn << std::endl;
    }
}

inline fix::tcp::tcp( fix::session_factory& factory, fix::receive_function recv ) :
    factory_( factory ),
    recv_( recv )
{
}

inline void fix::tcp::accept( const std::string& conn )
{
    std::string host;
    std::string port;
    parse_conn( conn, host, port );
    accept( new boost::asio::ip::tcp::acceptor(
        io_, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), boost::lexical_cast< int >( port ) ) ) );
}

inline void fix::tcp::accept( boost::asio::ip::tcp::acceptor* acceptor )
{
    tcp_socket* sock = new tcp_socket( *this );
    acceptor->async_accept(
        sock->socket_,
        boost::bind(
            &fix::tcp::on_accept,
            this,
            acceptor,
            sock,
            boost::asio::placeholders::error ) );
}

inline void fix::tcp::on_accept( boost::asio::ip::tcp::acceptor* acceptor, tcp_socket* sock, const boost::system::error_code& err )
{
    if( !err ) {
        sock->receive();
    } else {
        delete sock;
    }

    accept( acceptor );
}

inline fix::session* fix::tcp::connect( const std::string& conn, const fix::user& user )
{
    std::string host;
    std::string port;
    parse_conn( conn, host, port );

    boost::asio::ip::tcp::resolver resolver( io_ );
    boost::asio::ip::tcp::resolver::query query( boost::asio::ip::tcp::v4(), host, port );
    boost::asio::ip::tcp::resolver::iterator it = resolver.resolve( query );
    tcp_socket* sock = new tcp_socket( *this, user );
    boost::asio::connect( sock->socket_, it );

    sock->receive();

    return sock->session_;
}

inline void fix::tcp::start()
{
    io_.run();
}
