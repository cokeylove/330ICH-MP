/*-----------------------------------------------------------------------------
 * TITLE: OEM_ADC.H
 *
 * Author : Dino
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#ifndef OEM_ADC_H
#define OEM_ADC_H

//----------------------------------------------------------------------------
// The function of scaning ADC input
//----------------------------------------------------------------------------
#define ADCChannel_0     0x80
#define ADCChannel_1     0x81
#define ADCChannel_2     0x82
#define ADCChannel_3     0x83
#define ADCChannel_4     0x84
#define ADCChannel_5     0x85
#define ADCChannel_6     0x86
#define ADCChannel_7     0x87

#define ECprochot_132W        901            //COKEYXU041:ADD //132W, 2.64V //add for 135W adapter

#define ECprochot_88W         600            //COKEYXU005:ADD //88W, 1.75V

#define ECprochot_82W         560            //COKEYXU027:ADD //82W, 1.64V

#define ECprochot_78W         532            //COKEYXU027:ADD //78W, 1.56V

#define ECprochot_74W         505            //COKEYXU037:ADD //74W, 1.48V

#define ECprochot_65W         436            
#define ECprochot_45W         300            

#define A45WCP					126
#define A65WCP					201

#define AdapterID_NON_SUPPORT   0xFF
#define AdapterID_90W           0x00
#define AdapterID_120W          0x01
#define AdapterID_65W           0x02
#define AdapterID_170W          0x03
#define AdapterID_135W          0x04
#define AdapterID_45W           0x05

extern void ScanADCFixChannel(void);
extern void ScanADCDyChannel1(void);
extern void Init_ADC(void);
extern void Init_VC(BYTE Group);

extern void Chk_AC_STPP(void);
extern void SetPowerBatteryparameter(void);
extern void SetPowerBatteryparameter2(void); 

extern void Chk_HybridFORBQ24780_STPP(void); 
extern void psys_throttle(); 
extern void Check_heavy_loading_500ms(); 

#endif
