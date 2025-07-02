#include "hw32l003_hal.h"
//#include "i2c.h"
#include"mculib_typedef.h"
#include "mculib_i2c.h"
//#include "log.h"
//#include "io_mapping.h"
//#include "key.h"
#include "hdmi.h"
#include "mculib_uart.h"
#include "mculib_peripheral.h"
void hdmi_ldo_off(void);
//#define HDMI_PORTS	3 // move to head file
#define MS9601_I2C_ADD 0x52   // H: 0x56;  L: 0x52 
BOOL first_up = 0;
uint8_t g_sel_last,g_sel,g_pow = 0X00; // 0x07
static BOOL clk_4k, eq_need_config;

int sys_abs(int i)
{ /* compute absolute value of int argument */
	return (i < 0 ? -i : i);
}

UINT8 g_u8_input_clk = 0;

const UINT8 EQ_Address[] = {
	0x11, 0x12, 0x13, 0x14, 0x18, 0x19, 0x1E, 0x21, 0x22, 0x25};

const UINT8 PARA_3_EQ_1080[] = {
	0x2E, 0x02, 0x73, 0x33, 0x00, 0x00, 0x0B, 0x73, 0x00, 0x22};

const UINT8 PARA_3_EQ_4K[] = {
		0x2F, 0x00, 0x00, 0x00, 0x00, 0x77, 0x0B, 0x7D, 0x70, 0x30,	//0-2 ?//

};

const uint8_t hot_code[] = {
    0x01, 0x02, 0x04, 0x08, 0x10 //
};

const uint8_t PARA_3_SEL[] = {
    0x00, //Port 0
    0x01, //Port 1
    0x02, //Port 2
};
extern UINT8 g_u8_user_data; // kyq
void MS9601A_Write(uint8_t u8_index, uint8_t u8_value)
{
    i2c_write_8bidx8bval(MS9601_I2C_ADD, u8_index, u8_value);
}

uint8_t MS9601A_Read(uint8_t u8_index)
{
    return i2c_read_8bidx8bval(MS9601_I2C_ADD, u8_index);
}
// move to mculib_peripheral.c
//void mculib_chip_reset_ms9601()
//{
//    HAL_GPIO_WritePin(MS9601_Port, MS9601_RST_Pin, GPIO_PIN_RESET);
//    HAL_Delay(5);
//    HAL_GPIO_WritePin(MS9601_Port, MS9601_RST_Pin, GPIO_PIN_SET);
//}

void hdmi_eq_update(void)
{
   UINT8 idx, val, offset;
	if (!eq_need_config)
		return;
	eq_need_config = 0;
	//	MS9601A_Write(0x20,0x03);
	for (idx = 0; idx < 10; idx++)
	{
		offset = EQ_Address[idx];
		if (clk_4k)
			val = PARA_3_EQ_4K[idx]; 
		else
			val = PARA_3_EQ_1080[idx]; 
		MS9601A_Write(offset, val);
	}
	//if (g_u8_rx_ddc_state != 2)
	{
		MS9601A_Write(0x20, 0x0f);
	}
}

// 如果ch 这个通道有5v 则 g_sel = ch
void hdmi_logic_sel(uint8_t ch)
{
    UINT8 s;
	if (ch > HDMI_PORTS)
		return;
	s = hot_code[ch];
	// LOG2("s = ", s);
	if (ch < HDMI_PORTS)
	{
		if (s & g_pow)
		{
			g_sel = ch;
		}
	}
}

void hdmi_logic_next(void)
{
    UINT8 i;
	UINT8 rx_in;

	g_sel = g_sel_last;
	for (i = 0; i < HDMI_PORTS; i++)
	{
		g_sel++;
		if (g_sel >= HDMI_PORTS)
		{
			g_sel = 0;
		}
		rx_in = hot_code[g_sel];
		if (rx_in & g_pow) //g_pow = 0x07
		{
			return;
		}
	}
	MS9601A_Write(0x0f, 0x03);
}

void hdmi_pow_logic(void)
{
    UINT8 power, rpower, i;
	//update power
	power = 0;

	rpower = MS9601A_Read(0x51);

	if (rpower & 0x01)
		power |= (0x01 << 0);
	if (rpower & 0x02)
		power |= (0x01 << 1);
	if (rpower & 0x04)
		power |= (0x01 << 2);

	//    power logic
	if (g_pow != power)
	{
		LOG1("rx_5v = ", power);

		if ((~g_pow) & power)
		{
			rpower = (~g_pow) & power;
			for (i = 0; i < HDMI_PORTS; i++)
			{
				if (rpower & (0x01 << i))
					break;
			}
			//LOG("input");
			g_pow = power;
			g_sel_last = 3;
			hdmi_logic_sel(i);
		}
		else
		{
			//LOG("output");
			rpower = g_pow & (~power);
			g_pow = power;
			if (first_up == 0 && g_pow != 0)
			{
				if (rpower & (0x01 << g_sel_last) || power & 0x04)
				{
					g_sel_last = 3;
					hdmi_logic_next();
					first_up = 1;
				}
			}
			else
			{
				if (rpower & (0x01 << g_sel_last))
				{
					hdmi_logic_next();
				}
			}
		}
	}
}



void ms9601_init(void)
{
//    UINT8 chip_id = 0;
//	g_sel_last = HDMI_PORTS - 1;

//	mculib_chip_reset();
//	LOG("MS9601A_hdmi3_demo_E_20220617");
//	chip_id = MS9601A_Read(0x01);
//	if (chip_id == 0X60)
//	{
//		LOG("MS9601A chip valid");
//	}
//	else
//	{
//		LOG("MS9601A chip not valid");
//	}

}

// 切换端口
void HDMI_InPut_Switch(void)
{
    UINT8 i, val,test;

	if (g_sel_last != g_sel)
	{
		if (g_sel >= HDMI_PORTS)
		{
			return;
		}
		g_sel_last = g_sel;
		i = g_sel;
		val = PARA_3_SEL[i];

		MS9601A_Write(0x20, 0x03);
		//mculib_delay_ms(1);
		MS9601A_Write(0x0f, 0x03);

//		mculib_delay_ms(120);

			MS9601A_Write(0x0f, val);
		//mculib_delay_ms(1);
		//MS9601A_Write(0x20,0x0f);
	}

}

void hdmi_clk_det(void)
{
   UINT16 freqm_0, freqm_1, freqm_0_comp, freqm_1_comp;

	freqm_0 = MS9601A_Read(0x07);
	freqm_1 = MS9601A_Read(0x08);
	MS9601A_Write(0x05, 0X03);
	mculib_delay_ms(5);
	freqm_0_comp = MS9601A_Read(0x07);
	freqm_1_comp = MS9601A_Read(0x08);
	MS9601A_Write(0x05, 0X07);
	mculib_delay_ms(1);
	if (freqm_1 == freqm_1_comp)
	{
		freqm_0 = 0;
		freqm_1 = 0;
//		g_u8_input_clk = 0;
	}

	if (sys_abs(freqm_1 - g_u8_input_clk) >= 2)
	{
		g_u8_input_clk = freqm_1;

		if (freqm_1 == 0)
		{
			LOG2("input_clk = ", freqm_1);
			MS9601A_Write(0x20, 0x03);
			return;
		}
		LOG2("input_clk = ", freqm_1);

		if (freqm_1 > 72) //4K clk
		{
			clk_4k = 1; //
			LOG("input is 4K");
		}
		else
		{
			clk_4k = 0;
			LOG("input is not 4K");
		}

		eq_need_config = 1;
	}
}

void hdmi_ldo_off(void)
{

    MS9601A_Write(0x36, 0x81);

}

//void Key_Service(void)
//{
//    uint8_t u8key = key_service();
//
//    switch ( u8key )
//    {
//        case KEY_SHORT:
//            hdmi_logic_next();
//            break;
//
//        default:
//            break;
//    }
//}

//void IR_Service(void)
//{
// 	uint8_t u8key = ir_key_detect();
//	switch(u8key)
//	{
//	  case IR_KEY_NUM_1:
//	  	   hdmi_logic_next();
//  		break;
//	default:
//   		break;

//	}

//}

extern UINT8 g_u8_user_data;

void ms9601_Media_Service()
{
    if ((g_u8_user_data & 0x01) == 0x00)
    {
        hdmi_pow_logic(); //检测RX5V
       
    }
    HDMI_InPut_Switch(); //切换选择的端口
    hdmi_clk_det(); //获取输入的时钟
    hdmi_eq_update(); //修改设置的EQ  // cancel by kyq for test
    hdmi_ldo_off(); //关闭LDO// cancel by kyq for test

}


