#pragma once

#include "fix/message.hpp"

namespace fix {

class responder
{
public:
    virtual void respond( const fix::message& ) = 0;
};

}
