#include <iostream>

#include "eventLogger.h"

#include <time.h>
#include <sstream>

#include <linux/limits.h>

#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>


eventLogger::eventLogger( tntdb::Connection & conn ) : m_conn( conn )
{


}

eventLogger::~eventLogger()
{

}


// Log an event at a location
int eventLogger::logEventLocation( const std::string &location, const std::string& desc, const std::string& value )
{
  try
  {
	Event event( location, desc, value );

	return logEvent( event );
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
	std::string locationName = getSensorLocationName( sensorId );

	Event event( locationName, desc, value );

	return logEvent( event );
  }
  catch (const std::exception& e)
  {
	std::cerr << "Exception in logEvent : " << e.what() << std::endl;
	return 1;
  }
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

std::string eventLogger::getSensorLocationName( const std::string &sensor_id )
{
  std::string locationName= "Unknown"; // Default; unknown location

  try
  {
 	  tntdb::Statement st = m_conn.prepare( "SELECT location_name FROM sensors JOIN locations using (location_id)  WHERE sensor_id=:v1" );
	  tntdb::Value value = st.set("v1", sensor_id ).selectValue();
      locationName = value.getString();
   }
  catch (const std::exception& e)
  {
  	std::cerr << "Exception in getSensorLocationName for sensor " << sensor_id << " : " << e.what() << std::endl;
  }

  return locationName;
}

std::vector<Event> eventLogger::getLatestEvents()
{
	std::vector<Event> latestEvents;

	for (std::map<std::string, Event>::iterator it=m_latestEventMap.begin(); it != m_latestEventMap.end(); it++)
	{
		latestEvents.push_back( it->second );
	}

	return latestEvents;
}


int eventLogger::logEvent( const Event &event )
{
	std::map<std::string, Event>::iterator it = m_latestEventMap.find(event.getKey() );

	if (it != m_latestEventMap.end())
	{
		it->second.assign(event);	// Over-write
	}
	else
	{
		m_latestEventMap.insert( std::pair<std::string, Event>(event.getKey(),event) );
	}

	try
	{
		int locationId = getLocationId( event.getLocation() );

		std::cerr << "logEvent " << event << std::endl;

		tntdb::Statement st = m_conn.prepare(
		"INSERT INTO readings( timestamp,location_id,desc,value) VALUES( :v1,:v2,:v3,:v4)");
		st.setDatetime( "v1", event.getDateTime() ).setInt( "v2", locationId ).setString("v3",event.getDesc()).setString("v4",event.getValue()).execute();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in logEvent : " << e.what() << std::endl;
		return 1;
	}
	return 0;
}


