#include <thread>
#include <chrono>
#include <stdexcept>
#include "OIS2InputManager.h"

using namespace OIS2;

InputManager::InputManager(std::shared_ptr<ILogger> logger)
{
	if (!logger)
		throw std::logic_error("Invalid ILogger instance passed in to InputManager");
	
	_logger = logger;
	_running = true;
	_thread = std::thread(&InputManager::ThreadedRunLoop, this);
}

InputManager::~InputManager() 
{
	_running = false;
	_thread.join();
}

void InputManager::SetDeviceConnectedCallback(const std::function<void(const DeviceDescriptor&)> &callback)
{
	std::scoped_lock(_threadLock);
	_connectedCallback = callback;
}

//! Events may be raised from a different thread
void InputManager::SetDeviceDisconnectedCallback(const std::function<void(const DeviceDescriptor&)> &callback)
{
	std::scoped_lock(_threadLock);
	_disconnectedCallback = callback;
}

void InputManager::ThreadedRunLoop()
{
	_logger->Log(LogLevel::Trace, "InputManager thread started...");
	while(_running)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
	_logger->Log(LogLevel::Trace, "InputManager thread end");
}

std::vector<DeviceDescriptor> InputManager::GetDiscoveredDevices()
{
	return std::vector<DeviceDescriptor>();
}
