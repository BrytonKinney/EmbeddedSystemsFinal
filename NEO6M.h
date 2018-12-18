#ifndef _NEO6M_H_
#define _NEO6M_H_
#include "Common.h"
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
	bool IsEmpty;
};
class NEO6M 
{
public:
	NEO6M();
	GpsData GetReadings();
private:
	GpsData LastAvailableData;
	bool CheckToken(std::string& token);
	GpsData ParseSentence(std::string str);
	bool ValidChecksum(std::string sentence);
};
