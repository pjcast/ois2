#include "OIS2DeviceDescriptor.h"

std::ostream& OIS2::operator << (std::ostream &os, const OIS2::DeviceDescriptor &descriptor)
{
	return (os << "DeviceDescriptor: Type: " 
		<<  descriptor.Type() << " Name: "
		<< 	descriptor.Name() << " Id: "
		<< descriptor.Identifier());
}
