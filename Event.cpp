#include <iostream>
#include "Event.h"
#include <ctime>

using std::swap;

Event::Event() : m_location(), m_desc(), m_value()
{
	time( &m_time );
}

Event::Event( const Event& o ) :
	m_time( o.m_time ), m_location( o.m_location ), m_desc( o.m_desc ), m_value( o.m_value )
  {
  }

Event::Event( const std::string &location, const std::string &desc, const std::string &value ) :
	m_location( location ), m_desc( desc ), m_value( value )
{
	time( &m_time );
}

Event::Event( const time_t &rTime, const std::string &location, const std::string &desc, const std::string &value ) :
    m_time( rTime ), m_location( location ), m_desc( desc ), m_value( value )
{
}

Event & Event::operator= (const Event & o)
{
   if (this != &o) // protect against invalid self-assignment
   {
	   m_time = o.m_time;
	   m_location =  o.m_location;
	   m_desc = o.m_desc;
	   m_value = o.m_value;
   }
   // by convention, always return *this
   return *this;
}

void Event::assign( Event e )
{
	swap( *this, e );
}


void Event::init( const std::string &location, const std::string &desc, const std::string &value )
{
	time_t now;
	time( &now );

	init( now, location, desc, value );
}


void Event::init( const time_t &rTime, const std::string &location, const std::string &desc, const std::string &value )
{
	m_time = rTime;
	m_location = location;
	m_desc = desc;
	m_value = value;
}


time_t Event::getTime() const { return m_time; }
std::string Event::getLocation() const { return m_location; }
std::string Event::getDesc() const { return m_desc; }
std::string Event::getValue() const { return m_value; }

std::string Event::getKey() const
{
	std::string key = m_location;
	key.append(":");
	key.append(m_desc);
	return key;
}

tntdb::Datetime Event::getDateTime() const
{
	struct tm gmt;
	gmtime_r(&m_time, &gmt);
	tntdb::Datetime dt( 1900+gmt.tm_year, gmt.tm_mon + 1,gmt.tm_mday,gmt.tm_hour,gmt.tm_min,gmt.tm_sec );
	return dt;
}

std::ostream& operator<< (std::ostream& stream, const Event & event )
{
	struct tm gmt;

	gmtime_r(&event.m_time, &gmt);
	char buf[ 160 ];
	strftime (buf, sizeof(buf), "%F %T.000", &gmt );
	stream << buf << " " << event.m_location << ":" << event.m_desc << "=" << event.m_value;
	return stream;
}


// overload the operator >>= for our struct Configuration
void operator>>= (const cxxtools::SerializationInfo& si, Event& e)
{
    si.getMember("timestamp") >>= e.m_time;

    // if you want to make encoding optional with a default value use:
    //
    //   if (!si.getMember("encoding", c.encoding))
    //       c.encoding = "UTF-8";

    si.getMember("location") >>= e.m_location;
    si.getMember("desc") >>= e.m_desc;
    si.getMember("value") >>= e.m_value;
}

// overload the operator <<= for our struct Configuration
void operator<<= (cxxtools::SerializationInfo& si, const Event& e)
{
    si.addMember("timestamp") <<= e.m_time;
    si.addMember("location") <<= e.m_location;
    si.addMember("desc") <<= e.m_desc;
    si.addMember("value") <<= e.m_value;
}

void swap(Event& first, Event& second)
{
	std::swap( first.m_time,second.m_time );
	std::swap( first.m_location,second.m_location );
	std::swap( first.m_desc,second.m_desc );
	std::swap( first.m_value,second.m_value );
}

