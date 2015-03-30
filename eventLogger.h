#ifndef EVENT_LOGGER
#define EVENT_LOGGER

#include <string>
#include <tntdb/connection.h>
#include <boost/filesystem.hpp>

#include <map>
#include <vector>

#include "Event.h"

class eventLogger
{
  public:
     eventLogger( tntdb::Connection & conn );
     virtual ~eventLogger();

    int logEventLocation( const std::string &location, const std::string& desc, const std::string& value );
    int logEventSensor( const std::string& sensorId, const std::string& desc, const std::string& value );

    int logEvent( const Event &event );


    int getLocationId( const std::string &location_name );
    int getSensorLocationId( const std::string &location_name );
    std::string getSensorLocationName( const std::string &sensor_id );

    std::vector<Event> getLatestEvents();

  private:
     boost::filesystem::path m_LogRootPath;

     std::map<std::string, Event> m_latestEventMap;

     tntdb::Connection & m_conn;
};


#endif


