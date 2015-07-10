#pragma once

#include "fix/constants.hpp"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace fix {

typedef uint32_t tag;

typedef std::string type;

class message
{
public:
    message();

    message( const std::string& );

    void add( const fix::message& );

    template< typename T >
    void add( fix::tag, T );

    template< typename H >
    void parse( H ) const;

    size_t size() const;

    void clear();

    char operator[]( size_t n ) const;

    const std::string& str() const;

private:
    std::string buf_;
};

}



// ----------------------------------------------------------------------------
inline std::ostream& operator<<( std::ostream& out, const fix::message& msg )
{
    out << msg.str();
    return out;
}

inline fix::message::message()
{
}

inline fix::message::message( const std::string& buf ) :
    buf_( buf )
{
}

inline void fix::message::add( const fix::message& msg )
{
    buf_.append( msg.str() );
}

template< typename T >
inline void fix::message::add( fix::tag t, T v )
{
    std::stringstream ss;
    ss << t << "=" << v << delim_char;
    buf_.append( ss.str() );
}

template< typename H >
inline void fix::message::parse( H handler ) const
{
    static boost::char_separator< char > field_sep( delim_str );
    boost::tokenizer< boost::char_separator< char > > fields( buf_, field_sep );
    for( auto field : fields )
    {
        std::vector< std::string > strs;
        boost::split( strs, field, boost::is_any_of( "=" ) );
        handler( boost::lexical_cast< fix::tag >( strs[0] ), strs[1] );
    }
}

inline const std::string& fix::message::str() const
{
    return buf_;
}

inline size_t fix::message::size() const
{
    return buf_.size();
}

inline void fix::message::clear()
{
    buf_.clear();
}

inline char fix::message::operator[]( size_t n ) const
{
    return buf_[n];
}
