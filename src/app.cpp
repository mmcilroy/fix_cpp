#include "app.hpp"

class test_app : public fix::app
{
public:
    test_app()
    {
        ems( "", "", "" );
    }

protected:
    void on_event( fix::session&, const fix::message )
    {
    }
};

int main()
{
    test_app app;
    app.stop();
}
