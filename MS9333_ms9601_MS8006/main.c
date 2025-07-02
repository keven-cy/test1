#include "mculib_common.h"
#include "ms933x_comm.h"
#include "ms933x.h"
#include "hdmi.h"
/*
 8006 内置64K字节的嵌入式Flash;4K字节的SRAM
*/
//user define
#define SYS_AUDIO_PAD_OUTPUT_ENABLE     (1) //i2s and spdif pad output function enable

//#define pin_interupt_read()           //((UINT8)P2_3)

#define MS933X_SDK_VERSION  "v2.0.0_20200527"


#define HDMI_TX_CHN_NUM     (4) // kyq change 4 to 2
#define HDMI_TX_SH_CHN_NUM  (4)

#define DEBUG_EDID_FILE_SAVE    (0)

static e_RxPhy g_u8_rx_phy_select = PI;

static UINT16  g_u16Timer = 100; // uint ms

//
static VIDEOTIMING_T g_st_hdmi_in_timing;


#define TMDS_CLK_MARGIN (1000) //10MHz

static UINT16 g_u16_input_tmds_clk = 0;

BOOL g_b_hdmi_input_valid = FALSE;

UINT8 g_u8_source_5v_connect;
UINT8 g_u8_sink_hpd_connect[HDMI_TX_CHN_NUM];

#define LED_PIN_RX    (GPIO_PIN_1)
#define LED_PIN_TX(x) (GPIO_PIN_1 << (4 - x))

static HDMI_CONFIG_T g_st_hdmi_timing;

static UINT8 g_u8_rx_hdcp_support = TRUE;
//static UINT8 g_u8_tx_hdcp_support = TRUE;
static UINT8 g_u8_tx_hdcp_support = FALSE;// kyq 20220628 test  关掉HDCP

//for debug mode
static UINT8 g_u8_tx_chn_enable_flag = 0x09; //bit0: chn0 enable, bit1: chn1 enable...
static UINT8 g_u8_tx_hpd_test_flag = 0x00; //bit0: chn0 force output, bit1: chn1 force output...
static UINT8 g_u8_rx_hpd_test_flag = FALSE; //fixed set RX HPD true and rx edid use default edid
static UINT8 g_u8_hdmi_rxpll_error_counter_test_flag = FALSE; //enable hdmi rxpll error counter serivce
static UINT8 g_u8_hdmi_tx_shell_timing_stable_test_flag = FALSE; //enable hdmi tx shell timing stable service

static UINT8 g_u8_hdmi_in_timing_log_enable = FALSE; //enable hdmi input timing log printf.


#define RXPLL_ERROR_MAX (0x20)
#define RXPLL_ERROR_MID (0x10)
#define RXPLL_ERROR_MIN (0x10)

#define SYSTEM_ERROR_TIMEOUT_1S (1000 / g_u16Timer) //1000ms
#define SYSTEM_ERROR_TIMEOUT_10S (7000 / g_u16Timer) //7s
static UINT8 g_u8_system_error_service_flag = FALSE;
static UINT16 g_u8_system_error_timer_count_1s = 0;
static UINT16 g_u8_system_error_timer_count_10s = 0;
static UINT8  g_u8_system_error_flag = 0; //bit0: rxpll unlock; bit1: rxpll error; bit2: hdmi shell unstable
static UINT8  g_u8_system_error_action_flag = 0; //record system error, count hpd toggle times


#define CHIP_VALID_CHECK_TIMEOUT (3000 / g_u16Timer) //3000ms
static UINT16 g_u8_chip_valid_check_timer_count = 0;

#define HDMI_SHELL_ERROR_TIMEOUT (500 / g_u16Timer) //500ms
static UINT16 g_u8_hdmi_shell_error_timer_count = 0;

#define HDMI_TX_HPD_DET_TIMEOUT (200 / g_u16Timer) //200ms
#define HDMI_RX_HPD_OFF_TIMEOUT (7) //1400ms
static UINT16 g_u8_hdmi_tx_hpd_detect_count = 0;
static UINT16 g_u8_hdmi_rx_hpd_off_count = HDMI_RX_HPD_OFF_TIMEOUT;


#define RX_HDCP_TIMEOUT (500 / g_u16Timer) //500ms
static UINT16 g_u8_rx_hdcp_timer_count = 0;
static UINT8  g_u8_rx_hdcp_woking_flag = FALSE;

#define TX_HDCP_TIMEOUT (3000 / g_u16Timer) //3000ms
#define TX_HDCP_START_TIME ((3000 - 1000)  / g_u16Timer) //1000ms
static UINT16 g_u8_tx_hdcp_timer_count_buf[HDMI_TX_CHN_NUM];
static UINT8  g_u8_tx_hdcp_process_flag_buf[HDMI_TX_CHN_NUM];
static UINT8  g_u8_tx_hdcp_init_flag_buf[HDMI_TX_CHN_NUM];
static UINT8  g_u8_tx_hdcp_enable_flag_buf[HDMI_TX_CHN_NUM];

static UINT8  g_u8_rxpll_configed_status = 0;

#define MDT_CHANGE_TIMEOUT (200 / g_u16Timer) //200ms
static UINT16 g_u16_mdt_change_timer_count = 0;
static BOOL g_b_mdt_change_flag = FALSE;

#define RX_PACKET_TIMEOUT (200 / g_u16Timer) //200ms
static UINT16 g_u8_rx_packet_timer_count = 0;

static BOOL   g_b_rx_gcp_avmute_flag = FALSE;

static UINT8  g_u8_rx_hpd_flag = 0;
static UINT8  g_u8_sys_edid_flag = 0xff;  //none edid value
static HDMI_EDID_FLAG_T g_st_hdmi_edid_flag[HDMI_TX_CHN_NUM + 1];
static UINT8  g_u8_sys_edid_used_buf[256];
static UINT8  g_u8_sys_edid_buf[256];
static UINT8  g_u8_sys_edid_buf_last_id = 0xff;
static UINT32 g_u32_hdmi_sink_min_timing = 0xFFFFFFFFUL;
static UINT32 g_u32_dvi_sink_min_timing = 0xFFFFFFFFUL;

extern uint8_t g_sel;
uint8_t g_Output1Disable = 0; // kyq
UINT16 g_LedTimer = 0; // kyq
extern UINT8 g_u8_user_data; // kyq
#define HDMI_TX_DEFAULT_EDID_ID HDMI_TX_CHN_NUM

/* 4K EDID 
static UINT8 __CODE u8_sys_edid_default_buf[256] =
{
    // Explore Semiconductor, Inc. EDID Editor V2
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,   // address 0x00
    0x4C, 0x2D, 0xFF, 0x0D, 0x58, 0x4D, 0x51, 0x30,   //
    0x1C, 0x1C, 0x01, 0x03, 0x80, 0x3D, 0x23, 0x78,   // address 0x10
    0x2A, 0x5F, 0xB1, 0xA2, 0x57, 0x4F, 0xA2, 0x28,   //
    0x0F, 0x50, 0x54, 0xBF, 0xEF, 0x80, 0x71, 0x4F,   // address 0x20
    0x81, 0x00, 0x81, 0xC0, 0x81, 0x80, 0x95, 0x00,   //
    0xA9, 0xC0, 0xB3, 0x00, 0x01, 0x01, 0x04, 0x74,   // address 0x30
    0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58,   //
    0x8A, 0x00, 0x60, 0x59, 0x21, 0x00, 0x00, 0x1E,   // address 0x40
    0x00, 0x00, 0x00, 0xFD, 0x00, 0x18, 0x4B, 0x1E,   //
    0x5A, 0x1E, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20,   // address 0x50
    0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x55,   //
    0x32, 0x38, 0x48, 0x37, 0x35, 0x78, 0x0A, 0x20,   // address 0x60
    0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFF,   //
    0x00, 0x48, 0x54, 0x50, 0x4B, 0x37, 0x30, 0x30,   // address 0x70
    0x30, 0x35, 0x31, 0x0A, 0x20, 0x20, 0x01, 0xF7,   //
    0x02, 0x03, 0x26, 0xF0, 0x4B, 0x5F, 0x10, 0x04,   // address 0x80
    0x1F, 0x13, 0x03, 0x12, 0x20, 0x22, 0x5E, 0x5D,   //
    0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,   // address 0x90
    0x6D, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x80, 0x3C,   //
    0x20, 0x10, 0x60, 0x01, 0x02, 0x03, 0x02, 0x3A,   // address 0xA0
    0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,   //
    0x45, 0x00, 0x60, 0x59, 0x21, 0x00, 0x00, 0x1E,   // address 0xB0
    0x02, 0x3A, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40,   //
    0x10, 0x2C, 0x45, 0x80, 0x60, 0x59, 0x21, 0x00,   // address 0xC0
    0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0,   //
    0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x60, 0x59,   // address 0xD0
    0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0,   //
    0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00,   // address 0xE0
    0x60, 0x59, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00,   //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // address 0xF0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8,   //
};*/

static UINT8 __CODE u8_sys_edid_default_buf[256] =
{
    // Explore Semiconductor, Inc. EDID Editor V2
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,   // address 0x00
    0x34, 0x23, 0x01, 0x01, 0x01, 0x02, 0x03, 0x04,   //
    0x01, 0x18, 0x01, 0x03, 0x80, 0x33, 0x1D, 0x78,   // address 0x10
    0xEF, 0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26,   //
    0x0F, 0x50, 0x54, 0x2D, 0xCF, 0x00, 0x81, 0x00,   // address 0x20
    0x81, 0xC0, 0x81, 0x80, 0xD1, 0xC0, 0x01, 0x01,   //
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1D,   // address 0x30
    0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,   //
    0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E,   // address 0x40
    0xF3, 0x39, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,   //
    0x58, 0x2C, 0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00,   // address 0x50
    0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32,   //
    0x55, 0x1E, 0x51, 0x11, 0x00, 0x0A, 0x20, 0x20,   // address 0x60
    0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,   //
    0x00, 0x4D, 0x41, 0x43, 0x52, 0x4F, 0x53, 0x49,   // address 0x70
    0x4C, 0x49, 0x43, 0x4F, 0x4E, 0x0A, 0x01, 0xCC,   //
    0x02, 0x03, 0x1D, 0x41, 0x4A, 0x04, 0x1F, 0x02,   // address 0x80
    0x90, 0x06, 0x11, 0x15, 0x14, 0x05, 0x13, 0x23,   //
    0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x65,   // address 0x90
    0x03, 0x0C, 0x00, 0x10, 0x00, 0x02, 0x3A, 0x80,   //
    0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45,   // address 0xA0
    0x00, 0xDD, 0x0C, 0x11, 0x00, 0x00, 0x1E, 0x01,   //
    0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E,   // address 0xB0
    0x28, 0x55, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00,   //
    0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D,   // address 0xC0
    0x10, 0x10, 0x3E, 0x96, 0x00, 0x10, 0x09, 0x00,   //
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,   // address 0xD0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // address 0xE0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // address 0xF0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6,   //
};

static UINT8 __CODE u8_rx_ksv_buf[5] =
{
    0xc5,
    0xe3,
    0x7e,
    0x1a,
    0x88
};

static UINT8 __CODE u8_rx_ksv_clr_buf[5] =
{
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

static UINT8 __CODE u8_rx_key_buf[280] =
{
    0xfd, 0x36, 0x27, 0xdd, 0x61, 0xa8, 0x26,   //1
    0xa4, 0x38, 0x36, 0x6d, 0x8b, 0xc0, 0xec,   //2
    0x53, 0xc6, 0xbc, 0x10, 0xb6, 0xa5, 0x3d,   //3
    0x68, 0x88, 0x63, 0xb0, 0xf6, 0x5d, 0x32,   //4
    0x77, 0x68, 0x68, 0x50, 0x96, 0x22, 0xa2,   //5
    0xb3, 0x21, 0x92, 0xb3, 0xf5, 0xfa, 0xee,   //6
    0x72, 0x78, 0x2a, 0xa9, 0xa5, 0x41, 0xc1,   //7
    0x9f, 0x91, 0x5a, 0x55, 0x45, 0xf0, 0x18,   //8
    0xb0, 0xd8, 0x32, 0xb9, 0xc5, 0x1a, 0x46,   //9
    0x02, 0xa0, 0xe0, 0xdb, 0xf1, 0x77, 0x48,   //10
    0x6f, 0x12, 0xbb, 0xcb, 0x27, 0x40, 0xa6,   //11
    0x2d, 0x80, 0x5b, 0x51, 0x4f, 0x23, 0x71,   //12
    0x83, 0xc6, 0x97, 0x4a, 0xe9, 0x2d, 0x47,   //13
    0xa6, 0xd9, 0x83, 0x03, 0x9f, 0x8c, 0xe1,   //14
    0x15, 0xc4, 0x4d, 0x7f, 0x50, 0x58, 0x20,   //15
    0x25, 0x90, 0xcf, 0x71, 0x1f, 0x4c, 0x67,   //16
    0x53, 0x2e, 0x33, 0xb8, 0xd3, 0xa6, 0x33,   //17
    0x88, 0xa7, 0xd8, 0xe6, 0x60, 0x4c, 0x32,   //18
    0x17, 0x31, 0x3a, 0xf2, 0x2c, 0xae, 0xb5,   //19
    0x46, 0x04, 0xd0, 0xea, 0x74, 0x99, 0x03,   //20
    0xdb, 0x5d, 0x81, 0x5c, 0xbe, 0xdb, 0xc9,   //21
    0x99, 0xd5, 0x89, 0x9e, 0xbe, 0xa8, 0x5a,   //22
    0xd2, 0x79, 0x92, 0x97, 0x90, 0xfd, 0xbf,   //23
    0x06, 0x16, 0xd2, 0xae, 0x96, 0xdb, 0x40,   //24
    0xed, 0xf3, 0xe0, 0x85, 0x90, 0xa0, 0x32,   //25
    0x08, 0x7b, 0xeb, 0xff, 0x8f, 0xef, 0x48,   //26
    0x9e, 0x7a, 0x34, 0x54, 0x4b, 0x43, 0x48,   //27
    0xf5, 0x86, 0xd7, 0x23, 0xde, 0x09, 0x38,   //28
    0x4a, 0x65, 0x23, 0x44, 0xd2, 0x32, 0x60,   //29
    0x71, 0xa1, 0x95, 0xad, 0x81, 0xba, 0xb7,   //30
    0x74, 0xd5, 0x66, 0x75, 0xfa, 0xf6, 0xc9,   //31
    0xe5, 0x8f, 0x06, 0xa9, 0xb0, 0xe1, 0xef,   //32
    0x38, 0xa5, 0x73, 0x91, 0xa8, 0x35, 0xdd,   //33
    0x38, 0xa8, 0x14, 0x1b, 0xd1, 0x7a, 0x5d,   //34
    0xbf, 0xa1, 0xe4, 0x51, 0xd0, 0xba, 0xaf,   //35
    0x66, 0x47, 0x1e, 0x28, 0x46, 0xfb, 0xcb,   //36
    0x73, 0x07, 0xe7, 0xe7, 0xfa, 0x59, 0x25,   //37
    0x26, 0x94, 0xc0, 0x50, 0x3a, 0x54, 0x73,   //38
    0x72, 0x13, 0xf9, 0x0c, 0xf3, 0x1a, 0xcb,   //39
    0x53, 0xe1, 0x77, 0xfa, 0x1d, 0x78, 0xcf    //40
};

static UINT8 __CODE u8_tx_ksv_buf[5] =
{
    //##write the Bksv to Tx, and calculate the Km at the same time
    //
    0x75,
    0xbd,
    0x4e,
    0xc0,
    0x32,
};

static UINT8 __CODE u8_tx_key_buf[280] =
{
    //##write the Bksv to Tx, and calculate the Km at the same time
    0xda,
    0x8e,
    0x0b,
    0x36,
    0xd8,
    0x9f,
    0x1a,
    0x48,
    0x6d,
    0x69,
    0x4b,
    0x13,
    0x3b,
    0x99,
    0x7b,
    0xd0,
    0xc7,
    0x2f,
    0x3b,
    0xc0,
    0xd6,
    0x47,
    0xe4,
    0xd8,
    0x17,
    0x57,
    0xc8,
    0x9c,
    0x7b,
    0xaf,
    0x3d,
    0xb6,
    0xc1,
    0xfc,
    0x6b,
    0xab,
    0xa2,
    0x06,
    0x38,
    0x11,
    0x6c,
    0xcd,
    0x53,
    0xf7,
    0x0b,
    0x45,
    0xcc,
    0x49,
    0xa8,
    0xf4,
    0x3a,
    0x78,
    0xcc,
    0xea,
    0x96,
    0x24,
    0x95,
    0x5b,
    0xf4,
    0xa3,
    0xe4,
    0x4c,
    0xce,
    0x67,
    0xa2,
    0xac,
    0xf8,
    0x5d,
    0x12,
    0xc4,
    0x55,
    0x4a,
    0xab,
    0x66,
    0x85,
    0x94,
    0x29,
    0x47,
    0x1f,
    0xba,
    0x5b,
    0x39,
    0x6f,
    0x64,
    0xf6,
    0x81,
    0xbb,
    0xf4,
    0x6e,
    0x79,
    0x22,
    0xd7,
    0xcb,
    0xb5,
    0xca,
    0x67,
    0x52,
    0xce,
    0xe0,
    0x04,
    0x30,
    0x7c,
    0xcc,
    0x82,
    0xfb,
    0xaf,
    0xb1,
    0x5a,
    0x94,
    0x7f,
    0xd5,
    0x17,
    0x2d,
    0xe3,
    0xf6,
    0xa4,
    0x2a,
    0x4f,
    0xc8,
    0x6c,
    0x92,
    0xbb,
    0xa5,
    0x1b,
    0x6a,
    0x6f,
    0x5a,
    0xb3,
    0x87,
    0xef,
    0xc8,
    0x45,
    0xe9,
    0xd2,
    0x8a,
    0x2a,
    0x32,
    0x58,
    0xda,
    0x2d,
    0x86,
    0xd1,
    0x36,
    0xe0,
    0x6a,
    0xe6,
    0x8b,
    0x00,
    0x64,
    0x38,
    0xf2,
    0x68,
    0x6a,
    0xc9,
    0x8d,
    0x55,
    0xdb,
    0x75,
    0xe6,
    0x52,
    0x47,
    0x71,
    0x77,
    0xe7,
    0x3e,
    0xbb,
    0xaf,
    0x69,
    0x21,
    0x4e,
    0xff,
    0x14,
    0xc6,
    0x78,
    0x17,
    0xf3,
    0xcd,
    0xb4,
    0xa2,
    0x5e,
    0xb5,
    0xa3,
    0x16,
    0x65,
    0x41,
    0xfa,
    0xed,
    0xae,
    0xb1,
    0x77,
    0x56,
    0xf0,
    0x84,
    0x05,
    0x54,
    0x03,
    0x60,
    0x58,
    0x06,
    0x46,
    0xd1,
    0x69,
    0x22,
    0x36,
    0x36,
    0x1e,
    0xb9,
    0x57,
    0x47,
    0x74,
    0x13,
    0x93,
    0x3e,
    0x7a,
    0xca,
    0x12,
    0x75,
    0xe9,
    0x3a,
    0x8f,
    0x78,
    0xbd,
    0xac,
    0x32,
    0x12,
    0x57,
    0xa5,
    0x45,
    0xdc,
    0x1b,
    0x8c,
    0x2d,
    0x16,
    0x38,
    0x4e,
    0x7a,
    0xf4,
    0x88,
    0x1b,
    0x07,
    0x38,
    0xf6,
    0xb5,
    0x5d,
    0x8e,
    0xa9,
    0x48,
    0xf4,
    0x0e,
    0x11,
    0xd1,
    0x62,
    0xff,
    0xe7,
    0x77,
    0x9c,
    0x7b,
    0xe8,
    0x18,
    0x3b,
    0x13,
    0x16,
    0xb2,
    0x42,
    0xf4,
    0x78,
    0xca,
    0x0e,
    0x09,
    0x78,
    0xdc,
    0x05,
    0xca,
    0x2f,
    0x78,
    0xbf,
    0xab,
    0x2b,
    0x5f,
    0x85,
};


typedef enum _E_HDMI_TX_CONFIG_ITEM_
{
    HDMI_TX_CONFIG_AV       = 0x00, //video and audio
    HDMI_TX_CONFIG_AUD      = 0x01, //audio
    HDMI_TX_CONFIG_AUD_FS   = 0x02, //audio fs
    HDMI_TX_CONFIG_AVMUTE   = 0x03  //avmute packet
}HDMI_TX_CONFIG_ITEM_E;

//
UINT8 _sink_hpd_low_process(UINT8 u8_chn, UINT8 u8_hpd);
void sys_hotplug_init(void);
void _hdmi_tx_gcp_avmute_param_default(void);
void _shutdown_output(UINT8 u8_output_chn);
void sys_shutdown_output(void);
void _config_output(UINT8 u8_output_chn);
void sys_config_output(UINT8 u8_config_item, UINT8 u8_value); //refer to HDMI_TX_CONFIG_ITEM_E
//


//
void _hdmi_tx_edid_param_default(UINT8 u8_output_chn);
void _hdmi_tx_hdcp_param_default(UINT8 u8_output_chn);
void _hdmi_tx_hdcp_start(UINT8 u8_output_chn);


static int _abs(int i)
{      /* compute absolute value of int argument */
    return (i < 0 ? -i : i);
}

void sys_led_init(BOOL b_all_on)
{
    mculib_hdmi_rx_led_control(b_all_on);
    mculib_hdmi_tx_led_control(0, b_all_on);
    mculib_hdmi_tx_led_control(1, b_all_on);
    mculib_hdmi_tx_led_control(2, b_all_on);
    mculib_hdmi_tx_led_control(3, b_all_on);
}

void sys_led_process(void)
{
    if (g_b_hdmi_input_valid)
    {
        mculib_hdmi_rx_led_control(1);

        mculib_hdmi_tx_led_control(0, g_u8_sink_hpd_connect[0]);
        mculib_hdmi_tx_led_control(1, g_u8_sink_hpd_connect[1]);
#if(HDMI_TX_CHN_NUM>=4)
        mculib_hdmi_tx_led_control(2, g_u8_sink_hpd_connect[2]);
        mculib_hdmi_tx_led_control(3, g_u8_sink_hpd_connect[3]);
#endif
    }
    else
    {
        mculib_hdmi_rx_led_control(0);
        mculib_hdmi_tx_led_control(0, 0);
        mculib_hdmi_tx_led_control(1, 0);
        mculib_hdmi_tx_led_control(2, 0);
        mculib_hdmi_tx_led_control(3, 0);
    }
}

void ms933xdrv_hdmi_rx_hdcp_address_enable(BOOL b_enable)
{
#if 0
    if (b_enable)
    {
        HAL_ModBits_Ex(REG_HDCP_SETTINGS, MSRT_BITS7_1, 0x3a << 1);
    }
    else
    {
        HAL_ToggleBits_Ex(REG_HDCP_SETTINGS, MSRT_BITS7_1, TRUE);
    }
#endif
}

BOOL _convert_to_rgb_judge(UINT8 u8_tx_chn)
{
    //unsupport YUV444
    if (g_st_hdmi_timing.u8_color_space == HDMI_YCBCR444)
    {
        if ((g_st_hdmi_edid_flag[u8_tx_chn].u8_color_space & 0x20) == 0x00)
        {
            return TRUE;
        }
    }

    //unsupport YUV422
    if (g_st_hdmi_timing.u8_color_space == HDMI_YCBCR422)
    {
        if ((g_st_hdmi_edid_flag[u8_tx_chn].u8_color_space & 0x10) == 0x00)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void _param_default(void)
{
    UINT8 i;

    g_u16_input_tmds_clk = 0;
    g_u8_rx_hdcp_timer_count = 0;
    g_u8_rx_hdcp_woking_flag = 0;
    g_b_hdmi_input_valid = 0;
    g_u8_sys_edid_flag = 0xff;
    g_u8_sys_edid_buf_last_id = 0xff;
    g_u32_hdmi_sink_min_timing = 0xFFFFFFFFUL;
    g_u32_dvi_sink_min_timing = 0xFFFFFFFFUL;
    g_u8_source_5v_connect = 0;
    g_u8_hdmi_rx_hpd_off_count = HDMI_RX_HPD_OFF_TIMEOUT;
    //
    g_u8_chip_valid_check_timer_count = 0;
    //
    g_u8_hdmi_tx_hpd_detect_count = 0;
    //
    g_u8_system_error_service_flag = FALSE;
    g_u8_system_error_timer_count_1s = 0;
    g_u8_system_error_timer_count_10s = 0;
    g_u8_system_error_flag = 0;
    g_u8_system_error_action_flag = 0;
    //
    g_u8_hdmi_shell_error_timer_count = 0;
    //
    g_u8_rxpll_configed_status = 0;
    g_b_mdt_change_flag = FALSE;
    g_u16_mdt_change_timer_count = 0;
    g_u8_rx_packet_timer_count = 0;
    //
    _hdmi_tx_gcp_avmute_param_default();
    //
    memset(g_u8_sys_edid_used_buf, 0, 256);

    //buf init
    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        g_u8_sink_hpd_connect[i] = ((g_u8_tx_hpd_test_flag & g_u8_tx_chn_enable_flag) >> i) & 0x01;

        _hdmi_tx_edid_param_default(i);
        _hdmi_tx_hdcp_param_default(i);
    }
}

void ms933x_init(void)
{
    //reset chip
    mculib_chip_reset();

    //param default.
    _param_default();

    if (ms933xdrv_misc_chipisvalid())
    {
        LOG("1:2 chip connect.");
    }
    else
    {
        LOG("1:2 chip disconnect.");
    }

    ms933xdrv_hdmi_rx_controller_reset(HDMI_RX_CTRL_MAIN);
    ms933xdrv_hdmi_rx_init();

    ms933xdrv_hdmi_rx_pi_phy_init();

#if 0   //test
    HAL_WriteByte(0x102f, 0x82);
    HAL_WriteByte(0x1004, 0x41);

    HAL_WriteByte(0x0016, 0x02);
#endif

    //enable hdcp
    if (g_u8_rx_hdcp_support)
    {
        ms933xdrv_hdmi_rx_controller_hdcp_encryption_enable(TRUE);
        ms933xdrv_hdmi_rx_controller_hdcp_init((UINT8 *)u8_rx_ksv_buf, (UINT8 *)u8_rx_key_buf);
    }
    else
    {
        ms933xdrv_hdmi_rx_controller_hdcp_encryption_enable(FALSE);
    }

    sys_hotplug_init();

    LOG(MS933X_SDK_VERSION);
}

VOID sys_hdmi_rx_tmds_overload_service(VOID)
{
#if 0 //unused
    UINT8 u8Overload;
    u8Overload = ms933xdrv_hdmi_rx_get_tmds_overload();
    if (u8Overload)
    {
        LOG1("input tmds_overload = ", u8Overload);
        ms933xdrv_hdmi_rx_tmds_overload_protect_disable(TRUE);
        //delay for disable efftct.
        mculib_delay_ms(1);
        ms933xdrv_hdmi_rx_tmds_overload_protect_disable(FALSE);
    }
#endif
}

VOID sys_error_service_enable(BOOL b_enable)
{
    if (g_u8_system_error_action_flag >= 1)
    {
        g_u8_system_error_action_flag = 0;
        g_u8_system_error_service_flag = FALSE;
    }
    else
    {
        g_u8_system_error_service_flag = b_enable;
    }

    g_u8_system_error_flag = 0;
    g_u8_system_error_timer_count_1s = 0;
    g_u8_system_error_timer_count_10s = 0;
}

VOID _hdmi_tx_shell_error_service(VOID)
{
    UINT8 i;
    UINT8 u8_hdmi_tx_shell_stable_flag = 0;
    BOOL b_hdmi_tx_hpd_none_flag = TRUE;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        u8_hdmi_tx_shell_stable_flag <<= 1;
        if (g_u8_sink_hpd_connect[i])
        {
            b_hdmi_tx_hpd_none_flag = FALSE;
            //only check on shell stable
            ms933xdrv_hdmi_tx_set_channel(i);
            u8_hdmi_tx_shell_stable_flag = ms933xdrv_hdmi_tx_shell_timing_stable() ? 0x01 : 0x00;
            break;
        }
    }

    if (!b_hdmi_tx_hpd_none_flag)
    {
        if (!u8_hdmi_tx_shell_stable_flag)
        {
            g_u8_system_error_flag |= 0x04;
        }
    }
}

VOID sys_error_service(VOID)
{
    UINT8 u8_mask = 0;

    if (g_u16_input_tmds_clk < 500) //5MHz
    {
        g_u8_system_error_flag = 0;
        g_u8_system_error_timer_count_1s = 0;
        g_u8_system_error_timer_count_10s = 0;
        g_u8_hdmi_shell_error_timer_count = 0;
        return;
    }

    if (g_b_hdmi_input_valid)
    {
        g_u8_hdmi_shell_error_timer_count++;
        if (g_u8_hdmi_shell_error_timer_count >= HDMI_SHELL_ERROR_TIMEOUT)
        {
            g_u8_hdmi_shell_error_timer_count = 0;
            _hdmi_tx_shell_error_service();
        }
    }

    g_u8_system_error_timer_count_1s++;
    g_u8_system_error_timer_count_10s++;

    if (g_u8_system_error_timer_count_1s >= SYSTEM_ERROR_TIMEOUT_1S)
    {
        g_u8_system_error_timer_count_1s = 0;

        if (g_u8_system_error_flag)
        {
            LOG1("g_u8_system_flag = ", g_u8_system_error_flag);
        }

        if (g_u8_system_error_service_flag)
        {
            if (g_u8_hdmi_rxpll_error_counter_test_flag)
            {
                u8_mask |= 0x03;
            }

            if (g_u8_hdmi_tx_shell_timing_stable_test_flag)
            {
                u8_mask |= 0x04;
            }
        }

        //
        if (g_u8_system_error_flag & u8_mask)
        {
            if (g_u8_system_error_timer_count_10s >= SYSTEM_ERROR_TIMEOUT_10S)
            {
                g_u8_system_error_timer_count_10s = 0;
                g_u8_system_error_action_flag++;
                //do action
                LOG("systerm error timeout 7s, toggle hdmi rx HPD.");
                ms933xdrv_hdmi_rx_controller_hpd_set(FALSE);
                //delay for hpd
                mculib_delay_ms(200);
                ms933xdrv_hdmi_rx_controller_hpd_set(TRUE);
            }
        }
        else
        {
            g_u8_system_error_timer_count_10s = 0;
        }

        g_u8_system_error_flag = 0;
    }
}

UINT8 _rxpll_errdet_wrong_status(UINT16 u16_error_val)
{
#if 1
    UINT8 u8_flag = 0x00;

    if (!ms933xdrv_rxpll_lock_status())
    {
        //clr error only
        ms933xdrv_errdet_wrong_status(1);
        mculib_delay_ms(2);

        if (!ms933xdrv_rxpll_lock_status())
        {
            u8_flag = 0x01;
        }
        else
        {
            if (ms933xdrv_errdet_wrong_status(RXPLL_ERROR_MIN))
            {
                u8_flag = 0x04;
            }
            else
            {
                //report unlock
                g_u8_system_error_flag |= 0x01;
            }
        }
    }
    else if (ms933xdrv_errdet_wrong_status(u16_error_val))
    {
        mculib_delay_ms(2);
        if (ms933xdrv_errdet_wrong_status(RXPLL_ERROR_MID))
        {
            u8_flag = 0x02;
        }
        else
        {
            //report error
            g_u8_system_error_flag |= 0x02;
        }
    }

    return u8_flag;
#else

    UINT8 u8_flag = 0x00;

    if (!ms933xdrv_rxpll_lock_status())
    {
        u8_flag = 0x01;
    }
    else if (ms933xdrv_errdet_wrong_status(u16_error_val))
    {
        //report error
        g_u8_system_error_flag |= 0x02;
    }

    return u8_flag;
#endif
}

UINT8 _hdmi_rx_phy_config(UINT16 *p_u16_tmds_clk)
{
    UINT8 u8_times;
    UINT8 u8_flag = 0;
    UINT16 u16_clk1;
    UINT16 u16_clk2;

    u16_clk2 = (*p_u16_tmds_clk);

    //if rxpll unlock when rx config, do again
    for (u8_times = 0; u8_times < 3; u8_times++)
    {
        u8_flag &= 0xFC;
        u8_flag |= ms933xdrv_hdmi_rx_pi_phy_config(u16_clk2, 0xFF);

        u16_clk1 = ms933xdrv_hdmi_rx_get_tmds_clk();

        //20200520, if detect input clk invalid, return
        if (u16_clk1 < 500)
        {
            u8_flag |= 0x08;
            (*p_u16_tmds_clk) = u16_clk1;
            break;
        }

        if (u16_clk1 >= 500 && _abs(u16_clk1 - u16_clk2) >= 500) //5MHz
        {
            u16_clk2 = u16_clk1;
            (*p_u16_tmds_clk) = u16_clk1;

            u8_flag |= 0x04;
        }
        else
        {
            if (u8_flag & 0x01)
            {
                break;
            }
        }
    }

    return u8_flag | (u8_times << 4);
}

BOOL sys_hdmi_rx_pll_service(void)
{
    BOOL b_action = FALSE;
    UINT8 u8_rxphy_error = 0x00;
    BOOL b_hdmi_rx_pll_configed = FALSE;
    UINT8 u8_rxphy_flag = 0;

    UINT16 u16_clk = ms933xdrv_hdmi_rx_get_tmds_clk();
    //LOG2("u16_clk = ", u16_clk);

    if (_abs(u16_clk - g_u16_input_tmds_clk) >= TMDS_CLK_MARGIN)
    {
        b_action = TRUE;
        if (u16_clk >= 500)
        {
            sys_error_service_enable(TRUE);
        }
    }

    if (u16_clk >= 500 && !b_action)
    {
        u8_rxphy_error = _rxpll_errdet_wrong_status(RXPLL_ERROR_MAX);
        b_action = u8_rxphy_error ? TRUE : FALSE;
    }

    if (b_action)
    {
        //when input timing change or lost, first should shutdown output.
        sys_shutdown_output();

        if (u16_clk >= 500)
        {
            b_hdmi_rx_pll_configed = TRUE;

            //config hdmi rx PLL
            //20190711, disable hdcp source start.
            ms933xdrv_hdmi_rx_hdcp_detect_enable(FALSE);
            u8_rxphy_flag = _hdmi_rx_phy_config(&u16_clk);
            ms933xdrv_hdmi_rx_hdcp_detect_enable(TRUE);

            //mv to mdt process
            //20181227, clr packet status.bacause pll config maybe cause error.
            //ms933xdrv_hdmi_rx_controller_reset(HDMI_RX_CTRL_PDEC);
            //if rxpll lock, delay for MDT stable
            //if (u8_rxphy_flag & 0x01)
            //{
            //mculib_delay_ms(100);
            //}
        }

        //if have known input without HDCP, suggest use #if 1
        //because if input with HDCP, the HDMI reset maybe reset HDCP handshake

        #if 0
        if (u16_clk < 500  || (u8_rxphy_flag & 0x01) == 0x00)
        #else
        if (u16_clk < 500)
        #endif
        {
            //20190116, enhace, reset HDMI before rxpll clk disable.
            ms933xdrv_hdmi_rx_controller_reset(HDMI_RX_CTRL_HDMI);
            ms933xdrv_hdmi_rx_phy_power_down();
            ms933xdrv_hdmi_rx_pi_pll_release(FALSE);
            g_u8_rxpll_configed_status = 0;
        }
        else
        {
            g_u8_rxpll_configed_status = 1;
        }

        #if 0
        if (u8_rxphy_flag & 0x01)
        {
            g_u16_input_tmds_clk = u16_clk;
        }
        else
        {
            g_u16_input_tmds_clk = 0;
        }
        #else
        g_u16_input_tmds_clk = u16_clk;
        #endif

        //LOG2("u16_clk = ", u16_clk);// CANCLE KYQ

        if (u8_rxphy_error)
        {
           // LOG1("u8_rxphy_error = ", u8_rxphy_error);
        }

        if (u8_rxphy_flag)
        {
           // LOG1("u8_rxphy_flag = ", u8_rxphy_flag); // CANCEL KYQ
        }

        //reset MDT
        g_b_hdmi_input_valid = 0;
        //enhance, reset service timer
        g_b_mdt_change_flag = FALSE;
        g_u16_mdt_change_timer_count = 0;
        g_u8_rx_hdcp_timer_count = 0;
        g_u8_rx_hdcp_woking_flag = 0;
    }

    return b_hdmi_rx_pll_configed;
}

#define AUDIO_RATE_192K_MIN ((176400UL / 100) + ( (192000UL / 100) - (176400UL / 100) ) / 2)
#define AUDIO_RATE_176K_MIN ((96000UL  / 100) + ( (176400UL / 100) - (96000UL  / 100) ) / 2)
#define AUDIO_RATE_96K_MIN  ((88200UL  / 100) + ( (96000UL  / 100) - (88200UL  / 100) ) / 2)
#define AUDIO_RATE_88K2_MIN ((48000UL  / 100) + ( (88200UL  / 100) - (48000UL  / 100) ) / 2)
#define AUDIO_RATE_48K_MIN  ((44100UL  / 100) + ( (48000UL  / 100) - (44100UL  / 100) ) / 2)
#define AUDIO_RATE_44K1_MIN ((32000UL  / 100) + ( (44100UL  / 100) - (32000UL  / 100) ) / 2)
#define AUDIO_RATE_32K_MIN  ((32000UL  / 100) / 2)

UINT8 _hdmi_rx_audio_fs_type_get(UINT16 u16_fs)
{
    UINT8 u8_audio_sample_rate = HDMI_AUD_RATE_48K;

    if (u16_fs > AUDIO_RATE_192K_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_192K;
    }
    else if (u16_fs > AUDIO_RATE_176K_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_176K4;
    }
    else if (u16_fs > AUDIO_RATE_96K_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_96K;
    }
    else if (u16_fs > AUDIO_RATE_88K2_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_88K2;
    }
    else if (u16_fs > AUDIO_RATE_48K_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_48K;
    }
    else if (u16_fs > AUDIO_RATE_44K1_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_44K1;
    }
    else if (u16_fs > AUDIO_RATE_32K_MIN)
    {
        u8_audio_sample_rate = HDMI_AUD_RATE_32K;
    }

    return u8_audio_sample_rate;
}

//audio is PCM or AC
void sys_audio_i2s_mclk_pad_ctrl_service(BOOL b_auto)
{
    static BOOL s_b_audio_sample = TRUE;
    BOOL b_flag;

    if (!b_auto)
    {
        s_b_audio_sample = TRUE;
        ms933xdrv_misc_audio_i2s_mclk_pad_enable(TRUE);
        return;
    }

    b_flag = ms933xdrv_hdmi_tx_shell_get_audio_cbyte_status() ? FALSE : TRUE;

    if (s_b_audio_sample != b_flag)
    {
        s_b_audio_sample = b_flag;
        //disable i2s mclk output
        ms933xdrv_misc_audio_i2s_mclk_pad_enable(b_flag);
        if (!b_flag)
        {
            LOG("audio_type_unpcm.");
        }
    }
}

void sys_hdmi_rx_audio_config(BOOL b_config_all)
{
    UINT16 u16_fs;
    BOOL b_audio_chn_auto_config;

    //20191123, resolve sanxing DVD, speaker_locations is 0x0A, but chn number is 8 chn issue.
    b_audio_chn_auto_config = ms933xdrv_hdmi_rx_controller_audio_channel_config(g_st_hdmi_timing.u8_audio_channels, g_st_hdmi_timing.u8_audio_speaker_locations);

    if (g_u8_hdmi_in_timing_log_enable)
    {
        if (!b_audio_chn_auto_config)
        {
            LOG("hdmi_rx_aud_chn_manual.");
        }
    }

    ms933xdrv_hdmi_rx_audio_fifo_reset(TRUE);

    if (b_config_all)
    {
        u16_fs = ms933xdrv_hdmi_rx_audio_config(RX_AUDIO_DLL_TO_PLL_MODE, g_u16_input_tmds_clk);
        #if 0
        g_st_hdmi_timing.u8_audio_rate = _hdmi_rx_audio_fs_type_get(u16_fs);
        #endif
        LOG2("u16_audio_fs = ", u16_fs);

        #if SYS_AUDIO_PAD_OUTPUT_ENABLE
        //if (u16_fs > 0)
        {
            //176.4K, 192K, use 64fs
            ms933xdrv_misc_audio_i2s_mclk_div((u16_fs > 1360) ? 1 : 0);

            //fixed enable mclk pad.
            sys_audio_i2s_mclk_pad_ctrl_service(FALSE);
            ms933xdrv_misc_audio_out_pad_enable(TRUE);
        }
        #endif
    }

    mculib_delay_ms(1);
    ms933xdrv_hdmi_rx_audio_fifo_reset(FALSE);
}

UINT8 sys_hdmi_rx_audio_service(UINT8 u8_int_sts)
{
    UINT8 u8_audio_fifo_status;
    UINT8 u8_audio_status = 0;
    //BOOL b_acr_rcv_flag;
    BOOL b_acr_change_flag;

    //1. acr status process
    //b_acr_rcv_flag = (u8_int_sts & 0x08) ? TRUE : FALSE;
    b_acr_change_flag = (u8_int_sts & 0x10) ? TRUE : FALSE;

    if (b_acr_change_flag)
    {
        sys_hdmi_rx_audio_config(TRUE);
        return 1;
    }

    //2. audio fifo status process
    u8_audio_fifo_status = ms933xdrv_hdmi_rx_get_audio_fifo_status();

    if (u8_audio_fifo_status & AFIF_THS_PASS_STS)
    {
        u8_audio_status = (u8_audio_fifo_status & AFIF_UNDERFL_STS) ? 0x01 : 0x00;
        u8_audio_status |= (u8_audio_fifo_status & AFIF_OVERFL_STS) ? 0x02 : 0x00;

        #if SYS_AUDIO_PAD_OUTPUT_ENABLE
        sys_audio_i2s_mclk_pad_ctrl_service(TRUE);
        #endif

        if (u8_audio_status)
        {
            LOG("audio_fifo_reset.");
            sys_hdmi_rx_audio_config(FALSE);
            return 2;
        }
    }

    return 0;
}

VOID sys_hdmi_rx_video_fifo_reset(VOID)
{
    ms933xdrv_hdmi_rx_video_fifo_reset(TRUE);
    mculib_delay_us(100);
    ms933xdrv_hdmi_rx_video_fifo_reset(FALSE);
}

void printf_input_timing(void)
{
#if 1
    UINT16 u16detHFreq;

    if (!g_u8_hdmi_in_timing_log_enable)
    {
        return;
    }

    LOG2("Is Progr      = ", (g_st_hdmi_in_timing.u8_polarity & MSRT_BIT0) ? 1 : 0);
    LOG2("Hsync Pol     = ", (g_st_hdmi_in_timing.u8_polarity & MSRT_BIT1) ? 1 : 0);
    LOG2("Vsync Pol     = ", (g_st_hdmi_in_timing.u8_polarity & MSRT_BIT2) ? 1 : 0);
    LOG2("u16detHTotal  = ", g_st_hdmi_in_timing.u16_htotal);
    LOG2("u16detVTotal  = ", g_st_hdmi_in_timing.u16_vtotal);
    LOG2("u16detHActive = ", g_st_hdmi_in_timing.u16_hactive);
    LOG2("u16detVActive = ", g_st_hdmi_in_timing.u16_vactive);
    LOG2("u16detHst     = ", g_st_hdmi_in_timing.u16_hoffset);
    LOG2("u16detVst     = ", g_st_hdmi_in_timing.u16_voffset);
    LOG2("u16detHW      = ", g_st_hdmi_in_timing.u16_hsyncwidth);
    LOG2("u16detVW      = ", g_st_hdmi_in_timing.u16_vsyncwidth);

    LOG2("u16detPclk    = ", g_st_hdmi_in_timing.u16_pixclk);
    //HSyncFreq / 10
    u16detHFreq = (UINT32)g_st_hdmi_in_timing.u16_vfreq * g_st_hdmi_in_timing.u16_vtotal / 100 / 10;
    if ((g_st_hdmi_in_timing.u8_polarity & MSRT_BIT0) == 0) u16detHFreq >>= 1;
    LOG2("u16detHFreq   = ", u16detHFreq);
    LOG2("u16detVFreq   = ", g_st_hdmi_in_timing.u16_vfreq);
#endif
}

void printf_input_video_config(void)
{
#if 1
    LOG(" ");
    //LOG2("u8_hdmi_flag      = ", g_st_hdmi_timing.u8_hdmi_flag);
    LOG2("u8_vic            = ", g_st_hdmi_timing.u8_vic);
    LOG2("u16_video_clk     = ", g_st_hdmi_timing.u16_video_clk);
    LOG2("u8_clk_rpt        = ", g_st_hdmi_timing.u8_clk_rpt);
    LOG2("u8_scan_info      = ", g_st_hdmi_timing.u8_scan_info);
    LOG2("u8_aspect_ratio   = ", g_st_hdmi_timing.u8_aspect_ratio);
    LOG2("u8_color_space    = ", g_st_hdmi_timing.u8_color_space);
    LOG2("u8_color_depth    = ", g_st_hdmi_timing.u8_color_depth);
    LOG2("u8_colorimetry    = ", g_st_hdmi_timing.u8_colorimetry);
    LOG(" ");
#endif
}

void printf_input_vendor_specific_config(void)
{
#if 1
    LOG(" ");
    LOG2("u8_video_format   = ", g_st_hdmi_timing.u8_video_format);
    if (g_st_hdmi_timing.u8_video_format == HDMI_4Kx2K_FORMAT)
    {
        LOG2("u8_4Kx2K_vic      = ", g_st_hdmi_timing.u8_4Kx2K_vic);
    }
    else if (g_st_hdmi_timing.u8_video_format == HDMI_3D_FORMAT)
    {
        LOG2("u8_3D_structure   = ", g_st_hdmi_timing.u8_3D_structure);
    }
    LOG(" ");
#endif
}

void printf_input_audio_config(void)
{
#if 1
    LOG(" ");
    LOG2("u8_audio_mode     = ", g_st_hdmi_timing.u8_audio_mode);
    LOG1("u8_audio_rate     = ", g_st_hdmi_timing.u8_audio_rate);
    LOG2("u8_audio_bits     = ", g_st_hdmi_timing.u8_audio_bits);
    LOG2("u8_audio_channels = ", g_st_hdmi_timing.u8_audio_channels);
    LOG2("u8_spea_locations = ", g_st_hdmi_timing.u8_audio_speaker_locations);
    LOG(" ");
#endif
}

void printf_input_av_config(UINT8 u8_int_sts)
{
    if (!g_u8_hdmi_in_timing_log_enable)
    {
        return;
    }

    LOG(" ");
    LOG2("u8_hdmi_flag      = ", g_st_hdmi_timing.u8_hdmi_flag);

    if ((u8_int_sts & 0x01) == 0x01)
    {
        printf_input_video_config();
    }

    if ((u8_int_sts & 0x04) == 0x04)
    {
        printf_input_vendor_specific_config();
    }

    if ((u8_int_sts & 0x02) == 0x02)
    {
        printf_input_audio_config();
    }
}


void sys_default_hdmi_video_config(void)
{
    //g_st_hdmi_timing.u8_hdmi_flag = TRUE;
    g_st_hdmi_timing.u8_vic = 0;
    //g_st_hdmi_timing.u16_video_clk = 7425;
    g_st_hdmi_timing.u8_clk_rpt = HDMI_X1CLK;
    g_st_hdmi_timing.u8_scan_info = HDMI_OVERSCAN;
    g_st_hdmi_timing.u8_aspect_ratio = HDMI_16X9;
    g_st_hdmi_timing.u8_color_space = HDMI_RGB;
    g_st_hdmi_timing.u8_color_depth = HDMI_COLOR_DEPTH_8BIT;
    g_st_hdmi_timing.u8_colorimetry = HDMI_COLORIMETRY_709;
}

void sys_default_hdmi_vendor_specific_config(void)
{
    g_st_hdmi_timing.u8_video_format = HDMI_NO_ADD_FORMAT;
    g_st_hdmi_timing.u8_4Kx2K_vic = HDMI_4Kx2K_30HZ;
    g_st_hdmi_timing.u8_3D_structure = HDMI_FRAME_PACKING;
}

void sys_default_hdmi_audio_config(void)
{
    g_st_hdmi_timing.u8_audio_mode = HDMI_AUD_MODE_AUDIO_SAMPLE;
    g_st_hdmi_timing.u8_audio_rate = HDMI_AUD_RATE_48K;
    g_st_hdmi_timing.u8_audio_bits = HDMI_AUD_LENGTH_16BITS;
    g_st_hdmi_timing.u8_audio_channels = HDMI_AUD_2CH;
    g_st_hdmi_timing.u8_audio_speaker_locations = 0;
}

//BIT7: change flag, BIT0: valid flag
UINT8 _hotplug_process(UINT8 u8_output_chn)
{
    UINT8 u8_edid_parse_flag;
    UINT8 u8_flag = 0;
    UINT8 u8_sink_hpd_connect;
    UINT16 u16_pixel_clk;

    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);

    u8_sink_hpd_connect = ms933xdrv_hdmi_tx_shell_hpd() ? 0x01 : 0x00;
    u8_sink_hpd_connect |= (g_u8_tx_hpd_test_flag >> u8_output_chn) & 0x01;
    u8_sink_hpd_connect &= (g_u8_tx_chn_enable_flag >> u8_output_chn);

    ///mculib_led_control(LED_PIN_TX(u8_output_chn), u8_sink_hpd_connect);

    if (g_u8_sink_hpd_connect[u8_output_chn] != u8_sink_hpd_connect)
    {
        u8_flag |= 0x80;
        g_u8_sink_hpd_connect[u8_output_chn] = u8_sink_hpd_connect;

        if (u8_sink_hpd_connect)
        {
            LOG2("sink_hpd_connect_chn = ", u8_output_chn);
            u8_edid_parse_flag = ms933xdrv_hdmi_tx_parse_edid(g_u8_sys_edid_buf, &g_st_hdmi_edid_flag[u8_output_chn]);
            //20190725, double check for EDID read.
            if (!u8_edid_parse_flag)
            {
                mculib_delay_ms(50);
                LOG("EDID read again.");
                u8_edid_parse_flag = ms933xdrv_hdmi_tx_parse_edid(g_u8_sys_edid_buf, &g_st_hdmi_edid_flag[u8_output_chn]);
            }
            g_u8_sys_edid_buf_last_id = u8_output_chn;

            if (u8_edid_parse_flag)
            {
                //if u32_preferred_timing is 4K30, judge Compliance with standards
                if (g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing == (3840UL * 2160) && g_st_hdmi_edid_flag[u8_output_chn].u16_preferred_pixel_clk < 45000)
                {
                    u16_pixel_clk = g_st_hdmi_edid_flag[u8_output_chn].u16_preferred_pixel_clk / 100;
                    if (_abs(297 - u16_pixel_clk) >= 10)
                    {
                        g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing = g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing + 297 - u16_pixel_clk;
                        //LOG("4K non-standard.");
                    }
                }

                //20181129, video block timing and HDMI VSDB timing 4K. compare to preferred_timing
                if (g_st_hdmi_edid_flag[u8_output_chn].u32_max_video_block_timing != 0)
                {
                    if (g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing < g_st_hdmi_edid_flag[u8_output_chn].u32_max_video_block_timing)
                    {
                        //LOG("fixed to video_block_timing.");
                        g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing = g_st_hdmi_edid_flag[u8_output_chn].u32_max_video_block_timing;
                    }
                }
            }

            if (g_b_hdmi_input_valid)
            {
                _config_output(u8_output_chn);
            }
        }
        else
        {
            LOG2("sink_hpd_disconnect_chn = ", u8_output_chn);
            //clear edid data
            _hdmi_tx_edid_param_default(u8_output_chn);
            //
            _shutdown_output(u8_output_chn);
        }
    }

    if (u8_sink_hpd_connect)
    {
        u8_flag |= 0x01;
    }

    return u8_flag;
}

// Data Block Tag Codes
#define AUDIO_D_BLOCK                   (0x01)
#define VIDEO_D_BLOCK                   (0x02)
#define VENDOR_SPEC_D_BLOCK             (0x03)
#define SPKR_ALLOC_D_BLOCK              (0x04)
#define VESA_DTC_D_BLOCK                (0x05)
#define USE_EXTENDED_TAG                (0x07)

// Extended Data Block Tag Codes
#define VIDEO_CAPABILITY_D_BLOCK        (0x00)
#define VENDOR_SPEC_VIDEO_D_BLOCK       (0x01)
#define COLORIMETRY_D_BLOCK             (0x05)
#define YUV420_VIDEO_D_BLOCK            (0x0e)
#define YUV420_CAPABILITY_D_BLOCK       (0x0f)
#define CEA_MISC_AUDIO_FIELDS           (0x10)
#define VENDOR_SPEC_AUDIO_D_BLOCK       (0x11)

#if 0
#define EDID_DVI2HDMI
#endif
#define EDID_4K60_SUPPORT

BOOL sys_edid_convert(UINT8 *pu8_buf, HDMI_EDID_FLAG_T st_hdmi_edid_flag)
{
    UINT8 i, j;
    UINT8 u8DetailedOffset;
    UINT8 u8TotNum;
    UINT16 u16TPixClk;
    UINT8 u8BlockOffset;
    UINT8 u8ChangeOffset;
    UINT8 u8BlockTag;
    UINT8 u8BlockLength;
    BOOL bHdmi2_0Block;
    UINT8 u8Hdmi2_0Timing[6][2] = {
        { 96, FALSE }, { 97, FALSE }, { 101, FALSE }, { 102, FALSE }, { 106, FALSE }, { 107, FALSE } };
    UINT8 u8Yuv420TimingNum = 0;

#ifdef EDID_DVI2HDMI
    //change DVI EDID to HDMI EDID
    if (!st_hdmi_edid_flag.u8_hdmi_sink)
    {
        pu8_buf[0x7e] = 0x01;
        pu8_buf[0x7f] = 0;
        for (i = 0x00; i < 0x7f; i++)
        {
            pu8_buf[0x7f] -= pu8_buf[i];
        }

        memset(&pu8_buf[0x80], 0x00, 0x80);
        pu8_buf[0x80] = 0x02;
        pu8_buf[0x81] = 0x03;
        pu8_buf[0x82] = 0x04;
        pu8_buf[0x83] = 0x40;   //basic audio support
        u8BlockTag = VENDOR_SPEC_D_BLOCK;
        u8BlockLength = 5;
        pu8_buf[0x82] += u8BlockLength + 1;
        pu8_buf[0x84] = (u8BlockTag << 5) | u8BlockLength;
        pu8_buf[0x85] = 0x03;
        pu8_buf[0x86] = 0x0c;
        pu8_buf[0x87] = 0x00;
        pu8_buf[0x88] = 0x00;
        pu8_buf[0x89] = 0x00;

        pu8_buf[0xff] = 0;
        for (i = 0x80; i < 0xff; i++)
        {
            pu8_buf[0xff] -= pu8_buf[i];
        }
        //memcpy(&pu8_buf[0x80], &u8_sys_edid_default_buf[0x80], 128);
    }
#endif

    //chenge HDMI2.0 EDID to HDMI1.4 EDID
    if (st_hdmi_edid_flag.u8_hdmi_2_0_flag)
    {
        //block 0
        u8DetailedOffset = 0x36;
        u8TotNum = 4;
        u8BlockLength = 18;
        for (i = 0; i < u8TotNum; i++)
        {
            u16TPixClk = pu8_buf[u8DetailedOffset] + 256 * pu8_buf[u8DetailedOffset + 1];
            if (u16TPixClk > 34000)
            {
                u16TPixClk /= 2;
                pu8_buf[u8DetailedOffset] = u16TPixClk & 0xff;
                pu8_buf[u8DetailedOffset + 1] = u16TPixClk >> 8;
            }
            u8DetailedOffset += u8BlockLength;
        }

        pu8_buf[0x7f] = 0;
        for (i = 0x00; i < 0x7f; i++)
        {
            pu8_buf[0x7f] -= pu8_buf[i];
        }

        //block 1
        u8DetailedOffset = 0x80 + pu8_buf[0x82];
        u8BlockOffset = 0x84;
        u8ChangeOffset = u8BlockOffset;

        while (u8BlockOffset < u8DetailedOffset)
        {
            u8BlockTag = (pu8_buf[u8BlockOffset] & MSRT_BITS7_5) >> 5;
            u8BlockLength = pu8_buf[u8BlockOffset] & MSRT_BITS4_0;

            switch ( u8BlockTag )
            {
                case AUDIO_D_BLOCK:
                    bHdmi2_0Block = FALSE;
                    break;
                case VIDEO_D_BLOCK:
                    bHdmi2_0Block = FALSE;
                    for (i = u8BlockOffset + 1; i <= u8BlockOffset + u8BlockLength; i++)
                    {
                        if (((pu8_buf[i] & MSRT_BITS6_0) > 64) && (pu8_buf[i] > 95))
                        {
                            for (j = 0; j < sizeof(u8Hdmi2_0Timing) / sizeof(u8Hdmi2_0Timing[0]); j++)
                            {
                                if (pu8_buf[i] == u8Hdmi2_0Timing[j][0])
                                {
                                    bHdmi2_0Block = TRUE;
                                    pu8_buf[i] = 0;
                                    u8Hdmi2_0Timing[j][1] = TRUE;
                                    u8Yuv420TimingNum++;
                                    break;
                                }
                            }
                        }
                    }
                    if (bHdmi2_0Block)
                    {
                        j = u8ChangeOffset;
                        for (i = u8BlockOffset; i <= u8BlockOffset + u8BlockLength; i++)
                        {
                            if (pu8_buf[i] != 0)
                            {
                                pu8_buf[u8ChangeOffset++] = pu8_buf[i];
                            }
                        }
                        pu8_buf[j] = (pu8_buf[j] & MSRT_BITS7_5) | (u8ChangeOffset - j - 1);
                    }
                    break;
                case SPKR_ALLOC_D_BLOCK:
                    bHdmi2_0Block = FALSE;
                    break;
                case VENDOR_SPEC_D_BLOCK:
                    if ((pu8_buf[u8BlockOffset + 1] == 0x03) && (pu8_buf[u8BlockOffset + 2] == 0x0C) && (pu8_buf[u8BlockOffset + 3] == 0x00))
                    {
                        bHdmi2_0Block = FALSE;
                    }
                    else
                    {
                        bHdmi2_0Block = TRUE;
                    }
                    break;
                case VESA_DTC_D_BLOCK:
                    bHdmi2_0Block = FALSE;
                    break;
                case USE_EXTENDED_TAG:
                    switch ( pu8_buf[u8BlockOffset + 1] )
                    {
                        case VIDEO_CAPABILITY_D_BLOCK:
                        case VENDOR_SPEC_VIDEO_D_BLOCK:
                        case COLORIMETRY_D_BLOCK:
#ifdef EDID_4K60_SUPPORT
                        case YUV420_VIDEO_D_BLOCK:
#endif
                            //case YUV420_CAPABILITY_D_BLOCK:
                        case CEA_MISC_AUDIO_FIELDS:
                        case VENDOR_SPEC_AUDIO_D_BLOCK:
                            bHdmi2_0Block = FALSE;
                            break;
                        default:
                            bHdmi2_0Block = TRUE;
                    }
                    break;
                default:
                    bHdmi2_0Block = TRUE;
            }

            if (!bHdmi2_0Block)
            {
                if (u8BlockOffset != u8ChangeOffset)
                {
                    memcpy(&pu8_buf[u8ChangeOffset], &pu8_buf[u8BlockOffset], u8BlockLength + 1);
                }
                u8ChangeOffset += u8BlockLength + 1;
            }
            u8BlockOffset += u8BlockLength + 1;
        }

#ifdef EDID_4K60_SUPPORT
        if (u8Yuv420TimingNum)
        {
            u8BlockTag = USE_EXTENDED_TAG;
            u8BlockLength = u8Yuv420TimingNum + 1;
            pu8_buf[u8ChangeOffset++] = (u8BlockTag << 5) | u8BlockLength;
            pu8_buf[u8ChangeOffset++] = YUV420_VIDEO_D_BLOCK;

            for (i = 0; i < sizeof(u8Hdmi2_0Timing) / sizeof(u8Hdmi2_0Timing[0]); i++)
            {
                if (u8Hdmi2_0Timing[i][1])
                {
                    pu8_buf[u8ChangeOffset++] = u8Hdmi2_0Timing[i][0];
                }
            }
        }
#endif

        if (u8DetailedOffset != 0x80)
        {
            u8TotNum = 6;
            u8BlockLength = 18;
            for (i = 0; i < u8TotNum; i++)
            {
                if ((pu8_buf[u8DetailedOffset] == 0) || (u8DetailedOffset == 0xff))
                {
                    break;
                }
                u16TPixClk = pu8_buf[u8DetailedOffset] + 256 * pu8_buf[u8DetailedOffset + 1];
                if (u16TPixClk > 34000)
                {
                    u16TPixClk /= 2;
                    pu8_buf[u8DetailedOffset] = u16TPixClk & 0xff;
                    pu8_buf[u8DetailedOffset + 1] = u16TPixClk >> 8;
                }
                u8DetailedOffset += u8BlockLength;
            }
            u8DetailedOffset = 0x80 + pu8_buf[0x82];
            if (u8ChangeOffset != u8DetailedOffset)
            {
                pu8_buf[0x82] = u8ChangeOffset - 0x80;
                memcpy(&pu8_buf[u8ChangeOffset], &pu8_buf[u8DetailedOffset], i * u8BlockLength);
            }
            u8ChangeOffset += i * u8BlockLength;
        }

        memset(&pu8_buf[u8ChangeOffset], 0, 0x100 - u8ChangeOffset);
        for (i = 0x80; i < 0xff; i++)
        {
            pu8_buf[0xff] -= pu8_buf[i];
        }
    }

    //change EDID support color sapce
    if ((pu8_buf[0x83] & MSRT_BITS5_4) != st_hdmi_edid_flag.u8_color_space)
    {
        pu8_buf[0x83] &= ~MSRT_BITS5_4;
        pu8_buf[0x83] |= st_hdmi_edid_flag.u8_color_space & MSRT_BITS5_4;
        pu8_buf[0xff] = 0;
        for (i = 0x80; i < 0xff; i++)
        {
            pu8_buf[0xff] -= pu8_buf[i];
        }
    }

    return TRUE;
}

void _hdmi_tx_edid_param_default(UINT8 u8_output_chn)
{
    g_st_hdmi_edid_flag[u8_output_chn].u8_hdmi_sink = TRUE; //default HDMI sink
    g_st_hdmi_edid_flag[u8_output_chn].u8_color_space = 0; // only support RGB
    g_st_hdmi_edid_flag[u8_output_chn].u8_edid_total_blocks = 0;
    //
    g_st_hdmi_edid_flag[u8_output_chn].u16_preferred_pixel_clk = 0xFFFF;
    g_st_hdmi_edid_flag[u8_output_chn].u32_preferred_timing = 0xFFFFFFFFUL;
    g_st_hdmi_edid_flag[u8_output_chn].u8_max_tmds_clk = 0;
    g_st_hdmi_edid_flag[u8_output_chn].u32_max_video_block_timing = 0;
    g_st_hdmi_edid_flag[u8_output_chn].u8_hdmi_2_0_flag = 0;
}

UINT8 _mask_hpd_status(UINT8 u8_id)
{
    if (u8_id == 0)
    {
        return 0x08;
    }
    else if (u8_id == 1)
    {
        return 0x04;
    }
    else if (u8_id == 2)
    {
        return 0x02;
    }
    else if (u8_id == 3)
    {
        return 0x01;
    }
    else
    {
        return 0x00;
    }
}

VOID _edid_file_save(BOOL b_source)
{
#if DEBUG_EDID_FILE_SAVE
    // TODO: Add your control notification handler code here
    CString filename;
    FILE *fp = NULL;

    filename = b_source ? "source.bin" : "target.bin";
    size_t origsize = wcslen(filename) + 1;
    size_t convertedChars = 0;
    const size_t newsize = origsize * 2;
    char *nstring = new char[newsize];
    wcstombs_s(&convertedChars, nstring, newsize, filename, _TRUNCATE);
    fopen_s(&fp, nstring, "wb+");

    if (fp)
    {
        fwrite(&g_u8_sys_edid_buf, 1, 256, fp);
        fclose(fp);
    }
#endif
}

BOOL _check_hdmi_sink_exist(VOID)
{
    UINT8 i;
    UINT8 u8_mask;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        u8_mask = (g_u8_tx_chn_enable_flag >> i) & 0x01;
        if (u8_mask && g_u8_sink_hpd_connect[i])
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL _check_hdmi_sink(UINT8 u8_valid_flag)
{
    UINT8 i;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        if (u8_valid_flag & 0x01)
        {
            if (g_st_hdmi_edid_flag[(HDMI_TX_CHN_NUM - 1) - i].u8_hdmi_sink)
            {
                return TRUE;
            }
        }

        u8_valid_flag >>= 1;
    }

    return FALSE;
}

UINT8 _tx_edid_read_again(UINT8 u8_id)
{
    LOG1("tx EDID read again, channel = ", u8_id);
    ms933xdrv_hdmi_tx_set_channel(u8_id);
    if (!ms933xdrv_hdmi_tx_parse_edid(g_u8_sys_edid_buf, &g_st_hdmi_edid_flag[HDMI_TX_DEFAULT_EDID_ID]))
    {
        LOG("tx EDID read unstable.");
        u8_id = HDMI_TX_DEFAULT_EDID_ID;
        memcpy(g_u8_sys_edid_buf, u8_sys_edid_default_buf, 256);
    }

    return u8_id;
}

UINT8 _hotplug_service(void)
{
    UINT8 u8_edid_change = 0; //bit7: rx HPD toggle flag; bit0: EDID update flag
    UINT8 u8_change_flag = 0;
    UINT8 u8_valid_flag = 0;
    UINT8 u8_data;
    UINT8 u8_id = 0;
    UINT8 b_check = FALSE;
    UINT32 u32_temp;
    UINT32 u32_min_per_timing = 0xFFFFFFFFUL;
    #if 0
    UINT8 u8_used_color_space = 0x30;
    #endif
    UINT8 i;
    UINT8 u8_source_5v_connect;

    u8_source_5v_connect = ms933xdrv_hdmi_rx_5v_det();
    if (g_u8_source_5v_connect != u8_source_5v_connect)
    {
        LOG1("u8_source_5v_connect = ", u8_source_5v_connect);
        ///mculib_led_control(LED_PIN_RX, u8_source_5v_connect);
        g_u8_source_5v_connect = u8_source_5v_connect;
        if (!g_u8_rx_hpd_test_flag)
        {
            ms933xdrv_hdmi_rx_controller_hpd_set(u8_source_5v_connect && _check_hdmi_sink_exist());
        }
    }

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        u8_data = _hotplug_process(i);

        u8_valid_flag <<= 1;
        u8_valid_flag |= (u8_data & 0x01);

        u8_change_flag <<= 1;
        u8_change_flag |= ((u8_data & 0x80) >> 7);

        u32_temp = g_st_hdmi_edid_flag[i].u32_preferred_timing;
        if (u32_temp < u32_min_per_timing)
        {
            u8_id = i;
            u32_min_per_timing = u32_temp;
        }
        #if 0
        if (u8_data & 0x01)
        {
            u8_used_color_space &= g_st_hdmi_edid_flag[i].u8_color_space;
        }
        #endif
    }

    if (g_u8_rx_hpd_test_flag)
    {
        goto _JUMP_EDID_DEFALUT;
    }

    if (u8_change_flag)
    {
        if (u8_valid_flag)
        {
            //if pre hpd false
            if (g_u8_sys_edid_flag == 0xff)
            {
                u8_edid_change = 0x80;
            }

            //invalid edid or no edid, fixed use default EDID
            if (u32_min_per_timing == 0xFFFFFFFFUL)
            {
                g_u32_hdmi_sink_min_timing = 0xFFFFFFFFUL;
                g_u32_dvi_sink_min_timing = 0xFFFFFFFFUL;
                b_check = TRUE;
                u8_id = HDMI_TX_DEFAULT_EDID_ID;
                memcpy(g_u8_sys_edid_buf, u8_sys_edid_default_buf, 256);
            }
            else if (!_check_hdmi_sink(u8_valid_flag)) //all sink is dvi.
            {
                g_u32_hdmi_sink_min_timing = 0xFFFFFFFFUL;

                if (u32_min_per_timing < g_u32_dvi_sink_min_timing)
                {
                    g_u32_dvi_sink_min_timing = u32_min_per_timing;
                    b_check = TRUE;

                    //if selected EDID(g_u8_sys_edid_buf) is not the last be readed
                    if (g_u8_sys_edid_buf_last_id != u8_id)
                    {
                        u8_id = _tx_edid_read_again(u8_id);
                    }
                }
            }
            else if (u32_min_per_timing < g_u32_hdmi_sink_min_timing)
            {
                g_u32_dvi_sink_min_timing = 0xFFFFFFFFUL;
                g_u32_hdmi_sink_min_timing = u32_min_per_timing;
                b_check = TRUE;

                //if min_timing is dvi sink and hdmi sink exist, fixed use default EDID
                if (!g_st_hdmi_edid_flag[u8_id].u8_hdmi_sink)
                {
                    u8_id = HDMI_TX_DEFAULT_EDID_ID;
                    memcpy(g_u8_sys_edid_buf, u8_sys_edid_default_buf, 256);
                }
                //if selected EDID(g_u8_sys_edid_buf) is not the last be readed
                else if (g_u8_sys_edid_buf_last_id != u8_id)
                {
                    u8_id = _tx_edid_read_again(u8_id);
                }
            }

            if (b_check)
            {
                g_u8_sys_edid_buf_last_id = u8_id;
                g_u8_sys_edid_flag = u8_id;

                if (memcmp(g_u8_sys_edid_used_buf, g_u8_sys_edid_buf, 256) != 0)
                {
                    u8_edid_change = 0x81;
                    memcpy(g_u8_sys_edid_used_buf, g_u8_sys_edid_buf, 256);
                }
            }

            LOG1("u8_edid_change = ", u8_edid_change);

            if (u8_edid_change)
            {
                if (u8_edid_change & 0x01)
                {
                    LOG1("hdmi_rx_edid_id = ", u8_id);

                    ms933xdrv_hdmi_rx_controller_hpd_set(FALSE);

                    _edid_file_save(1);
                    g_st_hdmi_edid_flag[HDMI_TX_DEFAULT_EDID_ID] = g_st_hdmi_edid_flag[u8_id];
                    #if 1
                    g_st_hdmi_edid_flag[HDMI_TX_DEFAULT_EDID_ID].u8_color_space = 0; //only support RGB
                    #else
                    g_st_hdmi_edid_flag[HDMI_TX_DEFAULT_EDID_ID].u8_color_space = MSRT_BITS5_4; //support YUV444/YUV422
                    #endif
                    if (u8_id != HDMI_TX_DEFAULT_EDID_ID && _check_hdmi_sink(u8_valid_flag)) //if not the default EDID and have hdmi sink
                    {
                        sys_edid_convert(g_u8_sys_edid_buf, g_st_hdmi_edid_flag[HDMI_TX_DEFAULT_EDID_ID]);
                    }
                    ms933xdrv_hdmi_rx_edid_config(g_u8_sys_edid_buf);
                    _edid_file_save(0);
                    memcpy(g_u8_sys_edid_buf, g_u8_sys_edid_used_buf, 256);

                    if (g_u8_source_5v_connect)
                    {
                        //delay for hpd
                        mculib_delay_ms(200);
                    }
                }

                if (g_u8_source_5v_connect)
                {
                    ms933xdrv_hdmi_rx_controller_hpd_set(TRUE);
                }
            }
        }
        else
        {
            LOG("hdmi tx sink lost.");
            g_u8_sys_edid_flag = 0xff;
            g_u32_hdmi_sink_min_timing = 0xFFFFFFFFUL;
            g_u32_dvi_sink_min_timing = 0xFFFFFFFFUL;
            g_u8_hdmi_rx_hpd_off_count = 0;
        }
    }

_JUMP_EDID_DEFALUT:

    return u8_edid_change;
}

VOID sys_hotplug_service(void)
{
    g_u8_hdmi_tx_hpd_detect_count++;
    if (g_u8_hdmi_tx_hpd_detect_count < HDMI_TX_HPD_DET_TIMEOUT)
    {
        return;
    }
    g_u8_hdmi_tx_hpd_detect_count = 0;

    _hotplug_service();

    //rx hpd off service
    if (g_u8_sys_edid_flag == 0xff)
    {
        if (g_u8_hdmi_rx_hpd_off_count < HDMI_RX_HPD_OFF_TIMEOUT)
        {
            g_u8_hdmi_rx_hpd_off_count++;
            if (g_u8_hdmi_rx_hpd_off_count == HDMI_RX_HPD_OFF_TIMEOUT)
            {
                LOG("hdmi rx hpd off.");
                ms933xdrv_hdmi_rx_controller_hpd_set(FALSE);
            }
        }
    }
}


void sys_hotplug_init(void)
{
    //
    ms933xdrv_hdmi_rx_controller_hpd_set(FALSE);

    if (g_u8_rx_hpd_test_flag)
    {
        LOG("hdmi rx edid use default.");
        ms933xdrv_hdmi_rx_edid_config((UINT8 *)u8_sys_edid_default_buf);
    }

    _hotplug_service();

    if (g_u8_rx_hpd_test_flag)
    {
        LOG("hdmi rx hpd force on.");
        //delay for hpd
        mculib_delay_ms(200);
        ms933xdrv_hdmi_rx_controller_hpd_set(TRUE);
    }
}

//csc to rgb
VOID _csc_process(UINT8 u8_output_chn)
{
    BOOL b_hw_support_csc;
    BOOL convert = FALSE;

    //if input is YUV444 or YUV422
    if (g_st_hdmi_timing.u8_color_space == HDMI_YCBCR444  ||
        g_st_hdmi_timing.u8_color_space == HDMI_YCBCR422)
    {
        // if output is dvi sink
        if (!g_st_hdmi_edid_flag[u8_output_chn].u8_hdmi_sink)
        {
            convert = TRUE;
        }
        else
        {
            if (_convert_to_rgb_judge(u8_output_chn))
            {
                convert = TRUE;
            }
        }
    }

    if (convert)
    {
        b_hw_support_csc = ms933xdrv_hdmi_tx_csc(g_st_hdmi_timing.u8_color_space, HDMI_RGB);
        if (b_hw_support_csc)
        {
            g_st_hdmi_timing.u8_color_space = HDMI_RGB;
        }
    }
    else
    {
        //bypass
        ms933xdrv_hdmi_tx_csc(0, 0);
    }
}

void _config_output(UINT8 u8_output_chn)
{
    UINT8 u8_hdmi_support;
    UINT8 u8_in_color;
    UINT8 u8_audio_rate;
    if (g_Output1Disable && (u8_output_chn == 3))
    {
        return; // kyq
    }
    //LOG1("config_output = ", u8_output_chn);
    u8_hdmi_support = g_st_hdmi_timing.u8_hdmi_flag;
    u8_in_color = g_st_hdmi_timing.u8_color_space;
    g_st_hdmi_timing.u8_hdmi_flag &= g_st_hdmi_edid_flag[u8_output_chn].u8_hdmi_sink;

    //20190805, hdmi tx audio N/CTS value fixed to 48KHz case.
    u8_audio_rate = g_st_hdmi_timing.u8_audio_rate;
    g_st_hdmi_timing.u8_audio_rate = HDMI_AUD_RATE_48K;

    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);
    ms933xdrv_hdmi_tx_phy_output_enable(FALSE);
    ms933xdrv_hdmi_tx_hdcp_enable(FALSE);
    ms933xdrv_hdmi_tx_shell_set_gcp_packet_avmute(FALSE);

    _csc_process(u8_output_chn);

    #if SYS_AUDIO_PAD_OUTPUT_ENABLE
    ms933xdrv_hdmi_tx_shell_set_audio_cbyte_from_channel(u8_output_chn);
    #endif

    ms933xdrv_hdmi_tx_config(&g_st_hdmi_timing);
    g_st_hdmi_timing.u8_hdmi_flag = u8_hdmi_support;
    g_st_hdmi_timing.u8_color_space = u8_in_color;
    g_st_hdmi_timing.u8_audio_rate = u8_audio_rate;

    ms933xdrv_hdmi_tx_shell_video_mute_enable(FALSE);
    ms933xdrv_hdmi_tx_shell_audio_mute_enable(FALSE);
    ms933xdrv_hdmi_tx_phy_output_enable(TRUE);
    //
    _hdmi_tx_hdcp_start(u8_output_chn); // kyq test 20m cable 20220628
    _hdmi_tx_gcp_avmute_param_default();
}

void _config_output_audio(UINT8 u8_output_chn)
{
    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);

    ms933xdrv_hdmi_tx_audio_config(&g_st_hdmi_timing);
}

void _config_output_audio_fs(UINT8 u8_output_chn)
{
    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);

    ms933xdrv_hdmi_tx_audio_fs_update(g_st_hdmi_timing.u8_audio_rate);
}

void _config_output_avmute(UINT8 u8_output_chn, BOOL b_avmute)
{
    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);

    ms933xdrv_hdmi_tx_shell_set_gcp_packet_avmute(b_avmute);
}

void sys_config_output(UINT8 u8_config_item, UINT8 u8_value)
{
    UINT8 i;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        if (g_u8_sink_hpd_connect[i])
        {
            if (u8_config_item == HDMI_TX_CONFIG_AV)
            {
                _config_output(i);
            }
            else if (u8_config_item == HDMI_TX_CONFIG_AUD)
            {
                _config_output_audio(i);
            }
            else if (u8_config_item == HDMI_TX_CONFIG_AUD_FS)
            {
                #if 0 //20190805, do update cts value. because input detect value maybe 不准确
                _config_output_audio_fs(i);
                #endif
            }
            else if (u8_config_item == HDMI_TX_CONFIG_AVMUTE)
            {
                _config_output_avmute(i, u8_value);
            }

        }
    }
}

void _shutdown_output(UINT8 u8_output_chn)
{
    //LOG1("shutdown_output = ", u8_output_chn);
    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);
    ms933xdrv_hdmi_tx_phy_output_enable(FALSE);
    ms933xdrv_hdmi_tx_phy_power_down();
    //
    ms933xdrv_hdmi_tx_hdcp_enable(FALSE);
    _hdmi_tx_hdcp_param_default(u8_output_chn);
}

void sys_shutdown_output(void)
{
    UINT8 i;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        if (g_u8_sink_hpd_connect[i])
        {
            _shutdown_output(i);
        }
    }

    #if SYS_AUDIO_PAD_OUTPUT_ENABLE
    ms933xdrv_misc_audio_out_pad_enable(FALSE);
    ms933xdrv_misc_audio_i2s_mclk_pad_enable(FALSE);
    #endif
}

//BIT7: AVI change flag; BIT6: AIF change flag; BIT5: VSI change flag; BIT4: acr cts change flag;
//BIT0, AVI inforframe receive flag; BIT1: Audio inforframe receive flag;
//BIT2: VSI inforframe receive flag; BIT3: ACR packet receive flag;
UINT8 _get_infoframe_status(VOID)
{
    UINT8 u8_int_sts = 0;
    UINT32 u32_pdec_int_sts;

    u32_pdec_int_sts = ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status(PDEC_ALL_ISTS);
    if (u32_pdec_int_sts & AVI_CKS_CHG_ISTS)
    {
        u8_int_sts |= 0x80;
    }

    if (u32_pdec_int_sts & AIF_CKS_CHG_ISTS)
    {
        u8_int_sts |= 0x40;
    }

    if (u32_pdec_int_sts & VSI_CKS_CHG_ISTS)
    {
        u8_int_sts |= 0x20;
    }

    if (u32_pdec_int_sts & (ACR_CTS_CHG_ISTS | ACR_N_CHG_ISTS))
    {
        u8_int_sts |= 0x10;
    }

    if (u32_pdec_int_sts & AVI_RCV_ISTS)
    {
        u8_int_sts |= 0x01;
    }

    if (u32_pdec_int_sts & AIF_RCV_ISTS)
    {
        u8_int_sts |= 0x02;
    }

    if (u32_pdec_int_sts & VSI_RCV_ISTS)
    {
        u8_int_sts |= 0x04;
    }

    if (u32_pdec_int_sts & ACR_RCV_ISTS)
    {
        u8_int_sts |= 0x08;
    }

    return u8_int_sts;
}

BOOL sys_hdmi_av_config(UINT8 u8_int_sts, BOOL b_config)
{
    HDMI_CONFIG_T st_hdmi_timing;
    BOOL b_config_all;

    ms933xdrv_hdmi_rx_controller_get_input_config(&st_hdmi_timing);

    //20190704, enhance tmds clk fixed use common detect result. To prevent accidental
    st_hdmi_timing.u16_video_clk = g_u16_input_tmds_clk;

    if (!b_config && memcmp(&st_hdmi_timing, &g_st_hdmi_timing, sizeof(HDMI_CONFIG_T)) == 0)
    {
        LOG("infoframe ignore.");
        return FALSE;
    }

    g_st_hdmi_timing = st_hdmi_timing;

    //20190723, fixed bug for design, if dvi input no packet info
    if (!g_st_hdmi_timing.u8_hdmi_flag)
    {
        u8_int_sts = 0x00;
    }

    //do hdmi rx task
    b_config_all = b_config;

    if (u8_int_sts & 0xA0)
    {
        b_config_all = TRUE;
    }

    if ((u8_int_sts & 0x01) == 0x00)
    {
        sys_default_hdmi_video_config();
    }

    if ((u8_int_sts & 0x04) == 0x00)
    {
        sys_default_hdmi_vendor_specific_config();
    }

    if ((u8_int_sts & 0x02) == 0x00)
    {
        sys_default_hdmi_audio_config();
    }

    //if because packet updated and need update video and audio. first shutdown output.
    //Also do not expect shutdown output agagin, because this action maybe done before here.
    if (!b_config && b_config_all)
    {
        sys_shutdown_output();
    }

    if (b_config_all)
    {
        ms933xdrv_hdmi_rx_controller_pixel_clk_config(g_st_hdmi_timing.u8_color_depth, g_st_hdmi_timing.u8_clk_rpt);
        //20181221, config rx auto avmute color.
        ms933xdrv_hdmi_rx_controller_set_avmute_black_color(g_st_hdmi_timing.u8_color_space);

        //20190515, reset video fifo after pixel config done.
        sys_hdmi_rx_video_fifo_reset();
    }

    //audio config
    sys_hdmi_rx_audio_config(TRUE);

    //hdmi tx config
    //do

    if (b_config_all)
    {
        sys_config_output(HDMI_TX_CONFIG_AV, 0);
    }
    else
    {
        sys_config_output(HDMI_TX_CONFIG_AUD, 0);
    }

    return TRUE;
}

void _hdmi_tx_gcp_avmute_param_default(void)
{
    g_b_rx_gcp_avmute_flag = FALSE;
}

BOOL sys_hdmi_gcp_packet_avmute_service(void)
{
    BOOL b_avmute;

    b_avmute = ms933xdrv_hdmi_rx_controller_get_gcp_avmute();

    if (g_b_rx_gcp_avmute_flag != b_avmute)
    {
        g_b_rx_gcp_avmute_flag = b_avmute;
        LOG1("b_avmute = ", b_avmute);
        sys_config_output(HDMI_TX_CONFIG_AVMUTE, b_avmute);
        return b_avmute;
    }

    return FALSE;
}

void sys_hdmi_rx_packet_service(void)
{
    BOOL b_config = FALSE;
    UINT8 u8_int_sts = 0;

    ////if DVI input, no packet
    if (!g_b_hdmi_input_valid || !g_st_hdmi_timing.u8_hdmi_flag)
    {
        g_u8_rx_packet_timer_count = 0;
        return;
    }

    g_u8_rx_packet_timer_count++;
    if (g_u8_rx_packet_timer_count < RX_PACKET_TIMEOUT)
    {
        return;
    }
    g_u8_rx_packet_timer_count = 0;

    u8_int_sts = sys_hdmi_gcp_packet_avmute_service();

    //20191115
    //if get avmute status change to mute enable, this time do not process infoframe_status and hdmi_rx_audio_service
    if (u8_int_sts)
    {
        return;
    }

    u8_int_sts = _get_infoframe_status();

    if (u8_int_sts & 0xE0)
    {
        LOG1("packet_int_sts = ", u8_int_sts);

        b_config = sys_hdmi_av_config(u8_int_sts, FALSE);
        if (b_config)
        {
            printf_input_av_config(u8_int_sts);
            return;
        }
    }

    sys_hdmi_rx_audio_service(u8_int_sts);
}

void sys_hdmi_rx_hdcp_service(void)
{
    UINT8 u8_rx_woking_flag;

    if (g_u16_input_tmds_clk < 500) //5MHz
    {
        g_u8_rx_hdcp_timer_count = 0;
        return;
    }

    g_u8_rx_hdcp_timer_count++;
    if (g_u8_rx_hdcp_timer_count < RX_HDCP_TIMEOUT)
    {
        return;
    }
    g_u8_rx_hdcp_timer_count = 0;

    u8_rx_woking_flag = ms933xdrv_hdmi_rx_controller_hdcp_get_status();
    if (g_u8_rx_hdcp_woking_flag != u8_rx_woking_flag)
    {
        g_u8_rx_hdcp_woking_flag = u8_rx_woking_flag;
        if (g_u8_rx_hdcp_woking_flag)
        {
            LOG("rx hdcp woking.");
        }
        else
        {
            LOG("rx hdcp stop.");
        }
    }

}

void _hdmi_tx_hdcp_param_default(UINT8 u8_output_chn)
{
    g_u8_tx_hdcp_process_flag_buf[u8_output_chn] = 0;
    g_u8_tx_hdcp_init_flag_buf[u8_output_chn] = 0;
    g_u8_tx_hdcp_enable_flag_buf[u8_output_chn] = 0;
    g_u8_tx_hdcp_timer_count_buf[u8_output_chn] = 0;
}

void _hdmi_tx_hdcp_start(UINT8 u8_output_chn)
{
    g_u8_tx_hdcp_process_flag_buf[u8_output_chn] = 1;
    g_u8_tx_hdcp_init_flag_buf[u8_output_chn] = 1;
    g_u8_tx_hdcp_enable_flag_buf[u8_output_chn] = 0;
    //
    g_u8_tx_hdcp_timer_count_buf[u8_output_chn] = TX_HDCP_START_TIME;
}

void _hdmi_tx_hdcp_service(UINT8 u8_output_chn)
{
    UINT8 u8_flag = 0;
    HDMI_HDCP_RI st_val;

    u8_flag = u8_output_chn << 4;
    ms933xdrv_hdmi_tx_set_channel(u8_output_chn);

    if (g_u8_tx_hdcp_init_flag_buf[u8_output_chn])
    {
        g_u8_tx_hdcp_init_flag_buf[u8_output_chn] = FALSE;

        if (ms933xdrv_hdmi_tx_hdcp_init((UINT8 *)u8_tx_key_buf, (UINT8 *)u8_tx_ksv_buf))
        {
            u8_flag |= 0x01;

            //
            #if 0
            //delay for R0 calc, spec 100ms.
            //if use i2c bus, time cost on iic access. so here not delay.
            mculib_delay_ms(100);
            #endif

            if (ms933xdrv_hdmi_tx_hdcp_get_status(&st_val))
            {
                ms933xdrv_hdmi_tx_hdcp_enable(TRUE);
                g_u8_tx_hdcp_enable_flag_buf[u8_output_chn] = TRUE;
                //LOG1("TX_Ri0 = ", st_val.TX_Ri0);
                //LOG1("TX_Ri1 = ", st_val.TX_Ri1);
                //LOG1("RX_Ri0 = ", st_val.RX_Ri0);
                //LOG1("RX_Ri1 = ", st_val.RX_Ri1);
                u8_flag |= 0x02;
            }
        }

        LOG1("tx_hdcp_flag = ", u8_flag);
        return;
    }

    if (!g_u8_tx_hdcp_enable_flag_buf[u8_output_chn])
    {
        return;
    }

    if (ms933xdrv_hdmi_tx_hdcp_get_status(&st_val))
    {
        //LOG("TX HDCP verify ok. chn = ", u8_output_chn);
    }
    else
    {
        LOG1("tx_hdcp_error_chn = ", u8_output_chn);
        ms933xdrv_hdmi_tx_hdcp_enable(FALSE);
        g_u8_tx_hdcp_enable_flag_buf[u8_output_chn] = FALSE;
    }
}

void sys_hdmi_tx_hdcp_service(void)
{
    UINT8 i;

    for (i = 0; i < HDMI_TX_CHN_NUM; i++)
    {
        //if rx hdcp no working and tx hdcp not be enable, do not go tx hdcp process.
        if (!g_u8_rx_hdcp_woking_flag && !g_u8_tx_hdcp_enable_flag_buf[i])
        {
            g_u8_tx_hdcp_timer_count_buf[i] = TX_HDCP_START_TIME;
            continue;
        }

        if (!g_u8_tx_hdcp_process_flag_buf[i])
        {
            continue;
        }

        g_u8_tx_hdcp_timer_count_buf[i]++;

        if (g_u8_tx_hdcp_timer_count_buf[i] < TX_HDCP_TIMEOUT)
        {
            continue;
        }

        g_u8_tx_hdcp_timer_count_buf[i] = 0;

        _hdmi_tx_hdcp_service(i);
    }
}

void sys_ms933x_chip_valid_check(void)
{
    g_u8_chip_valid_check_timer_count++;
    if (g_u8_chip_valid_check_timer_count < CHIP_VALID_CHECK_TIMEOUT)
    {
        return;
    }
    g_u8_chip_valid_check_timer_count = 0;

    if (!ms933xdrv_misc_chipisvalid())
    {
        LOG("1:2 chip invalid.");
        ms933x_init();
    }
}

void sys_mdt_service(void)
{
    UINT16 u16_mdt_ists;
    UINT8 u8_int_sts = 0;

    if (g_u16_input_tmds_clk < 500) //5MHz
    {
        return;
    }

    u16_mdt_ists = ms933xdrv_hdmi_rx_get_mdt_interrupt_status(MDT_ALL_ISTS);

    if (u16_mdt_ists || g_u8_rxpll_configed_status)
    {
        if (g_u8_rxpll_configed_status == 0)
        {
            if (g_b_hdmi_input_valid && (u16_mdt_ists == MDT_STB_ISTS))
            {
                //do noting
                //it means input has been configed,
                //but MDT_STB_ISTS come between get_mdt_interrupt and get_input_timing.
            }
            else
            {
                g_b_mdt_change_flag = TRUE;

                if (g_b_hdmi_input_valid)
                {
                    g_b_hdmi_input_valid = FALSE;
                    sys_shutdown_output();
                }

                //if not stable ists, means mdt change, delay RX_MDT_TIMEOUT to process
                if (u16_mdt_ists != MDT_STB_ISTS)
                {
                    LOG1("u16_mdt_ists = ", u16_mdt_ists);

                    g_u16_mdt_change_timer_count = 0;
                    ms933xdrv_hdmi_rx_controller_reset(HDMI_RX_CTRL_PDEC | HDMI_RX_CTRL_AUD);
                    ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status(PDEC_ALL_ISTS);

                    return;
                }
            }
        }
        else if (g_u8_rxpll_configed_status == 1)
        {
            g_b_mdt_change_flag = TRUE;
            g_u8_rxpll_configed_status = 2;
            g_u16_mdt_change_timer_count = 0;

            //20180622, clr packet status.
            //20200520, add reset audio
            //20200520, reset audio must by audio clk and video vsync
            ms933xdrv_hdmi_rx_controller_reset(HDMI_RX_CTRL_PDEC | HDMI_RX_CTRL_AUD);
            //clr pdec interrupt status
            ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status(PDEC_ALL_ISTS);

            //config audio pll for audio reset.
            ms933xdrv_hdmi_rx_audio_config(RX_AUDIO_PLL_FREERUN_MODE, g_u16_input_tmds_clk);

            return;
        }
    }

    g_u16_mdt_change_timer_count++;

    if (g_u16_mdt_change_timer_count < MDT_CHANGE_TIMEOUT)
    {
        return;
    }

    g_u16_mdt_change_timer_count = MDT_CHANGE_TIMEOUT;
    g_u8_rxpll_configed_status = 0;

    if (g_b_mdt_change_flag)
    {
        g_b_mdt_change_flag = FALSE;

        g_b_hdmi_input_valid = ms933xdrv_hdmi_rx_get_input_timing(&g_st_hdmi_in_timing);

        if (g_b_hdmi_input_valid)
        {
            LOG("g_b_input_valid = 1");
            u8_int_sts = _get_infoframe_status();
            sys_hdmi_av_config(u8_int_sts, TRUE);
            //
            printf_input_timing();
            printf_input_av_config(u8_int_sts);
        }
        else
        {
            //hdmi tx output disable
            //do
            //sys_shutdown_output();

            //hdmi rx do

        }

        //enhance, reset service timer
        g_u8_rx_packet_timer_count = 0;
        g_u8_hdmi_shell_error_timer_count = 0;
    }
}

//BOOL mculib_chip_read_interrupt_pin(VOID)
//{
//    #if 1
//    //return pin_interupt_read();
//    #else
//    //if no use interrupt pin, must return TRUE
//    return TRUE;
//    #endif
//}

void ms933x_interrupt_service(void)
{
    BOOL b_register_int;

    if (!mculib_chip_read_interrupt_pin())
    {
        b_register_int = ms933xdrv_hdmi_rx_controller_hdmi_interrupt_get_status(CLK_CHANGE_ISTS) ? TRUE : FALSE;

        if (b_register_int)
        {
            sys_hdmi_rx_pll_service();
            //LOG2("b_register_int = ", b_register_int);
        }
    }
}

void ms933x_media_service(void)
{
    sys_ms933x_chip_valid_check();

    sys_hotplug_service();

    sys_hdmi_rx_tmds_overload_service();

    sys_hdmi_rx_pll_service();

    sys_mdt_service();

    sys_led_process();

    sys_hdmi_rx_packet_service();

    if (g_u8_rx_hdcp_support)
    {
        sys_hdmi_rx_hdcp_service();
    }

    if (g_u8_tx_hdcp_support)
    {
        sys_hdmi_tx_hdcp_service();
    }

    sys_error_service();
}




VOID system_init(VOID)
{
    mculib_clock_init();
    mculib_uart_init(9600);
    mculib_timer_config_ms(TIMER_BASE_10MS);
    mculib_led_init();
    mculib_key_init();
    mculib_i2c_init();
    mculib_i2c_set_speed(I2C_SPEED_100K);
//    mculib_enable_interrupt();
    LOG("Init Done.");
}

VOID key_service(VOID)
{
    UINT8 u8key_TEST = mculib_key_detect();
    LOG("test TEST.");
    switch ( u8key )
    {
        case KEY_MAP_SHORT0:
            //g_u8_user_data & 0x01
            g_sel++;
            if (g_sel >= HDMI_PORTS)
            {
                g_sel = 0;
            }
            if (g_u8_user_data & 0x01)
            {
                g_u8_user_data = (g_sel << 4) | (g_u8_user_data & 0x0f);
                mculib_user_save();
            }
            LOG("case short key0.");
            break;
        case KEY_MAP_SHORT1:
          LOG("short key1.");
            if (g_u8_user_data & 0x01)
            {
                g_u8_user_data = g_u8_user_data & 0xfe; // set to auto mode
                 LOG("auto mode");
            }
            else
            {
                g_u8_user_data = g_u8_user_data | 0x01; // manual mode
                LOG("manual mode");
            }
            if (g_u8_user_data & 0x01)
            {
                MS9601A_Write(0x64, 0x07);
            }
            else
            {
                MS9601A_Write(0x64, 0x17); // 会自动切
            }
            mculib_user_save();
            break;
        case KEY_MAP_SHORT2:
            g_Output1Disable = !g_Output1Disable;
            if (g_Output1Disable)
            {
                _shutdown_output(3);
            }
            else if (g_b_hdmi_input_valid)
            {
                _config_output(3);
            }
//     if (g_u8_user_data &0x02)
//      {
//          g_u8_user_data = g_u8_user_data&0xfd; // set to auto mode
//      }
//      else
//      {
//          g_u8_user_data = g_u8_user_data|0x01; // manual mode
//      }
            LOG("short key2.");
            break;

        case KEY_MAP_LONG4:
            LOG("long key.");
            break;
    }
}

extern uint8_t g_sel_last,g_sel;
// Program Entry
VOID main(VOID)
{
    //LVD2.75V
//    LVDCON = 0x2b;

    system_init();
    mculib_uart_log("Power by ZADAPRO\r\n");
    sys_led_init(1);
    mculib_delay_ms(100);
    sys_led_init(0);
    mculib_user_load();
    ms9601_init();
    ms933x_init();
    // system loop
    while (1)
    {
        ms9601_Media_Service();
//      mculib_uart_service();  // kyq

        ms933x_interrupt_service();

        if (g_u16_sys_timer >= SYS_TIMEOUT_100MS)
        {
            ms933x_media_service();
            g_u16_sys_timer = 0;
            if (g_u8_user_data & 0x01)
            {
                mculib_Led_toggle(); // manual mode
            }
        }

        if (g_u16_key_timer >= SYS_TIMEOUT_50MS)
        {
            key_service();
            g_u16_key_timer = 0;
            if (0 == (g_u8_user_data & 0x01))
            {
                mculib_Led_toggle(); // auto mode
            }
        }

        // Power reduction.
        mculib_delay_ms(1);
        if (g_u8_user_data & 0x01)
        {
            if (g_LedTimer > 100) // 1s
            {
                mculib_Led_toggle(); // manual mode
                g_LedTimer = 0;
            }
            
        }
        else if (g_LedTimer > 50) // 0.5s
            {
                mculib_Led_toggle(); //  auto mode
                g_LedTimer = 0;
            }
        
    }
}
