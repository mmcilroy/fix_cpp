#include "app.hpp"

class test_app : public fix::app
{
public:
    test_app()
    {
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
