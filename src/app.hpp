#pragma once

#ifdef EMS
#include "fix/ems.hpp"
#endif

#include "fix/tcp.hpp"
#include "fix/event.hpp"
#include <thread>

namespace fix {

class app
{
public:
    app();

#ifdef EMS
    fix::ems* ems(
        const std::string& url,
        const std::string& user,
        const std::string& pass );
#endif

    fix::tcp* tcp();

    void send( fix::session&, const fix::type&, const fix::message& );

    void start();

    void stop();

protected:

private:
    virtual void on_event( fix::session&, const fix::message& ) = 0;

    void biz_thr_fn();
    void out_thr_fn();

    fix::session_factory factory_;

    fix::event_publisher inp_pub_;
    fix::event_publisher out_pub_;

    fix::event_subscriber& biz_sub_;
    fix::event_subscriber& out_sub_;

#ifdef EMS
    std::list< std::unique_ptr< fix::ems > > ems_list_;
#endif

    std::list< std::unique_ptr< fix::tcp > > tcp_list_;

    std::thread biz_thr_;
    std::thread out_thr_;
};

}



// ----------------------------------------------------------------------------
const size_t Q = 8;

fix::app::app() :
    inp_pub_( Q ),
    out_pub_( Q ),
    biz_sub_( inp_pub_.subscribe() ),
    out_sub_( out_pub_.subscribe() ),
    out_thr_( &fix::app::biz_thr_fn, this ),
    biz_thr_( &fix::app::out_thr_fn, this )
{
}

#ifdef EMS
fix::ems* fix::app::ems(
    const std::string& url,
    const std::string& user,
    const std::string& pass )
{
    ems_list_.emplace_back( new fix::ems( url, user, pass, factory_, [&]( fix::session& sess, const fix::message& msg ) {
        inp_pub_.publish( 1, [&]( fix::event& ev, size_t n ) {
            ev.session_ = &sess;
            ev.message_ = msg;
        } );
    } ) );

    return ems_list_.back().get();
}
#endif

fix::tcp* fix::app::tcp()
{
    tcp_list_.emplace_back( new fix::tcp( factory_, [&]( fix::session& sess, const fix::message& msg ) {
        inp_pub_.publish( 1, [&]( fix::event& ev, size_t n ) {
            ev.session_ = &sess;
            ev.message_ = msg;
        } );
    } ) );

    return tcp_list_.back().get();
}

void fix::app::send( fix::session& sess, const fix::type& type, const fix::message& msg )
{
    out_pub_.publish( 1, [&]( fix::event& ev, size_t n ) {
        ev.session_ = &sess;
        ev.type_ = type;
        ev.message_ = msg;
    } );
}

void fix::app::start()
{
#ifdef EMS
    for( auto& e : ems_list_ ) {
        e->start();
    }
#endif

    for( auto& t : tcp_list_ ) {
        t->start();
    }

    biz_thr_.join();
    out_thr_.join();
}

void fix::app::stop()
{
}

void fix::app::biz_thr_fn()
{
    biz_sub_.dispatch( [&]( const fix::event& ei, size_t rem )
    {
        std::cout << "biz: " << ei.message_ << std::endl;

        on_event( *ei.session_, ei.message_ );
        return false;
    } );
}

void fix::app::out_thr_fn()
{
    out_sub_.dispatch( [&]( const fix::event& ei, size_t rem )
    {
        std::cout << "out: " << ei.type_ << ", " << ei.message_ << std::endl;

        ei.session_->response( ei.type_, ei.message_ );
        return false;
    } );
}
