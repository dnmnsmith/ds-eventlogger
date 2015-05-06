#include "Event.h"

#include <vector>

#include <iostream>
#include <cxxtools/log.h>
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/remoteprocedure.h>

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>



void printEvents( const std::vector<Event> &events, const std::string &prefix = std::string() )
{
   BOOST_FOREACH( Event e, events )
	 {
    if (!prefix.empty())
    {
     std::cout << prefix << " ";
    }
 		std::cout << e << std::endl;
    }
}

void usage( const std::string &progName )
{
 std::cerr << "Usage : " << progName << " -p <port> -s <server> -l -i -a -u -L -E <loc> -S" << std::endl;
 std::cerr << "\t-p <port>      Specify server RPC port, default 7004" << std::endl;
 std::cerr << "\t-s <server>    Specify server name, default webpi" << std::endl;
 std::cerr << "\t-l             Get latest events" << std::endl;
 std::cerr << "\t-i             Get minumum events" << std::endl;
 std::cerr << "\t-a             Get maximum events" << std::endl;
 std::cerr << "\t-u             Get unknown events" << std::endl;
 std::cerr << "\t-L             Get location list" << std::endl;
 std::cerr << "\t-E <location>  Get Location events" << std::endl;
 std::cerr << "\t-S             Get event summary (actual/min/max for each location)" << std::endl;
}

int main( int argc, char **argv )
{
  try
  {
    boost::filesystem::path progPath( argv[ 0 ] );
    std::string progName = progPath.filename().string(); 

    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7004);
    cxxtools::Arg<std::string> serverName(argc, argv, 's', "webpi");
    cxxtools::Arg<bool> bDoLatest( argc,argv,'l',(progName.compare("latestEvents") == 0));
    cxxtools::Arg<bool> bDoMin( argc,argv,'i',(progName.compare("minEvents") == 0));
    cxxtools::Arg<bool> bDoMax( argc,argv,'a',(progName.compare("maxEvents") == 0));
    cxxtools::Arg<bool> bUnknown( argc,argv,'u',(progName.compare("unknownEvents") == 0));   
    cxxtools::Arg<std::string> locationEvents( argc,argv,'E');   
    cxxtools::Arg<bool> bLocations( argc,argv,'L',(progName.compare("locations") == 0));
    cxxtools::Arg<bool> bDoSummary( argc,argv,'S',(progName.compare("summaryEvents") == 0));
    
    if (argc != 1)
    {
      usage( progName );
      exit(1);
    }
        
    if ( !bDoLatest.isSet() && !bDoMin.isSet() && !bDoMax.isSet() && !bUnknown.isSet() && 
         !locationEvents.isSet() && !bLocations.isSet() && !bDoSummary.isSet())
    {
      usage( progName );
      exit(1);
    } 
    
    // and a json rpc client
    cxxtools::json::RpcClient client(serverName, port);

    // Define remote procedure with dobule return value and a 2 double
    // parameters.
    cxxtools::RemoteProcedure< std::vector<Event> > getLatestEvents(client, "getLatestEvents");
    cxxtools::RemoteProcedure< std::vector<Event> > getMinEvents(client, "getMinEvents");
    cxxtools::RemoteProcedure< std::vector<Event> > getMaxEvents(client, "getMaxEvents");
    cxxtools::RemoteProcedure< std::vector<Event> > getUnknownSensorEvents(client, "getUnknownSensorEvents");
    cxxtools::RemoteProcedure< std::vector<Event>, const std::string & > getLocationEvents(client, "getLocationEvents");
    cxxtools::RemoteProcedure< std::vector<std::string> > getLocations(client, "getLocations");
    cxxtools::RemoteProcedure< std::map< std::string, std::vector<Event> > > getEventSummary(client, "getEventSummary");

    if (bLocations)
    {
    	std::vector<std::string> locations = getLocations();
      BOOST_FOREACH( std::string s, locations )
	    {
 		     std::cout << s << std::endl;
      }
    }


    if (bDoLatest)
    {
    	std::vector<Event> events = getLatestEvents();
      printEvents( events );
    }
    
    if (bDoMin)
    {
    	std::vector<Event> events = getMinEvents();
      printEvents( events );
    }
    
    if (bDoMax)
    {
    	std::vector<Event> events = getMaxEvents();
      printEvents( events );
    }   

    if (bUnknown)
    {
    	std::vector<Event> events = getUnknownSensorEvents();
      printEvents( events );
    }   
    
    if (locationEvents.isSet())
    {
    	std::vector<Event> events = getLocationEvents( locationEvents );
      printEvents( events );
    }   
    
    if (bDoSummary.isSet())
    {
       std::map< std::string, std::vector<Event> > typeMap = getEventSummary( );

       std::map< std::string, std::vector<Event> >::iterator typeIt = typeMap.begin();
       for (; typeIt != typeMap.end(); ++typeIt)
       {
         printEvents( typeIt->second, typeIt->first );
       }
    }
    
  }
 catch (const std::exception& e)
 {
   std::cerr << e.what() << std::endl;
   return 1;
 }

 return 0;

}
