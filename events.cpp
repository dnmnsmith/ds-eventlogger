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

void addNowEvent( std::vector<Event> &events )
{
  // 
  if (events.size() != 0)
  {
    time_t timeNow;
    time( &timeNow );
        
    Event nowValue = events.back();
    nowValue.setTime( timeNow );
    events.push_back( nowValue );
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

      addNowEvent( events );

      printEvents( events );
    }   
    
    if (bDoSummary.isSet())
    {
       //    evClass (min/max/...)
       std::map< std::string, std::vector<Event> > typeMap = getEventSummary( );
       std::set< std::string > locs;
       std::set< std::string > eventClass;

       std::map< std::string, std::vector<Event> >::iterator typeIt = typeMap.begin();
       for (; typeIt != typeMap.end(); ++typeIt)
       {
         //printEvents( typeIt->second, typeIt->first );
         if (eventClass.count( typeIt->first ) == 0 )
            eventClass.insert( typeIt->first );
         BOOST_FOREACH( Event e, typeIt->second )
         {
            if (locs.count( e.getKey() ) == 0)
               locs.insert( e.getKey() );
         }
       }
       // Now have list of locations and list of 
       //       Location Key             evClass       Value
       std::map< std::string, std::map< std::string, std::string > > locationEventMap;
       BOOST_FOREACH( std::string loc, locs )
       {
        std::map< std::string, std::string > locationEvents;
        BOOST_FOREACH( std::string evType, eventClass )
        {
           // Add blank value for each class at the location
           locationEvents.insert( std::pair< std::string, std::string >( evType, "" ) );  
        }
        // Add to location map.
        locationEventMap.insert( std::pair< std::string, std::map< std::string, std::string > >( loc, locationEvents ) );
       }
       
       for (typeIt = typeMap.begin(); typeIt != typeMap.end(); ++typeIt)
       {
         const std::string &evType = typeIt->first;  // "min/max/act/..."
         BOOST_FOREACH( Event e, typeIt->second )
         {
            std::string loc = e.getKey();

            if ( locationEventMap.find( loc ) == locationEventMap.end())
               throw std::runtime_error("Location lookup failure. Can't Happen");
            
            std::map< std::string, std::string > &locationSummary = locationEventMap.find( loc )->second;
            
            if ( locationSummary.find( evType ) == locationSummary.end() )
               throw std::runtime_error("Location/Event type lookup failure. Can't Happen");
               
            locationSummary.find( evType )->second.assign( e.getValue() );
            //locationSummary.find( evType )->second.append( " " );
            locationSummary.find( evType )->second.append( e.getUnits() );
         }
       }
       
       std::map< std::string, std::map< std::string, std::string > >::iterator locMapIt = locationEventMap.begin();
       
       for( ; locMapIt != locationEventMap.end(); ++locMapIt)
       {
         const std::string &loc = locMapIt->first;
         std::map< std::string, std::string > & locSummary = locMapIt->second;
         
//         std::cout << loc.substr( 0, loc.find_first_of(":") );
         
         std::cout << loc << ":";
         
         std::map< std::string, std::string >::iterator locSummaryIt = locSummary.begin();
         for (; locSummaryIt != locSummary.end(); ++locSummaryIt)
         {
            const std::string &value = locSummaryIt->first;
            if (locSummaryIt != locSummary.begin())
               std::cout << ","; 
            std::cout << value << "=" << locSummaryIt->second;
         }
         std::cout << std::endl;
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
