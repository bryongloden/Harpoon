#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <map>
#include <memory>
#include "queue/EventLoop.hpp"


class EventQueue;
class EventLoginResult;
class UserManager : public EventLoop {
    EventQueue* appQueue;
    std::map<size_t, std::map<size_t, std::shared_ptr<EventLoop>>> users;
public:
    UserManager(EventQueue* appQueue);
    virtual ~UserManager();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
