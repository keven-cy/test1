/**
******************************************************************************
* @file    mculib_i2c.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the i2c interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

/********************config by user****************************/
/*mcu i2c port & pin*/
#define I2C_PORT            (GPIOB) //config by user
#define I2C_PIN_SDA         (GPIO_PIN_5)
#define I2C_PIN_SCL         (GPIO_PIN_4)

/************************************************************/
#define I2C_SDA_HIGH        HAL_GPIO_WritePin(I2C_PORT, I2C_PIN_SDA, GPIO_PIN_SET)
#define I2C_SCL_HIGH        HAL_GPIO_WritePin(I2C_PORT, I2C_PIN_SCL, GPIO_PIN_SET)

#define I2C_SDA_LOW         HAL_GPIO_WritePin(I2C_PORT, I2C_PIN_SDA, GPIO_PIN_RESET)
#define I2C_SCL_LOW         HAL_GPIO_WritePin(I2C_PORT, I2C_PIN_SCL, GPIO_PIN_RESET)

#define _i2c_read_sda()     HAL_GPIO_ReadPin(I2C_PORT, I2C_PIN_SDA)

#define _i2c_delay()        mculib_delay_us(g_u8_i2c_delay)

static UINT8 g_u8_i2c_delay = 1; //default I2C speed is 100Kbit/s

#define I2C_TIMEOUT         (0x1000) //define I2C wait ack signal time out = I2C_TIMEOUT * _i2c_delay()

VOID _i2c_start(VOID)
{
    /* assume here all lines are HIGH */
    I2C_SDA_HIGH;
    I2C_SCL_HIGH;
    _i2c_delay();
    
    I2C_SDA_LOW;
    _i2c_delay();
    
    I2C_SCL_LOW;    
}

VOID _i2c_stop(VOID)
{
    I2C_SDA_LOW;
    _i2c_delay();
    
    I2C_SCL_HIGH;
    _i2c_delay();
    
    I2C_SDA_HIGH;
    _i2c_delay();
}

BOOL _i2c_write_byte(UINT8 value)
{
    UINT8  __DATA index;
    BOOL   __DATA result = FALSE;
    UINT16 __DATA u16Wait;
    
    /* Send 8 bits to the I2C Bus - msb first */
    for (index = 0; index < 8; index++)  
    {
        if (value & 0x80)
        {
            I2C_SDA_HIGH;
        }
        else    
        {
            I2C_SDA_LOW;
        }
        _i2c_delay();
        
        I2C_SCL_HIGH;        
        _i2c_delay();
        
        I2C_SCL_LOW;
        _i2c_delay();
        
        value <<= 1;    
    }
    
    /* wait for ack =  readbit0 */
    I2C_SDA_HIGH;       // avoid  SDA is always Low.    
    _i2c_delay();
    
    I2C_SCL_HIGH;            
    _i2c_delay();
    
    u16Wait = 0;
    while (_i2c_read_sda() && (u16Wait < I2C_TIMEOUT)) 
        u16Wait ++;
    
    if (!_i2c_read_sda())
        result = TRUE;
    
    I2C_SCL_LOW;  
    I2C_SDA_HIGH;
    _i2c_delay();
    
    return result; 
}

UINT8 _i2c_read_byte(BOOL lastByte)
{
    UINT8 index;
    UINT8 value = 0x00;
    
    I2C_SDA_HIGH;       
    for (index = 0; index < 8; index++) 
    {
        value <<= 1;        
        I2C_SCL_HIGH;           
        _i2c_delay();
        value |= _i2c_read_sda();  
        I2C_SCL_LOW;
        _i2c_delay();
    }
    
    if (!lastByte)
    {
        I2C_SDA_LOW;
    }
    else 
    {
        I2C_SDA_HIGH;
    }
    _i2c_delay();
    
    I2C_SCL_HIGH;    
    _i2c_delay();
    
    I2C_SCL_LOW;
    I2C_SDA_HIGH;
    _i2c_delay();
    
    return value;
}

VOID mculib_i2c_init(VOID)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_PIN_SDA | I2C_PIN_SCL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OpenDrain = GPIO_OPENDRAIN;
    muclib_gpio_init(I2C_PORT, &GPIO_InitStruct);

    _i2c_stop();
}

VOID mculib_i2c_set_speed(UINT8 u8_i2c_speed)
{
    switch (u8_i2c_speed)
    {
    case I2C_SPEED_20K:
        g_u8_i2c_delay = 16; //measure is 20KHz 
        break;
    case I2C_SPEED_100K:
        g_u8_i2c_delay = 3; //measure is 100KHz 
        break;
    case I2C_SPEED_400K:
        g_u8_i2c_delay = 1; //measure is 250KHz 
        break;
    case I2C_SPEED_750K:
        g_u8_i2c_delay = 1; //measure is 250KHz
        break;
    }
}

// 16-bit index for macrosilicon chip
UINT8 mculib_i2c_read_16bidx8bval(UINT8 u8_address, UINT16 u16_index)
{
    UINT8 u8_value = 0;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;      
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;
    
    u8_value = _i2c_read_byte(TRUE);
    
STOP:
    _i2c_stop();
    return u8_value;
}

BOOL mculib_i2c_write_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT8 u8_value)
{
    BOOL result = FALSE;

    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;  
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;  
    
    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    if (!_i2c_write_byte(u8_value))                       
        goto STOP;  
    
    result = TRUE;

STOP:
    _i2c_stop();
    return result;                 
}

#if 1
VOID mculib_i2c_burstread_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value)
{
    UINT16 i;
   
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;

    for (i = 0; i < (u16_length-1); i ++)
    {
        *pu8_value++ = _i2c_read_byte(FALSE);
    } 

    *pu8_value = _i2c_read_byte(TRUE);
        
STOP:
    _i2c_stop();
}

VOID mculib_i2c_burstwrite_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value)
{
    UINT16 i;
    
   _i2c_start();
   
    if (!_i2c_write_byte(u8_address))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    for(i = 0; i< (u16_length-1); i ++)
    {
        if (!_i2c_write_byte(*pu8_value++))           
            goto STOP;  
    }

    if (!_i2c_write_byte(*pu8_value))           
        goto STOP;  
       
STOP:
    _i2c_stop();
}
#endif

#if (USER_SAVE == USER_SAVE_EEPROM)
// 8-bit index fro eeprom AT24C02..
UINT8 mculib_i2c_read_8bidx8bval(UINT8 u8_address, UINT8 u8_index)
{
    UINT8 u8_value = 0;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;      
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;
    
    u8_value = _i2c_read_byte(TRUE);
    
STOP:
    _i2c_stop();
    return u8_value;
}

BOOL mculib_i2c_write_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_value)
{
    BOOL result = FALSE;

    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_value))                       
        goto STOP;  
    
    result = TRUE;

STOP:
    _i2c_stop();
    return result;                 
}

VOID mculib_i2c_burstread_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length, UINT8 *pu8_value)
{
    UINT8 i;
   
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;

    for (i = 0; i < (u8_length-1); i ++)
    {
        *pu8_value++ = _i2c_read_byte(FALSE);
    } 

    *pu8_value = _i2c_read_byte(TRUE);
        
STOP:
    _i2c_stop();
}

VOID mculib_i2c_burstread_8bidx8bval_ext(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length)
{
    UINT8 i;
    UINT8 read_falg;
   
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;

    for (i = 0; i < (u8_length-1); i ++)
    {
       read_falg= _i2c_read_byte(FALSE);
      // if(i==0)
       // mculib_uart_log1("read_falg=",read_falg);
    } 

    _i2c_read_byte(TRUE);
        
STOP:
    _i2c_stop();
}

BOOL mculib_i2c_write_blank(UINT8 u8_address, UINT8 u8_index)
{
    BOOL result = FALSE;

    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;  
    
    result = TRUE;

STOP:
    ///_i2c_stop();
    return result;    
}


VOID mculib_i2c_burstwrite_8bidx8bval(UINT8 u8_address, UINT8 u8_index, UINT8 u8_length, UINT8 *pu8_value)
{
    UINT8 i;
    
   _i2c_start();
   
    if (!_i2c_write_byte(u8_address))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u8_index) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u8_index >> 8) ))
        goto STOP;
    
    for(i = 0; i< (u8_length-1); i ++)
    {
        if (!_i2c_write_byte(*pu8_value++))           
            goto STOP;  
    }

    if (!_i2c_write_byte(*pu8_value))           
        goto STOP;  
       
STOP:
    _i2c_stop();
}
#endif

#if 0
// 16-bit index for eeprom AT24C04..
UINT8 mculib_i2c_read_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index)
{
    UINT8 u8_value = 0;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;      

    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;
    
    u8_value = _i2c_read_byte(TRUE);
    
STOP:
    _i2c_stop();
    return u8_value;
}

BOOL mculib_i2c_write_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT8 u8_value)
{
    BOOL result = FALSE;

    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;  

    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_value))                       
        goto STOP;  
    
    result = TRUE;

STOP:
    _i2c_stop();
    return result;                 
}

VOID mculib_i2c_burstread_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value)
{
    UINT16 i;
   
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;

    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;

    for (i = 0; i < (u16_length-1); i ++)
    {
        *pu8_value++ = _i2c_read_byte(FALSE);
    } 

    *pu8_value = _i2c_read_byte(TRUE);
        
STOP:
    _i2c_stop();
}

VOID mculib_i2c_burstwrite_eeprom_16bidx8bval(UINT8 u8_address, UINT16 u16_index, UINT16 u16_length, UINT8 *pu8_value)
{
    UINT16 i;
    
   _i2c_start();
   
    if (!_i2c_write_byte(u8_address))
        goto STOP;

    if (!_i2c_write_byte( (UINT8)(u16_index >> 8) ))
        goto STOP;
    
    if (!_i2c_write_byte( (UINT8)(u16_index) ))
        goto STOP;
    
    for(i = 0; i< (u16_length-1); i ++)
    {
        if (!_i2c_write_byte(*pu8_value++))           
            goto STOP;  
    }

    if (!_i2c_write_byte(*pu8_value))           
        goto STOP;  
       
STOP:
    _i2c_stop();
}
#endif

#if 0
//general I2C interfalce
VOID mculib_i2c_rw_register(MS_I2CX_RW_REGISTER_T *pst_i2cx_to_register)
{
    UINT16 u16_i = 0;

    if (pst_i2cx_to_register == NULL || pst_i2cx_to_register->p_u8_addr_buffer == NULL)
    {
        return;
    }

    _i2c_start();

    _i2c_write_byte(*(pst_i2cx_to_register->p_u8_addr_buffer) & 0xfe);
    
    for (u16_i = 0; u16_i < pst_i2cx_to_register->u8_addr_num - 1; u16_i++)
    {
        _i2c_write_byte(*(pst_i2cx_to_register->p_u8_addr_buffer + u16_i + 1));
    }

    if (pst_i2cx_to_register->u16_i2c_write_bytes_num != 0)
    {
        if ((pst_i2cx_to_register->p_u8_i2c_write_buffer == NULL))
        {
            return;
        }
        for (u16_i = 0; u16_i < pst_i2cx_to_register->u16_i2c_write_bytes_num; u16_i++)
        {
            _i2c_write_byte(*(pst_i2cx_to_register->p_u8_i2c_write_buffer + u16_i));
        }
    }

    if (pst_i2cx_to_register->u16_i2c_read_bytes_num == 0)
    {
        _i2c_stop();
    }
    else
    {
        if ((pst_i2cx_to_register->p_u8_i2c_read_buffer == NULL))
        {
            return;
        }
        
        _i2c_start();
        _i2c_write_byte(*(pst_i2cx_to_register->p_u8_addr_buffer) | 0x01);
        
        for (u16_i = 0; u16_i < pst_i2cx_to_register->u16_i2c_read_bytes_num; u16_i++)
        {
            if (u16_i < pst_i2cx_to_register->u16_i2c_read_bytes_num - 1)
            {
                *(pst_i2cx_to_register->p_u8_i2c_read_buffer + u16_i) = _i2c_read_byte(FALSE);
            }           
            else
            {
                *(pst_i2cx_to_register->p_u8_i2c_read_buffer + u16_i) = _i2c_read_byte(TRUE);
            }
        }
        
        _i2c_stop();
    }
}
#endif

uint8_t i2c_read_8bidx8bval(uint8_t u8_address, uint8_t u8_index)
{
    uint8_t u8_value = 0;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;      
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;
    
    _i2c_start();
    
    if (!_i2c_write_byte(u8_address | 0x01))
        goto STOP;
    
    u8_value = _i2c_read_byte(TRUE);
    
STOP:
    _i2c_stop();
    return u8_value;
}

BOOL i2c_write_8bidx8bval(uint8_t u8_address, uint8_t u8_index, uint8_t u8_value)
{
    BOOL result = FALSE;

    _i2c_start();
    
    if (!_i2c_write_byte(u8_address))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_index))
        goto STOP;  
    
    if (!_i2c_write_byte(u8_value))                       
        goto STOP;  
    
    result = TRUE;

STOP:
    _i2c_stop();
    return result;                 
}

