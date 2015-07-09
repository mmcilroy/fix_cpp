#pragma once

#include "fix/session.hpp"
#include "publisher.hpp"

namespace fix {

struct event
{
    fix::session* session_;
    fix::message message_;
};

typedef publisher< event, blocking_sequence > event_publisher;
typedef subscriber< event, blocking_sequence > event_subscriber;

}
