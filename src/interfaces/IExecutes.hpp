#ifndef SRC_INTERFACES_IEXECUTES_HPP_
#define SRC_INTERFACES_IEXECUTES_HPP_

#include "flecs.h"

// Interface class for executing actions
class IExecutes {
 public:
    virtual void Execute(flecs::world* ecs) = 0;  // Pure virtual function
};

#endif  // SRC_INTERFACES_IEXECUTES_HPP_
