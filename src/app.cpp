#include "app.hpp"

class test_app : public fix::app
{
public:
    test_app()
    {
        fix::ems* e = ems( 
            "tcp://tibems008-d1.eur.nsroot.net:7008",
            "ibint",
            "ibint" );

        fix::session* sess = e->producer(
            "DEV.GMA.FROM.sm87110", 
            fix::user( "FIX.4.4", "SM87110", "TEST" ) );

        e->consumer( "DEV.GMA.FROM.sm87110" );

        send( *sess, "?", fix::message( "58=hi|" ) );
    }

protected:
    void on_event( fix::session& sess, const fix::message& msg ) override
    {
    }
};

int main()
{
    test_app app;
    app.start();
}
