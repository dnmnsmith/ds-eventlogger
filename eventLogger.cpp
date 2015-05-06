#include <iostream>

#include "eventLogger.h"

#include <time.h>
#include <sstream>
#include <algorithm>

#include <linux/limits.h>

#include <tntdb/statement.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>

static const double MAX_CACHE_AGE = 24 * 60.0 * 60.0;	// Max age of things in cache; seconds.


eventLogger::eventLogger( tntdb::Connection & conn ) :
	m_eventCache(),
	m_unknownSensors(),
	m_conn( conn )
{
  initCache();
}

eventLogger::~eventLogger()
{

}


// Log an event at a location
int eventLogger::logEventLocation( const std::string &location, const std::string& desc, const std::string& value )
{
  try
  {
//	std::cerr << "logEventLocation("<< location << "," << desc << "," << value << ")" << std::endl;
	Event event( location, desc, value );

	return handleEvent( event );
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

	if (locationName.compare("Unknown") == 0)
	{
		Event event( sensorId, desc, value );
		logUnknownSensorEvent( event );
		return 1;
	}
	else
	{
		// Remove from unknown sensors
		knownSensorEvent( sensorId );
		Event event( locationName, desc, value );
		return handleEvent( event );
	}

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

	for (std::map< std::string, std::deque< Event > >::iterator it=m_eventCache.begin(); it != m_eventCache.end(); it++)
	{
		std::deque< Event > &queue = it->second;
		if (queue.size() != 0)
			latestEvents.push_back( queue.back() );
	}

	return latestEvents;
}

// Normal event handling
int eventLogger::handleEvent( const Event &event  )
{
	if ( logEvent( event ) )
		return storeEvent( event );
	else
		return 0;
}

bool eventLogger::logEvent( const Event &event )
{
	bool store=true;

  try
  {
  	std::map< std::string, std::deque< Event > >::iterator it = m_eventCache.find(event.getKey() );
  
    std::cerr << "logEvent " << event << " value = " << event.getValue() << std::endl;
  
  	if (it != m_eventCache.end())
  	{
     
  		// Already got an entry
  		std::deque< Event > &queue = it->second;
  
  		if (queue.size() != 0)
  		{
//        std::cerr << "Queue " << event.getKey() << " exists, has entries" << std::endl;
  			// Only store in DB
  			store = queue.back().differ( event );
  
  			if (store)
  			{
  				// New data.  Add it to the queue.
  				queue.push_back( event );
  			}
  			else
  			{
  				// Over-write, updates timestamp of newest event
  				queue.back().assign(event);
  			}
  		}
  		else
  		{
//        std::cerr << "Queue " << event.getKey() << " exists, empty" << std::endl;
  			// Queue is empty. Odd. Stuff it in anyways.
  			store = true;
  			queue.push_back( event );
  		}
  	}
  	else
  	{
//        std::cerr << "Queue " << event.getKey() << " needs created" << std::endl;
   
  		// New queue;
  		store=true;
  
  		// Create new queue; init with our event.
  		std::deque< Event > newQueue;
  		newQueue.push_back( event );
  
  		// Add to the lookup
  		m_eventCache.insert( std::pair<std::string, std::deque< Event > >(event.getKey(),newQueue) );
     
//      std::cerr << "Queue " << event.getKey() << " has size " << m_eventCache[ event.getKey() ].size() << std::endl;
  	}
  
  	if (store)
  	{
//      std::cerr << "Stored event. Recalc min/max" << std::endl;

  		std::map<std::string, Event>::iterator itMin = m_minReading.find(event.getKey());
  
  		if (itMin != m_minReading.end())
  		{
  			m_minReading[ event.getKey() ].assign( std::min( event, itMin->second ) );
  		}
  		else
  		{
  			m_minReading.insert( std::pair< std::string, Event>( event.getKey(), event ) );
  		}
  
  		std::map<std::string, Event>::iterator itMax = m_maxReading.find(event.getKey());
  		if (itMax != m_maxReading.end())
  		{
  			m_maxReading[ event.getKey() ].assign( std::max( event, itMax->second ) );
  		}
  		else
  		{
  			m_maxReading.insert( std::pair< std::string, Event>( event.getKey(), event ) );
  		}
  
  	}
  
  	// Go all 'Logan's Run' on old events.
  	flushCache();
  
    if (m_minReading.count( event.getKey() ) != 0)
       std::cerr << "Min is now " << m_minReading[ event.getKey() ] << std::endl;
    else
       std::cerr << "No min!?" << std::endl;
          
    if (m_maxReading.count( event.getKey() ) != 0)
       std::cerr << "Max is now " << m_maxReading[ event.getKey() ] << std::endl;
    else
       std::cerr << "No max!?" << std::endl;
       
  }
	catch (const std::exception& e)
	{
		std::cerr << "Exception in logEvent : " << e.what() << std::endl;
	}
    
	return store;
}

int eventLogger::storeEvent( const Event &event )
{
	try
	{
		int locationId = getLocationId( event.getLocation() );

		std::cerr << "storeEvent " << event << std::endl;
	
		tntdb::Statement st = m_conn.prepare(
		"INSERT INTO readings( timestamp,location_id,desc,value) VALUES( :v1,:v2,:v3,:v4)");
		st.setDatetime( "v1", event.getDateTime() ).setInt( "v2", locationId ).setString("v3",event.getDesc()).setString("v4",event.getValue()).execute();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in storeEvent : " << e.what() << std::endl;
		return 1;
	}
	return 0;
}


void eventLogger::flushCache()
{
//	std::cerr << "flushCache();" << std::endl;
	bool change = false;
	time_t now;
	time( & now );

	std::deque<std::string> measChanged;

	for (std::map< std::string, std::deque< Event > >::iterator it = m_eventCache.begin(); it != m_eventCache.end(); ++it)
	{
		std::deque< Event > &queue = it->second;

//		if (queue.size() > 0)
//		{
//			std::cerr << "Age of oldest of " << queue.size()  << " in " << it->first << " = " << difftime( now, queue.front().getTime()) << std::endl;
//		}

		while ( (queue.size() > 0) && ( difftime( now, queue.front().getTime()) > MAX_CACHE_AGE )  )
		{
			//Event logan = queue.front();

			queue.pop_front();
			change = true;
		}

		if (change)
			measChanged.push_back( it->first );
	}

	// Recalc min/max for all meas classes that have been changed.
	for (std::deque<std::string>::iterator it = measChanged.begin(); it != measChanged.end(); ++it)
	{
		recalcMinMax( *it );
	}

}



// Unknown sensor with location as the sensor id.
void eventLogger::logUnknownSensorEvent(const Event &event )
{
	std::map<std::string, Event>::iterator it = m_unknownSensors.find(event.getLocation() );
	if (it != m_unknownSensors.end())
	{
		it->second.assign(event);	// Over-write
	}
	else
	{
		m_unknownSensors.insert( std::pair<std::string, Event>(event.getLocation(),event) );
	}
}

std::vector<Event> eventLogger::getUnknownSensorEvents()
{
	std::vector<Event> unknownEvents;
	for (std::map<std::string, Event>::iterator it=m_unknownSensors.begin(); it != m_unknownSensors.end(); it++)
		unknownEvents.push_back( it->second );
	return unknownEvents;
}


// Remove known sensor from list of unknown if present in unknown list.
void eventLogger::knownSensorEvent( const std::string &sensorId )
{
	std::map<std::string, Event>::iterator it = m_unknownSensors.find(sensorId );
	if (it != m_unknownSensors.end())
		m_unknownSensors.erase( it );
}

void eventLogger::recalcMinMax()
{
	for (std::map< std::string, std::deque< Event > >::iterator it = m_eventCache.begin(); it != m_eventCache.end(); ++it)
	{
		recalcMinMax( it->first );
	}
}

void eventLogger::recalcMinMax( std::string measName )
{
	// Clear existing man/max
	std::map<std::string, Event>::iterator itMin = m_minReading.find(measName);
	if ( itMin != m_minReading.end())
			m_minReading.erase( itMin );

	std::map<std::string, Event>::iterator itMax = m_maxReading.find(measName);
	if (itMax != m_maxReading.end())
			m_maxReading.erase( itMax );

	std::map< std::string, std::deque< Event > >::iterator itMeasClass = m_eventCache.find( measName );

	// Meas class not found
	if (itMeasClass == m_eventCache.end())
  {
    std::cerr << "No events found for " << measName << std::endl;
		return;
  }

	std::deque< Event > &queue = itMeasClass->second;

	if (queue.begin() == queue.end())
		return;

	Event min = queue[0];///(*queue.begin());
	Event max = min;

  //std::cerr << "Scan queue..." << std::endl;
  
	for (std::deque< Event >::iterator it = queue.begin(); it != queue.end(); ++it)
	{
		min = std::min( min, (*it) );
		max = std::max( max, (*it) );
	}
	m_minReading.insert( std::pair< std::string, Event>( measName, min ));
	m_maxReading.insert( std::pair< std::string, Event>( measName, max ));
}


Event eventLogger::getMin( const std::string &location, const std::string& desc )
{
	Event min;
	std::map<std::string, Event>::iterator itMin = m_minReading.find(Event::makeKey( location, desc ));
	if ( itMin != m_minReading.end())
		min = itMin->second;
	return min;
}

Event eventLogger::getMax( const std::string &location, const std::string& desc )
{
	Event max;
	std::map<std::string, Event>::iterator itMax = m_maxReading.find(Event::makeKey( location, desc ));
	if ( itMax != m_maxReading.end())
		max = itMax->second;
	return max;
}

std::vector<Event> eventLogger::getMinEvents()
{
	std::vector<Event> events;
	for (std::map<std::string, Event>::iterator it=m_minReading.begin(); it != m_minReading.end(); it++)
		events.push_back( it->second );
	return events;
}

std::vector<Event> eventLogger::getMaxEvents()
{
	std::vector<Event> events;
	for (std::map<std::string, Event>::iterator it=m_maxReading.begin(); it != m_maxReading.end(); it++)
		events.push_back( it->second );
	return events;
}


std::vector<Event> eventLogger::getLocationEvents( const std::string &location )
{
	std::vector<Event> events;

  std::map< std::string, std::deque< Event > >::iterator mapIt = m_eventCache.find( location );
  if (  mapIt == m_eventCache.end())
  {
   std::cerr << "No events found for " << location << std::endl;
   return events;
  }
  
  std::deque< Event > &eventDeque = mapIt->second;

	for (std::deque< Event >::iterator it=eventDeque.begin(); it != eventDeque.end(); it++)
		events.push_back( (*it) );

	return events;
}

// Initialise the event cache from the database.
void eventLogger::initCache()
{
  try
  {
    m_eventCache.clear();
    
	  tntdb::Statement st = m_conn.prepare( 
                     "SELECT timestamp,location_name,desc,value from readings JOIN locations using (location_id) " \
                     "where  timestamp>date('now','-1 day');" );

    for (tntdb::Statement::const_iterator it = st.begin(); it != st.end(); ++it)
    {
      const tntdb::Row &r = *it;  // dereferencing the iterator gives us a result row
  
      tntdb::Datetime ts;
      std::string location;
      std::string desc;
      std::string value;
  
      r[0].get(ts);     // get is overloaded for different types
      r[1].get(location);
      r[2].get(desc);
      r[3].get(value);
  
      Event event( ts, location, desc, value );
      
      // Stick it in the cache...
      logEvent( event );
    }
  }
  catch (const std::exception& e)
  {
  	std::cerr << "Exception in initCache : " << e.what() << std::endl;
  }

//  recalcMinMax();

}

std::vector<std::string> eventLogger::getLocations()
{
 std::vector<std::string> result;
 
 	for (std::map< std::string, std::deque< Event > >::iterator it=m_eventCache.begin(); it != m_eventCache.end(); it++)
	{
    result.push_back( it->first );
	}
 
 return result;
}

std::map< std::string, std::vector<Event> > eventLogger::getEventSummary( )
{
  std::map< std::string, std::vector<Event> > result;

  result.insert( std::pair< std::string, std::vector<Event> >( "act", getLatestEvents() ) );
  result.insert( std::pair< std::string, std::vector<Event> >( "min", getMinEvents() ) );
  result.insert( std::pair< std::string, std::vector<Event> >( "max", getMaxEvents() ) );

  return result;
}
