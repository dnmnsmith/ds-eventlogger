#ifndef EVENT_LOGGER
#define EVENT_LOGGER

#include <string>
#include <tntdb/connection.h>
#include <boost/filesystem.hpp>

#include <map>
#include <vector>
#include <deque>

#include "Event.h"

class eventLogger
{
  public:
     eventLogger( tntdb::Connection & conn );
     virtual ~eventLogger();


    int logEventLocation( const std::string &location, const std::string& desc, const std::string& value );
    int logEventSensor( const std::string& sensorId, const std::string& desc, const std::string& value );

    int handleEvent( const Event &event  );	// Normal event handling
    bool logEvent( const Event &event );	// Push to local cache. Return true if unique enough to store in DB
    int storeEvent( const Event &event );   // Store event in DB

    void logUnknownSensorEvent(const Event &event );
    void knownSensorEvent( const std::string &sensorId );

    int getLocationId( const std::string &location_name );
    int getSensorLocationId( const std::string &location_name );
    std::string getSensorLocationName( const std::string &sensor_id );

    std::vector<Event> getLatestEvents();
    std::vector<Event> getUnknownSensorEvents();

    Event getMin( const std::string &location, const std::string& desc );
    Event getMax( const std::string &location, const std::string& desc );

    std::vector<Event> getMinEvents();
    std::vector<Event> getMaxEvents();
    
    std::vector<Event> getLocationEvents( const std::string &location );

    std::vector<std::string> getLocations();

    std::map< std::string, std::vector<Event> > getEventSummary( );

  private:
     boost::filesystem::path m_LogRootPath;

     // Map Event Key (Location:Measurement) to the event itself
//     std::map<std::string, Event> m_latestEventMap;
     std::map< std::string, std::deque< Event > > m_eventCache;

     void initCache();
     void flushCache();

     // Unknown sensor collection. Key is sensor id, Location is set to the sensor id.
     std::map<std::string, Event> m_unknownSensors;

     std::map<std::string, Event> m_minReading;
     std::map<std::string, Event> m_maxReading;

     void recalcMinMax();
     void recalcMinMax( std::string measName );

     tntdb::Connection & m_conn;
};


#endif


