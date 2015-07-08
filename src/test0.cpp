#include "fix/tcp.hpp"
#include "fix/ems.hpp"

int main()
{
    fix::receive_function recv = []( fix::session& sess, const fix::message& msg )
    {
        std::cout << "<<< " << msg << std::endl;
        sess.response( "8", fix::message( "58=response|" ) );
    };

    fix::session_factory factory;
    fix::tcp tcp( factory, recv );
    fix::ems ems( factory, recv );

    tcp.start();
    ems.start();
}
