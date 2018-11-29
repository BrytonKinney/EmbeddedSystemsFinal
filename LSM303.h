#ifndef _LSM303_H_
#define _LSM303_H_
#include "Common.h"
#endif
enum LsmCoefficients {
	ACC_X,
	ACC_Y,
	ACC_Z,
	MAG_X,
	MAG_Y,
	MAG_Z
};
struct LsmData
{
	float Heading;
	double X_Accel;
	double Y_Accel;
	double Z_Accel;
	float Roll;
	float Pitch;
	float Yaw;
	float X_Mag;
	float Y_Mag;
	float Z_Mag;
};

class LSM303 
{
public:
	LSM303();
	LsmData GetReadings();
private:
	char ReadRegister(char reg);
	void GetMagData(int * buffer);
	void GetAccelData(int * buffer);
	int I2C_LSM_FILE;
	const char * I2C_LOCATION = "/dev/i2c-1";
	// Accelerometer registers & data
	char I2C_ACCEL_ADDR = 0x19;
	char I2C_ACCEL_CONF_REG = 0x20;
	char I2C_ACCEL_XYZ_CLK_REG = 0x27;
	char I2C_CTRL_REG_4 = 0x23;
	char I2C_REG_4_CFG = 0x00;
	char I2C_ACCEL_X_LSB_REG = 0x28;
	char I2C_ACCEL_X_MSB_REG = 0x29;
	char I2C_ACCEL_Y_LSB_REG = 0x2A;
	char I2C_ACCEL_Y_MSB_REG = 0x2B;
	char I2C_ACCEL_Z_LSB_REG = 0x2C;
	char I2C_ACCEL_Z_MSB_REG = 0x2D;
	// Magnometer registers & data
	char I2C_MAG_ADDR = 0x1E;
	char I2C_MR_REG = 0x02;
	char I2C_MAG_CONT = 0x00;
	char I2C_CRA_REG = 0x00;
	char I2C_MAG_OUT_RATE = 0x10;
	char I2C_CRB_REG = 0x01;
	char I2C_MAG_GAIN = 0x20;
	char I2C_MAG_X_MSB_REG = 0x03;
	char I2C_MAG_X_LSB_REG = 0X04;
	char I2C_MAG_Y_MSB_REG = 0x05;
	char I2C_MAG_Y_LSB_REG = 0x06;
	char I2C_MAG_Z_MSB_REG = 0x07;
	char I2C_MAG_Z_LSB_REG = 0x08;
};
