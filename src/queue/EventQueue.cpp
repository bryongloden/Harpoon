#include "EventQueue.hpp"
#include "EventQueue_Impl.hpp"

#include <chrono>


EventQueue::EventQueue() :
	impl{new EventQueue_Impl()}
{
}

EventQueue::EventQueue(std::set<UUID> eventsToBeProcessed) :
	impl{new EventQueue_Impl()},
	eventsToBeProcessed{eventsToBeProcessed} {
}

EventQueue::~EventQueue() {
}

void EventQueue::sendEvent(std::shared_ptr<IEvent> event) {
	std::unique_lock<std::timed_mutex> lock(impl->queueMutex);
	impl->events.push_back(event);
	impl->eventCondition.notify_one();
}

int EventQueue::getEvent(int timeout /* milliseconds */, std::shared_ptr<IEvent>& event) {
	std::chrono::milliseconds timeoutMillis(timeout);
	std::unique_lock<std::timed_mutex> lock(impl->queueMutex, std::defer_lock);
	bool lockResult = lock.try_lock_for(timeoutMillis); // lock for checking queue fill status
	if (!lockResult)
		return false;

	if (impl->events.size() == 0) {
		lock.unlock();
		std::unique_lock<std::mutex> emptyLock(impl->emptyQueueMutex);
		// wait until queue is filled or timeout
		std::cv_status waitResult = impl->eventCondition.wait_for(emptyLock, timeoutMillis);
		if (waitResult == std::cv_status::timeout)
			return false;
		// try to lock queue
		lockResult = lock.try_lock_for(timeoutMillis);
		if (!lockResult)
			return false;
	}

	// get and remove event from queue
	event = impl->events.front();
	impl->events.pop_front();
	return true;
}

bool EventQueue::canProcessEvent(UUID eventType) {
	return eventsToBeProcessed.size() == 0 || eventsToBeProcessed.count(eventType) > 0;
}

