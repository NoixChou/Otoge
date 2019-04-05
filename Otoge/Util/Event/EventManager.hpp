#pragma once
#include "Event.hpp"
#include "../Debug/Logger.h"

class EventManager
{
private:
    static std::vector<std::function<bool(std::shared_ptr<Event>)>> EventHandlers_;

public:
    static void RegisterHandler(std::function<bool(std::shared_ptr<Event>)> eventHandler);
    static void SendEvent(std::shared_ptr<Event> e);
};
