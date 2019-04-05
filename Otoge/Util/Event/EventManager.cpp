#include "EventManager.hpp"

void EventManager::RegisterHandler(std::function<bool(std::shared_ptr<Event>)> eventHandler)
{
    EventHandlers_.push_back(eventHandler);
}

void EventManager::SendEvent(std::shared_ptr<Event> e)
{

}