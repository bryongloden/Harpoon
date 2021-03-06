#include <iostream>
#include "IrcDatabase_Dummy.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "utils/ModuleProvider.hpp"

using namespace std;


PROVIDE_MODULE("irc_database", "dummy", IrcDatabase_Dummy);

IrcDatabase_Dummy::IrcDatabase_Dummy(EventQueue* appQueue)
    : EventLoop({
          EventInit::uuid,
          EventQuit::uuid,
          EventLoginResult::uuid
      })
    , appQueue{appQueue}
{
}

IrcDatabase_Dummy::~IrcDatabase_Dummy() {
}

bool IrcDatabase_Dummy::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "IrcDB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventInit::uuid) {
        std::cout << "IrcDB received INIT event" << std::endl;

        size_t userId = 1;
        auto login = make_shared<EventIrcActivateService>(userId);

        size_t serverId = 1;
        auto& loginConfiguration = login->addLoginConfiguration(serverId,
                                                                "TestServer");
        loginConfiguration.addHostConfiguration("127.0.0.1",
                                                6667,
                                                "wealllikedebian",
                                                false,
                                                false);

        loginConfiguration.addNick("iirc");
        loginConfiguration.addNick("iirc2");
        loginConfiguration.addNick("iirc3");
        loginConfiguration.addChannelLoginData(1, "#test", "", true);
        loginConfiguration.addChannelLoginData(2, "#test2", "", false);

        appQueue->sendEvent(login);
    }
    return true;
}

