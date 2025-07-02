/**
******************************************************************************
* @file    mculib_uart.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the uart interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

#ifdef USE_DEBUGGING_INTERACTION
#define FSA_SYNC    0
#define FSA_CMD     1
#define FSA_LEN     2
#define FSA_DATA    3
#define FSA_CRC     4

#define TLNK_SYNC               0x10
#define TLNKCMD_ACK             0x06
#define TLNKCMD_NACK            0x21
#define TLNKCMD_DEBUG           0x30
#define TLNKCMD_GPIORE          0x62
#define TLNKCMD_GPIOWR          0x63
#define TLNKCMD_GENI2CR         0x66
#define TLNKCMD_GENI2CW         0x67
#define TLNKCMD_GENI2CR16       0x68
#define TLNKCMD_GENI2CW16       0x69
#define TLNKCMD_INVALID         0xff

static UINT8 buffer_in[10];
static UINT8 buffer_out[50];
static UINT8 buf_count = 0;
static UINT8 state = FSA_SYNC;
static UINT8 process = 0;
static UINT8 crc = 0;
static UINT8 len = 0;
static UINT8 cnt = 0;

static BOOL g_e_is_debug = FALSE;
#endif

UART_HandleTypeDef sUartxHandle = {0};
/* Exported constants ---------------------------------------------------------*/
//#define UARTx                                           UART1
//#define UARTx_TX_PORT                                   GPIOD
//#define UARTx_TX_PIN                                    GPIO_PIN_5
//#define UARTx_RX_PORT                                   GPIOD
//#define UARTx_RX_PIN                                    GPIO_PIN_6
//#define UARTx_TX_ALTERNATE_AFn                          GPIO_AF5_UART1_TXD
//#define UARTx_RX_ALTERNATE_AFn                          GPIO_AF5_UART1_RXD
//#define UARTx_IRQn                                      UART1_IRQn

/* Exported macros-------------------------------------------------------------*/
#define __HAL_RCC_UARTx_CLK_ENABLE()  			       __HAL_RCC_UART1_CLK_ENABLE()	
#define __HAL_RCC_UARTx_GPIO_CLK_ENABLE()  	           __HAL_RCC_GPIOD_CLK_ENABLE()	

VOID mculib_uart_init(UINT32 u32_baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_UARTx_GPIO_CLK_ENABLE();
    /**if UARTx is UART0 
	GPIO Configuration:    
    PA1     ------> UART0_RXD
    PA2     ------> UART0_TXD
    */
    GPIO_InitStruct.Pin = UARTx_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF;
    GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;	
    GPIO_InitStruct.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
    GPIO_InitStruct.SlewRate = GPIO_SLEW_RATE_HIGH;
    GPIO_InitStruct.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Alternate = UARTx_RX_ALTERNATE_AFn;
    HAL_GPIO_Init(UARTx_RX_PORT, &GPIO_InitStruct);
		
    GPIO_InitStruct.Pin = UARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF;
    GPIO_InitStruct.Alternate = UARTx_TX_ALTERNATE_AFn;
    HAL_GPIO_Init(UARTx_TX_PORT, &GPIO_InitStruct);		
    
    /* Peripheral clock enable */
    __HAL_RCC_UARTx_CLK_ENABLE();	
	
    sUartxHandle.Instance = UARTx;
    sUartxHandle.Init.BaudRate = u32_baud_rate;
    sUartxHandle.Init.BaudDouble = UART_BAUDDOUBLE_ENABLE;	
    sUartxHandle.Init.WordLength = UART_WORDLENGTH_8B;
    sUartxHandle.Init.Parity = UART_PARITY_NONE;
    sUartxHandle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&sUartxHandle);
    
    #ifdef USE_DEBUGGING_INTERACTION
    HAL_UART_Receive_IT(&sUartxHandle, &buffer_in[buf_count], sizeof(buffer_in[buf_count]));
    NVIC_EnableIRQ(UARTx_IRQn);
    #endif
}


#if 1
VOID mculib_uart_print_char(UINT8 u8_byte)
{
    #ifdef USE_DEBUGGING_INTERACTION
    NVIC_DisableIRQ(UARTx_IRQn);
    HAL_UART_Transmit(&sUartxHandle, &u8_byte, 1, 100);
    NVIC_EnableIRQ(UARTx_IRQn);
    #else
    HAL_UART_Transmit(&sUartxHandle, &u8_byte, 1, 100);
    #endif
}

VOID _send_ascii(UINT8 u8_byte)
{
    if (u8_byte > 0x09)
    {
        u8_byte = (u8_byte - 10) + 0x41;    // 'A' -> 0x41
    }
    else
    {
        u8_byte = u8_byte + 0x30;           // '0' -> 0x30
    }
    mculib_uart_print_char(u8_byte);   
}

VOID mculib_uart_print_dec(UINT16 u16_word)
{
    UINT8 u8_buff[5];
    UINT8 u8_i = 0;
    BOOL  b_start = FALSE;

    while(u8_i < 5)
    {
        u8_buff[u8_i] =  u16_word % 10;
        u16_word /= 10;
        u8_i ++;
    }

    while(u8_i)
    {
        if (u8_buff[u8_i - 1] != 0)
        {
            b_start = TRUE;
        }

        if (b_start)
        {
            mculib_uart_print_char(u8_buff[u8_i - 1] + 0x30);     
        }
        
        u8_i --;
    }

    if (!b_start)
    {
        mculib_uart_print_char(0x30);
    }
}

VOID mculib_uart_print_hex(UINT16 u16_word)
{
    UINT8 value;
    UINT8 i;
    
    i = (u16_word > 0xFF) ? 4 : 2;
    // High nibble    
    while (i --)
    {
        value = (u16_word >> (i * 4)) & 0x0F;
        _send_ascii(value);
    }
}

VOID mculib_uart_print_string(UINT8 *u8_string)
{
    while (*u8_string)
    {
        mculib_uart_print_char(*u8_string++);
    }
}

VOID mculib_uart_log(UINT8 *u8_string)
{
    mculib_uart_print_string(u8_string);
    mculib_uart_print_string((UINT8 *)"\r\n");
}

VOID mculib_uart_log1(UINT8 *u8_string, UINT16 u16_hex)
{
    mculib_uart_print_string(u8_string);
    mculib_uart_print_string((UINT8 *)"0x");
    mculib_uart_print_hex(u16_hex);
    mculib_uart_print_string((UINT8 *)"\r\n");
}

VOID mculib_uart_log2(UINT8 *u8_string, UINT16 u16_dec)
{
    mculib_uart_print_string(u8_string);
    mculib_uart_print_dec(u16_dec);
    mculib_uart_print_string((UINT8 *)"\r\n");
}
#endif

#ifdef USE_DEBUGGING_INTERACTION
VOID _send_string_com(UINT8 *str, UINT8 u8_len)
{
    UINT8 i = 0;  
    
    do  
    {  
        mculib_uart_print_char(*(str + i));  
        i ++;  
    } while (i < u8_len);
}

VOID _clean(VOID)
{
    buf_count = 0;
    state = FSA_SYNC;
}

VOID _answer_noack(VOID)
{
    /* send NACK sothat sender will re-send it */
    buffer_out[0]   = TLNK_SYNC;
    buffer_out[1]   = TLNKCMD_NACK;
    buffer_out[2]   = 0;
    buffer_out[3]   = TLNK_SYNC + TLNKCMD_NACK + 0;
    _send_string_com((UINT8 *)buffer_out, 4);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //HAL_UART_Receive_IT(huart, buffer_in, sizeof(buffer_in));
    //HAL_UART_Transmit(huart, &buffer_in[buf_count], sizeof(buffer_in[buf_count]), TIMEOUT_MAX);

    //if (RI0)
    {
        //buffer_in[buf_count] = S0BUF;

        switch (state)
        {
        case FSA_SYNC:
            if (buffer_in[buf_count] != TLNK_SYNC)
            {
                _clean();
                break;
            }
            crc = buffer_in[buf_count];
            buf_count = 1;
            state = FSA_CMD;
            break;
        case FSA_CMD:
            crc += buffer_in[buf_count];
            buf_count = 2;
            state = FSA_LEN;
            break;
        case FSA_LEN:
            len = buffer_in[buf_count];
            if (len == 0)
            {
                buf_count = 3;
                state = FSA_CRC;
            }
            else
            {
                cnt = 0;
                crc += buffer_in[buf_count];
                buf_count = 3;
                state = FSA_DATA;
            }
            break;
        case FSA_DATA:
            crc += buffer_in[buf_count];
            buf_count ++;
            cnt ++;
            if (cnt == len)
            {
                state = FSA_CRC;
            }
            break;
         case FSA_CRC:
            if (crc != buffer_in[buf_count]) 
            {
                _clean();
                break;
            }
            memcpy(buffer_out, buffer_in, 10);
            process = 1;
            _clean();
            break;
        default:
            _clean();
            break;
        }
        //RI0 = 0;    //must be clear by Software
    }
    HAL_UART_Receive_IT(huart, &buffer_in[buf_count], sizeof(buffer_in[buf_count]));
}

VOID _debug_service(VOID)
{    
    UINT8 cmd = TLNKCMD_INVALID;
#if 0
    UINT16 subaddr;
#endif

    while (process)
    {        
        cmd = buffer_out[1];

        buffer_out[0]   = TLNK_SYNC;
        buffer_out[1]   = TLNKCMD_ACK;
        crc = buffer_out[0] + buffer_out[1];
        
        switch (cmd)
        {
    #if 0
        case TLNKCMD_GENI2CR:
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            buffer_out[4] = mculib_i2c_read_8bidx8bval(buffer_out[5], buffer_out[6]);
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);
            break;
        case TLNKCMD_GENI2CW:
            buffer_out[2] = 1;
            if (mculib_i2c_write_8bidx8bval(buffer_out[5], buffer_out[6], buffer_out[7]))
            {
                buffer_out[3] = 0;
            }
            else
            {
                buffer_out[3] = 1;
            }
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);
            break;
        case TLNKCMD_GENI2CR16:
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            subaddr = (UINT8)buffer_out[7];
            subaddr <<= 8;
            subaddr += (UINT8)buffer_out[6];
            buffer_out[4] = mculib_i2c_read_16bidx8bval(buffer_out[5], subaddr);
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);
            break;
        case TLNKCMD_GENI2CW16:
            buffer_out[2] = 1;
            subaddr = (UINT8)buffer_out[7];
            subaddr <<= 8;
            subaddr += (UINT8)buffer_out[6];
            if (mculib_i2c_write_16bidx8bval(buffer_out[5], subaddr, buffer_out[8]))
            {
                buffer_out[3] = 0;
            }
            else
            {
                buffer_out[3] = 1;
            }
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);
            break;
     #endif
        case TLNKCMD_DEBUG:
            buffer_out[2] = 0;
            g_e_is_debug = buffer_out[3];        
            buffer_out[3] = crc;
            _send_string_com(buffer_out, 4);
            if (g_e_is_debug) //sned hex: 10 30 01 01 42
            {
                LOG("debug.");
            }
            else //sned hex: 10 30 01 00 41
            {
                LOG("run.");
            }
            break;
    #if 0
        case TLNKCMD_GPIORE:    //0x62
        {
            switch (buffer_out[3])
            {
                case 0: buffer_out[6] = P0; break;
                case 1: buffer_out[6] = P1; break;
                case 2: buffer_out[6] = P2; break;
                case 3: buffer_out[6] = P3; break;
                case 4: buffer_out[6] = P4; break;
                default: _answer_noack(); process = 0; return;
            }
            if (buffer_out[2] == 2)
            {
                if (buffer_out[4] >= 8)
                {
                    _answer_noack();
                    process = 0;
                    return;
                }
                buffer_out[6] = (buffer_out[6] >> buffer_out[4]) & 0x01;
            }
            if (buffer_out[2] != 1 && buffer_out[2] != 2)
            {
                _answer_noack();
                process = 0;
                return;
            }
            buffer_out[2] = 2;
            buffer_out[3] = 0;
            buffer_out[4] = buffer_out[6];
            crc += buffer_out[2] + buffer_out[3] + buffer_out[4];
            buffer_out[5] = crc;
            _send_string_com(buffer_out, 6);//10 06 02 00 val crc
            break;
        }
        case TLNKCMD_GPIOWR:    //0x63
        {
            if (buffer_out[2] == 3)
            {
                if (buffer_out[5] == 1)
                {
                    switch (buffer_out[4])
                    {
                        case 0: buffer_out[6] = 0x01; break;
                        case 1: buffer_out[6] = 0x02; break;
                        case 2: buffer_out[6] = 0x04; break;
                        case 3: buffer_out[6] = 0x08; break;
                        case 4: buffer_out[6] = 0x10; break;
                        case 5: buffer_out[6] = 0x20; break;
                        case 6: buffer_out[6] = 0x40; break;
                        case 7: buffer_out[6] = 0x80; break;
                        default: _answer_noack(); process = 0; return;
                    }
                    switch (buffer_out[3])
                    {
                        case 0: P0 |= buffer_out[6]; break;
                        case 1: P1 |= buffer_out[6]; break;
                        case 2: P2 |= buffer_out[6]; break;
                        case 3: P3 |= buffer_out[6]; break;
                        case 4: P4 |= buffer_out[6]; break;
                        default: _answer_noack(); process = 0; return;
                    }
                }
                else
                {
                    switch (buffer_out[4])
                    {
                        case 0: buffer_out[6] = 0xfe; break;
                        case 1: buffer_out[6] = 0xfd; break;
                        case 2: buffer_out[6] = 0xfb; break;
                        case 3: buffer_out[6] = 0xf7; break;
                        case 4: buffer_out[6] = 0xef; break;
                        case 5: buffer_out[6] = 0xdf; break;
                        case 6: buffer_out[6] = 0xbf; break;
                        case 7: buffer_out[6] = 0x7f; break;
                        default: _answer_noack(); process = 0; return;
                    }
                    switch (buffer_out[3])
                    {
                        case 0: P0 &= buffer_out[6]; break;
                        case 1: P1 &= buffer_out[6]; break;
                        case 2: P2 &= buffer_out[6]; break;
                        case 3: P3 &= buffer_out[6]; break;
                        case 4: P4 &= buffer_out[6]; break;
                        default: _answer_noack(); process = 0; return;
                    }
                }
            }
            else if (buffer_out[2] == 2)
            {
                switch (buffer_out[3])
                {
                    case 0: P0 = buffer_out[4]; break;
                    case 1: P1 = buffer_out[4]; break;
                    case 2: P2 = buffer_out[4]; break;
                    case 3: P3 = buffer_out[4]; break;
                    case 4: P4 = buffer_out[4]; break;
                    default: _answer_noack(); process = 0; return;
                }
            }
            else
            {
                _answer_noack();
                process = 0;
                return;
            }
            buffer_out[2] = 1;
            buffer_out[3] = 0;
            crc += buffer_out[2] + buffer_out[3];
            buffer_out[4] = crc;
            _send_string_com(buffer_out, 5);// 10 06 01 00 crc
            break;
        }
    #endif
        default:
            _answer_noack();
            break;
        }
        process = 0;
    }
}

VOID mculib_uart_service(VOID)
{   
    do
    {
        _debug_service();
    } while(g_e_is_debug);
}

void UART1_IRQHandler(void)
{
  /* USER CODE BEGIN UART1_IRQn 0 */
    HAL_UART_IRQHandler(&sUartxHandle);
  /* USER CODE END UART1_IRQn 0 */
}

#endif //endif USE_DEBUGGING_INTERACTION

