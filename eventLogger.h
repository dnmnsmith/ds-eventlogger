#ifndef EVENT_LOGGER
#define EVENT_LOGGER

#include <string>
#include <tntdb/connection.h>
#include <boost/filesystem.hpp>

class eventLogger
{
  public:
     eventLogger( tntdb::Connection & conn );
     virtual ~eventLogger();

    int logEvent( const std::string &location, const std::string& desc, const std::string& value );
    int logEventSensor(   const std::string& sensorId, const std::string& desc, const std::string& value );
    int logEventLocation( int locationId, const std::string& desc, const std::string& value );

    int getLocationId( const std::string &location_name );
    int getSensorLocationId( const std::string &location_name );

  private:
     boost::filesystem::path m_LogRootPath;

     tntdb::Connection & m_conn;
};


#endif


