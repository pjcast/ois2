#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
#include "OIS2InputManager.h"

using namespace std;
using namespace OIS2;

void TakesReference(const InputManager& happy)
{
	std::cout << "In TakesReference" << std::endl;

	std::string f = "Hi!";
	f += "Yes!";

	std::ostringstream str;
	str << "Hi" << 34 << "hmm" << 45.3 << endl;
	cout << str.str();

	vector<int> v = {1, 2, 3};
	for(auto it = v.begin(); it != v.end(); ++it)
	{
		cout << *it << endl;
	}
	for_each(v.begin(), v.end(), [](int& n)
	{
		cout << n << endl;
	});
	for(auto num : v)
	{
		cout << num << endl;
	}

	// Map - old way
	map<string, int> data = {{"Hi", 1}};
	for(auto it = data.begin(); it != data.end(); ++it)
	{
		cout << it->first << ": " << it->second << endl;
	}
	// Newer way c++11
	for(auto const& it : data)
	{
		cout << it.first << ": " << it.second << endl;
	}
	// Newest way c++17+
	for(auto const& [key, value] : data)
	{
		cout << key << ": " << value << endl;
	}
}

void TakesConstPtr(const InputManager* happy)
{
	std::cout << "In TakesConstPtr" << std::endl;
}

void DeviceConnectedCallback(const DeviceDescriptor& device)
{
	std::cout << "DeviceConnectedCallback: " << device << std::endl;
}

void DeviceDisconnectedCallback(const DeviceDescriptor& device)
{
	std::cout << "DeviceDisconnectedCallback: " << device << std::endl;
}

int main()
{
	auto inputManager = std::make_unique<InputManager>(ILogger::UseStdLogger());

	auto someLambda = [inputManager = move(inputManager)]() 
	{
		std::cout << "In Lambda" << std::endl;
		TakesReference(*inputManager);
		TakesConstPtr(inputManager.get());

		inputManager->SetDeviceConnectedCallback(std::bind(DeviceConnectedCallback, placeholders::_1));
		inputManager->SetDeviceDisconnectedCallback(std::bind(DeviceDisconnectedCallback, placeholders::_1));

		cout << "Current Devices: \n";
		auto devices = inputManager->GetDiscoveredDevices();
		for(auto device : devices)
		{
			cout << device;
		}

		cout << "Press any key: \n";
		getchar();
		cout << "Shutting down... (currently, takes ~10 secs to end)\n";
	};

	someLambda();
}
