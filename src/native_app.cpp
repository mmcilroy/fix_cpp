#include "app.hpp"

class native_app : public fix::app
{
public:
    native_app()
    {
    }

protected:
    void on_fix_event( fix::session& sess, const fix::message& msg ) override
    {
    }

    void on_timer_event( int time ) override
    {
    }
};

int main()
{
    native_app app;
    app.start();
}

