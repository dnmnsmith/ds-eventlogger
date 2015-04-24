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

    void logUnknownSensorEvent(const Event &event );
    void knownSensorEvent( const std::string &sensorId );

    int getLocationId( const std::string &location_name );
    int getSensorLocationId( const std::string &location_name );
    std::string getSensorLocationName( const std::string &sensor_id );

    std::vector<Event> getLatestEvents();
    std::vector<Event> getUnknownSensorEvents();

  private:
     boost::filesystem::path m_LogRootPath;

     // Map Event Key (Location:Measurement) to the event itself
     std::map<std::string, Event> m_latestEventMap;

     // Unknown sensor collection. Key is sensor id, Location is set to the sensor id.
     std::map<std::string, Event> m_unknownSensors;

     tntdb::Connection & m_conn;
};


#endif


