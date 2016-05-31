#include <iostream>
#include <csignal>
#include <list>

#include "Application.hpp"
#include "queue/EventQueue.hpp"
#include "user/UserManager.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "db/LoginDatabase_Dummy.hpp"
#include "db/IrcDatabase_Dummy.hpp"
#include "server/ws/WebsocketServer.hpp"

using namespace std;


Application::Application() :
	guard{this},
	EventLoop()
{
	EventQueue* queue = getEventQueue();
	userManager = make_shared<UserManager>(queue);
	eventHandlers.push_back(userManager);
	eventHandlers.push_back(make_shared<LoginDatabase_Dummy>(queue));
	eventHandlers.push_back(make_shared<IrcDatabase_Dummy>(queue));

#ifdef USE_WEBSOCKET_SERVER
	eventHandlers.push_back(make_shared<WebsocketServer>(queue));
#endif

	for (auto& eventHandler : eventHandlers)
		eventHandler->getEventQueue()->sendEvent(make_shared<EventInit>());
}

bool Application::onEvent(std::shared_ptr<IEvent> event) {
	// which event do we process?
	UUID eventType = event->getEventUuid();

	for (auto& eventHandler : eventHandlers) {
		auto eventQueue = eventHandler->getEventQueue();
		if (eventQueue->canProcessEvent(event.get()))
			eventQueue->sendEvent(event);

		// send events to managed subqueues
		auto userEventLoop = dynamic_cast<ManagingEventLoop*>(eventHandler.get());
		if (userEventLoop != nullptr)
			userEventLoop->sendEventToUser(event); // sends only to matching user (only if IUserEvent)
	}

	if (eventType == EventQuit::uuid) {
		cout << "Received Quit Event" << endl;
		for (auto& eventHandler : eventHandlers)
			eventHandler->join();
		cout << "Submodules were stopped" << endl;
		return false;
	}
	return true;
}

