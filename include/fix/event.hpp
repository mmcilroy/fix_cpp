#pragma once

#include "fix/session.hpp"
#include "pubsub/publisher.hpp"

namespace fix {

struct event
{
    fix::session* session_;
    fix::type type_;
    fix::message message_;
};

typedef pubsub::publisher< event, pubsub::blocking_sequence > event_publisher;
typedef pubsub::subscriber< event, pubsub::blocking_sequence > event_subscriber;

}
