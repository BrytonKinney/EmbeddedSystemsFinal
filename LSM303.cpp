#include "LSM303.h"

void check_overflow(int * buffer, char * byte_arr) 
{
	buffer[0] = (byte_arr[1] << 8) | byte_arr[0];
	//std::cout << "Buffer[0] = " << buffer[0] << std::endl;
	if(buffer[0] > 32767)
		buffer[0] -= 65536;
	buffer[1] = (byte_arr[3] << 8) | byte_arr[2];
	//std::cout << "Buffer[1] = " << buffer[1] << std::endl;
	if(buffer[1] > 32767)
		buffer[1] -= 65536;
	buffer[2] = (byte_arr[5] << 8) | byte_arr[4];
	//std::cout << "Buffer[2] = " << buffer[2] << std::endl;
	if(buffer[2] > 32767)
		buffer[2] -= 65536;
}

LSM303::LSM303() {}

char LSM303::ReadRegister(char reg)
{
	unsigned char data[1] = {0};
	char r[1] = {reg};
	write(this->I2C_LSM_FILE, r, 1);
	if(read(this->I2C_LSM_FILE, data, 1) != 1)
		std::cout << "Failed to read from register " << r[0] << std::endl;
	return data[0];
}

void LSM303::GetMagData(LsmData * data)
{
	int buffer[3] = {0, 0, 0};
	char mag_bytes[6] = {
		this->ReadRegister(this->I2C_MAG_X_LSB_REG), 
		this->ReadRegister(this->I2C_MAG_X_MSB_REG), 
		this->ReadRegister(this->I2C_MAG_Y_LSB_REG),
		this->ReadRegister(this->I2C_MAG_Y_MSB_REG), 
		this->ReadRegister(this->I2C_MAG_Z_LSB_REG), 
		this->ReadRegister(this->I2C_MAG_Z_MSB_REG)
	};
	check_overflow(buffer, mag_bytes);
	data->X_Mag = buffer[0];
	data->Y_Mag = buffer[1];
	data->Z_Mag = buffer[2];
	data->Heading = ((atan2((float)data->Y_Mag, (float)data->X_Mag) * 180.0 )/ 3.14159);
	data->Heading = data->Heading < 0 ? data->Heading += 360 : data->Heading;
}

void LSM303::GetAccelData(LsmData * data)
{
	int buffer[3] = {0, 0, 0};
	char accel_bytes[6] = {
		this->ReadRegister(this->I2C_ACCEL_X_LSB_REG),
		this->ReadRegister(this->I2C_ACCEL_X_MSB_REG),
		this->ReadRegister(this->I2C_ACCEL_Y_LSB_REG),
		this->ReadRegister(this->I2C_ACCEL_Y_MSB_REG),
		this->ReadRegister(this->I2C_ACCEL_Z_LSB_REG),
		this->ReadRegister(this->I2C_ACCEL_Z_MSB_REG)
	};
	check_overflow(buffer, accel_bytes);
	data->X_Accel = (double)buffer[0];
	data->Y_Accel = (double)buffer[1];
	data->Z_Accel = (double)buffer[2];
	data->Roll = (180.0/3.14159) * atan2((double)(data->Y_Accel), (double)(data->Z_Accel));
	data->Pitch = (180.0/3.14159) * atan((double)(-data->X_Accel) / ((double)(data->Y_Accel) * sin(data->Roll) + (double)(data->Z_Accel) * cos(data->Roll)));
	data->Yaw = (180.0/3.14159) * atan((double)data->Y_Accel/(sqrt((double)data->X_Accel * (double)data->Y_Accel + (double)data->Z_Accel * (double)data->Z_Accel)));
}

LsmData LSM303::GetReadings()
{
	LsmData data;
	if((this->I2C_LSM_FILE = open(this->I2C_LOCATION, O_RDWR)) < 0)
	{
		std::cout << "Unable to open I2C dev file." << std::endl;
		return data;
	}
	// Configure & grab I2C accelerometer device via its register
	ioctl(this->I2C_LSM_FILE, I2C_SLAVE, this->I2C_ACCEL_ADDR);
	char cfg[2] = { this->I2C_ACCEL_CONF_REG, this->I2C_ACCEL_XYZ_CLK_REG }; 
	write(this->I2C_LSM_FILE, cfg, 2);
	cfg[0] = this->I2C_CTRL_REG_4;
	cfg[1] = this->I2C_REG_4_CFG;
	write(this->I2C_LSM_FILE, cfg, 2);
	//usleep(10000);

	this->GetAccelData(&data);

	
	// Now, time for the magnometer
	ioctl(this->I2C_LSM_FILE, I2C_SLAVE, this->I2C_MAG_ADDR);
	cfg[0] = this->I2C_MR_REG;
	cfg[1] = this->I2C_MAG_CONT;
	write(this->I2C_LSM_FILE, cfg, 2);
	cfg[0] = this->I2C_CRA_REG;
	cfg[1] = this->I2C_MAG_OUT_RATE;
	write(this->I2C_LSM_FILE, cfg, 2);
	cfg[0] = this->I2C_CRB_REG;
	cfg[1] = this->I2C_MAG_GAIN;
	write(this->I2C_LSM_FILE, cfg, 2);

	//usleep(5000);

	this->GetMagData(&data);

	if(close(this->I2C_LSM_FILE) < 0)
		std::cout << errno << std::endl;
	usleep(10000);
	return data;
}
