
/*
 * i2c_master.c
 *
 * Created: 3.02.2022 13:55:35
 *  Author: haluk
 */ 
#define F_CPU 16000000UL
#include <util/delay.h>
#include "i2c_master.h"

static volatile uint8_t i2c_rx_bas=0,i2c_rx_son=0,i2c_tx_bas=0,i2c_tx_son=0,i2c_rx_len=0;
static volatile uint8_t i2c_rx_ring[I2C_Rx_Boyut];
static volatile uint8_t i2c_tx_ring[I2C_Tx_Boyut];
static volatile uint8_t Sladr_RW=0, i2c_rep=0;
static volatile i2c_state_t i2c_state=I2C_READY;
volatile uint32_t timeout=0;
volatile uint8_t adres_nack=0;
void i2c_init(){	
	cli();
	TWSR|=(0<<TWPS0)|(0<<TWPS1);//prescaler 1
	TWBR=((F_CPU/F_SCL)-16)>>1;//i2c scl frekans
	TWCR=(1<<TWEN)|(1<<TWIE);//i2c kesme açýk ack açýk i2c açýk (1<<TWEA)|	
	sei();
}
void i2c_disable(){
	TWCR=0x00;
}
void i2c_reset(){
	i2c_disable();
	i2c_init();	
}
void i2c_start(){
	i2c_state= I2C_BUSSY;//iletiþim baþladý meþgul bayraðý 
	TWCR=(1<<TWEN)|(1<<TWSTA)|(1<<TWINT)|(1<<TWIE);
	while(!(TWCR & (1<<TWINT))){
		timeout++;
		if (timeout>I2C_TIMEOUT){
			timeout=0;
			i2c_reset();
			return;
		}
	}
}
void i2c_adr(uint8_t adr,i2c_cmd_t cmd){
	
	while (i2c_state!=I2C_READY){
		timeout++;
		if (timeout>I2C_TIMEOUT){
			timeout=0;
			i2c_reset();
			return;
		}
	}	
	i2c_tx_bas=0;
	Sladr_RW=((adr<<1)|cmd);
}
void i2c_data(uint8_t data){
	i2c_tx_ring[i2c_tx_bas++]=data;
}
void i2c_end( i2c_rep_t repst){
	i2c_rep=repst;
	i2c_tx_son=0;	
	i2c_start();
}
void i2c_stop(){	
	TWCR=(1<<TWEN)|(1<<TWSTO)|(1<<TWINT);
	while(TWCR & (1<<TWSTO)){
	timeout++;
	if (timeout>I2C_TIMEOUT){
		timeout=0;
		i2c_reset();
		return;
		}
	}
	
	i2c_state= I2C_READY;
}
void i2c_send_data(uint8_t adr, uint8_t data,  i2c_rep_t repst){	
	
	i2c_adr(adr, I2C_WRITE);
	i2c_data(data);
	i2c_end(repst);
}
void i2c_send(uint8_t adr, uint8_t* str, uint8_t len,  i2c_rep_t repst){
	
	i2c_adr(adr, I2C_WRITE);
	for (uint8_t i=0;i<len;i++){
		i2c_data( str[i]);
	}
	i2c_end(repst);
}
void i2c_send_str(uint8_t adr, const char* str,  i2c_rep_t repst){
	i2c_adr(adr, I2C_WRITE);
	while (*str){
		i2c_data(*str++);		
	}	
	i2c_end(repst);	
}
void i2c_read(uint8_t adr, uint8_t len){
	
	i2c_adr(adr, I2C_READ);
	i2c_rx_bas=0;
	i2c_rx_son=0;
	i2c_rx_len=len-1;	
	i2c_start();
}
uint8_t i2c_gelen(){	

	if (i2c_rx_son==i2c_rx_bas)	return 0;
	return 1;	
}
uint8_t i2c_oku(){		
	return i2c_rx_ring[i2c_rx_son++];
}
uint8_t i2c_adres_scan(){
	for (uint8_t i=0;i<128;i++){
		adres_nack=1;
		i2c_send_data(i,0x00,N_REPEAT);
		_delay_ms(50);
		if (adres_nack)
		return i;
	}
	return 0;
}
ISR(TWI_vect){
	switch(I2C_STATUS){
		case (I2C_START):
		case (I2C_REP_START): 		
		TWDR=Sladr_RW;
		TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWIE);
		//while(!(TWCR & (1<<TWINT)));
		break;
		case (I2C_MTR_ADR_ACK):
		case (I2C_MTR_DATA_ACK):	
		if (i2c_tx_son!=i2c_tx_bas){			
			TWDR=i2c_tx_ring[i2c_tx_son++];
			TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWIE);
			//while(!(TWCR & (1<<TWINT)));
		}
		else if (i2c_rep==1){
			i2c_rep=0;
			i2c_state= I2C_READY;
			TWCR=(1<<TWEN);
		}
		else{
			i2c_stop();
		}		
			
			break;	
		case I2C_MTR_ADR_NACK:
			adres_nack=0;
			i2c_stop();	
			//tekrar dene, dur v.b
			break;				
		case I2C_MTR_DATA_NACK:	
			i2c_stop();
			//tekrar dene, dur v.b
			break;
		case I2C_ARB_LOST: 			
			break;
		case I2C_MRD_DATA_ACK:
		
		i2c_rx_ring[i2c_rx_bas++]=TWDR;	
		case I2C_MRD_ADR_ACK:
		if (i2c_rx_bas!=i2c_rx_len){
			TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWIE)|(1<<TWEA);
			//while(!(TWCR & (1<<TWINT)));
			}else{
			TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWIE);
			//while(!(TWCR & (1<<TWINT)));
		}
					
			break;			
		case I2C_MRD_ADR_NACK: 	
			i2c_stop();
			break;		
		case I2C_MRD_DATA_NACK:		
			i2c_rx_ring[i2c_rx_bas++]=TWDR;		
			i2c_stop();			
			break;
		case I2C_STR_ADR_ACK:
		break;					
		case I2C_STR_ADR_ACK_M_ARB_LOST:		
		break;	
		case I2C_STR_DATA_ACK:				
		break;
		case I2C_STR_DATA_NACK:				
		break;
		case I2C_STR_DATA_ACK_LAST_BYTE:	
		break;
		case I2C_SRD_ADR_ACK:	
		break;				
		case I2C_SRD_ADR_ACK_M_ARB_LOST:	
		break;	
		case I2C_SRD_GEN_ACK:	
		break;				
		case I2C_SRD_GEN_ACK_M_ARB_LOST:	
		break;	
		case I2C_SRD_ADR_DATA_ACK:
		break;			
		case I2C_SRD_ADR_DATA_NACK:		
		break;	
		case I2C_SRD_GEN_DATA_ACK:	
		break;		
		case I2C_SRD_GEN_DATA_NACK:
		break;			
		case I2C_SRD_STOP:		
		break;				
		case I2C_NO_INFO: 
		break;
		case I2C_BUS_ERR:
		break;
	}
}
