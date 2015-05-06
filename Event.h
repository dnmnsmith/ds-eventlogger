#ifndef EVENT_INCLUDED
#define EVENT_INCLUDED

#include <ostream>
#include <tntdb/datetime.h>
#include <time.h>
#include <cxxtools/serializationinfo.h>
#include <boost/lexical_cast.hpp>

class Event
{
	public:
		Event();
		Event( const Event& o );
		Event( const std::string &location, const std::string &desc, const std::string &value );
		Event( const time_t &rTime, const std::string &location, const std::string &desc, const std::string &value );
		Event( const tntdb::Datetime &rTime, const std::string &location, const std::string &desc, const std::string &value );

		Event & operator= (const Event & o);
		void assign( Event e );

		void init( const std::string &location, const std::string &desc, const std::string &value );
		void init( const time_t &rTime, const std::string &location, const std::string &desc, const std::string &value );

		time_t getTime() const;
		tntdb::Datetime getDateTime() const;

		std::string getLocation() const;
		std::string getDesc() const;
		std::string getValue() const;
		double getValueDouble() const;

		std::string getKey() const;
		static std::string makeKey( const std::string &location, const std::string &desc );
		
		// Compares, but not the timestamp
		bool differ( const Event &e ) const;

		friend std::ostream& operator<< (std::ostream& stream, const Event & event );

		friend void operator>>= (const cxxtools::SerializationInfo& si, Event& e);
		friend void operator<<= (cxxtools::SerializationInfo& si, const Event& e);

		friend bool operator ==(const Event &a, const Event &b);
		friend bool operator !=(const Event &a, const Event &b);
		friend bool operator <(const Event &a, const Event &b);
		friend bool operator >(const Event &a, const Event &b);
		friend bool operator <=(const Event &a, const Event &b);
		friend bool operator >=(const Event &a, const Event &b);

	    friend void swap(Event& first, Event& second); // nothrow

	private:
		time_t m_time;
		std::string m_location;
		std::string m_desc;
		std::string m_value;
};

#endif
