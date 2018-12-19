#ifndef _BMP280_H_
#define _BMP280_H_
#include "Common.h"
#endif
struct BmpData 
{
	double Pressure;
	double Temp_C;
	double Temp_F;
};
class BMP280
{
public:
	BMP280(int i2c_fd);
	~BMP280();
	BmpData GetReadings();
private:
	const char * I2C_LOCATION = "/dev/i2c-1";
	int I2C_DEV_ADDR = 0x77;
	char I2C_DATA_ADDR = 0x88;
	char I2C_CTRL_MSMT_REG = 0xF4;
	char I2C_OVERSAMPLING_RATE = 0x27;
	char I2C_CONFIG_REG = 0xF5;
	char I2C_STANDBY_TIME = 0xA0;
	char I2C_RAW_DATA_REG = 0xF7;
	int I2C_FILE;
	void CheckBounds(int * data);
	int * GetTemperatureCoefficients(char buffer[24], int tempCoefficients[3]);
	int * GetPressureCoefficients(char buffer[24], int pressureCoefficients[9]);
	BmpData CalculateData(long adc_p, long adc_t, int tempCoeff[], int pressCoeff[]);
	void SetConfigurationOption(char reg, char opt);
};
