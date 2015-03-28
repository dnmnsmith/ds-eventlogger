#include <iostream>

#include "eventLogger.h"

#include <time.h>
#include <sstream>

#include <linux/limits.h>

#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>


// Known locations
const std::string LOCATIONS_FILE = "locations.cfg" ;

// Map of devices to location names.
const std::string DEVICE_MAP_FILE = "locMapFile.cfg";

const std::string EVENTS_FILE_NAME = "event_log.txt";

const char *TIMESTAMP_FORMAT = "%F:%T";

const std::string FIELD_SEPERATOR = "\t";

eventLogger::eventLogger( tntdb::Connection & conn ) : m_conn( conn )
{


}

eventLogger::~eventLogger()
{

}


// Log an event at a location
int eventLogger::logEvent( const std::string &location, const std::string& desc, const std::string& value )
{
  try
  {
	// Get the location id
	int locationId = getLocationId( location );

	return logEventLocation( locationId, desc, value );
  }
  catch (const std::exception& e)
  {
	std::cerr << "Exception in logEvent : " << e.what() << std::endl;
	return 1;
  }
}

// Log an event from a sensor
int eventLogger::logEventSensor( const std::string& sensorId, const std::string& desc, const std::string& value )
{
  try
  {
	// Get the location id
	int locationId = getSensorLocationId( sensorId );

	return logEventLocation( locationId, desc, value );
  }
  catch (const std::exception& e)
  {
	std::cerr << "Exception in logEvent : " << e.what() << std::endl;
	return 1;
  }
}


int eventLogger::logEventLocation( int locationId, const std::string& desc, const std::string& value )
{
  try
  {
    std::cerr << "logEventLocation LocId=" << locationId << " " << " desc " << " " << value << std::endl;

    tntdb::Statement st = m_conn.prepare(
			"INSERT INTO readings( timestamp,location_id,desc,value) VALUES( datetime(\'now\'),:v1,:v2,:v3)");
	st.setInt( "v1", locationId ).setString("v2",desc).setString("v3",value).execute();
  }
  catch (const std::exception& e)
  {
	std::cerr << "Exception in logEventLocation : " << e.what() << std::endl;
	return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------------
// Return 0 on error, or the corresponding location id.
// Throws (probably 'not found') on error.
//
int eventLogger::getLocationId( const std::string &location_name )
{
  int locationId = 0; // Default; unknown location

  try
  {
	  tntdb::Statement st = m_conn.prepare( "SELECT location_id FROM locations WHERE location_name=:v1" );
	  tntdb::Value value = st.set("v1", location_name ).selectValue();
    locationId = value.getInt();
  }
  catch (const std::exception& e)
  {
  	std::cerr << "Exception in getLocationId for location " << location_name << " : " << e.what() << std::endl;
  }
   
	return locationId;
}

int eventLogger::getSensorLocationId( const std::string &sensor_id )
{
  int locationId = 0; // Default; unknown location

  try
  {
 	  tntdb::Statement st = m_conn.prepare( "SELECT location_id FROM sensors WHERE sensor_id=:v1" );
	  tntdb::Value value = st.set("v1", sensor_id ).selectValue();
    locationId = value.getInt();
   }
  catch (const std::exception& e)
  {
  	std::cerr << "Exception in getSensorLocationId for sensor " << sensor_id << " : " << e.what() << std::endl;
  }
   
	return locationId;
}

