#pragma once

#include "fix/message.hpp"

namespace fix {

class transport
{
public:
    virtual void send( const fix::message& ) = 0;
};

}
