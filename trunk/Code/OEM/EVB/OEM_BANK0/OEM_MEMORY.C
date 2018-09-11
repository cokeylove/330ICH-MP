 /*-----------------------------------------------------------------------------
 * Filename:OEM_MEMORY.C     For Chipset: ITE.IT85XX
 *
 * Function: Memory Definition for extern all code base reference
 *
 * [Memory Map Description]
 *
 * Chip Internal Ram : 0x00-0xFF For Kernel and Chip Level use
 *
 ***************************************************
 * Chip 8500       : External ram 0x000-0x7FF
 * Chip Other 85XX : External ram 0x000-0xFFF
 ***************************************************
 *
 * [OEM Memory Rang]
 * [External Ram]
 * 0x100-0x1FF   OEM RAM
 * 0x200-0x2FF   OEM RAM SPI buffer array
 * 0x300-0x3FF   OEM RAM EC Space
 * 0x400-0x4FF   OEM RAM
 * 0x500-0x5FF   OEM RAM
 * 0x600-0x6FF   OEM RAM

 * 0x800-0x8FF   OEM RAM keyboard matrix
 * 0x900-0x9FF   OEM RAM
 * 0xA00-0xAFF   OEM RAM
 * 0xB00-0xBFF   OEM RAM
 * 0xC00-0xCFF   OEM RAM
 * 0xD00-0xDFF   OEM RAM
 * 0xE00-0xEFF   OEM RAM
 * 0xF00-0xFFF   OEM RAM For HSPI ram code function
 *
 * Copyright (c) 2006-2009, ITE Tech. Inc. All Rights Reserved.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x100-0x1FF   OEM RAM 1
//-------------------------------------------------------------------------------
//*******************************************************************************
XBYTE	SysPowState							_at_(OEMRAM1+0x00); //(byte)
XBYTE	PWSeqStep							_at_(OEMRAM1+0x01); //(byte)
XBYTE	DeepSleepCunt						_at_(OEMRAM1+0x02); //(byte)
XWORD   DelayDoPowerSeq						_at_(OEMRAM1+0x03); //(word)
XWORD   PowSeqDelay							_at_(OEMRAM1+0x05); //(word)

XBYTE	SBSWReleaseCunt                     _at_(OEMRAM1+0x07); //(byte)  

XBYTE	Test109								_at_(OEMRAM1+0x09); //(byte)
XBYTE   UART_DB_RAM                       	_at_(OEMRAM1+0x0B);  
XBYTE	KB_OBF_count						_at_(OEMRAM1+0x0C);
XBYTE	UpdateLEDBL_delay					_at_(OEMRAM1+0x0D);
XBYTE	uVGATurboFun						_at_(OEMRAM1+0x0E);	//

XBYTE   CmdData2							_at_(ECRAM1+0x0F);
XBYTE	BatSMbusFailCount					_at_(ECRAM1+0x10);
XBYTE  	Batpollstep1 						_at_(ECRAM1+0x11);
XBYTE  	EC_oCCBQl							_at_(ECRAM1+0x12);
XBYTE  	EC_oCCBQh							_at_(ECRAM1+0x13);
XBYTE  	EC_oCBTl							_at_(ECRAM1+0x14);
XBYTE  	EC_oCBTh							_at_(ECRAM1+0x15);
XBYTE  	BAT1PERCL							_at_(ECRAM1+0x16);
XBYTE  	BAT1PERCH							_at_(ECRAM1+0x17);

XBYTE	uVPCeventSource						_at_(ECRAM1+0x18);
XBYTE	uVPCeventSource2					_at_(ECRAM1+0x19);
XBYTE	uNovoVPCCount						_at_(ECRAM1+0x1A);
XBYTE   DebugCombineKey						_at_(ECRAM1+0x1B);
XBYTE	PowerLed_Count						_at_(ECRAM1+0x1C);
XBYTE   LED_KB_PWM_Step						_at_(ECRAM1+0x1D);
XBITS_8 uReserve07							_at_(ECRAM1+0x1E);
XBYTE  	CombineKeyStatus					_at_(ECRAM1+0x1F);

XBYTE  	CHGIC_SMbusFailCnt					_at_(ECRAM1+0x20);
XBYTE  	CHGIC_ptr							_at_(ECRAM1+0x21);
XBYTE  	CHGIC_InputCurrentL					_at_(ECRAM1+0x22);
XBYTE  	CHGIC_InputCurrentH					_at_(ECRAM1+0x23);
XBYTE  	CHGIC_ReadCmd0x14L					_at_(ECRAM1+0x24);
XBYTE  	CHGIC_ReadCmd0x14H					_at_(ECRAM1+0x25);
XBYTE  	CHGIC_ReadCmd0x15L					_at_(ECRAM1+0x26);
XBYTE  	CHGIC_ReadCmd0x15H					_at_(ECRAM1+0x27);
XBYTE  	CHGIC_ReadCmd0x3FL					_at_(ECRAM1+0x28);
XBYTE  	CHGIC_ReadCmd0x3FH					_at_(ECRAM1+0x29);
XWORD  	BATTEMP								_at_(ECRAM1+0x2A);
XBYTE   EC_C_modeL							_at_(ECRAM1+0x2C);
XBYTE   EC_C_modeH							_at_(ECRAM1+0x2D);
//XBYTE	ManualFanPRM						_at_(ECRAM1+0x2E);//JERRYCH00:Modify the fan control from linear to the ladder.
XBYTE	AdapterID							_at_(ECRAM1+0x2F);

XBYTE	eFlashVarifyOK						_at_(ECRAM1+0x30);
XBYTE	eFlash_r_data						_at_(ECRAM1+0x31);
XBYTE	eFlashA1							_at_(ECRAM1+0x32);
XBYTE	eFlashA0							_at_(ECRAM1+0x33);
XBYTE	eEEPROMBank							_at_(ECRAM1+0x34);
XBYTE	eEEPROMAddrsss						_at_(ECRAM1+0x35);
XBYTE   eEEPROMData							_at_(ECRAM1+0x36);
XBYTE	eEEPROMMarkData						_at_(ECRAM1+0x37);
XBYTE   e256ByteCnt							_at_(ECRAM1+0x38);
XBYTE   eUpdateEEPROMCnt					_at_(ECRAM1+0x39);
XBYTE   eUpdatepDevStusCnt					_at_(ECRAM1+0x3A);
XBYTE  	BatLowCnt							_at_(ECRAM1+0x3B);
XBYTE  	WSMbusTemp01						_at_(ECRAM1+0x3C);
XBYTE  	WSMbusTemp02						_at_(ECRAM1+0x3D);
XBYTE  	SMbusFailCnt2						_at_(ECRAM1+0x3E);
XBYTE  	SMbusFailCnt3						_at_(ECRAM1+0x3F);

XBYTE   CombineKeyRN						_at_(ECRAM1+0x40);
XBYTE   EscScanCodeDelay					_at_(ECRAM1+0x41);
XWORD  	Psys								_at_(ECRAM1+0x42); //ANGELAG019: add
//XWORD  	API_ID								_at_(ECRAM1+0x43); 

XBYTE   SMbusFailCnt4                      	_at_(ECRAM1+0x44);//Start Shipmode disable 5s loop once 

XBYTE	Chk_Trickle_Current_status			_at_(ECRAM1+0x45);
XBYTE	Chk_Trickle_Current_count			_at_(ECRAM1+0x46);
XWORD	Chk_Trickle_Current_Now_Current		_at_(ECRAM1+0x47); //0x47~0x48
XWORD	Chk_Trickle_Current_Chg_Current		_at_(ECRAM1+0x49); //0x49~0x4A
XBYTE  	Get_Batt_debounce_count				_at_(ECRAM1+0x4B);
XBYTE  	Get_Batt_debounce_hash1				_at_(ECRAM1+0x4C);
XBYTE  	Get_Batt_debounce_hash2				_at_(ECRAM1+0x4D);
XBYTE   CombineKeyIO						_at_(ECRAM1+0x4E);
XBYTE   GPIO_STATUS							_at_(ECRAM1+0x4F);

XBYTE	Bat0x0BTempL						_at_(ECRAM1+0x50);
XBYTE	Bat0x0BTempH						_at_(ECRAM1+0x51);
XBYTE   Bat0x0BFakeCnt						_at_(ECRAM1+0x52);
XBYTE   Bat0x08OTPCnt						_at_(ECRAM1+0x53);
XBYTE   S3ResumeRSOC						_at_(ECRAM1+0x54);
XBYTE	BatteryAlarm						_at_(ECRAM1+0x55);
XBYTE   CHGIC_ReadCmd0x12L					_at_(ECRAM1+0x56);
XBYTE   CHGIC_ReadCmd0x12H					_at_(ECRAM1+0x57);
XBYTE   Bat0x3ETempL						_at_(ECRAM1+0x58);
XBYTE   Bat0x3ETempH						_at_(ECRAM1+0x59);
XBYTE	EC_BatteryStatusL					_at_(ECRAM1+0x5A);
XBYTE	EC_BatteryStatusH					_at_(ECRAM1+0x5B);
XBYTE   BattTemp							_at_(ECRAM1+0x5C);
XBYTE	BatteryOTPRelease					_at_(ECRAM1+0x5D);
XBYTE	BatteryOTP							_at_(ECRAM1+0x5E);
XBYTE	BatteryOTPShutdown					_at_(ECRAM1+0x5F);

XBYTE	FAN_Tab_OFF							_at_(ECRAM1+0x60);
XBYTE	FAN_Tab_LOW							_at_(ECRAM1+0x61);
XBYTE	FAN_Tab_Step1						_at_(ECRAM1+0x62);
XBYTE	FAN_Tab_Step2						_at_(ECRAM1+0x63);
XBYTE	FAN_Tab_HI							_at_(ECRAM1+0x64);
XBYTE	THR_PRO_OFF							_at_(ECRAM1+0x65);
XBYTE	THR_PRO_ON							_at_(ECRAM1+0x66);
XBYTE	THR_Turo_Rem						_at_(ECRAM1+0x67);
XBYTE	THR_Turo_OFF						_at_(ECRAM1+0x68);
XBYTE	THR_Tab_Shut						_at_(ECRAM1+0x69);
XBYTE	KB_S3Dly							_at_(ECRAM1+0x6A);
XBYTE	SLI_FAN_Tab_OFF						_at_(ECRAM1+0x6B);
XBYTE	SLI_FAN_Tab_LOW						_at_(ECRAM1+0x6C);
XBYTE	SLI_FAN_Tab_Step1					_at_(ECRAM1+0x6D);
XBYTE	SLI_FAN_Tab_Step2					_at_(ECRAM1+0x6E);
XBYTE	SLI_FAN_Tab_HI						_at_(ECRAM1+0x6F);

XBYTE	VFAN_Tab_OFF						_at_(ECRAM1+0x70);
XBYTE	VFAN_Tab_LOW						_at_(ECRAM1+0x71);
XBYTE	VFAN_Tab_Step1						_at_(ECRAM1+0x72);
XBYTE	VFAN_Tab_Step2						_at_(ECRAM1+0x73);
XBYTE	VFAN_Tab_HI							_at_(ECRAM1+0x74);
XBYTE	VTHR_PRO_OFF						_at_(ECRAM1+0x75);
XBYTE	VTHR_PRO_ON							_at_(ECRAM1+0x76);
XBYTE	VTHR_Turo_Rem						_at_(ECRAM1+0x77);
XBYTE	VTHR_Turo_OFF						_at_(ECRAM1+0x78);
XBYTE	VTHR_Tab_Shut						_at_(ECRAM1+0x79);


XBYTE	SLI_THR_PRO_OFF						_at_(ECRAM1+0x7B);
XBYTE	SLI_THR_PRO_ON						_at_(ECRAM1+0x7C);
XBYTE	SLI_THR_Turo_Rem					_at_(ECRAM1+0x7D);
XBYTE	SLI_THR_Turo_OFF					_at_(ECRAM1+0x7E);
XBYTE	SLI_THR_Tab_Shut					_at_(ECRAM1+0x7F);

XBYTE	inhibit2sec 						_at_(ECRAM1+0x80);
XBYTE   Battdata_ready						_at_(ECRAM1+0x81);
XWORD	TrickleChgTimeOutCnt				_at_(ECRAM1+0x82);//0x82,0x83
XWORD	FastChgTimeOutCnt					_at_(ECRAM1+0x84);//0x84,0x85
XBYTE	RSMshutdownCnt						_at_(ECRAM1+0x86);
XBYTE	ChkBattery_abnormal_status			_at_(ECRAM1+0x87);
XBYTE	ChkBattery_abnormal_count			_at_(ECRAM1+0x88);
XBYTE	LOWBATT_3TIMES 						_at_(ECRAM1+0x89);
XBYTE	Bat0x00TempL 						_at_(ECRAM1+0x8C);
XBYTE	Bat0x00TempH 						_at_(ECRAM1+0x8D);
XBYTE	Bat0x0FTempL 						_at_(ECRAM1+0x8E);
XBYTE	Bat0x0FTempH 						_at_(ECRAM1+0x8F);

//Mos: Keep ACIN_Count to judge does that ACIN status change
XBYTE	Chk_Wrong_ADP_Status				_at_(ECRAM1+0x90);
XBYTE	ACIN_FallINT_Count					_at_(ECRAM1+0x91);
XWORD 	Chk_Wrong_10ms_Count				_at_(ECRAM1+0x92);
XBYTE	Chk_ACOP_Status						_at_(ECRAM1+0x94);
XWORD	Chk_ACOP_Bat_Chg_Current			_at_(ECRAM1+0x95);
XWORD	Chk_ACOP_10ms_Count					_at_(ECRAM1+0x97);

XBYTE	Chk_Hybrid_STPP_Status						_at_(ECRAM1+0x99);
XBYTE	Chk_Hybrid_STPP_min_BattGasgauge			_at_(ECRAM1+0x9A);
XWORD 	Chk_Hybrid_STPP_Batt_Output_Power			_at_(ECRAM1+0x9B);
XWORD 	Chk_Hybrid_STPP_Batt_Output_Power_Limit		_at_(ECRAM1+0x9D);
XBYTE	BackLight_En_Delay							_at_(ECRAM1+0x9F);

XBYTE	UCS1002ID									_at_(ECRAM1+0xA0);

XWORD	Chk_Hybrid_STPP_Turboboost_Battery_Current_limit	_at_(ECRAM1+0xA2);
//ANGELAS016:s+ Change charge IC option setting.
XBYTE	CHGIC_ReadCmd0x37L						_at_(ECRAM1+0xA4);
XBYTE	CHGIC_ReadCmd0x37H						_at_(ECRAM1+0xA5);
//ANGELAS016:+e Change charge IC option setting.
XWORD	ADPI_Data[4]							_at_(ECRAM1+0xA8);//0xA8~0xAF
//ANGELAS016:s+ Change charge IC option setting.
XBYTE   chargerInitIndex						_at_(ECRAM1+0xB0);
XBYTE	CHGIC_ReadCmd0x3BL						_at_(ECRAM1+0xB1);
XBYTE	CHGIC_ReadCmd0x3BH						_at_(ECRAM1+0xB2);	
XBYTE	CHGIC_ReadCmd0x3CL						_at_(ECRAM1+0xB3);
XBYTE	CHGIC_ReadCmd0x3CH						_at_(ECRAM1+0xB4);
XBYTE	CHGIC_ReadCmd0x3DL						_at_(ECRAM1+0xB5);
XBYTE	CHGIC_ReadCmd0x3DH						_at_(ECRAM1+0xB6);
XBYTE	CHGIC_ReadCmd0x39L						_at_(ECRAM1+0xB7);
XBYTE	CHGIC_ReadCmd0x39H						_at_(ECRAM1+0xB8);
XBYTE	CHGIC_WriteCmd0x38L						_at_(ECRAM1+0xB9);
XBYTE	CHGIC_WriteCmd0x38H						_at_(ECRAM1+0xBA);
XBYTE	CHGIC_WriteCmd0x37L						_at_(ECRAM1+0xBB);
XBYTE	CHGIC_WriteCmd0x37H						_at_(ECRAM1+0xBC);
XBYTE	CHGIC_WriteCmd0x12L						_at_(ECRAM1+0xBD);
XBYTE	CHGIC_WriteCmd0x12H						_at_(ECRAM1+0xBE);
XBYTE   CHGIC_ReadCmd0x3DH_Temp                 _at_(ECRAM1+0xBF); //COKEYXU049:add
//ANGELAS016:+e Change charge IC option setting.
XWORD	XWTemp1								_at_(ECRAM1+0xC0);
XWORD	OCPCapacityRelease					_at_(ECRAM1+0xC2);
XWORD   OCPCapacity							_at_(ECRAM1+0xC4);
XBYTE	ECBIOSVersionH						_at_(ECRAM1+0xC6);
XBYTE	ECBIOSVersionL						_at_(ECRAM1+0xC7);
XBYTE   ECBIOSUbSVersion					_at_(ECRAM1+0xC8);

XBYTE   ADPIDON10MS_NUM						_at_(ECRAM1+0xC9); 
XBYTE   TEST_FLAG					        _at_(ECRAM1+0xCA);
XBYTE   AdapterIDOn_Flag                    _at_(ECRAM1+0xCB);  //MEILING001:add.
XBYTE   ResetTestFlag               		_at_(ECRAM1+0xCC);	 //MEILING009:add.
XBYTE   CPUProchotONCnt               		_at_(ECRAM1+0xCD);  //MEILING033:add. //MEILING055:modify ProchotONCnt name.
XBYTE   GPUProchotDelay              		_at_(ECRAM1+0xCE);  //MEILING042:add.


XBYTE	ADPI2Sec							_at_(ECRAM1+0xCF);

XWORD	TurboBoostCP						_at_(ECRAM1+0xD0);
XWORD	TurboBoostEn						_at_(ECRAM1+0xD2);
XWORD	TurboBoostthrottlEn					_at_(ECRAM1+0xD4);
XWORD	TurboBoostthrottlDis				_at_(ECRAM1+0xD6);
XWORD	TurboBoostDis						_at_(ECRAM1+0xD8);
XWORD   ACModeSTPPEn						_at_(ECRAM1+0xDA);
XWORD   ACModeSTPPDis						_at_(ECRAM1+0xDC);
XWORD	ADPI_AvgData						_at_(ECRAM1+0xDE);

XBYTE   POWER_FLAG1        	 				_at_(OEMRAM1+0xE0);	// EC power flag 1
XBYTE   BT1_STATUS1         				_at_(OEMRAM1+0xE1); // Battery 1 status1 flag
XBYTE   EVT_STATUS1         				_at_(OEMRAM1+0xE2); // Event status flag 1

XWORD   PSW_COUNTER         				_at_(OEMRAM1+0xE3); // (word)

//XBYTE	DSxPowState							_at_(OEMRAM1+0xE5); // (byte)
XBYTE	DS3PowSeqStep			    		_at_(OEMRAM1+0xE6);
XWORD   DS3PowSeqDelay						_at_(OEMRAM1+0xE7); // (word)
XBYTE   DRAMRST_CNTRL_CNT					_at_(ECRAM1+0xE9);
//XBYTE   SUSACK_HI_CNT					_at_(ECRAM1+0xEC);
XBYTE	DRAMRST_CNTRL_LOW_CNT				_at_(ECRAM1+0xED);
XBYTE   S3S4DelayCnt						_at_(ECRAM1+0xEE); //ANGELAS063:add
XBYTE   CPUThrottlingDelayTime              _at_(ECRAM1+0xEF); //MEILING052:add.

//===============================================================================
// OEMRAM1 Offset 0xF0 ~ 0xFF   Event debounce counter
XBYTE	DEBOUNCE_CONT1						_at_(DebounceRAM+0x00); //(byte)
XBYTE	DEBOUNCE_CONT2						_at_(DebounceRAM+0x01); //(byte)
XBYTE	DEBOUNCE_CONT3						_at_(DebounceRAM+0x02); //(byte)
XBYTE	DEBOUNCE_CONT4						_at_(DebounceRAM+0x03); //(byte)
XBYTE	DEBOUNCE_CONT5						_at_(DebounceRAM+0x04); //(byte)
XBYTE	DEBOUNCE_CONT6						_at_(DebounceRAM+0x05); //(byte)
XBYTE	DEBOUNCE_CONT7						_at_(DebounceRAM+0x06); //(byte)
XBYTE	DEBOUNCE_CONT8						_at_(DebounceRAM+0x07); //(byte)
XBYTE	DEBOUNCE_CONT9						_at_(DebounceRAM+0x08); //(byte)
XBYTE	DEBOUNCE_CONT10						_at_(DebounceRAM+0x09); //(byte)
XBYTE	DEBOUNCE_CONT11						_at_(DebounceRAM+0x0A); //(byte)
XBYTE	DEBOUNCE_CONT12						_at_(DebounceRAM+0x0B); //(byte)
XBYTE	DEBOUNCE_CONT13						_at_(DebounceRAM+0x0C); //(byte)
XBYTE	DEBOUNCE_CONT14						_at_(DebounceRAM+0x0D); //(byte)
XBYTE	DEBOUNCE_CONT15						_at_(DebounceRAM+0x0E); //(byte)
XBYTE	DEBOUNCE_CONT16						_at_(DebounceRAM+0x0F); //(byte)

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x200-0x2FF   OEM RAM 2
//-------------------------------------------------------------------------------
//*******************************************************************************

XBYTE	pProject0						_at_(OEMRAM2+0x02);
XBYTE	pDevStus						_at_(OEMRAM2+0x03);
//ANGELAS107:S-Add code for GBSI function.
//ANGELAS033:Add start
//XBYTE	batteryFirstUsedDateL			_at_(OEMRAM2+0x04);
//XBYTE	batteryFirstUsedDateH		_at_(OEMRAM2+0x05);
//ANGELAS033:Add end
//ANGELAS107:E-Add code for GBSI function.
XBYTE  batteryChemistry[4]              _at_(OEMRAM2+0x06);//ANGELAS107:Add code for GBSI function.
XBYTE	pOneKeyAPPStatus				_at_(OEMRAM2+0x0B);	// Lenovo_VGATurbo
XBYTE 	pPROCHOT						_at_(OEMRAM2+0x0C);
XBYTE	pProject4					   _at_(OEMRAM2+0x0D);//Y7JERRY085:Add CMD for press power button 4s don't shutdown when flash bios.

XBYTE 	SMartNoise						_at_(OEMRAM2+0x11);
 XBYTE	uPGID                           _at_(OEMRAM2+0x14);

XBYTE	uMBID							_at_(OEMRAM2+0x15);
XBYTE	MBID_READY						_at_(OEMRAM2+0x16); 
XBYTE	uMBGPU							_at_(OEMRAM2+0x17);
XBYTE	uMBpw							_at_(OEMRAM2+0x18);
XBYTE   uPJID                          	_at_(OEMRAM2+0x19);

XBYTE	BAT1_MD_1			            _at_(OEMRAM2+0x20);
XBYTE	BAT1_MD_2			            _at_(OEMRAM2+0x21);
XBYTE	BAT1_MD_3			            _at_(OEMRAM2+0x22);
XBYTE	BAT1_MD_4			            _at_(OEMRAM2+0x23);
XBYTE	BAT1_MD_5			            _at_(OEMRAM2+0x24);
XBYTE	BAT1_MD_6			            _at_(OEMRAM2+0x25);
XBYTE	BAT1_MD_7			            _at_(OEMRAM2+0x26);
XBYTE	BAT1_MD_8			            _at_(OEMRAM2+0x27);
XBYTE	BAT1_MD_9			            _at_(OEMRAM2+0x28);
XBYTE	BAT1_MD_A			            _at_(OEMRAM2+0x29);
XBYTE	BAT1_MD_B			            _at_(OEMRAM2+0x2A);
XBYTE	BAT1_MD_C			            _at_(OEMRAM2+0x2B);
XBYTE	BAT1_MD_D			            _at_(OEMRAM2+0x2C);
XBYTE	BAT1_MD_E			            _at_(OEMRAM2+0x2D);

XBYTE  	BAT1_Bar_Code[32]               _at_(OEMRAM2+0x2E); //2E~4D//MEILING031:add.


XBYTE	StartFanClean					_at_(OEMRAM2+0x61);
XBYTE	FanCleanFull					_at_(OEMRAM2+0x62);
XBYTE	FanCleanHalt					_at_(OEMRAM2+0x63);

XBYTE	pModuleID						_at_(OEMRAM2+0x66);
XBYTE	pLastSID						_at_(OEMRAM2+0x67);
XBYTE	pLastSID2						_at_(OEMRAM2+0x68);
XBYTE	pLastSID3						_at_(OEMRAM2+0x69);
XBYTE	pLastSID4						_at_(OEMRAM2+0x6A);

XBYTE	cPacketPowerLimit1				_at_(OEMRAM2+0x82);
XWORD	KeepBattRemineCap				_at_(OEMRAM2+0x83);
XBYTE	ChkBattery_FCCchg_count			_at_(OEMRAM2+0x85);
XBYTE	ChkBattery_FCCchg_lastFCCL		_at_(OEMRAM2+0x86);
XBYTE	ChkBattery_FCCchg_lastFCCH		_at_(OEMRAM2+0x87);
XBYTE	CountSecAfterPswPressed			_at_(OEMRAM2+0x88);
XBYTE	CountSecAfterNOVOPressed		_at_(OEMRAM2+0x89);

XBYTE	pVCompareFlag					_at_(OEMRAM2+0x8A);
XWORD	NTC_V2							_at_(OEMRAM2+0x8B);


XBYTE	P80CMOS[7]                      _at_(OEMRAM2+0x90);//from 0x90-0x9E
XBYTE	P80Index                        _at_(OEMRAM2+0x97);
XBYTE	P80CMOSSts                      _at_(OEMRAM2+0xA0);

XWORD   API_ID                          _at_(OEMRAM2+0xb0);

XBYTE	CombineKeyShip                  _at_(OEMRAM2+0xb3); //ANGELAS032: add
XBYTE	CombineKeyShip1                 _at_(OEMRAM2+0xb4); //ANGELAS056: add
LWORD	CalcBatRCC                      _at_(OEMRAM2+0xb5); //ANGELAS101:Modify battery discharge when battery RSOC is 59% in storage mode.

 //MEILING002: add start
XBYTE 	PMIC_counter        			_at_(OEMRAM2+0xC5);
XBYTE 	OTP_TEMP        				_at_(OEMRAM2+0xC6);
XBYTE 	PowerInitOK        				_at_(OEMRAM2+0xC7);
XBYTE 	PowerInitErrorCMD        		_at_(OEMRAM2+0xC8);
XBYTE 	PowerInitErrorDATA        		_at_(OEMRAM2+0xC9);
XBYTE 	PowerInitError        			_at_(OEMRAM2+0xCA);
XBYTE 	PowerInitIndex        			_at_(OEMRAM2+0xCB);
XBYTE 	PowerReturnData 				_at_(OEMRAM2+0xCC);

XBYTE 	PowerVENDODID 					_at_(OEMRAM2+0xCD);
XBYTE 	PowerREVID 						_at_(OEMRAM2+0xCE);
XBYTE 	PowerSDWN 						_at_(OEMRAM2+0xCF);
XBYTE 	PowerEN_RW 						_at_(OEMRAM2+0xD0);
XBYTE 	PowerEN_MASK 					_at_(OEMRAM2+0xD1);
XBYTE 	PowerEN_DEGL_SEL1 				_at_(OEMRAM2+0xD2);
XBYTE 	PowerEN_DEGL_SEL2 				_at_(OEMRAM2+0xD3);
XBYTE 	PowerPG_STATE 					_at_(OEMRAM2+0xD4);
XBYTE 	PowerOVP_REG 					_at_(OEMRAM2+0xD5);
XBYTE 	PowerUVP_REG 					_at_(OEMRAM2+0xD6);
XBYTE 	PowerOCP_REG 					_at_(OEMRAM2+0xD7);
XBYTE 	PowerOTP_REG 					_at_(OEMRAM2+0xD8);
XBYTE 	PowerBUCK1CTRL 					_at_(OEMRAM2+0xD9);
XBYTE	PowerBUCK2CTRL 					_at_(OEMRAM2+0xDA);
XBYTE 	PowerBUCK3CTRL 					_at_(OEMRAM2+0xDB);
XBYTE 	PowerLDO1CTRL 					_at_(OEMRAM2+0xDC);
XBYTE 	PowerLDO2CTRL 					_at_(OEMRAM2+0xDD);
XBYTE 	PowerDISCHCTRL1 				_at_(OEMRAM2+0xDE);
XBYTE 	PowerDISCHCTRL2 				_at_(OEMRAM2+0xDF);
XBYTE 	PowerOC_CTRL 					_at_(OEMRAM2+0xE0);
XBYTE 	PowerBUCK_FSW 					_at_(OEMRAM2+0xE1);
//MEILING002: add end

XBYTE 	CellCount						_at_(OEMRAM2+0xE5); //MEILING017: add

XBYTE 	BAT_LED_Cnt_ON       			_at_(OEMRAM2+0xE6);  //MEILING030:add.
XBYTE 	BAT_LED_Cnt_OFF      			_at_(OEMRAM2+0xE7);  //MEILING030:add.
XBYTE 	BAT_LOW_LED_Cnt      			_at_(OEMRAM2+0xE8);  //MEILING030:add.
//ANGELAG056: add start
XWORD	Psys_SUM_500ms					_at_(ECRAM2+0xE9); //E9-EA
XBYTE 	Psys_SUM_500ms_Cnt      		_at_(OEMRAM2+0xEB);  
XBYTE 	System_Status      				_at_(OEMRAM2+0xEC);
//ANGELAG056: add end

XBYTE 	LENOVOBATT2      			_at_(OEMRAM2+0xF0);  //ANGELAG017: add
//ANGELAG046: add start
XWORD	Psys_Data[4]					_at_(ECRAM2+0xF1); //0xf1~0xf8
XWORD	Psys_AvgData					_at_(ECRAM2+0xFB); //fb-fc
XBYTE 	Psys_Counter      			_at_(OEMRAM2+0xFD);  
//ANGELAG046: add end
XBYTE 	Fan_full_status      			_at_(OEMRAM2+0xFE);  //ANGELAG054: add


//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x300-0x3FF   OEM RAM 3
//-------------------------------------------------------------------------------
//*******************************************************************************
XBYTE	SYS_MISC1							_at_(OEMRAM3+0x00);
XBYTE 	PWM_LEDBeep_CNT						_at_(OEMRAM3+0x01);
XBYTE 	AutoTimer							_at_(OEMRAM3+0x02);

XBYTE 	QEVENT_DEBUG					    _at_(OEMRAM3+0x05); 

XBYTE 	SCI_Event_In_Index					_at_(OEMRAM3+0x06);
XBYTE 	SCI_Event_Out_Index					_at_(OEMRAM3+0x07);
XBYTE 	SCI_Event_Buffer[EVENT_BUFFER_SIZE]	_at_(OEMRAM3+0x08);
XWORD 	ADP_I								_at_(OEMRAM3+0x10);
XWORD 	NTC_V1								_at_(OEMRAM3+0x12);
XWORD	NOVO_COUNTER 						_at_ OEMRAM3+0x14;// Hang check//JERRYCH003:Enable SMI for hang debug.
//XWORD 	CPU_I							_at_(OEMRAM3+0x14);
//XWORD 	SLI_I							_at_(OEMRAM3+0x16);
//XWORD 	SD_PWR_EN						_at_(OEMRAM3+0x16);  //MEILING001:remove.
XBYTE 	LV_Authen_Step_CNT					_at_(OEMRAM3+0x18);
/*-----------------------------------------------------------------
; {LV_Authen_Step_CNT}
; B7 - Authentication Success
; B6 - Authentication Fail
; B5~B4 - Authentication Step from 01 to 03, 00=>stop
; B3~B0 - counter for 10 times
-----------------------------------------------------------------*/
XBYTE Bat1_FPChgFlag						_at_(OEMRAM3+0x19);
/*-----------------------------------------------------------
; {Bat1_FPChgFlag} Battery 1 Fast & Pre Charging Flag
; B7 - Reserved
; B6 - Reserved
; B5 - Fast-Charge Time-Out
; B4 - Reserved
; B3 - Reserved
; B2 - Reserved
; B1 - Pre-Charge Time-Out
; B0 - Charge SMBus Failed
------------------------------------------------------------*/
XWORD Auto_mode_AVG							_at_(OEMRAM3+0x1D);
XBYTE Auto_mode_EnPtr						_at_(OEMRAM3+0x1F);
/*-----------------------------------------------------------------
; {Auto_mode_EnPtr}
; B07 - reserved
; B06 - Over 7A
; B05 - ADC error
; B04~B00 - Counter
-----------------------------------------------------------------*/
XWORD Auto_mode[20]							_at_(OEMRAM3+0x20);

XBYTE PM1PendingTXCount						_at_(OEMRAM3+0x50);
XBYTE PM1PendingRXCount						_at_(OEMRAM3+0x51);
XBYTE PM1DataPending[6]						_at_(OEMRAM3+0x52);
XBYTE PM2PendingTXCount						_at_(OEMRAM3+0x58);
XBYTE PM2PendingRXCount						_at_(OEMRAM3+0x59);
XBYTE PM2DataPending[6]						_at_(OEMRAM3+0x5A);

XBYTE	uCritBattWakeThre					_at_(OEMRAM3+0x63);
XBYTE	uIffsCnt							_at_(OEMRAM3+0x64);

XBYTE	uISCT								_at_(OEMRAM3+0x70);
XBYTE	uISCT_2								_at_(OEMRAM3+0x71);


XWORD   ShipModeACK                        	_at_(OEMRAM3+0x75);//Start Shipmode disable 5s loop once 20130608 20:28 
XBYTE   ShipModeCnt                         _at_(OEMRAM3+0x77);//Start Shipmode disable 5s loop once 20130608 20:28 

XBYTE   ShipModeEn                          _at_(OEMRAM3+0x79);  
XBYTE ChkBattery_FCCchg_count2				_at_(OEMRAM3+0x8b); 
XBYTE   PCH_ColdBoot_TimeOut                _at_(OEMRAM3+0x8C); //ANGELAG012:add.
XBYTE	disable_turbo_counter                     _at_(OEMRAM3+0x8D); //ANGELAG050: add
//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x400-0x4FF   OEM RAM 4
//-------------------------------------------------------------------------------
//*******************************************************************************
XBYTE	nVPC_CMD		 					_at_(NameSpace+0x00);		// Customer EC name space
XBYTE	nVPC_DATA							_at_(NameSpace+0x01);		// Customer EC name space
XBYTE	VPC_iStatus 						_at_(NameSpace+0x02);		// Customer EC name space
XBYTE	CRESERVED03							_at_(NameSpace+0x03);		// Customer EC name space
XBYTE	CRESERVED04 						_at_(NameSpace+0x04);		// Customer EC name space
XBYTE	CRESERVED05							_at_(NameSpace+0x05);		// Customer EC name space
XBYTE	nAtmFanSpeed 						_at_(NameSpace+0x06);		// Fan speed. For Lenovo ATM
XBYTE	LENOVOBATT							_at_(NameSpace+0x07);		//
XBYTE	nACBriTableLimit					_at_(NameSpace+0x08);		// AC Brightness table limit
XBYTE	nDCBriTableLimit					_at_(NameSpace+0x09);		// DC brightness table limit
XBYTE	LENOVOPMFW 							_at_(NameSpace+0x0A);		//
XBYTE	SWITCHABLE							_at_(NameSpace+0x0B);		//
XBYTE	LENOVODEVICE						_at_(NameSpace+0x0C);		//
XBYTE	nRamTemp							_at_(NameSpace+0x0D);		// Ram temperature
XBYTE	nVramTemp 							_at_(NameSpace+0x0E);		// V Ram temperature
XBYTE	ATM_CMD								_at_(NameSpace+0x0F);		// ATM Fan control command

XBYTE	EM7FUNCTION							_at_(NameSpace+0x10);	// 0x10
XBYTE	BATTUPDATEFW						_at_(NameSpace+0x11);	// 0x11
//	#define  PriBattInhib	(1<<0)	// bit0 1-Disable reading information from primary battery for update battery firmware
//	#define  SecBattInhib	(1<<1)	// bit1 1-Disable reading information from second battery for update battery firmware.

XBYTE	 FirmwareUpdateStatusL				_at_(NameSpace+0x12);	// 0x12
XBYTE	 FirmwareUpdateStatusH				_at_(NameSpace+0x13);	// 0x13
XBYTE	 BatterymakerIDL					_at_(NameSpace+0x14);	// 0x14
XBYTE	 BatterymakerIDH					_at_(NameSpace+0x15);	// 0x15
XBYTE	 HardwareIDL						_at_(NameSpace+0x16);	// 0x16
XBYTE	 HardwareIDH						_at_(NameSpace+0x17);	// 0x17
XBYTE	 FirmwareversionL					_at_(NameSpace+0x18);	// 0x18
XBYTE	 FirmwareversionH					_at_(NameSpace+0x19);	// 0x19
XBYTE	 DataVersionL						_at_(NameSpace+0x1A);	// 0x1A
XBYTE	 DataVersionH						_at_(NameSpace+0x1B);	// 0x1B
/*
0x37 (Get Battery Information ) 	Read Block
byte 0,1: The battery maker ID
byte 2,3: Hardware ID
byte 4,5: Firmware version (0 if the firmware is corrupted
byte 6,7: Data version (0 if this is not supported or the firmware is corrupted.
*/
XBYTE	 FBClamp							_at_(NameSpace+0x1C);	// 0x1C
XBYTE	 CRESERVED1D						_at_(NameSpace+0x1D);	// 0x1D
XBYTE	 CRESERVED1E						_at_(NameSpace+0x1E);	// 0x1E
XBYTE	 CRESERVED1F						_at_(NameSpace+0x1F);	// 0x1F
XBYTE	 CRESERVED20						_at_(NameSpace+0x20);	// 0x20
XBYTE	 CRESERVED21						_at_(NameSpace+0x21);	// 0x21
XBYTE	 CRESERVED22						_at_(NameSpace+0x22);	// 0x22
XBYTE	 CRESERVED23						_at_(NameSpace+0x23);	// 0x23
XBYTE	 CRESERVED24						_at_(NameSpace+0x24);	// 0x24
XBYTE	 CRESERVED25						_at_(NameSpace+0x25);	// 0x25
XBYTE	 CRESERVED26						_at_(NameSpace+0x26);	// 0x26
XBYTE	 CRESERVED27						_at_(NameSpace+0x27);	// 0x27
XBYTE	 CRESERVED28						_at_(NameSpace+0x28);	// 0x28
XBYTE	 CRESERVED29						_at_(NameSpace+0x29);	// 0x29
XBYTE	 CRESERVED2A						_at_(NameSpace+0x2A);	// 0x2A
XBYTE	 CRESERVED2B						_at_(NameSpace+0x2B);	// 0x2B
XBYTE	 CRESERVED2C						_at_(NameSpace+0x2C);	// 0x2C

XBYTE	 PANEL_INFO[16]	    				_at_(NameSpace+0x30);	//ANGELAG008: modify 22 to 16 // 0x30~0x46
XBYTE    QEVENTFlag                         _at_(NameSpace+0x40);	// 0x40
XBYTE	 pDevStatus1 			            _at_(NameSpace+0x43);  // 0x43 //ANGELAG008: add
XBYTE EMStatusBit2                          _at_(NameSpace+0x4A);	// 0x4A //ANGELAS032: add
//ANGELAS107:S+Add code for GBSI function.
XBYTE	batteryFirstUsedDateL			    _at_(NameSpace+0x4C);
XBYTE	batteryFirstUsedDateH			    _at_(NameSpace+0x4D);
//ANGELAS107:E+Add code for GBSI function.
XBYTE	 ISCT_Timer							_at_(NameSpace+0x5A);	// 0x5A

// External name space 0x5D ~ 0x5F
XBYTE	 EXT_NAMESPACE_INDEX 				_at_(NameSpace+0x5D);	// 0x5D
XBYTE	 EXT_NAMESPACE_BANK	 				_at_(NameSpace+0x5E);	// 0x5E
XBYTE	 EXT_NAMESPACE_DATA	 				_at_(NameSpace+0x5F); // 0x5F

// SMBus EC interface ACPI RAM definition 0x60 ~ 0x87
// #define SMBUS_Base_Address    0x60                       //Set SMBUS base address from Protocol
XBYTE	SMB_PRTC							_at_ (NameSpace+0x60);	// 0x60,
XBYTE	SMB_STS								_at_ (NameSpace+0x61);	// 0x61,
XBYTE	SMB_ADDR							_at_ (NameSpace+0x62);	// 0x62,
XBYTE	SMB_CMD								_at_ (NameSpace+0x63);	// 0x63,
XBYTE	SMB_DATA							_at_ (NameSpace+0x64);	// 0x64,
//XBYTE SMB_DATA1[31]						_at_ (NameSpace+0x65);
XBYTE	SMB_BCNT							_at_ (NameSpace+0x84);	// 0x84,
// SBS Only Alarm registers
XBYTE	SMB_ALRA							_at_ (NameSpace+0x85);	// 0x85,
XBYTE	SMB_ALRD0							_at_ (NameSpace+0x86);	// 0x86,
XBYTE	SMB_ALRD1							_at_ (NameSpace+0x87);	// 0x87,

// Battery Information reading 0x90 ~ 0x9F
//XBYTE	 EXT_NAMESPACE_DATA[BATTMANUFACTURE]	 _at_(ECRAM1+0x60); // 0x60

//#define BATTMANUFACTURE			0x30				// (9 BYTES) Battery ManufactureName, reserve 9 byte
//    #define nManufact(x)		SMB_ACPI[x-BATTMANUFACTURE]	// Only support main battery so far, 0x90~0x98
//#define BATTDEVICENAME			0x27				// (7 BYTES) Battery DeviceName, reserve 7 byte
//    #define nDeviceName(x)	SMB_ACPI[x-BATTDEVICENAME]	// Only support main battery so far, 0x99~0x9F

XBYTE  	nBattery0x16L      					_at_(NameSpace+0x88);
XBYTE	nBattery0x16H						_at_(NameSpace+0x89);
XBYTE	EMStatusBit							_at_(NameSpace+0x8A);
XBYTE	TP_Type                           	_at_(NameSpace+0x8B);

XBYTE   OEMControl                          _at_(NameSpace+0x8C);  //ANGELAG005:add.

XBYTE 	BATTMANUFACTURE[9]					_at_(NameSpace+0x8F);		// (9 BYTES) Battery ManufactureName, reserve 9 byte
XBYTE	BATTDEVICENAME[8]					_at_(NameSpace+0x98);		// Only support main battery so far, 0x98~0x9F


XBYTE	AOAC_STATUS							_at_(NameSpace+0xA0);		//
XBYTE	ACPI_HOTKEY							_at_(NameSpace+0xA1);		//
XBYTE	nOSShutdownTemp2 					_at_(NameSpace+0xA2);		// OS Shutdown Temp. For system read setting. (DTS)
XBYTE	SYS_STATUS							_at_(NameSpace+0xA3);		//
XBYTE	WAKEUP_ENABLE 						_at_(NameSpace+0xA4);		//
XBYTE	ACOFF_SOURCE						_at_(NameSpace+0xA5);		//
XBYTE	CURRENT_STATUS 						_at_(NameSpace+0xA6);		// A6
XBYTE	nOSThrottlingTemp					_at_(NameSpace+0xA7);		// OS Throttling Temp. For system read setting.
XBYTE	nOSShutdownTemp						_at_(NameSpace+0xA8);		// Throttling Temp put a temp in for system throttling.
XBYTE	nThrottlingAPSet					_at_(NameSpace+0xA9);
XBYTE	TcontrolTemp 						_at_(NameSpace+0xAA);		// For Tcontrol temp, sysbios will put Tcontrol temp
XBYTE	THERMAL_STATUS						_at_(NameSpace+0xAB);		// HW throttling define
XBYTE	nShutDownTemp						_at_(NameSpace+0xAC);		// Shutsown Temperature
XBYTE	nFanStatus1							_at_(NameSpace+0xAD);		// Just for utility (low nibble: Fan's speed step number ; high nibble: Fan's number )
XBYTE	REAL_THROTTLING_INDEX 				_at_(NameSpace+0xAE);
XBYTE	THROTTLING_HW						_at_(NameSpace+0xAF);		// HW throttling define
XBYTE	nCpuTemp 							_at_(NameSpace+0xB0);		// CPU current temperature
XBYTE	nCpuTempLocal						_at_(NameSpace+0xB1);		// CPU local temperature
XBYTE	nSKINTemp90							_at_(NameSpace+0xB2);		// SKIN temperature address 90
//XBYTE	nOSShutdownTemp3				_at_(NameSpace+0xB3);		// OS Shutdown Temp. For system read setting.(VGA) //ANGELAS070:remove
XBYTE	ThermistorCPU_TEMP				    _at_(NameSpace+0xB3);		//For system read setting.(VGA)//ANGELAS070:add
XBYTE	VGA_TEMP							_at_(NameSpace+0xB4);		// VGA temperature
XBYTE	EXTVGA_TEMP							_at_(NameSpace+0xB5);
XBYTE	nNBTemp								_at_(NameSpace+0xB6);		// NorthBridge temperature
XBYTE	ACPI_STAT							_at_(NameSpace+0xB7);
XBYTE	SWI_EVENT							_at_(NameSpace+0xB8);		// SWI Event indicators
XBYTE	nBrightValue						_at_(NameSpace+0xB9);		// LCD Brightness value(0-0x09)

XBYTE	GPU_REAL_THROTTLING_INDEX 		    _at_(NameSpace+0xBA);  //MEILING033:add.

XBYTE	DEVICEMODULE 						_at_(NameSpace+0xBB);
XBYTE	nKbProjectID 						_at_(NameSpace+0xBC);
XBYTE	CPU_TYPE 							_at_(NameSpace+0xBD);
XBYTE	SEL_STATE0 							_at_(NameSpace+0xBE);
XBYTE	SEL_STATE1 							_at_(NameSpace+0xBF);
XBYTE	nBatteryStatL 						_at_(NameSpace+0xC0);
XBYTE	nBatteryStatH 						_at_(NameSpace+0xC1);
XBYTE	nRemainingCapL 						_at_(NameSpace+0xC2);		// (WORD) Remaining Capacity
XBYTE	nRemainingCapH 						_at_(NameSpace+0xC3);
XBYTE	nSerialNumL 						_at_(NameSpace+0xC4);		// (WORD) Serial number
XBYTE	nSerialNumH 						_at_(NameSpace+0xC5);
XBYTE	nPresentVoltL 						_at_(NameSpace+0xC6);		// (WORD) Present voltage
XBYTE	nPresentVoltH 						_at_(NameSpace+0xC7);
XBYTE	nDesignVoltL 						_at_(NameSpace+0xC8);		// (WORD) Design voltage
XBYTE	nDesignVoltH 						_at_(NameSpace+0xC9);
XBYTE	nDesignCapL 						_at_(NameSpace+0xCA);		// (WORD) Design Capacity
XBYTE	nDesignCapH 						_at_(NameSpace+0xCB);
XBYTE	nFullChgCapL 						_at_(NameSpace+0xCC);		// (WORD) Full charge Capacity
XBYTE	nFullChgCapH 						_at_(NameSpace+0xCD);
XBYTE	nBattGasgauge 						_at_(NameSpace+0xCE);		// (BYTE) Battery percentage
XBYTE	nCycleCounter 						_at_(NameSpace+0xCF);		// (BYTE) Battery cycle counter
XBYTE	nNowCurrentL 						_at_(NameSpace+0xD0);		// (WORD) Battery current
XBYTE	nNowCurrentH 						_at_(NameSpace+0xD1);
XBYTE	nAvgCurrentL 						_at_(NameSpace+0xD2);		// (WORD) Battery average current
XBYTE	nAvgCurrentH 						_at_(NameSpace+0xD3);
XBYTE	nBattComsup 						_at_(NameSpace+0xD4);		// (BYTE) System power comsumption,Watt will not over 255. So just only need 1 byte.
XBYTE	nBattVolt 							_at_(NameSpace+0xD5);		// (BYTE) Battery Volt
XBYTE	nBattTsTemp 						_at_(NameSpace+0xD6);		// (BYTE) Battery Temp.
XBYTE	nBattAverTemp 						_at_(NameSpace+0xD7);		// (BYTE) Battery Average Temp.
XBYTE	nBattCharCurrentL 					_at_(NameSpace+0xD8);		// (WORD) Battery charge currrent.
XBYTE	nBattCharCurrentH 					_at_(NameSpace+0xD9);
XBYTE	nBattTempCnt 						_at_(NameSpace+0xDA);		// (BYTE) Battery current Temp sample counter
XBYTE	nBattCmdIdx 						_at_(NameSpace+0xDB);		// (BYTE) Battery Command index for read battery through SMBus
XBYTE	nBattGetBattCnt 					_at_(NameSpace+0xDC);		// (BYTE) Count up to communicate battery
XBYTE	nBattOvrTempCnt 					_at_(NameSpace+0xDD);		// (BYTE) Count up if battery is arrived overtemp.
XBYTE	nBattStatusSMBL 					_at_(NameSpace+0xDE);		// (WORD) Battrery SMBus status
XBYTE	nBattStatusSMBH 					_at_(NameSpace+0xDF);		//
XBYTE	nBattOvrVoltCnt 					_at_(NameSpace+0xE0);		// (BYTE) Battery over voltage counter
XBYTE	nBattErrorCnt 						_at_(NameSpace+0xE1);		// (BYTE) Communicatiion fail counter
XBYTE	nBattTsVolt 						_at_(NameSpace+0xE2);		// (BYTE) Battery voltage of ADC
XBYTE	nCell1VoltL							_at_(NameSpace+0xE3);
XBYTE	nCell1VoltH							_at_(NameSpace+0xE4);
XBYTE	nCell2VoltL							_at_(NameSpace+0xE5);
XBYTE	nCell2VoltH							_at_(NameSpace+0xE6);
XBYTE	nCell3VoltL							_at_(NameSpace+0xE7);
XBYTE	nCell3VoltH							_at_(NameSpace+0xE8);
XBYTE	nCell4VoltL							_at_(NameSpace+0xE9);
XBYTE	nCell4VoltH							_at_(NameSpace+0xEA);
XBYTE	nManufactureAccessL					_at_(NameSpace+0xEB);
XBYTE	nManufactureAccessH					_at_(NameSpace+0xEC);
XBYTE	SHIPMODE_L							_at_(NameSpace+0xED);
XBYTE	SHIPMODE_H							_at_(NameSpace+0xEE);
XBYTE	RESERVEDEF							_at_(NameSpace+0xEF);
XBYTE  	nBatteryStatus1      				_at_(NameSpace+0xF0);
XBYTE	nBatteryStatus2						_at_(NameSpace+0xF1);
XBYTE	nStopChgStat3L						_at_(NameSpace+0xF2);
XBYTE	nStopChgStat3H						_at_(NameSpace+0xF3);
XBYTE	nManufactureDateL					_at_(NameSpace+0xF4);
XBYTE	nManufactureDateH					_at_(NameSpace+0xF5);
BYTE	nChargingVoltL						_at_(NameSpace+0xF6);
XBYTE	nChargingVoltH						_at_(NameSpace+0xF7);
XBYTE	BATTCELL							_at_(NameSpace+0xF8);

//XBYTE   PECIBIOSTEST						_at_(NameSpace+0xFC);//when enter S3/S4/CB,BIOS sent B4 CMD to EC from 66port for peci fail.//: Remove  and  PECI workaround.
XBYTE	MBID_FB								_at_(NameSpace+0xFB);  //T054+
XBYTE	F2_Pressed								_at_(NameSpace+0xFC);  //ANGELAG001: add
 XBYTE	nAtmFan1Speed 					_at_(NameSpace+0xFE);//JERRYCH00:Modify the fan control from linear to the ladder.
XBYTE	EM8_TEST							_at_(NameSpace+0xFF);


//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x500-0x5FF   OEM RAM 5
//-------------------------------------------------------------------------------
//*******************************************************************************
//---------------------PECI-------------------------
XBYTE	PECI_TIMEOUT				_at_(OEMRAM5+0x0B);
XBYTE	PECI_ERROR					_at_(OEMRAM5+0x0C);
XBYTE	PECIDelayCnt				_at_(OEMRAM5+0x0D);
XWORD	PECIBuffer0					_at_(OEMRAM5+0x0E);
XWORD	PECIBuffer1					_at_(OEMRAM5+0x10);
XWORD	PECIBuffer2					_at_(OEMRAM5+0x12);
XWORD	PECIBuffer3					_at_(OEMRAM5+0x14);
XWORD	PECIBuffer4					_at_(OEMRAM5+0x16);
XWORD	PECIBuffer5					_at_(OEMRAM5+0x18);
XWORD	PECIBuffer6					_at_(OEMRAM5+0x1A);
XWORD	PECIBuffer7					_at_(OEMRAM5+0x1C);
// reserved 0xC1F
XBYTE	TJMAX						_at_(OEMRAM5+0x20);
XBYTE	PECI_Err_CNT				_at_(OEMRAM5+0x21);

XBYTE	PECI_GET_TEMPL				_at_(OEMRAM5+0x24);
XBYTE	PECI_GET_TEMPH				_at_(OEMRAM5+0x25);

XBYTE   PCHErrCnt					_at_(OEMRAM5+0x38);
XBYTE	ERR_THMSTS					_at_(OEMRAM5+0x39);
XBYTE	TMErrCnt					_at_(OEMRAM5+0x3A);
XBYTE	VGAErrCnt					_at_(OEMRAM5+0x3B);
XBYTE	EXTVGAErrCnt				_at_(OEMRAM5+0x3C);
XBYTE	TmlICStep					_at_(OEMRAM5+0x3D);
XBYTE	vgaok						_at_(OEMRAM5+0x3E);

XWORD	cWriteCurrentPL1			_at_(OEMRAM5+0x40);
XWORD	cWriteCurrentPL2			_at_(OEMRAM5+0x42);

XBYTE 	USB_Delay					_at_(OEMRAM5+0x44); 

//COKEYXU013:S+ ladder fan relative regs.
XBYTE	  Fan1RPM			       _at_ (OEMRAM5+0x46);
XBYTE	  Fan2RPM			       _at_ (OEMRAM5+0x47);



XBYTE	   Fan1On_Step1			   _at_ (OEMRAM5+0x50);
XBYTE	   Fan1On_Step2 		   _at_ (OEMRAM5+0x51);
XBYTE	   Fan1On_Step3 		   _at_ (OEMRAM5+0x52);
XBYTE	   Fan1On_Step4 		   _at_ (OEMRAM5+0x53);
XBYTE	   Fan1On_Step5 	       _at_ (OEMRAM5+0x54);
XBYTE	   Fan1On_Step6 		   _at_ (OEMRAM5+0x55);
XBYTE	   Fan1On_Step7 		   _at_ (OEMRAM5+0x56);
XBYTE	   Fan1On_Step8 		   _at_ (OEMRAM5+0x57);
XBYTE	   Fan2On_Step1 		   _at_ (OEMRAM5+0x58);
XBYTE	   Fan2On_Step2 		   _at_ (OEMRAM5+0x59);
XBYTE	   Fan2On_Step3 		   _at_ (OEMRAM5+0x5A);
XBYTE	   Fan2On_Step4 		   _at_ (OEMRAM5+0x5B);
XBYTE	   Fan2On_Step5 		   _at_ (OEMRAM5+0x5C);
XBYTE	   Fan2On_Step6 		   _at_ (OEMRAM5+0x5D);
XBYTE	   Fan2On_Step7 		   _at_ (OEMRAM5+0x5E);
XBYTE	   Fan2On_Step8 		   _at_ (OEMRAM5+0x5F);
//COKEYXU013:E+ ladder fan relative regs.


XBYTE  pDevStatus3 				_at_ (OEMRAM5 + 0x60); //ANGELAG008: add 
XBYTE   DisCPUTurboFor90s                   _at_(OEMRAM5+0x6A);   //COKEYXU029:add
XBYTE   CountOfWinAndX          _at_(OEMRAM5+0x6B);


XBYTE	MaxCPU_MCHTemp				_at_(OEMRAM5+0x6C);
// Reserve 0x056D~057F

XBYTE DisCPUTurboForLTP            _at_ (OEMRAM5+0x6D);  //COKEYXU046:add

XBYTE   CMOS_TEST					_at_(OEMRAM5+0x6F); // RTC_RST

XBYTE	EC_DeepSleep_Temp0        	_at_(OEMRAM5+0x70);
XBYTE	EC_DeepSleep_Temp1        	_at_(OEMRAM5+0x71);
XBYTE	EC_DeepSleep_Temp2        	_at_(OEMRAM5+0x72);
XBYTE	EC_DeepSleep_Temp3        	_at_(OEMRAM5+0x73);
XBYTE	EC_DeepSleep_Temp4        	_at_(OEMRAM5+0x74);
XBYTE	EC_DeepSleep_Temp5        	_at_(OEMRAM5+0x75);
XBYTE	EC_DeepSleep_Temp6        	_at_(OEMRAM5+0x76);

//MARTINH154:Add start
XBYTE cmosdelay    					_at_(OEMRAM5+0x77);
XBYTE cmosdelay1  					_at_(OEMRAM5+0x78);
XBYTE cmosshutdelay  				_at_(OEMRAM5+0x79);
//MARTINH154:Add end

XBYTE	EC_DeepSleep_TempPortA    	_at_(OEMRAM5+0x80);
XBYTE	EC_DeepSleep_TempPortB    	_at_(OEMRAM5+0x81);
XBYTE	EC_DeepSleep_TempPortC    	_at_(OEMRAM5+0x82);
XBYTE	EC_DeepSleep_TempPortD    	_at_(OEMRAM5+0x83);
XBYTE	EC_DeepSleep_TempPortE    	_at_(OEMRAM5+0x84);
XBYTE	EC_DeepSleep_TempPortF    	_at_(OEMRAM5+0x85);
XBYTE	EC_DeepSleep_TempPortG    	_at_(OEMRAM5+0x86);
XBYTE	EC_DeepSleep_TempPortH    	_at_(OEMRAM5+0x87);
XBYTE	EC_DeepSleep_TempPortI    	_at_(OEMRAM5+0x88);
XBYTE	EC_DeepSleep_TempPortJ    	_at_(OEMRAM5+0x89);


//COKEYXU013:S+ ladder fan relative regs.
XBYTE     Fan1Off_Step1   		   _at_ (OEMRAM5+0x90);
XBYTE     Fan1Off_Step2   		   _at_ (OEMRAM5+0x91);
XBYTE     Fan1Off_Step3   		   _at_ (OEMRAM5+0x92);
XBYTE     Fan1Off_Step4   		   _at_ (OEMRAM5+0x93);
XBYTE     Fan1Off_Step5   		   _at_ (OEMRAM5+0x94);
XBYTE     Fan1Off_Step6   		   _at_ (OEMRAM5+0x95);
XBYTE     Fan1Off_Step7   		   _at_ (OEMRAM5+0x96);
XBYTE     Fan1Off_Step8   		   _at_ (OEMRAM5+0x97);
XBYTE     Fan2Off_Step1  	 	   _at_ (OEMRAM5+0x98);
XBYTE     Fan2Off_Step2  		   _at_ (OEMRAM5+0x99);
XBYTE     Fan2Off_Step3  		   _at_ (OEMRAM5+0x9A);
XBYTE     Fan2Off_Step4  		   _at_ (OEMRAM5+0x9B);
XBYTE     Fan2Off_Step5 		   _at_ (OEMRAM5+0x9C);
XBYTE     Fan2Off_Step6  		   _at_ (OEMRAM5+0x9D);
XBYTE     Fan2Off_Step7  		   _at_ (OEMRAM5+0x9E);
XBYTE     Fan2Off_Step8 		   _at_ (OEMRAM5+0x9F);
//COKEYXU013:E+ ladder fan relative regs.


XBYTE    cCmd                       _at_(OEMRAM5+0xA0);
XBYTE    cKBStatus                  _at_(OEMRAM5+0xA1);
XBITS_8  cBattFlag0                 _at_(OEMRAM5+0xA2);
XBITS_8  cDev                       _at_(OEMRAM5+0xA3);
XBITS_8  cSPIEEPROM                 _at_(OEMRAM5+0xA4);
XBITS_8  cThrottlingSet				_at_(OEMRAM5+0xA5); 	// 00: Nothing, 01: Thr 12.5%, 02: Thr 25%,..,08: Thr 100%
XBYTE    cTargetGauge               _at_(OEMRAM5+0xA6);
//XBYTE    uCpuLoadCnt                   	_at_(OEMRAM5+0xA7);
XBYTE    battery_critical           _at_(OEMRAM5+0xA7);  //ANGELAS093
XBYTE    cCPUKind                   _at_(OEMRAM5+0xA8);
XBYTE    cPanelId                   _at_(OEMRAM5+0xA9);
XBYTE    cBrightUpDelayCnt          _at_(OEMRAM5+0xAA);
XBYTE    cBrightDnDelayCnt          _at_(OEMRAM5+0xAB);
XBYTE    cRecoveryBrightValue       _at_(OEMRAM5+0xAC);
XBYTE    cSysActionFlag             _at_(OEMRAM5+0xAD);
XBYTE    cADPThrottling             _at_(OEMRAM5+0xAE);
XBYTE    cTHERMThrottling           _at_(OEMRAM5+0xAF);
XBYTE    cBATTThrottling            _at_(OEMRAM5+0xB0);
XBITS_16 cSysStus                   _at_(OEMRAM5+0xB1);
XBITS_16 cBattInform                _at_(OEMRAM5+0xB3);
XBITS_16 cOsLedCtrl                 _at_(OEMRAM5+0xB5);
XBITS_16 cOsLed1Ctrl                _at_(OEMRAM5+0xB7);
XBYTE    initMark                   _at_(OEMRAM5+0xB9);		// CMW 20121203
XBYTE    TEMP_Error                 _at_(OEMRAM5+0xBA);		// CMW 20121203

XBYTE    EM9_NEWFUN                 _at_(OEMRAM5+0xBB);    
XBYTE    testtoolflag				_at_(OEMRAM5+0xBC); 
XBYTE	 Bioswatchdog				_at_(OEMRAM5+0xBD); //ANGELAS094:Add workaround for hang bios.
XBYTE 	 Bioswatchdogtime           _at_(OEMRAM5+0xBE); //ANGELAS094:Add workaround for hang bios.

//MEILING033:add start.
XBYTE	ManualFanPRM						_at_(ECRAM5+0xBF);//JERRYCH00:Modify the fan control from linear to the ladder.
XBYTE	 cGPUThermalThrottling		_at_ (OEMRAM5+0xC0);
XBYTE	 cGPUBattThrottling			_at_ (OEMRAM5+0xC1);
XBYTE    cGPUBattOTPThrottling   	_at_ (OEMRAM5+0xC2);
XBYTE    cGPUBattLowThrottling   	_at_ (OEMRAM5+0xC3);
//MEILING033:add end.

//MEILING055:S+.
XBYTE    cGPUACtoBattThrottling     _at_ (OEMRAM5+0xC4);
XBYTE    cGPUACtoBattTime       	_at_ (OEMRAM5+0xC5);
XBYTE    GPUProchotONCnt        	_at_ (OEMRAM5+0xC6);
XBYTE    cBATTLowThrottling   		_at_ (OEMRAM5+0xC7);
//MEILING055:E+.
XBYTE    cGPUBattLTPThrottling   	_at_ (OEMRAM5+0xC8); //COKEYXU046:add
XBYTE    cGPUBattPsysThrottling   	_at_ (OEMRAM5+0xCA); //ANGELAG019: add

XBYTE	 GPU_Prochot		_at_ (OEMRAM5+0xCB); //ANGELAG020: add 
XBITS_8	 cGPUThrottlingSet  		_at_ (OEMRAM5+0xCC); //JERRYCH00:Modify the fan control from linear to the ladder.
XBYTE    Adapter90WWA            _at_ (OEMRAM5+0xCD); //COKEYXU030:add

//COKEYXU013:S+ ladder fan relative regs.
XBYTE	  Fan1RPM_Step1			   _at_ (OEMRAM5+0xE0);
XBYTE 	  Fan1RPM_Step2			   _at_ (OEMRAM5+0xE1);
XBYTE 	  Fan1RPM_Step3			   _at_ (OEMRAM5+0xE2);
XBYTE 	  Fan1RPM_Step4			   _at_ (OEMRAM5+0xE3);
XBYTE 	  Fan1RPM_Step5			   _at_ (OEMRAM5+0xE4);
XBYTE 	  Fan1RPM_Step6			   _at_ (OEMRAM5+0xE5);
XBYTE 	  Fan1RPM_Step7			   _at_ (OEMRAM5+0xE6);
XBYTE     Fan1RPM_Step8			   _at_ (OEMRAM5+0xE7);
XBYTE 	  Fan2RPM_Step1			   _at_ (OEMRAM5+0xE8);
XBYTE 	  Fan2RPM_Step2			   _at_ (OEMRAM5+0xE9);
XBYTE     Fan2RPM_Step3			   _at_ (OEMRAM5+0xEA);
XBYTE     Fan2RPM_Step4			   _at_ (OEMRAM5+0xEB);
XBYTE 	  Fan2RPM_Step5			   _at_ (OEMRAM5+0xEC);
XBYTE     Fan2RPM_Step6			   _at_ (OEMRAM5+0xED);
XBYTE	  Fan2RPM_Step7 		   _at_ (OEMRAM5+0xEE);
XBYTE 	  Fan2RPM_Step8		       _at_ (OEMRAM5+0xEF);
//COKEYXU013:E+ ladder fan relative regs.



XBYTE	ManualFan2PRM			_at_(OEMRAM5+0xFA);//JERRYCH00:Modify the fan control from linear to the ladder.
//ANGELAG031: remove start
//ANGELAG022: add start
/*XBYTE    cAMDGPUPsysThrott   	_at_ (OEMRAM5+0xCC);
XBYTE    cAMDGPUBattOTPThrott   	_at_ (OEMRAM5+0xCD);
XBYTE    cAMDGPUBattOCPThrott   	_at_ (OEMRAM5+0xCE);
XBYTE    cAMDGPUACtoBattThrott     _at_ (OEMRAM5+0xCF);
XBYTE    cAMDGPUThrottCount     _at_ (OEMRAM5+0xD0);
XBYTE    cAMDGPUBattLowThrott     _at_ (OEMRAM5+0xD1);

XBYTE    DGPUdebug     _at_ (OEMRAM5+0xFF);*/
//ANGELAG022: add end
//ANGELAG031: remove end
//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x600-0x6FF   OEM RAM 6
//-------------------------------------------------------------------------------
//*******************************************************************************

/* 
XBYTE  		cCmd					_at_(OEMRAM6+0x01);

XBYTE  		cKBStatus				_at_(OEMRAM6+0x05);
XBITS_8  	cBattFlag0				_at_(OEMRAM6+0x06);
XBITS_8		cDev   					_at_(OEMRAM6+0x07);

XBITS_8  	cSPIEEPROM  			_at_(OEMRAM6+0x0F);

XBYTE 		cTargetGauge  			_at_(OEMRAM6+0x2B);
XBYTE		uCpuLoadCnt			_at_(OEMRAM6+0x2C);

XBYTE 		cCPUKind  				_at_(OEMRAM6+0x63);

XBYTE 		cPanelId 					_at_(OEMRAM6+0x77);

XBYTE 		cBrightUpDelayCnt 		_at_(OEMRAM6+0x79);
XBYTE 		cBrightDnDelayCnt 		_at_(OEMRAM6+0x7A);
XBYTE 		cRecoveryBrightValue 		_at_(OEMRAM6+0x7B);

XBYTE 		cSysActionFlag 			_at_(OEMRAM6+0x8A);

XBYTE 		cADPThrottling			_at_(OEMRAM6+0x97);
XBYTE 		cTHERMThrottling			_at_(OEMRAM6+0x98);
XBYTE 		cBATTThrottling			_at_(OEMRAM6+0x99);

XBITS_16 	cSysStus				_at_(OEMRAM6+0xC0);
XBITS_16 	cBattInform				_at_(OEMRAM6+0xC2);

XBITS_16 	cOsLedCtrl				_at_(OEMRAM6+0xCA);
XBITS_16 	cOsLed1Ctrl				_at_(OEMRAM6+0xCC);

XBYTE		initMark					_at_(OEMRAM6+0xCE);		// CMW 20121203
XBYTE		TEMP_Error				_at_(OEMRAM6+0xCF);		// CMW 20121203
XBYTE       	delayEDPTm                     _at_(OEMRAM6+0xD0);          // EDP Delay Initial Timer
XBYTE		cCHIPType				_at_(OEMRAM6+0xF8);
*/ 
//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x700-0x7FF   OEM RAM 7
//-------------------------------------------------------------------------------
//*******************************************************************************
// Keil C

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x800-0x8FF   OEM RAM 8
//-------------------------------------------------------------------------------
//*******************************************************************************
// Keyboard matrix
XBYTE	KB_RAM_Table[128]				_at_(OEMRAM8+0x00);	// 0x00
XBYTE	ExKB_RAM_Table[24]				_at_(OEMRAM8+0x80);	// 0x80

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0x900-0x9FF   OEM RAM 9
//-------------------------------------------------------------------------------
//*******************************************************************************

//COKEYXU013:S+ ladder fan relative regs.
XBYTE     Fan1RPMVGA_Step1  	   _at_ (OEMRAM9+0x00);
XBYTE     Fan1RPMVGA_Step2  	   _at_ (OEMRAM9+0x01);
XBYTE     Fan1RPMVGA_Step3  	   _at_ (OEMRAM9+0x02);
XBYTE     Fan1RPMVGA_Step4  	   _at_ (OEMRAM9+0x03);
XBYTE     Fan1RPMVGA_Step5  	   _at_ (OEMRAM9+0x04);
XBYTE     Fan1RPMVGA_Step6  	   _at_ (OEMRAM9+0x05);
XBYTE     Fan1RPMVGA_Step7  	   _at_ (OEMRAM9+0x06);
XBYTE     Fan1RPMVGA_Step8  	   _at_ (OEMRAM9+0x07);
XBYTE     Fan2RPMVGA_Step1  	   _at_ (OEMRAM9+0x08);
XBYTE     Fan2RPMVGA_Step2  	   _at_ (OEMRAM9+0x09);
XBYTE     Fan2RPMVGA_Step3  	   _at_ (OEMRAM9+0x0A);
XBYTE     Fan2RPMVGA_Step4  	   _at_ (OEMRAM9+0x0B);
XBYTE     Fan2RPMVGA_Step5 		   _at_ (OEMRAM9+0x0C);
XBYTE     Fan2RPMVGA_Step6 		   _at_ (OEMRAM9+0x0D);
XBYTE     Fan2RPMVGA_Step7  	   _at_ (OEMRAM9+0x0E);
XBYTE     Fan2RPMVGA_Step8  	   _at_ (OEMRAM9+0x0F);
XBYTE     FanLevelVGA 			   _at_ (OEMRAM9+0x10);
XBYTE     FanLevel2CPU 			   _at_ (OEMRAM9+0x11); 
XBYTE     Fan1RPMTemp 			   _at_ (OEMRAM9+0x12); 
XBYTE     Fan2RPMTemp 			   _at_ (OEMRAM9+0x13);  
XBYTE     Fan1RPMCount 			   _at_ (OEMRAM9+0x14);  
XBYTE     Fan2RPMCount 			   _at_ (OEMRAM9+0x15);
XBYTE     Fan1RPMBackUp 		   _at_ (OEMRAM9+0x16); 
XBYTE     Fan2RPMBackUp 		   _at_ (OEMRAM9+0x17);

XWORD	  RPM1 					   _at_ (OEMRAM9+0x18);

XWORD	  RPM2				       _at_ (OEMRAM9+0x1A);
//COKEYXU013:E+ ladder fan relative regs.

XBYTE     delayEDPTm               _at_ (OEMRAM9+0x1C);       // EDP Delay Initial Timer
XBYTE     cCHIPType                _at_ (OEMRAM9+0x1D);
XBYTE	  VFAN_Tab_Step3		   _at_ (OEMRAM9+0x1E);  //MEILING003:add.
XBYTE     FAN_Tab_Step3            _at_ (OEMRAM9+0x1F);  //MEILING003:add.




//Mos: add for control in Mailbox
XBYTE	MMIO_rDATA_index				_at_(OEMRAM9+0x91);	// 0x81
XBYTE	MMIO_CMD_Ctrl					_at_(OEMRAM9+0x82);	// 0x82
/*XBYTE MMIO_rDATA83					_at_(OEMRAM9+0x83);	// 0x83
XBYTE	MMIO_rDATA84					_at_(OEMRAM9+0x84);	// 0x84
XBYTE	MMIO_rDATA85					_at_(OEMRAM9+0x85);	// 0x85
XBYTE	MMIO_rDATA86					_at_(OEMRAM9+0x86);	// 0x86
XBYTE	MMIO_rDATA87					_at_(OEMRAM9+0x87);	// 0x87
XBYTE	MMIO_rDATA88					_at_(OEMRAM9+0x88);	// 0x88
XBYTE	MMIO_rDATA89					_at_(OEMRAM9+0x89);	// 0x89
XBYTE	MMIO_rDATA9A					_at_(OEMRAM9+0x8A);	// 0x8A*/
XBYTE	MMIO_rDATA[8]					_at_(OEMRAM9+0x83);	// 0x83 ~ 0x8A

XBYTE	MMIO_iCMD						_at_(OEMRAM9+0x8B);	// 0x8B
XBYTE	MMIO_iDATA8C					_at_(OEMRAM9+0x8C); 	// 0x8C
XBYTE	MMIO_iDATA8D					_at_(OEMRAM9+0x8D); 	// 0x8D
XBYTE	MMIO_iDATA8E					_at_(OEMRAM9+0x8E); 	// 0x8E
XBYTE	MMIO_iDATA8F					_at_(OEMRAM9+0x8F); 	// 0x8F
XBYTE	MMIO_iDATA90					_at_(OEMRAM9+0x90); 	// 0x90

XBYTE	FnStatus						_at_(OEMRAM9+0xE0);

XBYTE	SysStatus				        _at_(OEMRAM9+0xF0);
XBYTE	LID_DEBOUNCE_CNT                _at_(OEMRAM9+0xF1);
XBYTE	PWRBTN_pressCnt  				_at_(OEMRAM9+0xF2);
//XBYTE	LID_readytime 				_at_(OEMRAM9+0xF3); //ANGELAS007:Optimize power on sequence.
//XBYTE	NTC_FLAG 					_at_(OEMRAM9+0xF3); //ANGELAS007:Optimize power on sequence. //ANGELAS038:remove  
XBYTE	ExtTimeCnt                      _at_(OEMRAM9+0xF4);
XBYTE	SysStatus2       		        _at_(OEMRAM9+0xF5);
XBYTE	EC_ON_Flag      		        _at_(OEMRAM9+0xF6);  
XBYTE	TPID1						    _at_(OEMRAM9+0xFA);
XBYTE	TPID2						    _at_(OEMRAM9+0xFB);
XBYTE	TPID3						    _at_(OEMRAM9+0xFC);
XBYTE	TPID4						    _at_(OEMRAM9+0xFD);

XBYTE	LENOVOPMFW_Temp                 _at_(OEMRAM9+0xFF);

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0xA00-0xAFF   OEM RAM A
//-------------------------------------------------------------------------------
//*******************************************************************************


//*******************************************************************************
//-------------------------------------------------------------------------------
//	0xB00-0xBFF   OEM RAM B
//-------------------------------------------------------------------------------
//*******************************************************************************
//---------------------FAN--------------------------
XBYTE	TEMP_Buff_1				_at_(OEMRAMB+0x00);
XBYTE	TEMP_Buff_2				_at_(OEMRAMB+0x01);
XBYTE	TEMP_Buff_3				_at_(OEMRAMB+0x02);
XBYTE	EXT_Duty				_at_(OEMRAMB+0x03);
XBYTE	Target_Duty				_at_(OEMRAMB+0x04);
XBYTE	MBID_Reload				_at_(OEMRAMB+0x05);
XBYTE	Fan_ON_Count			_at_(OEMRAMB+0x06);
XBYTE	Fan_Debug_Temp			_at_(OEMRAMB+0x07);
XBYTE	VGA_TBuff1				_at_(OEMRAMB+0x08);
XBYTE	VGA_TBuff2				_at_(OEMRAMB+0x09);
XBYTE	VGA_TBuff3				_at_(OEMRAMB+0x0A);
XBYTE	WLAN_OFF_Cnt			_at_(OEMRAMB+0x0B);
XBYTE	Average_Temp			_at_(OEMRAMB+0x0C);	// Average temperature stage
XBYTE	SLI_Status				_at_(OEMRAMB+0x0D);	// SLI check byte.
XBYTE	USB_Charger				_at_(OEMRAMB+0x0E);	// USB charger EN/Dis
XBYTE	USB_Error				_at_(OEMRAMB+0x0F);
XBYTE	MuteCount				_at_(OEMRAMB+0x10);
XBYTE	EXT_VGA_Buff1			_at_(OEMRAMB+0x11);
XBYTE	EXT_VGA_Buff2			_at_(OEMRAMB+0x12);
XBYTE	EXT_VGA_Buff3			_at_(OEMRAMB+0x13);
XBYTE	Thro_Status				_at_(OEMRAMB+0x14);
XBYTE	Thro_Status2			_at_(OEMRAMB+0x15);
XBYTE	CPU_Shut_Cnt			_at_(OEMRAMB+0x16);
XBYTE	VGA_Shut_Cnt			_at_(OEMRAMB+0x17);
XBYTE	EXTVGA_Shut_Cnt			_at_(OEMRAMB+0x18);
XBYTE	LED_KB_PWM_Count		_at_(OEMRAMB+0x19);
XBYTE	FAN2PRM 				_at_(OEMRAMB+0x1A);
XBYTE	FAN_CpTarget 			_at_(OEMRAMB+0x1B);
XBYTE	SMSC_ChgDelay			_at_(OEMRAMB+0x1C);
XBYTE	USB_ChargerStep			_at_(OEMRAMB+0x1D);
//XBYTE	ManualFan2PRM			_at_(OEMRAMB+0x1E);//JERRYCH00:Modify the fan control from linear to the ladder.
XWORD 	SHA1_SEED				_at_(OEMRAMB+0x1F);

XBYTE 	uODDPWRsaving			_at_(OEMRAMB+0x21);	//
XBYTE 	uODDdelaytimeStep		_at_(OEMRAMB+0x22);	//
XBYTE 	SHA1failCnt				_at_(OEMRAMB+0x23);
XBYTE 	uWLBTLanTemp		 	_at_(OEMRAMB+0x24);	//
XBYTE 	uODDlowCnt				_at_(OEMRAMB+0x25);	//
XBYTE 	uODDtimeCnt				_at_(OEMRAMB+0x26);	//
XBYTE 	Service_Auth_Step		_at_(OEMRAMB+0x27);

XBYTE 	FAN_Std_Max				_at_(OEMRAMB+0x28);
XBYTE 	FAN_DOS_Max				_at_(OEMRAMB+0x29);
XBYTE 	FAN_Quite_Max			_at_(OEMRAMB+0x2A);

//XBYTE	FAN_SP_Max			_at_(OEMRAMB+0x2B);
XBYTE 	FAN_RPM_L               _at_(OEMRAMB+0x2B);

XBYTE 	FAN_PWM_1				_at_(OEMRAMB+0x2C);
XBYTE 	FAN_PWM_2				_at_(OEMRAMB+0x2D);
XBYTE 	StatusKeeper			_at_(OEMRAMB+0x2E);
XBYTE 	ECSleepCount			_at_(OEMRAMB+0x2F);

XBYTE 	TouchPadCount			_at_(OEMRAMB+0x30);
XBYTE 	EEPROM_PwrSts			_at_(OEMRAMB+0x31);

//XWORD  	FAN1_RPM				_at_(OEMRAMB+0x32);



XBYTE 	PwrOnDly5Sec			_at_(OEMRAMB+0x34); 

XBYTE 	SHA1FailRetry           _at_(OEMRAMB+0x35); 

//ANGELAS043: add start
XBYTE 	RSOC1PTO0PCount			_at_(OEMRAMB+0x36);  
XBYTE 	RSOC1PTO0PSaveSpace		_at_(OEMRAMB+0x37);
//ANGELAS043:add end

XBYTE 	SLI_GPUMax				_at_(OEMRAMB+0x38);
XBYTE 	SLI_GPUPWM1				_at_(OEMRAMB+0x39);
XBYTE 	SLI_GPUPWM2				_at_(OEMRAMB+0x3A);
XBYTE 	SLI_FANMax				_at_(OEMRAMB+0x3B);
XBYTE 	SLI_FANPWM1				_at_(OEMRAMB+0x3C);
XBYTE 	SLI_FANPWM2				_at_(OEMRAMB+0x3D);

XBYTE 	FAN_PWM_3               _at_(OEMRAMB+0x3E); //MEILING003:add.

XBYTE   Thro_Status3			_at_ (OEMRAMB+0x3F);  //MEILING040:add.

LWORD 	K[5]					_at_(OEMRAMB+0x40);    //4*5 = 20 byte
LWORD 	RBATH0					_at_(OEMRAMB+0x60); 	// 0x0B60 ~ 0x0B63
LWORD 	RBATH1					_at_(OEMRAMB+0x64); 	// 0x0B64 ~ 0x0B67
LWORD 	RBATH2					_at_(OEMRAMB+0x68); 	// 0x0B68 ~ 0x0B6B
LWORD 	RBATH3					_at_(OEMRAMB+0x6C); 	// 0x0B6C ~ 0x0B6F
LWORD 	RBATH4					_at_(OEMRAMB+0x70); 	// 0x0B70 ~ 0x0B73
LWORD 	BATchallenger[5]		_at_(OEMRAMB+0x80); 	// 0x0B80 ~ 0x0B93




XBYTE 	ShutDnCause	           	_at_(OEMRAMB+0xA3);    
XBYTE 	FunctionKeyDebounce	    _at_(OEMRAMB+0xA4);  //ANGELAS084:add
XBYTE 	Thro_Status4	    	_at_(OEMRAMB+0xA5);  ////ANGELAG046: add

//XWORD     FAN2_RPM				   _at_ (OEMRAMB+0xA6);

//COKEYXU013:S+ ladder fan relative regs.
XBYTE     Fan1OffVGA_Step1   	   _at_ (OEMRAMB+0xB0);
XBYTE     Fan1OffVGA_Step2   	   _at_ (OEMRAMB+0xB1);
XBYTE     Fan1OffVGA_Step3   	   _at_ (OEMRAMB+0xB2);
XBYTE     Fan1OffVGA_Step4   	   _at_ (OEMRAMB+0xB3);
XBYTE     Fan1OffVGA_Step5   	   _at_ (OEMRAMB+0xB4);
XBYTE     Fan1OffVGA_Step6   	   _at_ (OEMRAMB+0xB5);
XBYTE     Fan1OffVGA_Step7   	   _at_ (OEMRAMB+0xB6);
XBYTE     Fan1OffVGA_Step8   	   _at_ (OEMRAMB+0xB7);
XBYTE     Fan2OffVGA_Step1  	   _at_ (OEMRAMB+0xB8);
XBYTE     Fan2OffVGA_Step2  	   _at_ (OEMRAMB+0xB9);
XBYTE     Fan2OffVGA_Step3  	   _at_ (OEMRAMB+0xBA);
XBYTE     Fan2OffVGA_Step4  	   _at_ (OEMRAMB+0xBB);
XBYTE     Fan2OffVGA_Step5 		   _at_ (OEMRAMB+0xBC);
XBYTE     Fan2OffVGA_Step6  	   _at_ (OEMRAMB+0xBD);
XBYTE     Fan2OffVGA_Step7  	   _at_ (OEMRAMB+0xBE);
XBYTE     Fan2OffVGA_Step8 		   _at_ (OEMRAMB+0xBF);


XBYTE	  FanLevelCPU		       _at_ (OEMRAMB+0xC0);
XBYTE	  FanLevel2VGA			   _at_ (OEMRAMB+0xC1);
XBYTE	  temperature1			   _at_ (OEMRAMB+0xC2); 
XBYTE	  temperature2			   _at_ (OEMRAMB+0xC3);
XBYTE	  Debugtemperature1        _at_ (OEMRAMB+0xC4);	   
XBYTE	  Debugtemperature2 	   _at_ (OEMRAMB+0xC5);   
XBYTE	  DummyStep 			   _at_ (OEMRAMB+0xC6);
XBYTE	  FanFullOnCont 	       _at_ (OEMRAMB+0xC7);
XBYTE	  RPMTimeShift			   _at_ (OEMRAMB+0xC8); 
XBYTE	  DebugFan1RPMT 		   _at_ (OEMRAMB+0xC9);
//COKEYXU013:E+ ladder fan relative regs.

XBYTE      PCHTEMP_Buff_1 		   _at_ (OEMRAMB+0xCA);
XBYTE      PCHTEMP_Buff_2 		   _at_ (OEMRAMB+0xCB); 
XBYTE      PCHTEMP_Buff_3 		   _at_ (OEMRAMB+0xCC);








//ANGELAG018: add start
XBYTE 	sdAMDTH0				_at_(OEMRAMB+0xd0); 
XBYTE 	sdAMDTH1				_at_(OEMRAMB+0xd1); 
XBYTE 	sdAMDTH2				_at_(OEMRAMB+0xd2); 
XBYTE 	sdAMDTH3				_at_(OEMRAMB+0xd3); 
//ANGELAG018: add end
/*LWORD 	sdAMDTH0				_at_(OEMRAMB+0xd0);  //ANGELAG018: remove start
LWORD 	sdAMDTH1				_at_(OEMRAMB+0xd4); 
LWORD 	sdAMDTH2				_at_(OEMRAMB+0xd8); 
LWORD 	sdAMDTH3				_at_(OEMRAMB+0xdC); */ //ANGELAG018: remove end

//COKEYXU013:S+ ladder fan relative regs.
XBYTE	  Fan1OnVGA_Step1		   _at_ (OEMRAMB+0xE0);
XBYTE	  Fan1OnVGA_Step2		   _at_ (OEMRAMB+0xE1);
XBYTE     Fan1OnVGA_Step3   	   _at_ (OEMRAMB+0xE2);
XBYTE     Fan1OnVGA_Step4   	   _at_ (OEMRAMB+0xE3);
XBYTE     Fan1OnVGA_Step5    	   _at_ (OEMRAMB+0xE4);
XBYTE     Fan1OnVGA_Step6    	   _at_ (OEMRAMB+0xE5);
XBYTE     Fan1OnVGA_Step7    	   _at_ (OEMRAMB+0xE6);
XBYTE     Fan1OnVGA_Step8    	   _at_ (OEMRAMB+0xE7);
XBYTE     Fan2OnVGA_Step1  		   _at_ (OEMRAMB+0xE8);
XBYTE     Fan2OnVGA_Step2  		   _at_ (OEMRAMB+0xE9);
XBYTE     Fan2OnVGA_Step3  		   _at_ (OEMRAMB+0xEA);
XBYTE     Fan2OnVGA_Step4  		   _at_ (OEMRAMB+0xEB);
XBYTE     Fan2OnVGA_Step5   	   _at_ (OEMRAMB+0xEC);
XBYTE     Fan2OnVGA_Step6  		   _at_ (OEMRAMB+0xED);
XBYTE     Fan2OnVGA_Step7  		   _at_ (OEMRAMB+0xEE);
XBYTE     Fan2OnVGA_Step8 		   _at_ (OEMRAMB+0xEF);
//COKEYXU013:E+ ladder fan relative regs.





XBYTE 	Tgpuproup				_at_(OEMRAMB+0xf0); 
XBYTE 	Tgpuprodown				_at_(OEMRAMB+0xf1);  
XBYTE 	local_Shut_Cnt          _at_(OEMRAMB+0xf2); 
XBYTE 	remote_Shut_Cnt         _at_(OEMRAMB+0xf3); 



//*******************************************************************************
//-------------------------------------------------------------------------------
//	0xC00-0xCFF   OEM RAM C
//	0xD00-0xDFF   OEM RAM D
//-------------------------------------------------------------------------------
//*******************************************************************************
LWORD	SHA1_W[80]							_at_ ECRAMC; // 0x0C00 ~ 0x0D3F

LWORD	WS_temp								_at_(ECRAMD+0x40); // 0x0D40 ~ 0x0D4F

LWORD	SHA1_a								_at_(ECRAMD+0x50); // 0x0D50 ~ 0x0D53
LWORD	SHA1_b								_at_(ECRAMD+0x54); // 0x0D54 ~ 0x0D57
LWORD	SHA1_c								_at_(ECRAMD+0x58); // 0x0D58 ~ 0x0D5B
LWORD	SHA1_d								_at_(ECRAMD+0x5C); // 0x0D5C ~ 0x0D5F
LWORD	SHA1_e								_at_(ECRAMD+0x60); // 0x0D60 ~ 0x0D63

LWORD	H0									_at_(ECRAMD+0x64); // 0x0D64 ~ 0x0D67
LWORD	H1									_at_(ECRAMD+0x68); // 0x0D68 ~ 0x0D6B
LWORD	H2									_at_(ECRAMD+0x6C); // 0x0D6C ~ 0x0D6F
LWORD	H3									_at_(ECRAMD+0x70); // 0x0D70 ~ 0x0D73
LWORD	H4									_at_(ECRAMD+0x74); // 0x0D74 ~ 0x0D77

LWORD	K0									_at_(ECRAMD+0x78); // 0x0D78 ~ 0x0D7B

// --- here is EC result ---
LWORD	Hn0									_at_(ECRAMD+0x7C); // 0x0D7C ~ 0x0D7F
LWORD	Hn1									_at_(ECRAMD+0x80); // 0x0D80 ~ 0x0D83
LWORD	Hn2									_at_(ECRAMD+0x84); // 0x0D84 ~ 0x0D87
LWORD	Hn3									_at_(ECRAMD+0x88); // 0x0D88 ~ 0x0D8B
LWORD	Hn4									_at_(ECRAMD+0x8C); // 0x0D8C ~ 0x0D8F

// --- here is battery result ---
LWORD	BATH0								_at_(ECRAMD+0x90); // 0x0D90 ~ 0x0D93
LWORD	BATH1								_at_(ECRAMD+0x94); // 0x0D94 ~ 0x0D97
LWORD	BATH2								_at_(ECRAMD+0x98); // 0x0D98 ~ 0x0D9B
LWORD	BATH3								_at_(ECRAMD+0x9C); // 0x0D9C ~ 0x0D9F
LWORD	BATH4								_at_(ECRAMD+0xA0); // 0x0DA0 ~ 0x0DA3

//XBYTE	Service_Auth_Step					_at_ ECRAMD+0xFF; // 0x0DFF

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0xE00-0xEFF   OEM RAM E
//-------------------------------------------------------------------------------
//*******************************************************************************

//*******************************************************************************
//-------------------------------------------------------------------------------
//	0xF00-0xFFF   OEM RAM F
//-------------------------------------------------------------------------------
//*******************************************************************************
//ANGELAS064:add start
XBYTE   nDiagInterface_CMD             _at_(OEMRAMF+0x00);  
XBYTE   nDiagInterface_DATA            _at_(OEMRAMF+0x01);                     

XBYTE   nDiagInterface_ECRegL          _at_(OEMRAMF+0x02); //or used as EC Space offset
XBYTE   nDiagInterface_ECRegH          _at_(OEMRAMF+0x03); 

XBYTE   nDiagInterface_DATA_High       _at_(OEMRAMF+0x04); 


XBYTE 	SMB_PRTC_Diag				   _at_(OEMRAMF+0x60);// 0x60,
XBYTE 	SMB_STS_Diag				   _at_(OEMRAMF+0x61);// 0x61,
XBYTE 	SMB_ADDR_Diag			       _at_(OEMRAMF+0x62);// 0x62,
XBYTE 	SMB_CMD_Diag				   _at_(OEMRAMF+0x63);// 0x63,
XBYTE 	SMB_DATA_Diag				   _at_(OEMRAMF+0x64);// 0x64,
//0x64~0x83  for 32 bytes max block R/W 
XBYTE 	SMB_BCNT_Diag				   _at_(OEMRAMF+0x84);// 0x84,
// SBS Only Alarm registers  
XBYTE 	SMB_ALRA_Diag				   _at_(OEMRAMF+0x85);// 0x85,
XBYTE 	SMB_ALRD0_Diag				   _at_(OEMRAMF+0x86);// 0x86,
XBYTE 	SMB_ALRD1_Diag				   _at_(OEMRAMF+0x87);// 0x87,

XBYTE 	SMB_EC_Chennel_Diag			   _at_(OEMRAMF+0x88);// 0x88,
//ANGELAS064:add end



 
  
	




