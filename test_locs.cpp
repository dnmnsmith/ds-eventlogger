#include <unistd.h>
#include <iostream>

#include "locations.h"

int main( int argc, char **argv )
{
 locations sensorLocations;
 
 if(!sensorLocations.init("locations.cfg","locMapFile.cfg"))
 {
   std::cout << "Load failed" << std::endl;
   return 1;
 }
 
 std::cout << "Hello" << std::endl;

 std::cout << sensorLocations;
 
 for (int i = 0; i < 60; i++)
 {
     std::cout << "." << std::endl;
     sleep( 1 );
 }
 std::cout << sensorLocations;
 
 return 0;
}
