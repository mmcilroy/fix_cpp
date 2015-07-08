#pragma once

namespace fix {

class ems
{
public:
    ems( fix::session_factory&, fix::receive_function );

    void consumer( const std::string& );

    void producer( const std::string&, const fix::user& );

    void start();

private:
    fix::session_factory& factory_;
    fix::receive_function recv_;
};

}



// ----------------------------------------------------------------------------
fix::ems::ems( fix::session_factory& factory, fix::receive_function recv ) :
    factory_( factory ),
    recv_( recv )
{
}

void fix::ems::consumer( const std::string& )
{
}

void fix::ems::producer( const std::string&, const fix::user& )
{
}

void fix::ems::start()
{
    fix::message msg( "8=FIX.4.4|49=s|56=t|58=request|" );
    fix::session* sess = factory_.get_session( msg );
    recv_( *sess, fix::message( msg ) );
}
