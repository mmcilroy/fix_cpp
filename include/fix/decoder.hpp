#pragma once

#include "fix/message.hpp"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace fix {

class decoder
{
public:
    decoder();

    template< typename H >
    void decode( const char* msg, size_t size, H handler );

private:
    template< class H >
    void on_field( H handler );

    void init_state();

    enum field_state {
        TAG, VALUE
    };

    enum message_state {
        SEEK_HEADER, SEEK_CHECKSUM
    };

    field_state field_state_;
    message_state message_state_;

    std::string tag_;
    std::string val_;

    fix::message msg_;
};

}

inline fix::decoder::decoder()
{
    init_state();
}

inline void fix::decoder::init_state()
{
    tag_.clear();
    val_.clear();
    msg_.clear();

    field_state_ = TAG;
    message_state_ = SEEK_HEADER;
}

template< typename H >
inline void fix::decoder::decode( const char* msg, size_t size, H handler )
{
    for( size_t i = 0; i < size; ++i )
    {
        char c = msg[i];

        if( field_state_ == TAG )
        {
            if( c == '=' ) {
                field_state_ = VALUE; // got the tag now look for the value
            } else if( !isdigit( c ) ) {
                tag_.clear(); // ignore non numeric tags
            } else {
                tag_.push_back( c );
            }
        }
        else
        {
            if( c == delim_char )
            {
                on_field( handler );
                field_state_ = TAG;
                tag_.clear();
                val_.clear();
            }
            else
            {
                val_.push_back( c );
            }
        }
    }
}

template< typename H >
inline void fix::decoder::on_field( H handler )
{
    if( tag_.size() == 0 || val_.size() == 0 ) {
        return;
    }

    tag t = boost::lexical_cast< tag >( tag_.c_str() );
    msg_.add( t, val_ );

    if( message_state_ == SEEK_HEADER )
    {
        if( t == 8 ) {
            message_state_ = SEEK_CHECKSUM;
        }
    }
    else
    {
        if( t == 10 )
        {
            handler( msg_ );
            init_state();
        }
    }
}
