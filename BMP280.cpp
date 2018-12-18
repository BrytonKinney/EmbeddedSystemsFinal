#include "BMP280.h"

BMP280::BMP280() {}
int * BMP280::GetTemperatureCoefficients(char buffer[24], int tempCoefficients[3])
{
	for(int i = 0; i < 3; i++)
	{
		tempCoefficients[i] = (buffer[(i * 2 + 1)] << 8) + (buffer)[(i * 2)];
		if(i >= 1)
		{
			this->CheckBounds(&tempCoefficients[i]);
		}
	}
	return tempCoefficients;
}


int * BMP280::GetPressureCoefficients(char buffer[24], int pressureCoefficients[9])
{
	for(int i = 0; i < 10; i++)
	{
		pressureCoefficients[i] = (buffer[i * 2 + 7]  << 8) + buffer[i * 2 + 6];
		if(i * 2 + 7 > 7)
		{
			this->CheckBounds(&pressureCoefficients[i]);
		}
	}
	return pressureCoefficients;
}

void BMP280::SetConfigurationOption(char reg, char opt)
{
	char config[2] = {0};
	config[0] = reg;
	config[1] = opt;
	write(this->I2C_FILE, config, 2);
}

BmpData BMP280::GetReadings()
{
	usleep(10000);
	this->I2C_FILE = open(BMP280::I2C_LOCATION, O_RDWR);
	if(this->I2C_FILE < 0)
	{
		std::cout << "Unable to open the I2C bus. Retrying in 1 second." << std::endl;
		std::cout << errno << std::endl;
		usleep(100000);
		this->I2C_FILE= open(BMP280::I2C_LOCATION, O_RDWR);
	}
	ioctl(this->I2C_FILE, I2C_SLAVE, this->I2C_DEV_ADDR);
	char reg[1] = {this->I2C_DATA_ADDR};
	write(this->I2C_FILE, reg, 1);
	char buff[24] = {0};
	read(this->I2C_FILE, buff, 24);
	int tempCoeff[3] = {0};
	this->GetTemperatureCoefficients(buff, tempCoeff);
	int t_coeff[3] = {0};
	// memcpy seems to make the coefficients arrays stay allocated correctly
	
	std::memcpy(t_coeff, tempCoeff, 3);
	int pressCoeff[9] = {0};
	this->GetPressureCoefficients(buff, pressCoeff);
	int p_coeff[9] = {0};
	std::memcpy(p_coeff, pressCoeff, 9);
	
	// Select control measurement register(0xF4)
	// Normal mode, temp and pressure over sampling rate = 1(0x27)
	this->SetConfigurationOption(this->I2C_CTRL_MSMT_REG, this->I2C_OVERSAMPLING_RATE);

	// Select config register(0xF5)
	// Stand_by time = 1000 ms(0xA0)
	this->SetConfigurationOption(this->I2C_CONFIG_REG, this->I2C_STANDBY_TIME);

	usleep(10000);
	// Read 8 bytes of buff from register(0xF7)
	// pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb, humidity lsb, humidity msb
	reg[0] = this->I2C_RAW_DATA_REG;
	write(this->I2C_FILE, reg, 1);
	if(read(this->I2C_FILE, buff, 8) != 8)
	{
		std::cout << "IO error. Unable to read from I2C handle." << std::endl;
		exit(1);
	}
	
	// Convert pressure and temperature buff to 19-bits
	long adc_p = (((long)buff[0]  << 16) + ((long)buff[1] << 8) + (long)(buff[2] & 0xF0)) / 16;
	long adc_t = (((long)buff[3] << 16) + ((long)buff[4] << 8) + (long)(buff[5] & 0xF0)) / 16;
	return this->CalculateData(adc_p, adc_t, tempCoeff, pressCoeff);
}

void BMP280::CheckBounds(int * buff) 
{
	if(*(buff) > 32767)
		*(buff) -= 65536;
}

BmpData BMP280::CalculateData(long adc_p, long adc_t, int tempCoeff[], int pressCoeff[])
{
// Temperature offset calculations
	double offset1 = (((double)adc_t) / 16384.0 - ((double)tempCoeff[0]) / 1024.0) * ((double)tempCoeff[1]);	
	double offset2 = ((((double)adc_t) / 131072.0 - ((double)tempCoeff[0]) / 8192.0) *(((double)adc_t)/131072.0 - ((double)tempCoeff[0])/8192.0)) * ((double)tempCoeff[2]);
	double t_fine = (long)(offset1 + offset2);
	double cTemp = (offset1 + offset2) / 5120.0;
	double fTemp = cTemp * 1.8 + 32;
		
	// Pressure offset calculations
	offset1 = ((double)t_fine / 2.0) - 64000.0;
	offset1 = offset1 * offset1 * ((double)pressCoeff[5]) / 32768.0;
	offset2 = offset2 + offset1 * ((double)pressCoeff[4]) * 2.0;
	offset2 = (offset2 / 4.0) + (((double)pressCoeff[3]) * 65536.0);
	offset1 = (((double) pressCoeff[2]) * offset1 * offset1 / 524288.0 + ((double) pressCoeff[1]) * offset1) / 524288.0;
	offset1 = (1.0 + offset1 / 32768.0) * ((double)pressCoeff[0]);
	double p = 1048576.0 - (double)adc_p;
	p = (p - (offset2 / 4096.0)) * 6250.0 / offset1;
	offset1 = ((double) pressCoeff[8]) * p * p / 2147483648.0;
	offset2 = p * ((double) pressCoeff[7]) / 32768.0;
	double pressure = (p + (offset1 + offset2 + ((double)pressCoeff[6])) / 16.0) / 100;
	
	BmpData bmp;
	bmp.Temp_F = fTemp;
	bmp.Temp_C = cTemp;
	bmp.Pressure = pressure;
	if(close(this->I2C_FILE) < 0)
		std::cout << errno << std::endl;
	return bmp;
}




