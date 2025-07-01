#ifndef __HDMI_H__
#define __HDMI_H__
#define HDMI_PORTS	3   // Org is 3  // kyq 20210812
void ms9601_init(void);
void syetem_init(void);
void HDMI_InPut_Switch(void);
void ms9601_Media_Service(void);
void Key_Service(void);
void IR_Service(void);
void MS9601A_Write(uint8_t u8_index, uint8_t u8_value);
#endif
