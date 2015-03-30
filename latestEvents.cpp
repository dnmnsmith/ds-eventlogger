#include "Event.h"

#include <vector>

#include <iostream>
#include <cxxtools/log.h>
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/remoteprocedure.h>

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>


int main( int argc, char **argv )
{
  try
  {
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7004);
    cxxtools::Arg<std::string> serverName(argc, argv, 's', "webpi");

    // and a json rpc client
    cxxtools::json::RpcClient client(serverName, port);

    // Define remote procedure with dobule return value and a 2 double
    // parameters.
    cxxtools::RemoteProcedure< std::vector<Event> > getLatestEvents(client, "getLatestEvents");

	std::vector<Event> events = getLatestEvents();

	for (std::vector<Event>::iterator it = events.begin(); it != events.end(); it++)
	{
		std::cout << (*it) << std::endl;
	}
  }
 catch (const std::exception& e)
 {
   std::cerr << e.what() << std::endl;
   return 1;
 }

 return 0;

}
