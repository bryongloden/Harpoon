#include <iostream>
#include "ChatDatabaseDummy.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLoginResult.hpp"
#include "event/EventActivateUser.hpp"

using namespace std;


ChatDatabaseDummy::ChatDatabaseDummy(EventQueue* appQueue) :
	EventLoop({
		EventInit::uuid,
		EventQuit::uuid,
		EventLogin::uuid
	}),
	appQueue{appQueue}
{
}

void ChatDatabaseDummy::doLogin(EventLogin* login) {
	shared_ptr<EventLoginResult> loginResult{make_shared<EventLoginResult>()};
	//loginResult->client = login->client;
	loginResult->success = true;
	loginResult->userId = 1;
#warning login always successful
	appQueue->sendEvent(loginResult);
}

bool ChatDatabaseDummy::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventInit::uuid) {
		std::cout << "DB received INIT event" << std::endl;

		size_t userId = 1;	
		std::map<size_t, IrcServerConfiguration> loginData;

		appQueue->sendEvent({make_shared<EventActivateUser>(userId, loginData)});
	} else if (eventType == EventQuit::uuid) {
		std::cout << "DB received QUIT event" << std::endl;
		return false;
	} else if (eventType == EventLogin::uuid) {
		doLogin(event->as<EventLogin>());
	}
	return true;
}

