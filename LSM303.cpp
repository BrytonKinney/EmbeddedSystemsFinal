#include "LSM303.h"

LSM303::LSM303() {}

char LSM303::ReadRegister(char reg)
{
	char data[1] = {0};
	char r[1] = {reg};
	write(this->I2C_LSM_FILE, r, 1);
	if(read(this->I2C_LSM_FILE, data, 1) != 1)
		std::cout << "Failed to read from register " << r[0] << std::endl;
	return data[0];
}
void LSM303::GetMagData(int * buffer)
{
	char x_mag_lsb = this->ReadRegister(this->I2C_MAG_X_LSB_REG);
	char x_mag_msb = this->ReadRegister(this->I2C_MAG_X_MSB_REG);
	char y_mag_lsb = this->ReadRegister(this->I2C_MAG_Y_LSB_REG);
	char y_mag_msb = this->ReadRegister(this->I2C_MAG_Y_MSB_REG);
	char z_mag_lsb = this->ReadRegister(this->I2C_MAG_Z_LSB_REG);
	char z_mag_msb = this->ReadRegister(this->I2C_MAG_Z_MSB_REG);
	buffer[0] = (x_mag_msb << 8) + x_mag_lsb;
	if(buffer[0] > 32767)
		buffer[0] -= 65536;
	buffer[1] = (y_mag_msb << 8) + y_mag_lsb;
	if(buffer[1] > 32767)
		buffer[1] -= 65536;
	buffer[2] = (z_mag_msb << 8) + z_mag_lsb; 
	if(buffer[2] > 32767)
		buffer[2] -= 65536;
}
void LSM303::GetAccelData(int * buffer)
{
	char x_accel_lsb = this->ReadRegister(this->I2C_ACCEL_X_LSB_REG);
	char x_accel_msb = this->ReadRegister(this->I2C_ACCEL_X_MSB_REG);
	char y_accel_lsb = this->ReadRegister(this->I2C_ACCEL_Y_LSB_REG);
	char y_accel_msb = this->ReadRegister(this->I2C_ACCEL_Y_MSB_REG);
	char z_accel_lsb = this->ReadRegister(this->I2C_ACCEL_Z_LSB_REG);
	char z_accel_msb = this->ReadRegister(this->I2C_ACCEL_Z_MSB_REG);
	buffer[0] = (x_accel_msb << 8) + x_accel_lsb;
	if(buffer[0] > 32767)
		buffer[0] -= 65536;
	buffer[1] = (y_accel_msb << 8) + y_accel_lsb;
	if(buffer[1] > 32767)
		buffer[1] -= 65536;
	buffer[2] = (z_accel_msb << 8) + z_accel_lsb; 
	if(buffer[2] > 32767)
		buffer[2] -= 65536;
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
	cfg[1] = 0x01; //this->I2C_REG_4_CFG;
	write(this->I2C_LSM_FILE, cfg, 2);
	usleep(10000);
	int accel_data[3];
	this->GetAccelData(accel_data);
	data.X_Accel = accel_data[0] / 1000.0;
	data.Y_Accel = accel_data[1] / 1000.0;
	data.Z_Accel = accel_data[2] / 1000.0;
	data.Roll = (3.14159/180.0) * atan2((double)(data.Y_Accel), (double)(data.Z_Accel));
	data.Pitch = (3.14159/180.0) * atan((double)(-data.X_Accel) / ((double)(data.Y_Accel) * sin(data.Roll) + (double)(data.Z_Accel) * cos(data.Roll)));
	
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
	usleep(5000);
	int mag_data[3];
	this->GetMagData(mag_data);
	data.X_Mag = mag_data[0];
	data.Y_Mag = mag_data[1];
	data.Z_Mag = mag_data[2];
	data.Heading = ((atan2((float)data.Y_Mag, (float)data.X_Mag) * 180.0 )/ 3.14159);
	data.Heading = data.Heading < 0 ? data.Heading += 360 : data.Heading;
	close(this->I2C_LSM_FILE);
	return data;
}
