#pragma once

class Event
{
public:
    Event();
    ~Event();

    std::string name = "";
    void *sender = nullptr;
};
