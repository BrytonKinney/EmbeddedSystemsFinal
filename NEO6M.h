#ifndef _NEO6M_H_
#define _NEO6M_H_
#include "Common.h"
#include <nmea.h>
#include <nmea/gpgll.h>
#include <nmea/gpgga.h>
#include <nmea/gprmc.h>
#endif
struct GpsData 
{
	double Time;
	double Latitude;
	std::string NS;
	double Longitude;
	std::string EW;
	float FixQuality;
	int NumberOfSatellites;
	float HorizDilution;
	float Altitude;
	std::string Meter;
	float Height;
	std::string Checksum;
};
class NEO6M 
{
public:
	NEO6M();
	GpsData GetReadings();
};
