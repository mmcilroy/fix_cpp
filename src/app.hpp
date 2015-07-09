#pragma once

#include "fix/ems.hpp"
#include "fix/event.hpp"
#include <thread>

namespace fix {

class app
{
public:
    app();

    fix::ems* ems(
        const std::string& url,
        const std::string& user,
        const std::string& pass );

    void stop();

protected:

private:
    static void on_event( fix::session&, const fix::message );

    void biz_thr_fn();
    void out_thr_fn();

    fix::session_factory factory_;

    fix::event_publisher inp_pub_;
    fix::event_publisher out_pub_;

    fix::event_subscriber& biz_sub_;
    fix::event_subscriber& out_sub_;

    std::thread biz_thr_;
    std::thread out_thr_;
};

}



// ----------------------------------------------------------------------------
const size_t Q = 8;

void on_receive( fix::session& sess, const fix::message& msg )
{
}

fix::app::app() :
    inp_pub_( Q ),
    out_pub_( Q ),
    biz_sub_( inp_pub_.subscribe() ),
    out_sub_( out_pub_.subscribe() ),
    out_thr_( &fix::app::biz_thr_fn, this ),
    biz_thr_( &fix::app::out_thr_fn, this )
{
}

fix::ems* fix::app::ems(
    const std::string& url,
    const std::string& user,
    const std::string& pass )
{
    return new fix::ems( url, user, pass, factory_, on_event );
}

void fix::app::on_event( fix::session&, const fix::message )
{
}

void fix::app::stop()
{
    biz_thr_.join();
    out_thr_.join();
}

void fix::app::biz_thr_fn()
{
    biz_sub_.dispatch( [&]( const fix::event& ei, size_t rem )
    {
        return false;
    } );
}

void fix::app::out_thr_fn()
{
    out_sub_.dispatch( [&]( const fix::event& ei, size_t rem )
    {
        return false;
    } );
}
