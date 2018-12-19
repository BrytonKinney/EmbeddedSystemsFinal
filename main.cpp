#include "BMP280.h"
#include "LSM303.h"
#include "NEO6M.h"
#include <sstream>
#include "json.hpp"
#include <exception>
#include <stdexcept>
#include "WS.h"

void print_info(BmpData data, LsmData lsm_data, GpsData gps_data);
void send_info(BmpData data, LsmData lsm_data, GpsData gps_data,  WebSock& sock);

using json = nlohmann::json;

int main() 
{
	WebSock sock("localhost", 8080);
	int i2c_fd = open("/dev/i2c-1", O_RDWR);
	BMP280 * bmp = new BMP280(i2c_fd);
	LSM303 * lsm = new LSM303(i2c_fd);
	NEO6M * neo = new NEO6M();
	while(true)
	{
		BmpData data = bmp->GetReadings();
		std::cout << "BMP data gathered." << std::endl;
		//delete bmp;
		usleep(1000);
		LsmData lsm_data = lsm->GetReadings();
		//delete lsm;
		std::cout << "LSM data gathered." << std::endl;
		usleep(1000);
		GpsData gps_data = neo->GetReadings();
		//delete neo;
		std::cout << "GPS data gathered." << std::endl;
		usleep(1000);
		print_info(data, lsm_data, gps_data);
		send_info(data, lsm_data, gps_data, sock);
	}
	return 0;
}

void print_info(BmpData data, LsmData lsm_data, GpsData gps_data)
{
	std::cout << "Temperature: " << data.Temp_F << " Fahrenheit" << std::endl;
	std::cout << "Temperature: " << data.Temp_C << " Celsius" << std::endl;
	std::cout << "Pressure: " << data.Pressure << " hectoPascals" << std::endl;
	std::cout << "X Acceleration: " << lsm_data.X_Accel << " mgs" << std::endl;
	std::cout << "Y Acceleration: " << lsm_data.Y_Accel << " mgs" << std::endl;
	std::cout << "Z Acceleration: " << lsm_data.Z_Accel << " mgs" << std::endl;
	std::cout << "Pitch: " << lsm_data.Pitch << " degrees" << std::endl;
	std::cout << "Roll: " << lsm_data.Roll << " degrees" << std::endl;
	std::cout << "Yaw: " << lsm_data.Yaw << " degrees" << std::endl;
	std::cout << "X Magnetic Field Magnitude: " << lsm_data.X_Mag << " mGauss" << std::endl;
	std::cout << "Y Magnetic Field Magnitude: " << lsm_data.Y_Mag << " mGauss" << std::endl;
	std::cout << "Z Magnetic Field Magnitude: " << lsm_data.Z_Mag << " mGauss" << std::endl;
	std::cout << "Heading: " << lsm_data.Heading << " degrees" << std::endl;
	if(!gps_data.IsEmpty)
	{
		std::cout << "Longitude: " << gps_data.Longitude << std::endl;
		std::cout << "Latitude: " << gps_data.Latitude << std::endl;
	}
	//std::cout << "Number of satellites: " << gps_data->n_satellites << std::endl;
	//std::cout << "Altitude (per GPS): " << gps_data->altitude << " " << gps_data->altitude_unit << std::endl;
}

void send_info(BmpData data, LsmData lsm_data, GpsData gps_data, WebSock& sock)
{
	json payload;
	payload["sensor_data"] = {
		{"LSM", 
			{
				{"pitch", lsm_data.Pitch},
				{"roll", lsm_data.Roll},
				{"yaw", lsm_data.Yaw},
				{"x_mag", lsm_data.X_Mag},
				{"y_mag", lsm_data.Y_Mag},
				{"z_mag", lsm_data.Z_Mag},
				{"x_acc", lsm_data.X_Accel},
				{"y_acc", lsm_data.Y_Accel},
				{"z_acc", lsm_data.Z_Accel},
				{"heading", lsm_data.Heading}
			}
		},
		{"BMP", 
			{
				{"pressure", data.Pressure},
				{"temp_f", data.Temp_F},
				{"temp_c", data.Temp_C}
			}
		},
		{"GPS", 
			{
				{"lat", gps_data.Latitude}, 
				{"ns", gps_data.NS},
				{"long", gps_data.Longitude},
				{"ew", gps_data.EW}
			}
		}
	};
	sock.Send(payload.dump());
}




