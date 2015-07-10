#pragma once

#include "fix/session_factory.hpp"

#include "tibems/tibems.h"

namespace fix {

class ems;

class ems_producer : public fix::responder
{
public:
    ems_producer(
        const std::string& topic,
        fix::session& fix_sess,
        tibemsSession& ems_sess );

    ~ems_producer();

    void respond( const fix::message& msg ) override;

private:
    fix::session* session_;

    tibemsDestination destination_;
    tibemsMsgProducer producer_;
};



class ems_consumer
{
public:
    ems_consumer( const std::string& topic, fix::ems& ems, tibemsSession& session );

    ~ems_consumer();

    void consume( const fix::message& msg );

private:
    fix::ems& ems_;

    tibemsDestination destination_;
    tibemsMsgConsumer consumer_;
};



class ems
{
friend ems_consumer;

public:
    ems( const std::string& url,
         const std::string& user,
         const std::string& pass,
         fix::session_factory&,
         fix::receive_function );

    ~ems();

    void consumer( const std::string& topic );

    fix::session* producer( const std::string& topic, const fix::user& user );

    void start();

private:
    void fail();

    fix::session_factory& fix_factory_;
    fix::receive_function recv_;

    tibemsConnectionFactory ems_factory_;
    tibemsConnection connection_;
    tibemsSession producer_session_;
    tibemsSession consumer_session_;
    tibemsErrorContext error_;

    std::list< std::unique_ptr< ems_consumer > > consumers_;
    std::list< std::unique_ptr< ems_producer > > producers_;
};

}



// ----------------------------------------------------------------------------
inline fix::ems_producer::ems_producer(
    const std::string& topic,
    fix::session& fix_sess,
    tibemsSession& ems_sess ) :
    session_( &fix_sess )
{
    session_->set_responder( this );
    tibemsTopic_Create( &destination_, topic.c_str() );
    tibemsSession_CreateProducer( ems_sess, &producer_, destination_ );

    std::cout << "ems producer: " << topic << std::endl;
}

inline fix::ems_producer::~ems_producer()
{
    if( session_ ) {
        session_->set_responder( nullptr );
    }

    tibemsMsgProducer_Close( producer_ );
    tibemsDestination_Destroy( destination_ );
}

inline void fix::ems_producer::respond( const fix::message& msg )
{
    std::cout << "ems: " << msg << std::endl;
    tibemsTextMsg ems_msg;
    tibemsTextMsg_Create( &ems_msg );
    tibemsTextMsg_SetText( ems_msg, msg.str().c_str() );
    tibemsMsgProducer_Send( producer_, ems_msg );
    tibemsMsg_Destroy( ems_msg );
}



// ----------------------------------------------------------------------------
void on_message(
    tibemsMsgConsumer consumer,
    tibemsMsg msg,
    void* closure )
{
    const char* txt;
    tibemsTextMsg_GetText( msg, &txt );
    ((fix::ems_consumer*)closure)->consume( fix::message( txt ) );
    tibemsMsg_Destroy( msg );
}

fix::ems_consumer::ems_consumer( const std::string& topic, fix::ems& ems, tibemsSession& session ) :
    ems_( ems )
{
    tibemsTopic_Create( &destination_, topic.c_str() );
    tibemsSession_CreateConsumer( session, &consumer_, destination_, NULL, TIBEMS_FALSE );
    tibemsMsgConsumer_SetMsgListener( consumer_, on_message, this );

    std::cout << "ems consumer: " << topic << std::endl;
}

void fix::ems_consumer::consume( const fix::message& msg )
{
    fix::session* session = ems_.fix_factory_.get_session( msg );
    ems_.recv_( *session, msg );
}

fix::ems_consumer::~ems_consumer()
{
    tibemsMsgConsumer_Close( consumer_ );
    tibemsDestination_Destroy( destination_ );
}



// ----------------------------------------------------------------------------
inline fix::ems::ems(
    const std::string& url,
    const std::string& user,
    const std::string& pass,
    fix::session_factory& fix_factory,
    fix::receive_function recv ) :

    fix_factory_( fix_factory ),
    recv_( recv ),
    ems_factory_( 0 ),
    connection_( 0 ),
    producer_session_( 0 ),
    consumer_session_( 0 ),
    error_( 0 )
{
    tibems_status status;
    tibemsErrorContext_Create( &error_ );

    ems_factory_ = tibemsConnectionFactory_Create();
    if( !ems_factory_ ) {
        fail();
    }

    status = tibemsConnectionFactory_SetServerURL( ems_factory_, url.c_str() );
    if( status != TIBEMS_OK ) {
        fail();
    }

    status = tibemsConnectionFactory_CreateConnection( ems_factory_, &connection_, user.c_str(), pass.c_str() );
    if( status != TIBEMS_OK ) {
        fail();
    }

    status = tibemsConnection_CreateSession( connection_, &producer_session_, TIBEMS_FALSE, TIBEMS_AUTO_ACKNOWLEDGE );
    if( status != TIBEMS_OK ) {
        fail();
    }

    status = tibemsConnection_CreateSession( connection_, &consumer_session_, TIBEMS_FALSE, TIBEMS_AUTO_ACKNOWLEDGE );
    if( status != TIBEMS_OK ) {
        fail();
    }

    std::cout << "connected to ems: " << url << std::endl;
}

inline fix::ems::~ems()
{
    producers_.clear();
    consumers_.clear();

    tibemsConnection_Close( connection_ );
}

inline void fix::ems::consumer( const std::string& topic )
{
    consumers_.emplace_back( new ems_consumer( topic, *this, consumer_session_ ) );
}

inline fix::session* fix::ems::producer( const std::string& topic, const fix::user& user )
{
    fix::session* session = fix_factory_.get_session( user );
    producers_.emplace_back( new ems_producer( topic, *session, producer_session_ ) );
    return session;
}

inline void fix::ems::start()
{
    tibemsConnection_Start( connection_ );
}

inline void fix::ems::fail()
{
    const char* str;

    tibemsErrorContext_GetLastErrorString( error_, &str );
    std::cerr << "ems error: " << str << std::endl;

    tibemsErrorContext_GetLastErrorStackTrace( error_, &str );
    std::cerr << "ems stack:\n" << str << std::endl;

    throw std::runtime_error( "ems error!" );
}
