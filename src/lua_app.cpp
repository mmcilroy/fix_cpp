#include "app.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class lua_app : public fix::app
{
public:
    lua_app();

    ~lua_app();

    void load( const std::string& file );

protected:
    void on_event( fix::session& sess, const fix::message& msg ) override;

private:
    lua_State* lua_;
};

lua_app* app;



// ----------------------------------------------------------------------------
const char* l_ems_meta = "ems_meta";
const char* l_tcp_meta = "tcp_meta";
const char* l_session_meta = "session_meta";

void l_push_message( lua_State* l, const fix::message& msg )
{
    int i=0;

    lua_newtable( l );
    msg.parse( [&]( fix::tag tag, const std::string& val ) {
        lua_pushinteger( l, ++i );
        lua_newtable( l );
        lua_pushinteger( l, 1 );
        lua_pushinteger( l, tag );
        lua_settable( l, -3 );
        lua_pushinteger( l, 2 );
        lua_pushstring( l, val.c_str() );
        lua_settable( l, -3 );
        lua_settable( l, -3 );
    } );
}

void l_pop_message( lua_State* l, fix::message& msg )
{
    int i=0;
    int tag;

    lua_pushnil( l );
    while( lua_next( l, -2 ) != 0 )
    {
        lua_pushnil( l );
        while( lua_next( l, -2 ) != 0 )
        {
            if( ++i % 2 == 1 ) {
                tag = (int)lua_tonumber( l, -1 );
            } else {
                msg.add( tag, lua_tostring( l, -1 ) );
            }
            lua_pop( l, 1 );
        }
        lua_pop( l, 1 );
    }
}

#ifdef EMS
int l_ems( lua_State* l )
{
    const char* url = luaL_checkstring( l, -3 );
    const char* user = luaL_checkstring( l, -2 );
    const char* pass = luaL_checkstring( l, -1 );

    fix::ems** udata = (fix::ems**)lua_newuserdata( l, sizeof( fix::ems* ) );
    *udata = app->ems( url, user, pass );
    luaL_getmetatable( l, l_ems_meta );
    lua_setmetatable( l, -2 );

    return 1;
}

int l_ems_producer( lua_State* l )
{
    fix::ems* ems = *(fix::ems**)lua_touserdata( l, -5 );
    const char* topic = luaL_checkstring( l, -4 );
    const char* protocol = luaL_checkstring( l, -3 );
    const char* sender = luaL_checkstring( l, -2 );
    const char* target = luaL_checkstring( l, -1 );
    
    fix::session** udata = (fix::session**)lua_newuserdata( l, sizeof( fix::session* ) );
    *udata = ems->producer( topic, fix::user( protocol, sender, target ) );
    luaL_getmetatable( l, l_session_meta );
    lua_setmetatable( l, -2 );

    return 1;
}

int l_ems_consumer( lua_State* l )
{
    fix::ems* ems = *(fix::ems**)lua_touserdata( l, -2 );
    const char* topic = luaL_checkstring( l, -1 );

    ems->consumer( topic );

    return 0;
}
#endif

int l_tcp( lua_State* l )
{
    fix::tcp** udata = (fix::tcp**)lua_newuserdata( l, sizeof( fix::tcp* ) );
    *udata = app->tcp();
    luaL_getmetatable( l, l_tcp_meta );
    lua_setmetatable( l, -2 );

    return 1;}

int l_tcp_accept( lua_State* l )
{
    fix::tcp* tcp = *(fix::tcp**)lua_touserdata( l, -2 );
    const char* conn = luaL_checkstring( l, -1 );

    tcp->accept( conn );

    return 0;
}

int l_tcp_connect( lua_State* l )
{
    fix::tcp* tcp = *(fix::tcp**)lua_touserdata( l, -5 );
    const char* conn = luaL_checkstring( l, -4 );
    const char* protocol = luaL_checkstring( l, -3 );
    const char* sender = luaL_checkstring( l, -2 );
    const char* target = luaL_checkstring( l, -1 );
    
    fix::session** udata = (fix::session**)lua_newuserdata( l, sizeof( fix::session* ) );
    *udata = tcp->connect( conn, fix::user( protocol, sender, target ) );
    luaL_getmetatable( l, l_session_meta );
    lua_setmetatable( l, -2 );

    return 1;
}

int l_session_send( lua_State* l )
{
    fix::message msg;
    fix::session* sess = *(fix::session**)lua_touserdata( l, 1 );
    const char* type = luaL_checkstring( l, 2 );
    l_pop_message( l, msg );

    app->send( *sess, type, msg );

    return 0;
}

void l_register( lua_State* l )
{
    luaL_Reg fix_reg[] = {
#ifdef EMS
        { "ems", l_ems },
#endif
        { "tcp", l_tcp },
        { NULL, NULL }
    };

    luaL_newmetatable( l, "fix" );
    luaL_setfuncs( l, fix_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setglobal( l, "fix" );

#ifdef EMS
    luaL_Reg ems_reg[] = {
        { "producer", l_ems_producer },
        { "consumer", l_ems_consumer },
        { NULL, NULL }
    };

    luaL_newmetatable( l, l_ems_meta );
    luaL_setfuncs( l, ems_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setfield( l, -1, "__index" );
    lua_setglobal( l, "EMS" );
#endif

    luaL_Reg tcp_reg[] = {
        { "accept", l_tcp_accept },
        { "connect", l_tcp_connect },
        { NULL, NULL }
    };

    luaL_newmetatable( l, l_tcp_meta );
    luaL_setfuncs( l, tcp_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setfield( l, -1, "__index" );
    lua_setglobal( l, "TCP" );

    luaL_Reg session_reg[] = {
        { "send", l_session_send },
        { NULL, NULL }
    };

    luaL_newmetatable( l, l_session_meta );
    luaL_setfuncs( l, session_reg, 0 );
    lua_pushvalue( l, -1 );
    lua_setfield( l, -1, "__index" );
    lua_setglobal( l, "SESSION" );
}



// ----------------------------------------------------------------------------
lua_app::lua_app() :
    lua_( luaL_newstate() )
{
    luaL_openlibs( lua_ );
    l_register( lua_ );
}

lua_app::~lua_app()
{
    lua_close( lua_ );
}

void lua_app::load( const std::string& file )
{
    int err = luaL_dofile( lua_, file.c_str() );
    if( err ) {
        std::cerr << "lua error: " << luaL_checkstring( lua_, -1 ) << std::endl;
    }
}

void lua_app::on_event( fix::session& sess, const fix::message& msg )
{
}

int main( int argc, char** argv )
{
    app = new lua_app;
    app->load( argv[1] );
    app->start();
}
