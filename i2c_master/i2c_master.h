
/*
 * i2c_master.h
 *
 * Created: 3.02.2022 13:55:35
 *  Author: haluk
 */ 

#ifndef I2C_MASTER_H
#define I2C_MASTER_H



#define F_SCL 400000 
//#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define I2C_Rx_Boyut					16
#define I2C_Tx_Boyut					16
#define I2C_Rx_Mask						(I2C_Rx_Boyut-1)
#define I2C_Tx_Mask						(I2C_Tx_Boyut-1)
#define I2C_TIMEOUT						1000
//I2C status
#define I2C_STATUS_MASK					0xF8//tws7-tws3
#define I2C_STATUS						(TWSR&I2C_STATUS_MASK)
#define I2C_START						0x08// gönderim hazýr
#define I2C_REP_START					0x10// tekrar gönderim //sonrasý adres ister
//I2C master status
#define I2C_MTR_ADR_ACK					0x18// adres yazýldý ack geldi
#define I2C_MTR_ADR_NACK				0x20// adres yazýldý nack geldi
#define I2C_MTR_DATA_ACK				0x28// veri gitti ack geldi
#define I2C_MTR_DATA_NACK				0x30// veri gitti nack geldi
#define I2C_MRD_ADR_ACK					0x40//adres yazýldý ack geldi
#define I2C_MRD_ADR_NACK				0x48// adres yazýldý nack geldi
#define I2C_MRD_DATA_ACK				0x50// veri alýndý ack gönderildi
#define I2C_MRD_DATA_NACK				0x58//veri alýndý nac gönderildi
//I2C slave status
#define I2C_STR_ADR_ACK					0xA8
#define I2C_STR_ADR_ACK_M_ARB_LOST		0xB0
#define I2C_STR_DATA_ACK				0xB8
#define I2C_STR_DATA_NACK				0xC0
#define I2C_STR_DATA_ACK_LAST_BYTE		0xC8
#define I2C_SRD_ADR_ACK					0x60
#define I2C_SRD_ADR_ACK_M_ARB_LOST		0x68
#define I2C_SRD_GEN_ACK					0x70
#define I2C_SRD_GEN_ACK_M_ARB_LOST		0x78
#define I2C_SRD_ADR_DATA_ACK			0x80
#define I2C_SRD_ADR_DATA_NACK			0x88
#define I2C_SRD_GEN_DATA_ACK			0x90
#define I2C_SRD_GEN_DATA_NACK			0x98
#define I2C_SRD_STOP					0xA0
//I2C error status
#define I2C_ARB_LOST					0x38//hat yönetim kaybý
#define I2C_NO_INFO						0xF8//belirsiz durum
#define I2C_BUS_ERR						0x00//hat hatasý
//////////////////////////////////////////////////////////////////////////
typedef enum{
	DISABLE			=0x00,
	ENABLE			=0x05,
	CL_FLAG			=0x85,
	NACK			=0x85,
	STOP			=0x95,
	START			=0xA5,
	STO_STR			=0xB5,
	ACK				=0xC5	
}i2c_twcr_t;
typedef enum {
	I2C_WRITE		=0,
	I2C_READ		=1,
}i2c_cmd_t;
typedef  enum {
	_REPEAT			=1,
	N_REPEAT		=0,
}i2c_rep_t;
typedef  enum {
	I2C_READY		=0,
	I2C_BUSSY		=1,
}i2c_state_t;
void i2c_init();
void i2c_disable();
void i2c_reset();
void i2c_start();
void i2c_stop();
void i2c_adr(uint8_t adr,i2c_cmd_t cmd);
void i2c_data(uint8_t data);
void i2c_end( i2c_rep_t repst);
void i2c_send_data(uint8_t adr, uint8_t data,  i2c_rep_t repst);
void i2c_send(uint8_t adr, uint8_t* str,uint8_t len,  i2c_rep_t repst);
void i2c_send_str(uint8_t adr, const char* str,  i2c_rep_t repst);
void i2c_read(uint8_t adr, uint8_t len);
uint8_t i2c_gelen();
uint8_t i2c_oku();
uint8_t i2c_adres_scan();

#endif