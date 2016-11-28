#include "mpu9250.h"
#include "hspi.h"
#include "gpio.h"	// to control the discrete signals.
// #include "esp_common.h"
#include "i2c.h"

// Let's try to use a more comprehensive IO setup routine:
GPIO_ConfigTypeDef hspi_io[4];

//////////////////////////// I2C ACCESS ///////////////////////////////////////////////////////////

void mpu9250_i2c_init ()
{
	// force the address LSB to zero (AD0)
	PIN_FUNC_SELECT(AD0_MUX, AD0_FUNC);
	GPIO_OUTPUT_SET(AD0_PIN,0);

	// force nCS (through GPIO15) to "1"
	PIN_FUNC_SELECT(nCS_MUX, nCS_FUNC);
	GPIO_OUTPUT_SET(nCS_PIN,1);

	// force FSYNC (through GPIO15) to "0"
	PIN_FUNC_SELECT(FSYNC_MUX, FSYNC_FUNC);
	GPIO_OUTPUT_SET(FSYNC_PIN,0);
/*
	// SDA
	hspi_io[1].GPIO_Pin = GPIO_Pin_13;
	hspi_io[1].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[1].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[1].GPIO_Mode = GPIO_Mode_Out_OD;

	// SCL
	hspi_io[2].GPIO_Pin = GPIO_Pin_14;
	hspi_io[2].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[2].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[2].GPIO_Mode = GPIO_Mode_Out_OD;
*/

	// initialize bit-banging driver
	i2c_init ();

	// moar config
	gpio_config(&hspi_io[1]);
	gpio_config(&hspi_io[2]);
}

/////////////////////////// SPI ACCESS /////////////////////////////////////////////////////////////

// This replaces the HSPI library's init function
void mpu9250_init()
{
	// Mux FSYNC (D1 / GPIO5) to GPIO and make it zero.
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	GPIO_OUTPUT_SET (5, 0);

	// HSPI init
	hspi_init_gpio();		// Set pin muxing for HSPI
	hspi_mode(1, 0);		// Set SPI mode (polarity / phase)
	hspi_clock(40);		    // set SCK to 1 MHz for init phase
	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

    // MEMS init
	mpu9250_reg_init ();

	// Increase SCK to operational speed (20 MHz typical)
	// hspi_clock(0);		    // set SCK to 80 MHz - may not be reliable
	hspi_clock(2);		    // set SCK to 20 MHz (40 / 2)
}

// Write a single register
inline void mpu9250_write (uint8 reg, uint8 val)
{
	hspi_transaction(0,0,8,reg,8,val,0,0);
}

// Read a single 8 bit register (no auto-incrementation)
inline uint8 mpu9250_read (uint8 addr)
{
	return hspi_transaction(0,0,8,addr | 0x80,0,0,8,0);
}

// Read a single 16 bit register (no auto-incrementation)
inline uint16 mpu9250_read16 (uint8 addr)
{
	return hspi_transaction(0,0,8,addr | 0x80,0,0,16,0);
}

// Read all sensor registers (16 bits x 10 from address 59)
void mpu9250_read_sensors ()
{
	hspi_wait_ready (); //wait for SPI to be ready

	//disable all phases of the transaction in case they were previously set
	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI|SPI_USR_MISO|SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_DUMMY);

	// Setup the number of bits for each phase of the SPI transaction
	WRITE_PERI_REG(SPI_USER1(HSPI),   ((0x7)&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S | 			// Data Out
									  ((159)&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S); 			// Data In

	// Enable the transmission (MOSI) and reception (MISO) phases
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI);
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO);

	// Copy register address to W0. Note : 0xBB = 0x80 | 59
	WRITE_PERI_REG(SPI_W0(HSPI), 0xBB000000); // put the register address in the MSByte

	// Start the SPI transaction
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);

	hspi_wait_ready (); //wait for SPI to be ready
}

// Burst Read, 1 to 64 bytes
// Result will be in the HSPI buffer
// din_bits should never be zero
void mpu9250_read_burst (uint32 addr, uint32 din_bits)
{
	addr |= 0x80;		// read command needs to have MSB set

	hspi_wait_ready (); //wait for SPI to be ready

	//disable all phases of the transaction in case they were previously set
	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI|SPI_USR_MISO|SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_DUMMY);

	// Setup the number of bits for each phase of the SPI transaction
	WRITE_PERI_REG(SPI_USER1(HSPI),   ((0x7)&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S | 			// Data Out
									  ((din_bits-1)&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S    ); 			// Data In

	// Enable the transmission (MOSI) and reception (MISO) phases
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI);
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO);

	// Copy register address to W0
	WRITE_PERI_REG(SPI_W0(HSPI), addr<<(24)); // put the register address in the MSByte

	// Start the SPI transaction
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);

	hspi_wait_ready (); //wait for SPI to be ready
}


/////////////////////////////////////////////////////////////////////////////

////// UNTESTED /////////////////////////////////

/*-----------------------------------------------------------------------------------------------
                                    INITIALIZATION
usage: call this function at startup, giving the sample rate divider (raging from 0 to 255) and
low pass filter value; suitable values are:
BITS_DLPF_CFG_256HZ_NOLPF2
BITS_DLPF_CFG_188HZ
BITS_DLPF_CFG_98HZ
BITS_DLPF_CFG_42HZ
BITS_DLPF_CFG_20HZ
BITS_DLPF_CFG_10HZ
BITS_DLPF_CFG_5HZ
BITS_DLPF_CFG_2100HZ_NOLPF
returns 1 if an error occurred
-----------------------------------------------------------------------------------------------*/

#define config_steps	16
#define low_pass_filter		BITS_DLPF_CFG_188HZ

void mpu9250_reg_init ()
{
    uint8_t i = 0;

    // SCK should be 1 MHz here

    // CONFIGURATION SEQUENCE
    const uint8 mpu_config [config_steps][2] = {
    		{MPUREG_PWR_MGMT_1, 0x80},     // Reset Device
    		{MPUREG_PWR_MGMT_1, 0x01},     // Clock Source : autoselect best
    		//{MPUREG_PWR_MGMT_2, 0x00},     // Enable Acc & Gyro (this is the default value)
    		{MPUREG_CONFIG, 0x1B},    // No DLPF on gyroscope, max sampling rate (32 KHz)
    		{MPUREG_GYRO_CONFIG, 0x18},    // +-2000dps
    		{MPUREG_ACCEL_CONFIG, 0x08},   // +-4G
    		{MPUREG_ACCEL_CONFIG_2, 0x09}, // Set Acc Data Rate to 4 KHz, disable LPF.
    		{MPUREG_INT_PIN_CFG, 0x30},    // INT will stay up until the host reads the MPU9250
    		{MPUREG_USER_CTRL, 0x20},       // I2C Master mode (Enable AUX)
    		{MPUREG_I2C_MST_CTRL, 0x0D}, //  I2C configuration multi-master  IIC 400KHz

    		{MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR},  //Set the I2C slave addres of AK8963 and set for write.
    		//{MPUREG_I2C_MST_DELAY_CTRL, 0x81}, //Enable I2C delay
    		{MPUREG_I2C_SLV0_REG, AK8963_CNTL2}, //I2C slave 0 register address from where to begin data transfer
    		{MPUREG_I2C_SLV0_DO, 0x01}, 	// Reset AK8963
    		{MPUREG_I2C_SLV0_CTRL, 0x81},  	//Enable I2C and set 1 byte

    		{MPUREG_I2C_SLV0_REG, AK8963_CNTL1}, //I2C slave 0 register address from where to begin data transfer
    		{MPUREG_I2C_SLV0_DO, 0x12}, // Register value to continuous measurement in 16bit
    		{MPUREG_I2C_SLV0_CTRL, 0x81}  //Enable I2C and set 1 byte
    };

    for(i=0; i<config_steps; i++)
    	mpu9250_write(mpu_config[i][0], mpu_config[i][1]);

    mpu9250_set_acc_scale(BITS_FS_2G);
    mpu9250_set_gyro_scale (BITS_FS_250DPS);

    //AK8963_calib_Magnetometer();
}

/*-----------------------------------------------------------------------------------------------
                                ACCELEROMETER SCALE
usage: call this function at startup, after initialization, to set the right range for the
accelerometers. Suitable ranges are:
BITS_FS_2G
BITS_FS_4G
BITS_FS_8G
BITS_FS_16G
returns the range set (2,4,8 or 16)
-----------------------------------------------------------------------------------------------*/

float acc_divider;

void mpu9250_set_acc_scale(int scale)
{
    mpu9250_write(MPUREG_ACCEL_CONFIG, scale);

    switch (scale)
    {
        case BITS_FS_2G:
            acc_divider=16384;
            break;
        case BITS_FS_4G:
            acc_divider=8192;
            break;
        case BITS_FS_8G:
            acc_divider=4096;
            break;
        case BITS_FS_16G:
            acc_divider=2048;
            break;
    }
}

/*-----------------------------------------------------------------------------------------------
                                GYROSCOPE SCALE
usage: call this function at startup, after initialization, to set the right range for the
gyroscopes. Suitable ranges are:
BITS_FS_250DPS
BITS_FS_500DPS
BITS_FS_1000DPS
BITS_FS_2000DPS
returns the range set (250,500,1000 or 2000)
-----------------------------------------------------------------------------------------------*/

float gyro_divider;

void mpu9250_set_gyro_scale(int scale)
{
    mpu9250_write(MPUREG_GYRO_CONFIG, scale);

    switch (scale)
    {
        case BITS_FS_250DPS:
            gyro_divider=131;
            break;
        case BITS_FS_500DPS:
            gyro_divider=65.5;
            break;
        case BITS_FS_1000DPS:
            gyro_divider=32.8;
            break;
        case BITS_FS_2000DPS:
            gyro_divider=16.4;
            break;
    }
}
/*
void mpu9250_read_Magnetometer()
{
    uint8_t response[7];
    int16_t bit_data;
    float data;
    int i;

    WriteReg(MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    WriteReg(MPUREG_I2C_SLV0_REG, AK8963_HXL); //I2C slave 0 register address from where to begin data transfer
    WriteReg(MPUREG_I2C_SLV0_CTRL, 0x87); //Read 6 bytes from the magnetometer

    wait(0.001);
    ReadRegs(MPUREG_EXT_SENS_DATA_00,response,7);
    //must start your read from AK8963A register 0x03 and read seven bytes so that upon read of ST2 register 0x09 the AK8963A will unlatch the data registers for the next measurement.
    for(i=0; i<3; i++) {
        bit_data=((int16_t)response[i*2+1]<<8)|response[i*2];
        data=(float)bit_data;
        Magnetometer[i]=data*Magnetometer_ASA[i];
    }
}
*/




//////////// SENSOR CONVERSIONS ////////////////////////////////////////

float accel_data[3];
float gyro_data[3];
float mag_data[3];
float temperature;

void mpu9250_convert ()
{
	// Accelerometer conversions
	/*
	HSPI_FIFO_16[1];	// acc X
	HSPI_FIFO_16[0];	// acc Y
	HSPI_FIFO_16[3];	// acc Z
	HSPI_FIFO_16[2];	// temp
	HSPI_FIFO_16[5];	// gyro X
	HSPI_FIFO_16[4];	// gyro Y
	HSPI_FIFO_16[7];	// gyro Z
	HSPI_FIFO_16[6];	// mag X
	HSPI_FIFO_16[9];	// mag Y
	HSPI_FIFO_16[8];	// mag Z
	*/

    accel_data[0]= ((float) HSPI_FIFO_16[1]) / acc_divider;
	accel_data[1]= ((float) HSPI_FIFO_16[0]) / acc_divider;
	accel_data[2]= ((float) HSPI_FIFO_16[3]) / acc_divider;

	gyro_data[0]= ((float) HSPI_FIFO_16[5]) / gyro_divider;
	gyro_data[1]= ((float) HSPI_FIFO_16[4]) / gyro_divider;
	gyro_data[2]= ((float) HSPI_FIFO_16[7]) / gyro_divider;

	// The following doesn't match the spec
	temperature = (((float) HSPI_FIFO_16[2]) / 340) + 36.53;
}



