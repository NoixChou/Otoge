#pragma once
#include "Event.hpp"

class EventHandler
{
private:
    std::vector<std::function<bool(Event*)>> EventHandlers_;

public:
    void registerHandler(std::function<bool(Event*)> eventHandler);
};
