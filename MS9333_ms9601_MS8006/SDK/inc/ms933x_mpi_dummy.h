/******************************************************************************
* @file    ms933x_mpi_dummy.h
* @author  
* @version V1.0.0
* @date    15-Nov-2014
* @brief   mpi dummy implemented by user.
*
* Copyright (c) 2009-2014, MacroSilicon Technology Co.,Ltd.
******************************************************************************/
#ifndef __MACROSILICON_MS933X_MPI_DUMMY_H__
#define __MACROSILICON_MS933X_MPI_DUMMY_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
*  Function name:   mculib_chip_reset
*  Description:     hardware short reset to chip
*  Entry:           [in]None
* 
*  Returned value:  None
*  Remark:
***************************************************************/
extern VOID mculib_chip_reset(VOID);


/***************************************************************
*  Function name:   mculib_chip_read_interrupt_pin
*  Description:     read chip interrupt pin status
*  Entry:           [in]None
* 
*  Returned value:  BOOL (high level return TRUE, else return FALSE)
*  Remark:
***************************************************************/
extern BOOL mculib_chip_read_interrupt_pin(VOID);


/***************************************************************
*  Function name:   mculib_delay_ms
*  Description:     delay ms
*  Entry:           [in]u8_ms, uint ms
* 
*  Returned value:  None
*  Remark:
***************************************************************/
extern VOID mculib_delay_ms(UINT8 u8_ms);


/***************************************************************
*  Function name:   mculib_delay_us
*  Description:     delay us
*  Entry:           [in]u8_us, uint us
* 
*  Returned value:  None
*  Remark:
***************************************************************/
extern VOID mculib_delay_us(UINT8 u8_us);


/***************************************************************
*  Function name:   mculib_i2c_read_16bidx8bval
*  Description:     read back 8 bits register value with 16 bits specified index
*  Entry:           [in]u8_address: 8 bits I2C slave address
*                   [in]u16_index:  16 bits register index
*
*  Returned value: UINT8 type register value
*  Remark:
***************************************************************/
extern UINT8 mculib_i2c_read_16bidx8bval(UINT8 u8_address, UINT16 u16_index);


/***************************************************************
*  Function name:   mculib_i2c_write_16bidx8bval
*  Description:     write 8 bits register value to 16 bits specified index
*  Entry:           [in]u8_address: 8 bits I2C slave address
*                   [in]u16_index:  16 bits register index
*                   [in]u8_value:   8 bits register value
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern BOOL mculib_i2c_write_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT8 u8_value);

//I2C burst read/write mode 
/***************************************************************
*  Function name:   mculib_i2c_burstread_16bidx8bval
*  Description:     8 bits i2c burst read for 16 bits specified index
*  Entry:           [in]u8_address: 8 bits I2C slave address
*                   [in]u16_index:  16 bits register index
*                   [in]u16_length: 16 bits length to read
*                   [in]pu8_value:  read buffer
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern VOID  mculib_i2c_burstread_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);

/***************************************************************
*  Function name:   mculib_i2c_burstwrite_16bidx8bval
*  Description:     8 bits i2c burst write for 16 bits specified index
*  Entry:           [in]u8_address: 8 bits I2C slave address
*                   [in]u16_index:  16 bits register index
*                   [in]u16_length: 16 bits length to write
*                   [in]pu8_value:  data buffer to write
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern VOID  mculib_i2c_burstwrite_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value);

//8bits I2C slaver address for HDMI DDC
extern VOID  mculib_i2c_set_speed(UINT8 u8_i2c_speed);
extern UINT8 mculib_i2c_read_8bidx8bval(UINT8 u8_address, UINT8 u8_index);
extern BOOL  mculib_i2c_write_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_value);
extern VOID  mculib_i2c_burstread_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length, UINT8 *pu8_value);

//8-bit index for HDMI EDID block 2-3 read
extern BOOL mculib_i2c_write_blank(UINT8 u8_address, UINT8 u8_index);
extern VOID mculib_i2c_burstread_8bidx8bval_ext(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length);


/***************************************************************
*  Function name:   mculib_uart_log
*  Description:     printf log thru uart
*  Entry:           [in]u8_string:  string to printf
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern VOID mculib_uart_log(UINT8 *u8_string);

/***************************************************************
*  Function name:   mculib_uart_log1
*  Description:     printf log thru uart with string & hex value
*  Entry:           [in]u8_string:  string to printf
*                   [in]u16_hex:  hex value to printf
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern VOID mculib_uart_log1(UINT8 *u8_string, UINT16 u16_hex);

/***************************************************************
*  Function name:   mculib_uart_log2
*  Description:     printf log thru uart with string & decimal value
*  Entry:           [in]u8_string:  string to printf
*                   [in]u16_dec:  decimal value to printf
*
*  Returned value: if write success return TRUE, else return FALSE
*  Remark:
***************************************************************/
extern VOID mculib_uart_log2(UINT8 *u8_string, UINT16 u16_dec);


#ifndef LOG
#define LOG(X)      mculib_uart_log((UINT8*)X)
#endif

#ifndef LOG1
#define LOG1(X, Y)  mculib_uart_log1((UINT8*)X, Y)
#endif

#ifndef LOG2
#define LOG2(X, Y)  mculib_uart_log2((UINT8*)X, Y)
#endif


#ifdef __cplusplus
}
#endif

#endif  // __MACROSILICON_MS933X_MPI_DUMMY_H__
