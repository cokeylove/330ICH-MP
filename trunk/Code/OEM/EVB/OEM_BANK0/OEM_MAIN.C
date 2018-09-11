/*-----------------------------------------------------------------------------
 * TITLE: OEM_MAIN.C - OEM specific code
 *
 * Some "HOOKS" from CHIPHOOK.C are copied into this file in order to replace
 * (or augment) core functions.
 *
 * Copyright (c) 1983-2007, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>

//------------------------------------------------------------
// Hook 1ms events
// EventId is 0 ~ 9 cycle
//------------------------------------------------------------
void Hook_Timer1msEvent(IBYTE EventId)
{
	if(SystemIsS0)
    {
	  	if(USB_Delay!=0x00)
	 	{
			USB_Delay--;
			if(USB_Delay==0x00)
			{
            	USB_ON_OUTPUT;
                USB_ON_LOW();
			}
       }
   }  
#if !EN_PwrSeqTest 
  	//GC6_FBClamp(); 
#endif  
    PollingBIOS80Port();
    EventManager(EventId);      // Polling system event
    Oem_SysPowerContrl();       // System Power Control
    SetVGA_AC_DET();

	//if(SystemIsS0) //ANGELAS013:Modify SD power function.
		//Set_SD_PWR_EN();//angela  //MEILING001:remove.

    if ( !Read_ENBKL_IN() )
    { 
        BKOFF_OFF(); 
    }
#if PS8625_SUPPORT 
	//  for PS8625 translator initial+
    if( SystemIsS0 && (delayEDPTm==0) )           // CMW Temp
    {
    	if( (initMark !=0xAA) && (TEMP_Error < 20) )
        {
        	Init_PS8625();
        }
    }
#endif  
	// CWM  for PS8625 translator initial -
	
}

//------------------------------------------------------------
// Hook 5ms events
//------------------------------------------------------------
void Hook_Timer5msEvent(void)
{
	//VPC_interface();
}

//------------------------------------------------------------
// Hook 10ms events
//------------------------------------------------------------
void Hook_Timer10msEventA(void)
{
  	/* if((Reset_Delay==0x44)||(Reset_Delay==0x55))
     	{
        	Reset_Delay_Count++;
       	 if(Reset_Delay_Count==0xFF)  //T070B:change 0x64 to 0xFF
       	{
            		Reset_Delay_Count=0x00; //G30:Remove work around
      
       	}
     	} 
     	else 
     	{
           	Reset_Delay_Count=0x00;
     	}*/
	// start for save P80 code to setup. 
	/*   //remove below code.
    	if(CheckDebugMode())
    	{
       	SET_MASK(P80CMOSSts,P80CMOSDis);
       	//RamDebug(0x88+P80CMOSSts);
     	}
	*/     
	// end for save P80 code to setup. 
	ScanADCFixChannel();
	CPUProchotCtrl(); ////ANGELAG028: add.  
	GPUProchotOnControl();  //ANGELAG028:add.
	//H2RAMprocess();
	TPInfor_ToVPC(); //ANGELAG008: add
	Backlight_Control();
	EscScanCode(); //10ms
#if !EN_PwrSeqTest
	if (PWRBTN_pressCnt !=0)
	{
		PWRBTN_pressCnt--;
		if (PWRBTN_pressCnt==0)
		{
			PM_PWRBTN_HI();
			//CLEAR_MASK(SysStatus,EnterDS3byLid);
		}
	}
	/*//ANGELAS007:s-Optimize power on sequence.
	//ANGELAS005:s+ Support UMA SKU EC_ON pin design.
	if(NTC_V1>682)
	{
		if ((LID_readytime != 0) && (IS_MASK_SET(GPDRF,BIT(0))))
		{ 
			LID_readytime--; 
		}
	}
	else
	{
	//ANGELAS005:e+ Support UMA SKU EC_ON pin design.
	if ((LID_readytime != 0) && (Read_EC_ON()))
	{ 
	        LID_readytime--; 
       }
	}//ANGELAS005:Support UMA SKU EC_ON pin design.
	*///ANGELAS007:e-Optimize power on sequence.
    //if(!Read_ADAPTERID_ON())  //MEILING001:remove.
    if(AdapterIDOn_Flag) //MEILING001:add.
    {
        if(ADPIDON10MS_NUM>0x00)
        {
         	ADPIDON10MS_NUM--; 
        }
        else 
        {
            //SetPowerBatteryparameter(); //
            //ADAPTERIDON_HI();  //MEILING001:remove.
            SetPowerBatteryparameter2(); //MEILING001:add.
            AdapterIDOn_Flag = 0x00; //MEILING001:add.
            ADPIDON10MS_NUM=0xFF; 
        }    
    } 
    else 
    {
        if(ADPIDON10MS_NUM>0x01)
        {
            ADPIDON10MS_NUM--; 
        }
        else 
        {   
            if(ADPIDON10MS_NUM==0x01)
            {
            	//T058B-  CLEAR_MASK(SYS_STATUS,AC_ADP);	//T058A+
		        //ACOFF_HI();         //T078-                                //T058A+
                //T058B-  ECSMI_SCIEvent(ACPI_ACOUT_SCI); 
                ADPIDON10MS_NUM--;
            }     
         }
   }
#endif
	//change deep sleep From 1ms hook to 1s hook start
#if	Support_EC_Sleep
	if( CheckCanEnterDeepSleep() )
	{
		InitEnterDeepSleep();
		EnableAllInterrupt();	// enable global interrupt
		PLLCTRL = 0x01;
		PCON	= 0x02; 		/* Enter Deep Sleep mode.  */
		_nop_();				/* Wake-up delay.  */
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		IER3 = 0x00;
		InitWakeFromDeepSleep();
		WakeUp_DO_Function();
		//WakeUp_30ms_Function();
	}
	else
	{
		PCON=1; // Enter idle mode
	}
#else
	PCON=1;	// Enter idle mode
#endif	// Support_EC_Sleep
	//:change deep sleep From 1ms hook to 1s hook end
}


//MARTINH154:Add  start
void Deal_CLEAR_CMOS(void)
{
	if (IS_MASK_SET(CMOS_TEST,b0_CMOS_FunctionKey))
  	{
		if(SystemIsS5)
		{     
	    	if(IS_MASK_CLEAR(CMOS_TEST,b1_CMOS_delay1flag))
			{ 
		    	cmosdelay++;
				RSMRST_LOW();
				if(cmosdelay==0x01)
				{
		    		PCH_PWR_EN_OFF();
				}
				if(cmosdelay==0x02)
				{
		       		EC_ON_LOW();
				}
				if(cmosdelay==0x03)
				{
		       		//PCH_PWREN_HI();
		        	PM_PWRBTN_LOW();
		        	AC_PRESENT_LOW();
				}
		    	if(cmosdelay==22)
				{   		
			    	RTCRST_ON_HI();
		        	cmosdelay=0;
		        	RamDebug(0x23);
					SET_MASK(CMOS_TEST,b1_CMOS_delay1flag);
					cmosdelay1 = 0x00;
				}	
			}
			else if(cmosdelay1>=35) //3.5S
			{
		 		cmosdelay++;
				RTCRST_ON_LOW();
				SET_MASK(CMOS_TEST,BIT3);	
				if(cmosdelay==10)
				{
					if(Read_AC_IN())
		    			EC_ON_HI();
		 		}
		    	if(cmosdelay==11)
				{
			    	//PCH_PWREN_LOW();
 					if(Read_AC_IN())
						PM_PWRBTN_HI();
				}		
				if(cmosdelay==12)
				{
			    	if(Read_AC_IN())
		            	PCH_PWR_EN_ON(); 
		 		}
		    	if(cmosdelay==15)
				{
					if(Read_AC_IN())
		        		RSMRST_HI();
			        	CLEAR_MASK(CMOS_TEST,b1_CMOS_delay1flag);
		            	CLEAR_MASK(CMOS_TEST,b0_CMOS_FunctionKey);
						CLEAR_MASK(CMOS_TEST,BIT3);
		            	cmosdelay1=0;
 					if(Read_AC_IN())
		            	AC_PRESENT_HI();	
		            	RamDebug(0x4A);
		 		}
		
			}
		}
  	}
  	else
  	{
	  	cmosdelay=0;
  	}
}
//MARTINH154:Add  end

//------------------------------------------------------------
// Hook 10ms events
//------------------------------------------------------------
void Hook_Timer10msEventB(void)
{
    CheckSBPowerButton(); 
#if !EN_PwrSeqTest
	ScanADCDyChannel1();
	//Chk_Wrong_ADP();
	
	if(SystemIsS0&& (PwrOnDly5Sec==0)) 
	{
		PollingCPURT(); 
		//if(uMBGPU&0x02) //ELMERZH002:all are dis
		{
			PollingGPURT(); 
		}
	}
	
#endif
	//Mos: Conflict with turbo boost, Pending.
	//Chk_ACOP();
#if EC_Brightness_Ctrl
	Brightness_Control(); // EC control brightness.
#endif	// EC_Brightness_Ctrl
  	if( delayEDPTm > 0 )       // CMW Temp
  	{
        delayEDPTm--;
  	}
	
	//Deal_CLEAR_CMOS();
}

//------------------------------------------------------------
// Hook 50ms events A
//------------------------------------------------------------
void Hook_Timer50msEventA(void)
{
#if !EN_PwrSeqTest
	#if UCS1022_Support
	UCS1002_TimerX_FakeISR();
	#endif
#endif	// EN_PwrSeqTest

	if (cOsLedCtrl.fbit.cOL_CtrlRight ==0 )
		Lenovo_LED();
	else
		MFG_LED();
}

//------------------------------------------------------------
// Hook 50ms events B
//------------------------------------------------------------
void Hook_Timer50msEventB(void)
{
	PWM_TimeCount();
	/* //ANGELAS050:remove start
#if Support_ODDZeroPower
	ODDPowerSaving();
#endif	// Support_ODDZeroPower
	*///ANGELAS050:remove end
	//Check_heavy_loading_500ms(); //ANGELAG056: add //COKEYXU021: remove for 8% RSOC change to 5% suddenly.
}

//------------------------------------------------------------
// Hook 50ms events C
//------------------------------------------------------------
void Hook_Timer50msEventC(void)
{
	MuteProcess();
#if !EN_PwrSeqTest
	Chk_pDevStus();
	Update_EEPROMB07();
#endif
//JERRYCH00:s+Modify the fan control from linear to the ladder.	
	//if(SystemIsS0)
		//Chk_FAN_RPM_Control();	// Check FAN is AP control.

		 #if !EN_PwrSeqTest
		//if ( nFanManual == 0 )
		//{
			if ( IS_MASK_CLEAR(THERMAL_STATUS, INITOK) )
			{	// EC control fan.
			  #if FAN_TABLE_Already
				  CheckFanRPM1();
	              CheckFanRPM2();
			  #endif	// FAN_TABLE_Already
			}
		//}
//JERRYCH00:e+Modify the fan control from linear to the ladder.	
  #endif	
}

//------------------------------------------------------------
// Hook 100ms events A
//------------------------------------------------------------
void Hook_Timer100msEventA(void)
{
	//ThmIC_Temperature();  //disable thermal.G06 ENABLE thermal  
	/*  
      if(SystemIsS0)
     {
		if(USB_Delay!=0x00)
	 	{
			USB_Delay--;
			if(USB_Delay==0x00)
			{
                   		USB_ON_OUTPUT;
                   		USB_ON_LOW();
			}
       	}
   	}  
 	*/  	
#if !EN_PwrSeqTest
	#if WDT_Support
	ResetInternalWDT();
	#endif
	
  	if (IS_MASK_SET(SEL_STATE0,PRESENT_A))
  	{ 
  		OEM_PollingBatData_TASK(); 
    }
	else
	{ CLEAR_MASK(StatusKeeper, BatteryProtectCHG); }
    #if Support_USB_Charge 
		#if UCS1022_Support
		UCS1002ID_Main();
		#else
		GL887_Main();
		#endif
    #endif                          
#endif	// EN_PwrSeqTest
	//MARTINH154 add start
	if (IS_MASK_SET(CMOS_TEST,b1_CMOS_delay1flag)&&IS_MASK_CLEAR(CMOS_TEST,BIT3))
	{
    	cmosdelay1++;
	}

	if(IS_MASK_SET(pProject4, b1WinAndXpress))
	{
	    if(CountOfWinAndX != 0)
	    {
            CountOfWinAndX--;
	    }
	    else
	    {
            CLEAR_MASK(pProject4, b1WinAndXpress);
            e0_prefix_code(0x1F,BREAK_EVENT);
    		simple_code(0x22,BREAK_EVENT);
            simple_code(0x3C,MAKE_EVENT);
            simple_code(0x3C,BREAK_EVENT);
            F_Service_SEND = 1;
        }
        
	}
}

//------------------------------------------------------------
// Hook 100ms events B
//------------------------------------------------------------
void Hook_Timer100msEventB(void)
{
    //VGA_Temperature();  
#if !EN_PwrSeqTest
	#if Support_WebCam
	CameraProcess();
	#endif	// Support_WebCam
	WirelessProcess();		// Control WLAN and BT.
	IFFSProcess();
#endif
	ACOutProchotRelease();  //MEILING055:add.
	//ANGELAS084 add start
    if((FunctionKeyDebounce&0x0F)!=0)
    {
      FunctionKeyDebounce--;
      if((FunctionKeyDebounce&0x0F)==0x00)
      {
          (HotKey_Fn_Fx[((FunctionKeyDebounce&0xF0)>>0x04)-1])(1);
	      FunctionKeyDebounce=0x00;
      }	
    }
	//ANGELAS084 add end
}

//------------------------------------------------------------
// Hook 100ms events C
//------------------------------------------------------------
void Hook_Timer100msEventC(void)
{
#if !EN_PwrSeqTest
	//Oem_Fan_Speed();//JERRYCH00:Modify the fan control from linear to the ladder.
    ThrottlingControl(); 
	GPUThrottlingControl(); //MEILING033:add.
	//CPUProchotCtrl(); //ANGELAG028: remove //MEILING033:add.  
	//GPUProchotOnControl();  //ANGELAG028: remove //MEILING055:add.
	Battery100ms();
	if (TouchPadCount !=0)
	{ 
	    TouchPadCount--; 
    }
	
	//Oem_Thermal_Control();	// Read CPU temperature by PECI.//:Change read cpu temperature from 100ms to 1s.
///JERRYCH00:s+Modify the fan control from linear to the ladder.
	//if ( uReserve07.fbit.nFanManual == 0 )
	//{
		if ( IS_MASK_CLEAR(THERMAL_STATUS, INITOK) )
		{	// EC control fan.
			#if FAN_TABLE_Already
			//Oem_Fan_Control();
			FanManager();
			#else
			FanSpeedConWhenFanTableNotReady();
			#endif	// FAN_TABLE_Already
		}
		else
		{ 
			//FAN_Dust_Mode(); 
        }
	//}
///JERRYCH00:e+Modify the fan control from linear to the ladder.
	/*#if FAN_TABLE_Already
	OEM_Throttling_Ctrl();
	#endif	*/// FAN_TABLE_Already  //G68:change OEM_Throttling_Ctrl from 100ms to 1s.
#endif
}

//------------------------------------------------------------
// Hook 500ms events A
//------------------------------------------------------------
void Hook_Timer500msEventA(void)
{
#if !EN_PwrSeqTest
  	if (IS_MASK_SET(SEL_STATE0,PRESENT_A))
  	{
		Chk_BatSMbusFailCount();

		if (Read_AC_IN() && IS_MASK_CLEAR(BATTUPDATEFW,BIT0))
		{
			Battery_Expresscharge(); //COKEYXU011: Add express charge method.
    		WriteSmartChgIC();
			ReadSmartChgIC();
		}
  	}

	//ThmIC_Temperature();

	//Mos: Update Battery info after FW update
	if(IS_MASK_SET(BATTUPDATEFW, BIT0))
		SET_MASK(StatusKeeper, BatteryFwUpdate);
	if(IS_MASK_SET(StatusKeeper, BatteryFwUpdate) && IS_MASK_CLEAR(BATTUPDATEFW, BIT0))
	{
		CLEAR_MASK(StatusKeeper, BatteryFwUpdate);
		FirstGetBatData();
		Service_Auth_Step = 1;				// start SHA1
	}

    #if SHA1_Support
    if(nDesignCapL !=0 && nPresentVoltH != 0)
    {
		if ((IS_MASK_SET(SEL_STATE0,PRESENT_A))
      && IS_MASK_CLEAR(StatusKeeper, BatteryFwUpdate))//MARTINA033:add "BatteryFwUpdate" judge
		{
			LV_BAT_Authentication();
		}
    }
    #endif	// SHA1_Support
#endif	// EN_PwrSeqTest
}

//------------------------------------------------------------
// Hook 500ms events B
//------------------------------------------------------------
void Hook_Timer500msEventB(void)
{
#if !EN_PwrSeqTest
	//VGA_Temperature();   
	SetKeyboardLED(); //MEILING001:add.

	#if SW_ISCT
	ISCT_Process();
	#endif	// SW_ISCT

	#if UCS1022_Support
	Ck_UpdateProfiles();
	#endif	// UCS1022_Support

	//MEILING055:S+
	if(cGPUACtoBattTime!=0)
	{
		cGPUACtoBattTime--;
	}
	DownACtoBatteryGPUState();
	//MEILING055:E+
	
		//ANGELAG012:S+.
	if(PCH_ColdBoot_TimeOut > 0) 
	{
		PCH_ColdBoot_TimeOut--;
		if((PCH_ColdBoot_TimeOut == 0)&&SystemIsS0)
		{
			ProcessSID(COLDBOOTFAIL_ID);
			SET_MASK(ACPI_HOTKEY, b7BIOS_NoShut);
			RSMRST_LOW();
			Delay1MS(1); 
			RSMRST_HI();
		}
		
		if(SysPowState != SYSTEM_S0) 
			PCH_ColdBoot_TimeOut = 0; 
	}
	//ANGELAG012:E+.	

	
	
	
#endif
}

//------------------------------------------------------------
// Hook 500ms events C
//------------------------------------------------------------
void Hook_Timer500msEventC(void)
{
#if !EN_PwrSeqTest
	Clear_Batt_First_Used_Date(); //ANGELAG015: add
	/*
      if(IS_MASK_CLEAR(SysStatus,LidKBIgnore))
      { 
		// change LID implement method.
          	#if !LID_OnlyQEvent_Support 
        	//Lid_Process();
		#else   
		//EC_LID_OUT_INPUT(); 
		#endif
		// change LID implement method.
      }
      */
	       Lid_Process(); //ANGELAG008: add

	// Check KBC Output Buffer Full
    if (IS_MASK_SET(KBHISR,OBF)&&(SystemIsS0))
    {
		KB_OBF_count++;
		if ((KB_OBF_count & 0x0F) > 10)				// OBF=1 over 5sec?
		{
			KB_OBF_count = KB_OBF_count + 0x10;		// high nibble for count times of clear
			KB_OBF_count &= 0xF0 ;
			GENIRQ = 0x01; //ANGELAG026: modify 0x0c to 0x01, because TP is not ues 6064 and 0X12 IRQ no resource
		}
    }
    else
    {
		KB_OBF_count &= 0xF0 ;
    }
#endif
}


//MARTINH154:Add start
void checkclearcmos(void)
{
	if (IS_MASK_SET(CMOS_TEST,BIT2))
	{
		if((!Read_SLPS3())&&(!Read_SLPS4()))
		{
			cmosshutdelay++;
			RamDebug(0x4B);
		}
		else
		{
			cmosshutdelay=0;
			CLEAR_MASK(CMOS_TEST,BIT2);
		}
		if(cmosshutdelay==5)
		{
			 SysPowState=SYSTEM_S5;  
			 //SET_MASK(CMOS_TEST,BIT0); //ANGELAS052:remove
			 SET_MASK(CMOS_TEST,b0_CMOS_FunctionKey); //ANGELAS052:add
			 CLEAR_MASK(CMOS_TEST,BIT2);
			 cmosshutdelay = 0x00;		 
		}
	}
}	
//MARTINH154:Add end

//------------------------------------------------------------
// Hook 1sec events A
//------------------------------------------------------------
void Hook_Timer1SecEventA(void)
{
	checkclearcmos();//jerry cmos //MARTINH154:Add
	//ANGELAG007: add start
	if(PowerOVP_REG|PowerUVP_REG|PowerOCP_REG|PowerOTP_REG)
	{
		
	}
	else
	{
		if(SystemNotS5)
			PollingOTP();
	}
	//ANGELAG007: add end
	if ( SystemIsS0 && (PwrOnDly5Sec!=0) )
	{ PwrOnDly5Sec--; } 
	//COKEYXU032: S+ Enable charge for 65W and 45W Adapter when S3/S5. 
	if(SystemIsS3 || SystemIsS5)
	{
		CLEAR_MASK(ACOFF_SOURCE,BATTLEARN);
		CLEAR_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable);
	}
	//COKEYXU032: E+
	
#if !EN_PwrSeqTest
    VGA_Temperature();// :Remove read VGA temperature.

    Oem_Thermal_Control();  // Read CPU temperature by PECI.//G56:Change read cpu temperature from 100ms to 1s.

	// ThmIC_Temperature(); //:transfer from 100ms to 1s //ANGELAS022: Remove thermal sensor function.
	// #if FAN_TABLE_Already//:change OEM_Throttling_Ctrl from 100ms to 1s. //ANGELAS026:Modify PROCHOT# function.
	OEM_Throttling_Ctrl();
	// #endif	// FAN_TABLE_Already //ANGELAS026:Modify PROCHOT# function.

	Battery1Sec();
    

	if ( SystemIsS5 )
	{
		if (ECSleepCount !=0)
		{ 
		    ECSleepCount--; 
        }
	}

	if (CountSecAfterPswPressed < 255)
		CountSecAfterPswPressed++;
	if (CountSecAfterNOVOPressed < 255)
		CountSecAfterNOVOPressed++;

	//ACOutProchotRelease();  //MEILING055:remove.
	//GPUProchotOnControl();  //MEILING055:remove.
	CPUThrottlingDelay();   //MEILING052:add.
	disCPUturbo90s(); //COKEYXU044:add
#endif
}

//------------------------------------------------------------
// Hook 1sec events B
//------------------------------------------------------------
void Hook_Timer1SecEventB(void)
{
#if !EN_PwrSeqTest
	if(QEVENT_DEBUG!=0x00)
    {
        ECQEvent(QEVENT_DEBUG);
        QEVENT_DEBUG=0x00;
    }  
	SystemAutoOn();
	//Lenovo_CPU_auto_mode();
	//ChkBattery_abnormal();  //LMLKBL0007:remove.

	if(IS_MASK_CLEAR(BATTUPDATEFW,BIT0) && IS_MASK_SET(BT1_STATUS1,bat_in))
  	{
		bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr,C_FW_Status,&FirmwareUpdateStatusL,SMBus_NoPEC);
		bRSMBusBlock(SMbusChB,SMbusRBK,SmBat_Addr,C_BAT_Info,&BatterymakerIDL);
  	}
#endif
	//ANGELAS033:Add start			
	if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
 	{
     	if(IS_MASK_SET(pProject0,b5FUBIOSWriteReady))
     	{
		  	if(bRWSMBus(SMbusChB,SMbusWW,SmBat_Addr,FirstUsedDate,&batteryFirstUsedDateL,SMBus_NeedPEC))
		  	{    
				RamDebug(0xAF);
		  	}
		  	CLEAR_MASK(pProject0,b5FUBIOSWriteReady);
		  	CLEAR_MASK(pProject0,b3uBatteryTimeNoOK);
     	}	  
 	} 		
	//ANGELAS033:Add end

}

//------------------------------------------------------------
// Hook 1sec events C
//------------------------------------------------------------
void Hook_Timer1SecEventC(void)
{
	//  remove MBID
    /*   if((uMBID!=MBID_FB)&&SystemIsS0)
       {
		uMBID = eEEPROMData=MBID_FB;              
             SET_MASK(LENOVOPMFW_Temp,EEPROM_Token);
		Update_EEPROMMark();
             CLEAR_MASK(LENOVOPMFW_Temp,EEPROM_Token);       
             if(((uMBID&0xFC)==0x30)||((uMBID&0xFC)==0x20)||((uMBID&0xFC)==0x10)||((uMBID&0xFC)==0x34)||((uMBID&0xFC)==0x24)||((uMBID&0xFC)==0x14)||((uMBID&0xFC)==0x3C)||((uMBID&0xFC)==0x2C)||((uMBID&0xFC)==0x1C)) //T050C: add 1C,2C,3C judge.
             {
             	MBID_READY=0x01;
             }
             else 
             {
                   MBID_READY=0x00;
             }    
       }*/
	//Start Shipmode disable 6s loop once 20130608 20:28 
	if(IS_MASK_CLEAR(StatusKeeper, BatteryFwUpdate) ) //ANGELAS032
	{ //ANGELAS032
		if(ShipModeCnt>0)
		{
			ShipModeCnt--;
			if(ShipModeCnt==0)
			{
				Unlock_ShipMode();
			}
		}
	} //ANGELAS032
	// End Shipmode disable 6s loop once 20130608 20:28 

#if !EN_PwrSeqTest
	if ( uReserve07.fbit.nFanManual == 0 )
	{
		if ( IS_MASK_CLEAR(THERMAL_STATUS, INITOK) )
		{
			EC_FAN_ANTI_OFF();//JERRYCRZ014:Add fan can be reversed  in dust removal mode.
			EC_FAN_ANTI_INPUT;//JERRYCRZ014:Add fan can be reversed  in dust removal mode.
		}
		else
		{ 
		   // FAN_Dust_Mode(); //JERRYCH00:-Modify the fan control from linear to the ladder.
        }
	}
	//ANGELAS094:s+Add workaround for hang bios.
	#if WorkaroundHangBios
    Workaround_For_HangBios();
    #endif
	//ANGELAS094:e+Add workaround for hang bios.
#endif


	/*   
	Oem_Thermal_Control();	// Read CPU temperature by PECI.

#if !EN_PwrSeqTest
	if ( uReserve07.fbit.nFanManual == 0 )
	{
		if ( IS_MASK_CLEAR(THERMAL_STATUS, INITOK) )
		{	// EC control fan.
			#if FAN_TABLE_Already
			Oem_Fan_Control();
			#endif	// FAN_TABLE_Already
		}
		else
		{ 
		        FAN_Dust_Mode(); 
              }
	}

	#if FAN_TABLE_Already
	OEM_Throttling_Ctrl();
	#endif	// FAN_TABLE_Already
#endif
	*/   
	// After enter OS 5S,then read CPU temperature for PECI fail test.
	/*
	if( ((SYS_STATUS & 0x07) != 0 )&&(PECIBIOSTEST==2))

	{ 
		PwrOnDly5Sec=5;
		PECIBIOSTEST=0;
		Init_PECI();//:Change PECI initialize after S0 states.

	}
	*/
	// After enter OS 5S,then read CPU temperature for PECI fail test.
	
}

//------------------------------------------------------------
// Hook1min events
//------------------------------------------------------------
void Hook_Timer1MinEvent(void)
{
#if !EN_PwrSeqTest
	Chk_CHG_TimeOut();

	#if SW_ISCT
	ISCT_TimerCnt();
	#endif	// SW_ISCT
#endif
}

//------------------------------------------------------------
// service_OEM_1
//------------------------------------------------------------
void service_OEM_1(void)
{

}

//------------------------------------------------------------
// service_OEM_2
//------------------------------------------------------------
void service_OEM_2(void)
{

}

//------------------------------------------------------------
// service_OEM_3
//------------------------------------------------------------
void service_OEM_3(void)
{

}

//------------------------------------------------------------
// service_OEM_4
//------------------------------------------------------------
void service_OEM_4(void)
{

}

//------------------------------------------------------------
//
//------------------------------------------------------------
void Hook_main_service_H(void)
{

}

//------------------------------------------------------------
//
//------------------------------------------------------------
void Hook_main_service_L(void)
{

}

//------------------------------------------------------------
// Timer event control function
// Return :
//  Only_Timer1msEvent : Only Timer1msEvent and Hook_Timer1msEvent function
//  All_TimerEvent : All timer events are OK.
//------------------------------------------------------------
BYTE Hook_Only_Timer1msEvent(void)
{
	// Return :
	//  Only_Timer1msEvent : Only Timer1msEvent and Hook_Timer1msEvent function
	//  All_TimerEvent : All timer events are OK.
    return(All_TimerEvent);
}

//----------------------------------------------------------------------------
// OEM_SkipMainServiceFunc
//  Note:
//      1. Always return(0xFF|Normal_MainService) to run normal main_service function.
//      2. If you don't understand the use of OEM_SkipMainServiceFunc function, don't change anything.
//----------------------------------------------------------------------------
BYTE OEM_SkipMainServiceFunc(void)
{

    // Return :
    return(Normal_MainService);
}


//-----------------------------------------------------------------------------
const BYTE code SEG7_Table[]=
{
	/*            a
               ---------
               |       |
           f   |   g   |  b
              ---------
               |       |
           e   |       |  c
               ---------
                   d      . h
     */
	// COMMON ANODE SEVEN SEGMENT DISPLAY TABLE
    //  0	  1     2     3     4     5     6     7
       0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    //  8     9     A     b     C     d     E     F
       0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E,
};

//-----------------------------------------------------------------------------
// Out7SegLED
//-----------------------------------------------------------------------------
void Out7SegLED(BYTE bData)
{
	BYTE bIndex = 7;
	
	while(bIndex != -1)
	{
		if (bData & BIT(bIndex))
		{ 
		    O_DEBUG_DAT_HI(); 
        }
		else
		{ 
		    O_DEBUG_DAT_LO(); 
        }
		O_DEBUG_CLK_LO();
		O_DEBUG_CLK_HI();
		bIndex--;
	}
}

//-----------------------------------------------------------------------------
// Dark7SegLed
//-----------------------------------------------------------------------------
void Dark7SegLed(void)
{
	Out7SegLED(0xFF);	// drak LED
	Out7SegLED(0xFF);	// drak LED
}


//-----------------------------------------------------------------------------
// P80LedOut
//-----------------------------------------------------------------------------
void P80LedOut(BYTE bData)
{
	BYTE code * data_pntr;

	//if(FgDebugCarIn)							// if card dose not exist, return
	{
		// out upper byte
		data_pntr = SEG7_Table;					// get 7-seg table
		data_pntr +=((bData & 0xF0) >> 4);		// mask upper byte and shit 4 bytes	then get data of table

		BRAM3A = *data_pntr;
		Out7SegLED(*data_pntr);					// display upper value.

		// out lower byte
		data_pntr = SEG7_Table;					// get 7-seg table
		data_pntr += (bData & 0x0F);	 		// mask lower byte then get data of table.

		BRAM3B = *data_pntr;

		Out7SegLED(*data_pntr);					// dispaly lower value.
	}
}


//-----------------------------------------------------------------------------
// PollingBIOS80Port
//-----------------------------------------------------------------------------
void PollingBIOS80Port(void)
{
	if ( SystemNotS0 )
	{
		EC_RX_INPUT;
		EC_TX_INPUT;
        UART_DB_RAM = 0x00; 
		return;
	}

	//if(UART_DB_RAM != 0xEE)  //T043-   // For Only once detect
	if((IS_MASK_CLEAR(UART_DB_RAM,BIT7))&&(IS_MASK_CLEAR(UART_DB_RAM,BIT3))) 
	{
		if(Read_EC_TX())     // EC_TX for detect debug card plug in Dont hot plug
		{
			GPCRE6 = OUTPUT;
	    	GPCRC7 = OUTPUT; //EC_RX and BT_DIS common used WLAN/BT Module
			//UART_DB_RAM = 0xEE; 
			UART_DB_RAM = UART_DB_RAM|0x80;
		}

        if(IS_MASK_CLEAR(P80CMOSSts,P80CMOSDis))
        {
            UART_DB_RAM = UART_DB_RAM|0x08;
        }
              
		return;
	}


	BRAM3D++;
	if(BRAM3F != BRAM3E)				// if no data in , return.
	{	// clear it.
		printf("\nP80h : 0x%bX",BRAM3F);
		BRAM3E=	BRAM3F;					// clear it.
		DelayXms(1);
		/*
		DelayXms(1);

		EC_RX_OUTPUT;
		EC_TX_OUTPUT;
        	P80LedOut(BRAM3F);				// to show it
        	EC_RX_ALT;
		EC_TX_ALT;
		*/
	}
	else
	{
		if(BRAM3F != BRAM3C)			// if no data in , return.
		{
			BRAM3C= BRAM3F;				// clear it.
        	//GCR15 &= 0x3F;
            if(IS_MASK_SET(UART_DB_RAM,BIT7))  //T042 +  T043A:IS_MASK_SET
            {
				EC_RX_OUTPUT;
				EC_TX_OUTPUT;
        	    P80LedOut(BRAM3F);			// to show it
            }      
			// start for save P80 code to setup. 
        	if((P80Index < 0x07)&&(IS_MASK_CLEAR(P80CMOSSts,P80CMOSDis)))
			{
				P80CMOS[P80Index]= BRAM3F;
				P80Index++;
				if(P80Index==0x07)
				{
			    	P80Index=0;
				}
			}
			// end for save P80 code to setup. 
       		//GCR15 |= 0xC0;
            if(IS_MASK_SET(UART_DB_RAM,BIT7)) 
            {
        		EC_RX_ALT;
				EC_TX_ALT;
            }
		}
	}
}

void ProcessSID(BYTE SID)	// Shutdown ID.
{
	pLastSID4 = pLastSID3;
	pLastSID3 = pLastSID2;
	pLastSID2 = pLastSID;
	pLastSID  = SID;

	Update_16ByteEEPROM();
}

//----------------------------------------------------------------------------
// Output : 1 Debug mode
//          0 Normal mode
//----------------------------------------------------------------------------
//T043: start for save P80 code to setup. 
bit CheckDebugMode(void)
{	// To add condition here
	BYTE BKSOL,BKSOH1,BKSOH2;
    BKSOL = KSOL;
    BKSOH1 = KSOH1;
    BKSOH2 = KSOH2;

	KSOL  = 0xFF;
	KSOH1 = 0xBF;
	KSOH2 = 0xFF;			// Fn key (scan line b14)
	WNCKR = 0x00;           // Delay 15.26 us
	WNCKR = 0x00;           // Delay 15.26 us
	
	if ((KSI&0x10) != 0)	// Fn key (data line b4)
	{
		KSOH1 = 0xFF;
        KSOL  = BKSOL;
        KSOH1 = BKSOH1;
        KSOH2 = BKSOH2;
	    return FALSE;
	}
	KSOL  = 0xFD;
	KSOH1 = 0xFF;
	KSOH2 = 0xFF;			// "D" key (scan line b1)
	WNCKR = 0x00;           // Delay 15.26 us
	WNCKR = 0x00;           // Delay 15.26 us
	
	if ((KSI&0x10) != 0)	// "D" key (data line b4)
	{
		KSOL  = 0xFF;
        KSOL  = BKSOL;
        KSOH1 = BKSOH1;
        KSOH2 = BKSOH2;
	    return FALSE;
	}
	KSOL  = 0xFF;
    KSOL  = BKSOL;
    KSOH1 = BKSOH1;
    KSOH2 = BKSOH2;

	return TRUE;
}

//ANGELAS094:s+Add workaround for hang bios.
void Workaround_For_HangBios(void)
{
	if(Bioswatchdog==1)
	{
		Bioswatchdogtime=Bioswatchdogtime+1;
		if(Bioswatchdogtime==60)
		{
			BRAM[63]=0x55;
			bios_count++;
            WinFlashMark=0x53;//Modify after flash EC can auto poweron follow bios.
           	WinFlashMark2=0x35;//Modify after flash EC can auto poweron follow bios.
        	WDTRST = 1;				// Reset watch dog timer
			WDTEB = 1;				// Enable watch dog
       		while(1);				// Wait for watch dog time-out
		}
	}
	else
	{
		Bioswatchdogtime=0;
	}
}
//ANGELAS094:e+Add workaround for hang bios.

//MEILING039:S+ add CPU and GPU prochot release after AC out 2S.
void ACOutProchotRelease(void)
{
	if(CPUProchotONCnt > 0) //MEILING055:modify ProchotONCnt name.
	{ 
		CPUProchotONCnt--; 
	}

	//MEILING055:S+
	if(GPUProchotONCnt > 0)
	{ 
		GPUProchotONCnt--; 
	}
	//ANGELAG017: add start
	if(BatteryOCPDelay > 0) 
	{ 
		BatteryOCPDelay--; 
	}
	//ANGELAG017: add end
	//MEILING055:E+
	
	//MEILING042:S- remove.
	/*if(ProchotONCnt == 0)
	{
		GPU_PROCHOT_OFF();
		GPU_PROCHOT_INPUT; 
	}*/
	//MEILING042:E- remove.
}
//MEILING039:E+.

//MEILING042:S+ add GPU prochot on after D5 3S.
void GPUProchotOnControl(void)
{
//	if(SystemIsS0 && cGPUBattThrottling == 4) //ANGELAG017: remove
	//if((SystemIsS0) && ( (cGPUBattThrottling == 4)  || (cGPUBattOCPThrottling == 4) )) //ANGELAG019: remove //ANGELAG017: add
	//ANGELAG024: remove start
	/*if((SystemIsS0) && ( (cGPUBattOTPThrottling == 4)  || (cGPUBattOCPThrottling == 4) || (cGPUBattPsysThrottling == 4))) //ANGELAG019: add
		GPUProchotDelay++;
	else
		GPUProchotDelay = 0;*/  //ANGELAG024: remove end
		
	//ANGELAG050: add start	
	//add D5 to prochot delay counter
	if((SystemIsS0) && ( (cGPUBattOTPThrottling == 4)  || (cGPUBattLowThrottling == 4))) //ANGELAG019: add
		GPUProchotDelay++;
	else
		GPUProchotDelay = 0;
	//ANGELAG050: add end


	//if((GPUProchotONCnt == 0)&&(GPUProchotDelay < 20)) //ANGELAG020: remove //ANGELAG019: 500ms to 2s //MEILING055:modify ProchotONCnt name.
	//if((GPUProchotONCnt == 0)&&(GPUProchotDelay < 20)&& (IS_MASK_CLEAR(GPU_Prochot, (b0_Thermal)))) //ANGELAG024: remove //ANGELAG020: add
	//ANGELAG028: add start
	if(SystemNotS0)
	{
		GPU_Prochot = 0;
		//ANGELAG033: remove start
		/*
		GPU_PROCHOT_OFF();
		GPU_PROCHOT_INPUT;
		return;
		*/ //ANGELAG033: remove end
	}
	//ANGELAG028: add end
	//if((SystemIsS0) && (GPUProchotONCnt == 0) && (IS_MASK_CLEAR(GPU_Prochot, (b0_Thermal+b1_Psys+b2_battery_OCP+b3_battery_OTP+b4_battery_low)))) //ANGELAG028: remove //ANGELAG024: add
	if((GPUProchotONCnt == 0) && (GPUProchotDelay < 20) && (IS_MASK_CLEAR(GPU_Prochot, (b0_Thermal+b1_Psys+b2_battery_LTP+b3_battery_OTP+b4_battery_low+b5_adapter)))) //COKEYXU039: add b2_battery_LTP check
	{
		GPU_PROCHOT_OFF();
		GPU_PROCHOT_INPUT;
	}
	else
	{
		GPU_PROCHOT_OUTPUT;
		GPU_PROCHOT_ON();
		GPUProchotDelay = 20; //ANGELAG050: add 2s delay //ANGELAG024: remove delay //ANGELAG019: 500ms to 2s
	}
//ANGELAG013: add start
/*	if((SystemIsS0) && (cGPUBattLowThrottling == 4)) //ANGELAG024: remove start
	{
		GPU_PROCHOT_OUTPUT;
		GPU_PROCHOT_ON();
	}*/ //ANGELAG024: remove end
//ANGELAG013: add end
}
//MEILING042:E+.

//MEILING052:S+ add CPU throttling 0x1D event delay.
void CPUThrottlingDelay(void)
{
	if(CPUThrottlingDelayTime > 0)
		CPUThrottlingDelayTime--;
}
//MEILING052:E+

