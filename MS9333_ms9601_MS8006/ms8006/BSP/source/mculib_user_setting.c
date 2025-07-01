#include "..\header\mculib_common.h"

// Increase this number when upgrade to a new version.
#define PRJ_VERSION         0x02

// just a sample
UINT8 g_u8_user_data = 0x00; // 0x01 手动模式
//extern UINT8   OUTPUT_EDID_FLAG;
extern uint8_t g_sel;
BOOL mculib_user_load(VOID)
{
    UINT8 u8val;
    u8val = mculib_iap_read_byte(FLASH_PROGRAM_ADDRESS_START + 0);
    LOG2("VERSION = \n",u8val);
    if (u8val == PRJ_VERSION)
    {
        g_u8_user_data = mculib_iap_read_byte(FLASH_PROGRAM_ADDRESS_START + 1);
        LOG1("g_u8_user_data = ", g_u8_user_data);
//		LOG2("OUTPUT_EDID_FLAG = ", OUTPUT_EDID_FLAG);
        if (g_u8_user_data & 0x01)
        {
            // 手动模式
            LOG("manual mode\n");
            g_sel = (g_u8_user_data&0xF0)>>4;// 
        }
        else
        {
             LOG("Auto mode\n");
        }
    }
    else
    {
//        OUTPUT_EDID_FLAG = 3; //set default value

        LOG("user data default.");
        
        mculib_user_save();
    } 
    
    return (u8val == PRJ_VERSION);
}

VOID mculib_user_save(VOID)    
{   
    UINT8 bDataBuf[2];
    bDataBuf[0] = PRJ_VERSION;
    bDataBuf[1] = g_u8_user_data;
   
//    bDataBuf[1] = OUTPUT_EDID_FLAG;
//	LOG1("OLD PRJ_VERSION = " ,bDataBuf[0]);
//	LOG1("OLD OUTPUT_EDID_FLAG = " ,OUTPUT_EDID_FLAG);
    mculib_iap_erase();
    mculib_iap_write(FLASH_PROGRAM_ADDRESS_START, 2, bDataBuf);
//	LOG1("NEW PRJ_VERSION = " ,mculib_iap_read_byte(FLASH_PROGRAM_ADDRESS_START));
//	LOG1("NEW OUTPUT_EDID_FLAG = " ,mculib_iap_read_byte(FLASH_PROGRAM_ADDRESS_START+1));
}

