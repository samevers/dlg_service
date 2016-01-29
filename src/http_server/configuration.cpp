#include "configuration.hpp"
#include <netdb.h>
#include "config_map.hpp"
#include <string>
#include <iostream>
//#include "service_log.hpp"

int Configuration::open(const char *filename, const char *key)
{
	if (readParameter(filename, key))
		return -1;
	if (readThreadNumber(filename, key))
		return -1;
	
	return 0;
}

int Configuration::SeparateIP(const char * value,char * ip, int * port)
{
	char * pos = strchr(const_cast<char*>(value),':');
	if(!pos)
	{
		return -1;
	}
	//*port = htons(atoi(pos+1));
	*port = atoi(pos+1);
	int len = pos - value;
	memcpy(ip,value,len);
	ip[len] = '\0';
	return 0;
}

int Configuration::readParameter(const char *filename, const char * key)
{
	config_map config;
	config.import(filename);

	std::string full_key(std::string(key) + "/Parameter");
	config.set_section(full_key.c_str());
	const char *value;

	if (config.get_value("ListenPort", value))
	{
		return -1;
	}
	//listen_port = htons(atoi(value));
	listen_port = atoi(value);

	if (config.get_value("ThreadStackSize", value))
	{
		thread_stack_size = 256 * (1 << 10);
	}
	else
		thread_stack_size = atoi(value) * (1 << 10);

	return 0;
}
	

int Configuration::readThreadNumber(const char *filename, const char * key)
{
	config_map config;
	//if (config.import(filename))
	config.import(filename);

	std::string full_key(std::string(key) + "/ThreadNumber");
	config.set_section(full_key.c_str());
	const char *value;

	if (config.get_value("ReceiverThread", value))
	{
		receiver_num = 3;
	}
	else
		receiver_num = atoi(value);

	return 0;
}

std::string Configuration::readString(char const * filename, char const * path, char const * name)
{
	config_map config;
	std::string ret = "";
	const char * value;

	if (config.import(filename))
		return ret;

	config.set_section(path);
	if (config.get_value(name, value))
		return ret;

	ret = value;
	return ret;
}

