#include "fix/event.hpp"
#include "fix/ems.hpp"

const size_t Q = 8;

fix::event_publisher inp_pub( Q );
fix::event_publisher out_pub( Q );
fix::event_subscriber& biz_sub = inp_pub.subscribe();
fix::event_subscriber& out_sub = out_pub.subscribe();

fix::session_factory factory;

void biz_thr_fn( fix::event_subscriber* sub, fix::event_publisher* pub )
{
    sub->dispatch( [&]( const fix::event& ei, size_t rem )
    {
        return false;
    } );
}

void out_thr_fn( fix::event_subscriber* sub )
{
    sub->dispatch( [&]( const fix::event& ei, size_t rem )
    {
        return false;
    } );
}

void on_receive( fix::session& sess, const fix::message& msg )
{
    std::cout << "<<< " << msg << std::endl;

    inp_pub.publish( 1, [&]( fix::event& ev, size_t n )
    {
        ev.session_ = &sess;
        ev.message_ = msg;
    } );
}

int main( int argc, char** argv )
{
    std::thread out_thr( biz_thr_fn, &biz_sub, &out_pub );
    std::thread biz_thr( out_thr_fn, &out_sub );

    fix::ems ems( 
        "tcp://tibems008-d1.eur.nsroot.net:7008",
        "ibint",
        "ibint",
        factory,
        on_receive );

    fix::session* sess = ems.producer(
        "DEV.GMA.FROM.sm87110", 
        fix::user( "FIX.4.4", "SM87110", "TEST" ) );

    ems.consumer( "DEV.GMA.FROM.sm87110" );

    ems.start();

    sess->response( "D", fix::message( "58=hi|" ) );

    biz_thr.join();
    out_thr.join();
}
