#ifndef QUERY_CORE_SERVER_CONFIGURATION_HPP
#define QUERY_CORE_SERVER_CONFIGURATION_HPP

#include <vector>
#include <netinet/in.h>
#include <string>
#include <string.h>

class Configuration {
	public:
		int open(const char *filename, const char *key);
		static std::string readString(char const * filename, char const * path, char const * name);

	protected:
		int readParameter(const char *filename, const char * key);
		int readThreadNumber(const char *filename, const char * key);
		int SeparateIP(const char *value,char * ip,int * port);

	public:
		unsigned int listen_port;
		unsigned int thread_stack_size;

		unsigned int receiver_num;

};

#endif /* QUERY_CORE_SERVER_CONFIGURATION_HPP */

