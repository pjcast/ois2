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

//https://github.com/gavv/snippets/blob/master/udev/udev_monitor_usb.c
#include <libudev.h>
#include <stdio.h>

using namespace std;
using namespace OIS2;

static struct udev_device* get_child(struct udev* udev, struct udev_device* parent, const char* subsystem)
{
	struct udev_device* child = NULL;
	struct udev_enumerate *enumerate = udev_enumerate_new(udev);

	udev_enumerate_add_match_parent(enumerate, parent);
	udev_enumerate_add_match_subsystem(enumerate, subsystem);
	udev_enumerate_scan_devices(enumerate);

	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
	struct udev_list_entry *entry;

	udev_list_entry_foreach(entry, devices) {
		const char *path = udev_list_entry_get_name(entry);
		child = udev_device_new_from_syspath(udev, path);
		break;
	}

	udev_enumerate_unref(enumerate);
	return child;
}

void enumerate_subsystems(struct udev* udev, std::vector<std::string> &subsystems)
{
	auto enumerate = udev_enumerate_new(udev);

	udev_enumerate_scan_subsystems(enumerate);

	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
	struct udev_list_entry *entry;

	udev_list_entry_foreach(entry, devices) 
	{
		const char* path = udev_list_entry_get_name(entry);
		struct udev_device* dev = udev_device_new_from_syspath(udev, path);

		printf("Subsystem: path:%s\n", path);

		udev_device_unref(dev);
	}


	udev_enumerate_unref(enumerate);
}

void enumerate_usb_mass_storage(struct udev* udev)
{
	struct udev_enumerate* enumerate = udev_enumerate_new(udev);

	udev_enumerate_add_match_subsystem(enumerate, "usb");
	udev_enumerate_add_match_property(enumerate, "DEVTYPE", "input");
	udev_enumerate_scan_devices(enumerate);

	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
	struct udev_list_entry *entry;

	udev_list_entry_foreach(entry, devices) {
		const char* path = udev_list_entry_get_name(entry);
		struct udev_device* scsi = udev_device_new_from_syspath(udev, path);
		printf("Hello\n");

		//struct udev_device* block = get_child(udev, scsi, "block");
		//struct udev_device* scsi_disk = get_child(udev, scsi, "scsi_disk");

		//struct udev_device* usb = udev_device_get_parent_with_subsystem_devtype(scsi, "usb", "usb_device");

		// if (block && scsi_disk && usb) {
		//     printf("block = %s, usb = %s:%s, scsi = %s\n",
		//            udev_device_get_devnode(block),
		//            udev_device_get_sysattr_value(usb, "idVendor"),
		//            udev_device_get_sysattr_value(usb, "idProduct"),
		//            udev_device_get_sysattr_value(scsi, "vendor"));
		// }

		// if (block) {
		//     udev_device_unref(block);
		// }

		// if (scsi_disk) {
		// 	udev_device_unref(scsi_disk);
		// }

		// udev_device_unref(scsi);
	}

	udev_enumerate_unref(enumerate);
}

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
	struct udev* udev = udev_new();

	std::vector<std::string> subsystems;
	enumerate_subsystems(udev, subsystems);

	udev_unref(udev);

	// /sys/class/input

	// auto inputManager = std::make_unique<InputManager>(ILogger::UseStdLogger());

	// auto someLambda = [inputManager = move(inputManager)]() 
	// {
	// 	std::cout << "In Lambda" << std::endl;
	// 	TakesReference(*inputManager);
	// 	TakesConstPtr(inputManager.get());

	// 	inputManager->SetDeviceConnectedCallback(std::bind(DeviceConnectedCallback, placeholders::_1));
	// 	inputManager->SetDeviceDisconnectedCallback(std::bind(DeviceDisconnectedCallback, placeholders::_1));

	// 	cout << "Current Devices: \n";
	// 	auto devices = inputManager->GetDiscoveredDevices();
	// 	for(auto device : devices)
	// 	{
	// 		cout << device;
	// 	}

	// 	cout << "Press any key: \n";
	// 	getchar();
	// 	cout << "Shutting down... (currently, takes ~10 secs to end)\n";
	// };

	// someLambda();
}
