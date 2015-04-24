#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/json/rpcserver.h>
#include <cxxtools/eventloop.h>

#include <tntdb/connection.h>
#include <tntdb/connect.h>

#include <string>
#include <boost/filesystem.hpp>

#include "eventLogger.h"

////////////////////////////////////////////////////////////////////////
// This defines functions, which we want to be called remotely.
//

// Parameters and return values of the functions, which can be exported must be
// serializable and deserializable with the cxxtools serialization framework.
// For all standard types including container classes in the standard library
// proper operators are defined in cxxtools.
//

std::string echo(const std::string& message)
{
  std::cout << message << std::endl;
  return message;
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    // initialize logging - this reads the file log.xml from the current directory
    log_init();

    // read the command line options

    // option -i <ip-address> defines the ip address of the interface, where the
    // server waits for connections. Default is empty, which tells the server to
    // listen on all local interfaces
    cxxtools::Arg<std::string> ip(argc, argv, 'i');

    // option -p <number> specifies the port, where jsonrpc requests are expected.
    // The default port is 7004 here.
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7004);
    cxxtools::Arg<std::string> dbname( argc, argv, 'd', "events.db");

    boost::filesystem::path path( dbname );
    if ( !exists(path) )
    {
     std::cerr << "Events database " << path << " doesn't exist" << std::endl;
     return false;
    }

    std::string dbconnect = "sqlite:";
    dbconnect.append(dbname);

    // We need a connection
    tntdb::Connection conn = tntdb::connect(dbconnect);

    eventLogger logger( conn );

    std::cerr << "Kitchen location id = " << logger.getLocationId( "Kitchen" ) << std::endl;

    logger.logEventLocation( "Kitchen", "Temperature", "22.0" );

    // create an event loop
    cxxtools::EventLoop loop;

    ////////////////////////////////////////////////////////////////////////
    // Json rpc

    // for the json rpc server we define a json server
    cxxtools::json::RpcServer jsonServer(loop, ip, port);

    // and register the functions in the server
    jsonServer.registerFunction("echo", echo);

    jsonServer.registerMethod< int, eventLogger, const std::string &, const std::string &, const std::string &>("logEventLocation", logger, &eventLogger::logEventLocation);
    jsonServer.registerMethod< int, eventLogger, const std::string &, const std::string &, const std::string &>("logEventSensor", logger, &eventLogger::logEventSensor);
    jsonServer.registerMethod< std::vector<Event>, eventLogger>("getLatestEvents", logger, &eventLogger::getLatestEvents);
    jsonServer.registerMethod< std::vector<Event>, eventLogger>("getUnknownSensorEvents", logger, &eventLogger::getUnknownSensorEvents);

    ////////////////////////////////////////////////////////////////////////
    // Run

    // now start the servers by running the event loop
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
