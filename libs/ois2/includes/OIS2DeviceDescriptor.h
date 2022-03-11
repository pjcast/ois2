#pragma once
#ifndef _HGDDEC
#define _HGDDEC
#include <string_view>
#include <sstream>

namespace OIS2
{
	enum DeviceType
	{
		DT_GenericHid,
		DT_JoyStrick,
		DT_Keyboard,
		DT_Mouse
	};

	class DeviceDescriptor
	{
	private:

	public:
		DeviceDescriptor()
		{
		}

		DeviceType Type() const { return DeviceType::DT_GenericHid; }
		bool IsOpen() const { return false; }
		const std::string_view Name() const { return "Hello!"; }
		const std::string_view Identifier() const { return "USB:0:HID:0:Device:3:Mouse"; }
	};

	//! Convience method to debug to output stream
	std::ostream& operator << (std::ostream &os, const DeviceDescriptor &descriptor);
}
#endif