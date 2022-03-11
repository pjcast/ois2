#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include "OIS2DeviceDescriptor.h"
#include "OIS2Logger.h"

namespace OIS2
{
	class InputManager
	{
	private:
		std::thread _thread;
		std::mutex _threadLock;
		std::shared_ptr<ILogger> _logger;
		bool _running; // TODO: Use promise? future? condition variable
		std::function<void(const DeviceDescriptor&)> _connectedCallback;
		std::function<void(const DeviceDescriptor&)> _disconnectedCallback;

	public:
		InputManager(std::shared_ptr<ILogger> logger);
		~InputManager();

		//! Events may be raised from a different thread. Register a callback when a new/existing device connects
		void SetDeviceConnectedCallback(const std::function<void(const DeviceDescriptor&)> &callback);

		//! Events may be raised from a different thread
		void SetDeviceDisconnectedCallback(const std::function<void(const DeviceDescriptor&)> &callback);

		//! Returns all currently known devices. DeviceConnected callback may arrive after/before calling this. This method does not block.
		std::vector<DeviceDescriptor> GetDiscoveredDevices();

	private:
		void ThreadedRunLoop();
	};
}
