/**
******************************************************************************
* @file    mculib_i2c.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the i2c interface driver function.  
*
* Copyright (c) 2020, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef _MCULIB_I2C_H
#define _MCULIB_I2C_H

//MCU I2C module typedef
typedef struct _I2CX_TO_REGISTER_INFOMATION
{
    UINT8 *p_u8_addr_buffer;        //slave address and register address
    UINT8 u8_addr_num;              //address number, if the register address has one byte, this value is 2, if the register address has two bytes, this value is 3.
    UINT8 *p_u8_i2c_write_buffer;   //write to slave
    UINT16 u16_i2c_write_bytes_num; //number to write
    UINT8 *p_u8_i2c_read_buffer;    //read from slave
    UINT16 u16_i2c_read_bytes_num;  //number to read
} MS_I2CX_RW_REGISTER_T;

typedef enum _I2C_SPEED_CHOOSE_INFOMATION
{
    I2C_SPEED_20K  = (UINT8)0,  //20Kbit/s
    I2C_SPEED_100K = (UINT8)1,  //100Kbit/s
    I2C_SPEED_400K = (UINT8)2,  //400Kbit/s
    I2C_SPEED_750K = (UINT8)3   //750Kbit/s
} I2C_SPEED_E;


VOID mculib_i2c_init(VOID);
VOID mculib_i2c_set_speed(UINT8 u8_i2c_speed); //enum to I2C_SPEED_E

// 16-bit index for macrosilicon chip
UINT8 mculib_i2c_read_16bidx8bval(UINT8 u8_address, UINT16 u16_index);
BOOL  mculib_i2c_write_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT8 u8_value);

VOID  mculib_i2c_burstread_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);
VOID  mculib_i2c_burstwrite_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);

//8-bit index for HDMI EDID block 2-3 read
BOOL mculib_i2c_write_blank(UINT8 u8_address, UINT8 u8_index);
VOID mculib_i2c_burstread_8bidx8bval_ext(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length);

// 8-bit index fro eeprom AT24C02..
UINT8 mculib_i2c_read_8bidx8bval(UINT8 u8_address, UINT8 u8_index);
BOOL  mculib_i2c_write_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_value);
VOID  mculib_i2c_burstread_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length, UINT8 *pu8_value);
VOID  mculib_i2c_burstwrite_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length, UINT8 *pu8_value);

// 16-bit index for eeprom AT24C04..
UINT8 mculib_i2c_read_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index);
BOOL  mculib_i2c_write_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT8 u8_value);
VOID  mculib_i2c_burstread_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);
VOID  mculib_i2c_burstwrite_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);

//general I2C interfalce
VOID mculib_i2c_rw_register(MS_I2CX_RW_REGISTER_T *pst_i2cx_to_register);
uint8_t i2c_read_8bidx8bval(uint8_t u8_address, uint8_t u8_index);// kyq
BOOL i2c_write_8bidx8bval(uint8_t u8_address, uint8_t u8_index, uint8_t u8_value);//kyq
#endif
