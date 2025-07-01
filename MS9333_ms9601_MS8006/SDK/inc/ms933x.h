/**
******************************************************************************
* @file    ms933x.h
* @author  
* @version V1.0.0
* @date    15-Nov-2014
* @brief   ms933x SDK Library interfaces declare
* @history    
*
* Copyright (c) 2009-2014, MacroSilicon Technology Co.,Ltd.
******************************************************************************/
#ifndef __MACROSILICON_MS933X_H__
#define __MACROSILICON_MS933X_H__

#include "ms933x_comm.h"


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////
//misc module APIs

/***************************************************************
*  Function name:   ms933xdrv_misc_chipisvalid
*  Description:     check ms933x chip is valid or not
*  Entry:           None
*
*  Returned value:  BOOL (valid is true)
*  Remark: 
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_misc_chipisvalid(VOID);


/***************************************************************
*  Function name:   ms933xdrv_misc_chip_id_get
*  Description:     get ms933x chip id
*  Entry:           NULL             
*  Returned value:  UINT32 id.
*  Remark: 
***************************************************************/
MS933X_DRV_API UINT32 ms933xdrv_misc_chip_id_get(VOID);


/***************************************************************
*  Function name:   ms933xdrv_misc_audio_out_pad_enable
*  Description:     I2S and SPDIF output PAD enable. Don't ctrl i2s mclk
*  Entry:           [in]b_enable, if true enable output, else disable output         
*  Returned value:  None
*  Remark: 
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_misc_audio_out_pad_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_misc_audio_i2s_mclk_pad_enable
*  Description:     I2S mclk output PAD enable
*  Entry:           [in]b_enable, if true enable output, else disable output         
*  Returned value:  None
*  Remark: 
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_misc_audio_i2s_mclk_pad_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_misc_audio_i2s_mclk_div
*  Description:     I2S mclk divider, default 128fs
*  Entry:           [in]u8_div
*                       0: 128fs
*                       1: 64fs
*                       2: 32fs
*                       3: 16fs
*
*  Returned value:  None
*  Remark: 
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_misc_audio_i2s_mclk_div(UINT8 u8_div);


///////////////////////////////////////////////////////////////////
//hdmi rx module APIs

/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_init
*  Description:     hdmi rx controllor module init
*  Entry:           [in]None
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_init(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_pi_phy_init
*  Description:     hdmi rxphy config, default init term enable
*  Entry:           None
*
*  Returned value:  BOOL (init success is true)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_pi_phy_init(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_5v_det
*  Description:     get hdmi rx 5V status
*  Entry:           [in]None
*
*  Returned value:  BOOL (5V connect is true)
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_5v_det(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hpd_set
*  Description:     set hdmi rx hpd status
*  Entry:           [in]bReady, if true set HPD High
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_hpd_set(BOOL bReady);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_edid_config
*  Description:     hdmi rx edid config
*  Entry:           [in]u8Edid, const 256byte
*                   
*  Returned value: None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_edid_config(UINT8 *u8Edid);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_tmds_clk
*  Description:     get hdmi rx input tmds clk
*  Entry:           [in]None
*
*  Returned value:  return UINT16 value, uint 10000Hz
*  Remark:
***************************************************************/
MS933X_DRV_API UINT16 ms933xdrv_hdmi_rx_get_tmds_clk(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_pi_pll_config
*  Description:     hdmi rxphy config base on detect input tmds clk
*  Entry:           [in]u16TmdsClk, uint 10000Hz
*                       u8_eq_gain, EQ value config. 
*                          manual EQ, set value from 0~7
*                          auto EQ, set value 0xFF
*                          toggle reset rxphy, set vaule 0xFE
*
*  Returned value:  return config status.
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_pi_phy_config(UINT16 u16TmdsClk, UINT8 u8_eq_gain);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_pi_phy_config_with_28eq
*  Description:     hdmi rxphy config with 28eq base on detect input tmds clk
*  Entry:           [in]u16TmdsClk, uint 10000Hz
*                       u8_eq_gain, EQ value config. 
*                          manual EQ, set value from 0~27
*                          auto EQ, set value 0xFF
*
*  Returned value:  return config status.
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_pi_phy_config_with_28eq(UINT16 u16TmdsClk, UINT8 u8_eq_gain);


/***************************************************************
*  Function name:   ms933xdrv_errdet_wrong_status
*  Description:     hdmi rxphy error counter detect
*  Entry:           [in]u16_error_val, max thresthod of error
*
*  Returned value:  BOOL (when detect error large than u16_error_val, return true)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_errdet_wrong_status(UINT16 u16_error_val);


/***************************************************************
*  Function name:   ms933xdrv_rxpll_lock_status
*  Description:     hdmi rxpll lock status detect
*  Entry:           [in]None
*
*  Returned value:  BOOL (when rxpll lock return true)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_rxpll_lock_status(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_pi_pll_release
*  Description:     hdmi rxpll enable ctrl
*  Entry:           [in]bEnable, if false disable rxpll
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_pi_pll_release(BOOL bEnable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_phy_power_down
*  Description:     hdmi rxphy power down
*  Entry:           [in]None
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_phy_power_down(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdcp_init
*  Description:     hdmi rx hdcp init key
*  Entry:           [in]u8RxKsv, 5byte 
*                       u8RxKey, 280byte
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_hdcp_init(UINT8 *u8RxKsv, UINT8 *u8RxKey);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdcp_encryption_enable
*  Description:     hdmi rx hdcp enable
*  Entry:           [in]bEnable, if true eanble HDCP decode
*                   
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_hdcp_encryption_enable(BOOL bEnable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdcp_get_status
*  Description:     hdmi rx hdcp working status
*  Entry:           [in]None
*
*  Returned value:  return true if hdcp working
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_controller_hdcp_get_status(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdcp_get_Ri
*  Description:     hdmi rx hdcp get Link verification response value 
*  Entry:           [in]None
*
*  Returned value:  return UINT16 value
*  Remark:
***************************************************************/
MS933X_DRV_API UINT16 ms933xdrv_hdmi_rx_controller_hdcp_get_Ri(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_hdcp_detect_enable
*  Description:     enable hdmi rx hdcp detect encoding start   
*  Entry:           [in]bEnable, if true enable hdcp detect source encoding
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_hdcp_detect_enable(BOOL bEnable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdcp_data_enable
*  Description:     enable hdmi rxphy data to hdcp moudle  
*  Entry:           [in]bEnable, if true enable
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_hdcp_data_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_mdt_interrupt_status
*  Description:     get hdmi rx mdt interrupt status, auto clear status
*  Entry:           [in]u16_mask, mask bit
*               
*  Returned value:  return UINT16 status, enum refer to MDTISTS_E
*  Remark:
***************************************************************/ 
MS933X_DRV_API UINT16 ms933xdrv_hdmi_rx_get_mdt_interrupt_status(UINT16 u16_mask);



/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_input_timing
*  Description:     get hdmi rx input video timing information
*  Entry:           [out]ptTiming
*                        u8_polarity, enum to e_RxMdtPol
*                        u16_htotal,  by tmds clk
*                        u16_vtotal,  uint line
*                        u16_hactive, by tmds clk
*                        u16_vactive, uint line
*                        u16_pixclk,  input tmds clk, uint 10000Hz,
*                        u16_vfreq,   uint 0.01Hz
*                        u16_hoffset, by pixel clk
*                        u16_voffset, uint line
*                        u16_hsyncwidth, by tmds clk
*                        u16_vsyncwidth, uint line
*
*  Returned value:  if input timing valid return true, else return false
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_get_input_timing(VIDEOTIMING_T *ptTiming);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_reset
*  Description:     reset hdmi rx controller logic
*  Entry:           [in]eModule, define refer to hdmi_rx_reset_ctrl
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_reset(UINT32 eModule);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_audio_fifo_status
*  Description:     get hdmi rx audio fifo status
*  Entry:           [in]None
*
*  Returned value:  return UINT8 value, refer to AUDIO_FIFO_STATUS_E
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_get_audio_fifo_status(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_audio_fifo_reset
*  Description:     hdmi rx audio fifo reset
*  Entry:           [in]b_reset, if true reset fifo, else release fifo reset
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_audio_fifo_reset(BOOL b_reset);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_video_fifo_reset
*  Description:     hdmi rx video fifo reset
*  Entry:           [in]b_reset, if true reset fifo, else release fifo reset
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_video_fifo_reset(BOOL b_reset);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status
*  Description:     get hdmi rx packet decode change int stauts, auto clear status
*  Entry:           [in]u32_mask, mask bit
*
*  Returned value:  return UINT32 status, define refer to hdmi_rx_packet_ists
*  Remark:
***************************************************************/
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status(UINT32 u32_mask);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_hdmi_interrupt_get_status
*  Description:     get hdmi rx hdmi information change int stauts, auto clear status
*  Entry:           [in]u32_mask, mask bit
*
*  Returned value:  return UINT32 status, define refer to hdmi_rx_hdmi_ists
*  Remark:
***************************************************************/
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_hdmi_interrupt_get_status(UINT32 u32_mask);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_audio_fifo_interrupt_get_status
*  Description:     get hdmi rx audio fifo information change int stauts, auto clear status
*  Entry:           [in]u32_mask, mask bit
*
*  Returned value:  return UINT32 status, define refer to AUDIO_FIFO_STATUS_E
*  Remark:
***************************************************************/
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_audio_fifo_interrupt_get_status(UINT32 u32_mask);


//get status, but do not clear interrput status.
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_pdec_interrupt_get_status_ext(UINT32 u32_mask);
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_hdmi_interrupt_get_status_ext(UINT32 u32_mask);
MS933X_DRV_API UINT32 ms933xdrv_hdmi_rx_controller_audio_fifo_interrupt_get_status_ext(UINT32 u32_mask);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_interrupt_to_pin_ctrl
*  Description:     hdmi rx interrupt status to INT_PIN ctrl. 
                    when interrupt coming, INT_PIN 1->0.
                    when interrupt register bit be cleared, INT_PIN 0->1
*  Entry:           [in]u8_module,
*                                 0: HDMI_RX_CTRL_PDEC,  mask refer to hdmi_rx_packet_ists
*                                 1: HDMI_RX_CTRL_MODET, mask refer to MDTISTS_E
*                                 2: HDMI_RX_CTRL_HDMI,  mask refer to hdmi_rx_packet_ists
*                                                        default enable CLK_CHANGE_ISTS on hdmi rx init
*                                 3: HDMI_RX_CTRL_AUD,   mask refer to AUDIO_FIFO_STATUS_E
*                       b_enable_to_pin: interruput to pin enable
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_interrupt_to_pin_ctrl(UINT8 u8_module, UINT32 u32_mask, BOOL b_enable_to_pin);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_mdt_interrupt_status_ext
*  Description:     get hdmi rx mdt interrupt status, but not clear status
*  Entry:           [in]u16_mask, mask bit
*               
*  Returned value:  return UINT16 status, enum refer to MDTISTS_E
*  Remark:
***************************************************************/ 
MS933X_DRV_API UINT16 ms933xdrv_hdmi_rx_get_mdt_interrupt_status_ext(UINT16 u16_mask);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_get_gcp_avmute
*  Description:     get hdmi rx controller receive gcp avmute packet
*  Entry:           [in]None
*
*  Returned value:  BOOL (return true if receive gcp avmute packet)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_controller_get_gcp_avmute(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_set_avmute_black_color
*  Description:     set hdmi rx avmute black color by different color space
*  Entry:           [in]u8_cs, enum refer to HDMI_CS_E
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_set_avmute_black_color(UINT8 u8_cs);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_get_input_config
*  Description:     get hdmi rx input HDMI information
*  Entry:           [out]pt_hdmi_rx, refer to HDMI_CONFIG_T
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_get_input_config(HDMI_CONFIG_T *pt_hdmi_rx);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_get_pdec_header_audio_mode
*  Description:     get hdmi rx audio mode from audio packet 192bit header
*  Entry:           None
*
*  Returned value:  UINT8 audio mode
*                   0:AUDS_RCV; 0:OBA_RCV; 2:DST_RCV; 3:HBR_RCV; 4:OBM_RCV; 5:MAS_RCV
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_controller_get_pdec_header_audio_mode(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_get_inforframe_audio_mode
*  Description:     get hdmi rx audio mode from audio inforframe
*  Entry:           None
*
*  Returned value:  UINT8 audio mode, refer to HDMI_AUDIO_CT_E
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_controller_get_inforframe_audio_mode(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_pixel_clk_config
*  Description:     hdmi rx set pixel clock based on detect input deep color mode and clk repeat
*  Entry:           [in]u8ColorDepth, refer to HDMI_COLOR_DEPTH_E
*                       u8ClkRepeat,  refer to HDMI_CLK_RPT_E
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_controller_pixel_clk_config(UINT8 u8ColorDepth, UINT8 u8ClkRepeat);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_controller_audio_channel_config
*  Description:     hdmi rx set audio channel based on detect Audio infoframe
*  Entry:           [in]u8_chn, refer to HDMI_AUDIO_CHN_E
*                       u8_speaker_locations,  refer to CEA-861 audio infoframe, BYTE4
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_controller_audio_channel_config(UINT8 u8_chn, UINT8 u8_speaker_locations);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_audio_config
*  Description:     config hdmi rx audio clk base detect input tmds clk
*  Entry:           [in]u8_audio_clk_mode: refer to e_RxAudioClkMode
*                       u16_tmds_clk: uint:10000Hz
*                       
*  Returned value: return UINT16 audio fs, uint:100Hz
*  Remark:
***************************************************************/
MS933X_DRV_API UINT16 ms933xdrv_hdmi_rx_audio_config(UINT8 u8_audio_clk_mode, UINT16 u16_tmds_clk);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_audio_output_enable
*  Description:     hdmi rx audio output ctrl, default enable in ms933xdrv_hdmi_rx_init
*  Entry:           [in]b_enable: if true enable 
*
*  Returned value: None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_audio_output_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_pi_phy_term_enable
*  Description:     hdmi rxphy clk and data internal term ctrl
*  Entry:           [in]b_enable: if true enable hdmi rxphy internal term
*                   
*  Returned value: None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_pi_phy_term_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_get_tmds_overload
*  Description:     get hdmi rx tmds whether overload, (reserve function) 
*  Entry:           [in]None
*                   
*  Returned value: return UINT8 stauts
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_rx_get_tmds_overload(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_rx_tmds_overload_protect_disable
*  Description:     hdmi rx tmds overload ctrl, (reserve function)
*  Entry:           [in]b_disable: if true disable tmds overload protect
*                   
*  Returned value: None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_tmds_overload_protect_disable(BOOL b_disable);


///////////////////////////////////////////////////////////////////
//hdmi tx APIs


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_set_channel
*  Description:     select which hdmi tx channel need to be configed
*  Entry:           [in]u8_chn: enum refer to HDMI_CHANNEL_E
*               
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_set_channel(UINT8 u8_chn);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_get_channel
*  Description:     get current keep hdmi tx channel id
*  Entry:           [in]None
*               
*  Returned value:  return UINT8 value enum refer to HDMI_CHANNEL_E
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_get_channel(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_hpd
*  Description:     hdmi tx hot plug detection
*  Entry:           [in] None
*               
*  Returned value:  if hdmi cable plug in return true, else return false
*  Remark:
***************************************************************/ 
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_shell_hpd(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_config
*  Description:     hdmi tx config
*  Entry:           [in]pt_hdmi_tx: struct refer to HDMI_CONFIG_T
*                      
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_config(HDMI_CONFIG_T *pt_hdmi_tx);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_output_enable
*  Description:     hdmi tx module output timing on/off
*  Entry:           [in]b_enable: if true turn on 
*                                 else turn off
*               
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_output_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_video_mute_enable
*  Description:     hdmi tx shell module video mute
*  Entry:           [in]b_enable, if true mute screen else normal video
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_shell_video_mute_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_audio_mute_enable
*  Description:     hdmi tx shell module audio mute
*  Entry:           [in]b_enable, if true mute audio else normal audio
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_shell_audio_mute_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_set_gcp_packet_avmute
*  Description:     hdmi tx shell send gcp avmute packet
*  Entry:           [in]b_mute, if true tx will send gcp avmute packet per frame.
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_shell_set_gcp_packet_avmute(BOOL b_mute);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_csc
*  Description:     hdmi tx shell color space covertion fucntion.unsupport YUV420 csc
*  Entry:           [in]in_color, shell input color space, refer to HDMI_CS_E
*                       out_color, shell output color space, refer to HDMI_CS_E
*
*  Returned value:  BOOL (if support csc return TRUE)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_csc(UINT8 in_color, UINT8 out_color);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_audio_config
*  Description:     hdmi tx audio config
*  Entry:           [in]pt_hdmi_tx: struct refer to HDMI_CONFIG_T
*                      
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_audio_config(HDMI_CONFIG_T *pt_hdmi_tx);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_audio_fs_update
*  Description:     hdmi tx audio sample rate update
*  Entry:           [in]u8_audio_rate: refer to HDMI_AUDIO_RATE_E
*                      
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_audio_fs_update(UINT8 u8_audio_rate);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_set_audio_cbyte_from_channel
*  Description:     set hdmi tx cbyte status from which tx channel
*  Entry:           [in]u8_tx_chn: refer to HDMI_CHANNEL_E
*                      
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_shell_set_audio_cbyte_from_channel(UINT8 u8_tx_chn);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_get_audio_cbyte_status
*  Description:     get hdmi tx input audio c byte
*  Entry:           [in]None
*                      
*  Returned value:  UINT8
*                   BIT0: 0:PCM; 1: none-PCM
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_shell_get_audio_cbyte_status(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_power_down
*  Description:     hdmi txphy module power down
*  Entry:           [in]None
*               
*  Returned value:  None
*  Remark:
***************************************************************/ 
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_power_down(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_shell_timing_stable
*  Description:     hdmi tx shell timing stable status
*  Entry:           [in]None
*               
*  Returned value:  BOOL (if shell timing is stable return true)
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_shell_timing_stable(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_data_R200_enable
*  Description:     hdmi txphy tmds data R200 ctrl 
*  Entry:           [in]b_enable, if true enable tmds data R200
*               
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_data_R200_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_clk_drive_config
*  Description:     hdmi txphy clk drive adjust
*  Entry:           [in]u8_main_pre, 0~7
*                       u8_main_po,  0~15
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_clk_drive_config(UINT8 u8_main_pre, UINT8 u8_main_po);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_data_drive_config
*  Description:     hdmi txphy data drive adjust
*  Entry:           [in]u8_main_pre, 0~7
*                       u8_main_po,  0~15
*                       u8_post_po,  0~15
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_data_drive_config(UINT8 u8_main_pre, UINT8 u8_main_po, UINT8 u8_post_po);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_output_auto_ctrl
*  Description:     hdmi txphy output auto ctrl by input timing or clk status
*  Entry:           [in]u8_ctrl_mode: 0: by input clk change int, CLK_CHANGE_ISTS
*                                     1: by input timng unstable, MDT_USTB_ISTS
*                       b_auto: auto ctrl function enable
*
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_output_auto_ctrl(UINT8 u8_ctrl_mode, BOOL b_auto);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_phy_data_drive_enhance
*  Description:     hdmi txphy drive enhance
*  Entry:           [in]b_enable, if true enhance output tmds data drive
*               
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_phy_data_drive_enhance(BOOL b_enable);


//hdmi tx ddc APIs

/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_ddc_is_busy
*  Description:     hdmi tx module DDC sda/scl whether is busy.
*  Entry:           [in]None
*               
*  Returned value:  BOOL (if sda or scl is low level, return TRUE)
*  Remark:
***************************************************************/ 
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_ddc_is_busy(VOID);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_ddc_enable
*  Description:     hdmi tx module DDC enable
*  Entry:           [in]b_enable, if true enable ddc, else disable
*               
*  Returned value:  None
*  Remark:
***************************************************************/ 
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_ddc_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_hdcp_enable
*  Description:     hdmi hdcp enable
*  Entry:           [in]b_enable, if true enable hdcp, else disable
*               
*  Returned value:  None
*  Remark:
***************************************************************/
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_hdcp_enable(BOOL b_enable);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_hdcp_init
*  Description:     hdmi hdcp init
*  Entry:           [in]p_u8_key, 280 bytes hdmi tx key
*                       p_u8_ksv, 5 bytes hdmi tx ksv
*
*  Returned value:  if hdcp init success return true, else return false
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_hdcp_init(UINT8 *p_u8_key, UINT8 *p_u8_ksv);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_hdcp_get_status
*  Description:     hdmi hdcp get tx/rx Ri verify result, suggest 2s period polled
*  Entry:           [out]pt, refer to HDMI_HDCP_RI
*
*  Returned value:  if verify sucess return 0x01, else return 0x00
*  Remark:
***************************************************************/
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_hdcp_get_status(HDMI_HDCP_RI *pt);


/***************************************************************
*  Function name:   ms933xdrv_hdmi_tx_parse_edid
*  Description:     parse hdmi sink edid
*  Entry:           [out]p_u8_edid_buf, buf for EDID, 256bytes
*                        pt_edid, refer to HDMI_EDID_FLAG_T
*
*  Returned value:  if parse sucess return 0x01, else return 0x00
*  Remark:
***************************************************************/
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_parse_edid(UINT8 *p_u8_edid_buf, HDMI_EDID_FLAG_T *pt_edid);


//for HDMI repeater APIs

//hdmi rx
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_core_hdcp_bcaps_fast_ddc_support_enable(BOOL b_fast_support);
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_core_hdcp_bcaps_rpt_enable(BOOL b_enable);
MS933X_DRV_API VOID ms933xdrv_hdmi_rx_core_hdcp_rpt_reset_enable(BOOL b_reset);
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_core_hdcp_rpt_ksv_init(UINT8 u8_dev_count, UINT8 u8_dev_depth, UINT8 *p_u8_ksv_buf);
MS933X_DRV_API BOOL ms933xdrv_hdmi_rx_core_hdcp_rpt_ready_get(VOID);

//hdmi tx
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_hdcp_get_bcaps_from_rx(UINT8 *p_data);
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_hdcp_get_bstatus_from_rx(UINT8 *p_u8_dev_count, UINT8 *p_u8_dev_depth);
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_hdcp_get_bksv_from_rx(UINT8 *p_data);
MS933X_DRV_API VOID ms933xdrv_hdmi_tx_hdcp_rpt_enable(BOOL b_enable);
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_hdcp_repeat_init(UINT8 *p_u8_dev_count, UINT8 *p_u8_dev_depth, UINT8 *p_u8_ksv_list_buf);
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_hdcp_rpt_status_get(VOID);


//hdmi tx hdcp key init for HW new function. only need init 280byte once.
MS933X_DRV_API UINT8 ms933xdrv_hdmi_tx_hdcp_key_init(UINT8 *p_u8_key);

//hdmi tx hdcp video encryption for HW new function
MS933X_DRV_API BOOL ms933xdrv_hdmi_tx_hdcp_encryption_enable(BOOL b_enable);


#ifdef __cplusplus
}
#endif


#endif  //__MACROSILICON_MS933X_H__
