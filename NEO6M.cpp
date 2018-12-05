#include "NEO6M.h"

// Utility functions
void check_numeric_token(std::string& token)
{
	if(token == "")
		token == "0.0";
}

GpsData parse_sentence(std::string str)
{
	std::string token;
	std::string delimiter = ",";
	size_t pos = 0;
	GpsData data;
	int i = 0;
	while((pos = str.find(delimiter)) != std::string::npos) 
	{
		token = str.substr(0, pos);
		str.erase(0, pos + delimiter.length());
		std::string::size_type sz;
		switch(i) 
		{
			case 0:
				break;
			case 1:
				break;
			case 2:
				check_numeric_token(token);
				data.Latitude = std::stod(token, &sz);
				break;
			case 3:
				data.NS = token;
				if(data.NS == "S")
					data.Latitude = -data.Latitude;
				break;
			case 4:
				check_numeric_token(token);
				data.Longitude = std::stod(token, &sz);
			case 5:
				data.EW = token;
				if(data.EW == "W")
					data.Longitude = -data.Longitude;
			default:
				break;	
		}
		i++;
	}
	return data;
}

// NEO6M class implementations
NEO6M::NEO6M(){}

GpsData NEO6M::GetReadings()
{
	FILE * file = fopen("/dev/serial0", "r");
	if(file != NULL) 
	{
		char line[128];
		while(fgets(line, sizeof line, file) != NULL)
		{
			std::stringstream ss;
			ss << line;
			if(ss.str().rfind("$GPGGA", 0) == 0)
			{
				std::string str = ss.str();
				return parse_sentence(str);
			}
			else
				ss.str(std::string());
		}
	}
	fclose(file);
}
