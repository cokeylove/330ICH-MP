/*-----------------------------------------------------------------------------
 * TITLE: OEM_EVENT.C
 *
 * Author : Dino
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>

//-----------------------------------------------------------------------------
// no change function

//-----------------------------------------------------------------------------
void NullEvent(void)
{

}

/*
*******************************************************************************
* Function name: AdapterIn       
*
* Descriptoin: Adapter in function.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void AdapterIn(void)
{
	PECI_SETPL2Watts(2);
	
	if(SystemIsS0)
	{
        AC_PRESENT_HI(); 
    }
	ACInOutIntEnable(); 
	Unlock_ShipMode();  
	SET_MASK(SYS_STATUS,AC_ADP);	//set AC flag
	if( IS_MASK_SET(nBatteryStatus1, ENEXIST) )  //add check battery state
	{
		Chk_Battery_Full();
	}

   	ECSMI_SCIEvent(ACPI_ACIN_SCI);
   	if( SystemIsS0 )
	{
		PWM_BEEP = 0x7F;			// Out beep
		PWM_LEDBeep_CNT = 0x0A;		// 50ms*10
	}

	InitChargerIC();//Change charge IC option setting.
	BatSMbusFailCount = 0;
	nBattErrorCnt = 0;
	// Set 0x12h and 0x3Fh before set ChargeCurrent and ChargeVoltage.
	CHGIC_ptr = 1;
	WriteSmartChgIC();
	WriteSmartChgIC();
	CHGIC_ptr = 3;
	ReadSmartChgIC();
    if (IS_MASK_SET(SYS_STATUS,AC_ADP)) 
    {
	    AdapterIDOn_Flag = 0x01; 
        ADPIDON10MS_NUM =0x0A; //change 0x05 to 0x0A to avoid read dirty data.
    }    
}

/*
*******************************************************************************
* Function name: AdapterOut       
*
* Descriptoin: Adapter out function.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void AdapterOut(void)
{
	AdapterIDOn_Flag = 0x00;
		
	BATT_LEN_OFF();  //remove AC_OFF function follow charge IC bq24780.
	AC_PRESENT_LOW(); 
	ACInOutIntEnable(); 
    CLEAR_MASK(SYS_STATUS,b4IllegalAdp);
	CLEAR_MASK(SYS_STATUS,b5UnSuitAdpPow);
	SetVGA_AC_DET();
	PECI_SETPL2Watts(1);

	CLEAR_MASK(SYS_STATUS,AC_ADP);  //clear battery flag
	Chk_Battery_Full();
   	ECSMI_SCIEvent(ACPI_ACOUT_SCI);

	//Set P-State highest to force that ChkBattery_OCP will decress P-State step by step.
	SET_MASK(BatteryAlarm,BATOCP);
	ThrottlingControl(); 

   	if( SystemIsS0 )
	{
		PWM_BEEP = 0x7F;			// Out beep
		PWM_LEDBeep_CNT = 0x0A;		// 50ms*10


        if(IS_MASK_SET(LENOVOPMFW,BATTERY_MAIN_CAL))
        {
		    uVPCeventSource = General;
		    uVPCeventSource2 = 0;
		    ECSMI_SCIEvent(SDV_VPC_notify);
        }
	}

	nAvgCurrentL = 0;
	nAvgCurrentH = 0;
	Bat0x0BFakeCnt = 60;
    ADPIDON10MS_NUM=0xFF; 
	Charger_3D_Config();
}


/*
*******************************************************************************
* Function name: PSWPressed       
*
* Descriptoin: power button was pressed.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void PSWPressed(void)
{
	if ( IS_MASK_SET(cCmd, b6TestBtnEn) )
	{
		Buffer_Key(0xE0);		//
		Buffer_Key(0x25);		//
		F_Service_SEND = 1;
	}

	ECSleepCount = 3;

	//Clear PswPressedCounter
	CountSecAfterPswPressed = 0;

	CLEAR_MASK(SysStatus2,PWRBTN_press_retry);

	if(!Read_LID_SW_IN())
	{ return; }

	
    switch (SysPowState)
    {
    case SYSTEM_S4 :
    	break;
        
    case SYSTEM_DSX:
    case SYSTEM_S5 :
		//S+Modify code for befor power on check Battery RSOC in DC mode.
		if(!Read_AC_IN()||IS_MASK_SET(ACOFF_SOURCE, BATTLEARN))
		{			 	
			BatSMbusFailCount = 0;
			if(!bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr, C_RSOC, &BAT1PERCL,SMBus_NoPEC))
			{
				BatSMbusFailCount++;	
				while((BatSMbusFailCount<=4)) 
				{		
					if(!bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr, C_RSOC, &BAT1PERCL,SMBus_NoPEC))
					{
						BatSMbusFailCount++;
					}
					else
			 		{
						BatSMbusFailCount=0;
			            break; 
					}
				}
			 				  			 			 
			}
			if(BatSMbusFailCount>=4) 
				break;
			
			if (BAT1PERCL < 0x02)  //modify battery capacity point from 0 to 1 for fliker.
			{
				SET_MASK(nBatteryStatL,CMBS_LOWBATT);
				LOWBATT_3TIMES = 100;		// set 5 sec
				break;
			}
		}

		if ((!Read_AC_IN()))			// if DC mode
		{
			BatSMbusFailCount = 0;
			nBattErrorCnt = 0;
			if (IS_MASK_SET(LENOVOBATT,BATTERY_LEGAL))	//SHA1 fail
			{
				Service_Auth_Step = 1;				// start SHA1 again
				SHA1failCnt = 1;
			}
		}
        if(IS_MASK_SET(SysStatus2, b3EC8S_Reset))
        {
            break;
        }
		uNovoVPCCount = 0;
		PWSeqStep = 1;
        PowSeqDelay = 0x00;
        RamDebug(0x51);
		SysPowState=SYSTEM_S5_S0;
		break;

    case SYSTEM_S3 :
	case SYSTEM_S0 :
		RamDebug(0xF4);
		if (IS_MASK_CLEAR(pProject4,pPWSWdisable)) //Add CMD for press power button 4s don't shutdown when flash bios.
        {            
			PM_PWRBTN_LOW();	
        }

        e0_prefix_code(0x1F,MAKE_EVENT);
		simple_code(0x22,MAKE_EVENT);
		SET_MASK(pProject4, b1WinAndXpress);
		F_Service_SEND = 1;
		break;

    default :
       	break;
    }
}

/*
*******************************************************************************
* Function name: PSWReleased       
*
* Descriptoin: power button was released.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void PSWReleased(void)
{
	if ( IS_MASK_SET(cCmd, b6TestBtnEn) )
	{
		Buffer_Key(0xE0);	//
		Buffer_Key(0xF0);	//
		Buffer_Key(0x25);	//
		F_Service_SEND = 1;
	}
    
    if(IS_MASK_SET(SysStatus2, b3EC8S_Reset)) 
	{
		CLEAR_MASK(SysStatus2, b3EC8S_Reset);
	}

    if(!Read_LID_SW_IN())
	{ return; }


	PSW_COUNTER = T_PSWOFF;	// initialize counter
	switch (SysPowState)
	{
	case SYSTEM_S4 :
		break; 

	case SYSTEM_S5 :
		break;

	case SYSTEM_S3 :
	case SYSTEM_S0 :
		PM_PWRBTN_HI();
		//ECQEvent(ACPI_PWRBTN_SCI); //Send POWER Button Low Event when Release Power button
		break;

	default :
		break;
	}
}

/*
*******************************************************************************
* Function name: PSWOverrided       
*
* Descriptoin: power button was overrided.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void PSWOverrided(void)
{
	if(!Read_LID_SW_IN())
	{ return; }
    if (Read_EC_PWRBTN()&&Read_EC_NOVO())
    { return; }
}


/*
*******************************************************************************
* Function name: NOVOPressed       
*
* Descriptoin: NOVO button was pressed.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void NOVOPressed(void)
{
	if ( IS_MASK_SET(cCmd, b6TestBtnEn) )
	{
		Buffer_Key(0xE0);	//
		Buffer_Key(0x49);	// page down make
		F_Service_SEND = 1;
	}

	ECSleepCount = 3;

	CLEAR_MASK(SysStatus2,b1NOVOBTN_retry);

    if(!Read_LID_SW_IN())
	{ return; }

	//Clear NOVOPressedCounter
	CountSecAfterNOVOPressed = 0;

	switch (SysPowState)
    {
	case SYSTEM_S4 :
    case SYSTEM_S5 :
    case SYSTEM_DSX:
		//Modify code for befor power on check Battery RSOC in DC mode.
		if(!Read_AC_IN()||IS_MASK_SET(ACOFF_SOURCE, BATTLEARN))
		{			 	
			BatSMbusFailCount = 0;
			if(!bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr, C_RSOC, &BAT1PERCL,SMBus_NoPEC))
			{
				BatSMbusFailCount++;	
				while(BatSMbusFailCount <=4) 
				{		
					if(!bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr, C_RSOC, &BAT1PERCL,SMBus_NoPEC))
					{
						BatSMbusFailCount++;										 
					}
					else
			 		{
						BatSMbusFailCount=0;
			 			break;
					}
				}
			 				  			 			 
			}
			if(BatSMbusFailCount>=4) 
			 	break;
			
			if (BAT1PERCL < 0x02)  //modify battery capacity point from 0 to 1 for fliker.
			{
				SET_MASK(nBatteryStatL,CMBS_LOWBATT);
				LOWBATT_3TIMES = 100;		// set 5 sec
				break;
			}
		}
		//Modify code for befor power on check Battery RSOC in DC mode.
		if(LOWBATT_3TIMES == 0)
        {
          	// NOVO button should be no function when resuming from S4
	        if( (EEPROM_PwrSts & 0x10) == 0 )
	        {
				if ( (EEPROM_PwrSts & 0x01) != 0 )
				{ 
				    break; 
                }
	        }
			uNovoVPCCount = 1;
			PWSeqStep = 1;
			PowSeqDelay = 0x00;
            RamDebug(0x52);
			SysPowState=SYSTEM_S5_S0;
		}
        break;

	case SYSTEM_S3 :
      	break;
	case SYSTEM_S0 :
		// NOVO button should initialize ONEKEY Recovery Windows Module,
		// or be no function if ONEKEY Recovery Windows Module hasn't been installed
		if ( IS_MASK_SET(SYS_MISC1, ACPI_OS) )
		{
			uNovoVPCCount++;
			if( uNovoVPCCount > (10+1) )
				{ uNovoVPCCount--; }

		}
    default :
        break;
    }
}


/*
*******************************************************************************
* Function name: NOVOReleased       
*
* Descriptoin: NOVO button was released.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void NOVOReleased(void)
{
	NOVO_COUNTER = T_PSWOFF;//Enable SMI for hang debug.
	if ( IS_MASK_SET(cCmd, b6TestBtnEn) )
	{
		Buffer_Key(0xE0);	//
		Buffer_Key(0xF0);	//
		Buffer_Key(0x49);	// page down make
		F_Service_SEND = 1;
	}

    if(!Read_LID_SW_IN())
	{ return; }

	switch (SysPowState)
	{
	case SYSTEM_S4 :

	case SYSTEM_S5 :

	case SYSTEM_S3 :
		break;
	case SYSTEM_S0 :
		if ( IS_MASK_SET(SYS_MISC1, ACPI_OS) )
		{
			if( uNovoVPCCount > 10 )		// Delay 100 mSec.
		    { uNovoVPCCount = 1; }
		    else
			{ uNovoVPCCount = 0; }
			uVPCeventSource = VPCeventNovo; // bit3:NOVO.
		    uVPCeventSource2 = 0;
			ECQEvent(SDV_VPC_notify);		// 0x44 for Levono used.
		}
	default :
		break;
	}
}

/*
*******************************************************************************
* Function name: NOVOOverrided       
*
* Descriptoin: NOVO button was overrided.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void NOVOOverrided(void)
{
	//Enable SMI for hang debug.
	if (!Read_EC_NOVO())
	{
		return; 
	}
	if(NOVO_COUNTER != 0)
	{
		NOVO_COUNTER--;
		return;
	}
	NOVO_COUNTER = T_PSWOFF;
	ECSMIEvent();
	//Enable SMI for hang debug.
}

/*
*******************************************************************************
* Function name: LanWakeLow       
*
* Descriptoin: LAN_WAKE was low.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void LanWakeLow(void)
{
	#if	Support_EC_LANWake

	switch (SysPowState)
	{
	case SYSTEM_S4 :
		break;

	case SYSTEM_S5 :
		break;

	case SYSTEM_S3 :
		if ( IS_MASK_SET(AOAC_STATUS, ISCT_Enable) )	// Check ISCT enable
		{
			PM_PWRBTN_LOW();
			AOAC_STATUS |= 0x80;	// Set PME(LAN) wake function.
		}
		else
		{
			if ( IS_MASK_SET(SYS_STATUS,AC_ADP) )
 			{ 
 				RamDebug(0x99);
				PCIE_WAKE_OUTPUT;
				PCIE_WAKE_LOW();
            } // Set PME(LAN) wake function.
		}
		break;

	case SYSTEM_S0 :
		break;

	default :
		break;
	}
	#endif	// Support_EC_LANWake
}

/*
*******************************************************************************
* Function name: LanWakeHi       
*
* Descriptoin: LAN_WAKE was Hi.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void LanWakeHi(void)
{
	#if	Support_EC_LANWake

	switch (SysPowState)
	{
	case SYSTEM_S4 :
		break;

	case SYSTEM_S5 :
		break;

	case SYSTEM_S3 :
		PM_PWRBTN_HI();
        RamDebug(0x9A);
		PCIE_WAKE_HI();
		PCIE_WAKE_INPUT;
		break;

	case SYSTEM_S0 :
		PM_PWRBTN_HI();
		PCIE_WAKE_HI();
		PCIE_WAKE_INPUT;
		break;

	default :
		PM_PWRBTN_HI();
		PCIE_WAKE_HI();
		PCIE_WAKE_INPUT;
		break;
	}
	#endif	// Support_EC_LANWake
}

/*
*******************************************************************************
* Function name: Battery1In       
*
* Descriptoin: battery 1 in function.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Battery1In(void)
{
	SET_MASK(SEL_STATE0,PRESENT_A);  	//set battery flag
	SET_MASK(nBatteryStatus1,ENEXIST);  //set battery flag
	FirstGetBatData();					//first get battery data
   	ECSMI_SCIEvent(ACPI_BAT1IN_SCI);
	Service_Auth_Step = 1;				// start SHA1
	//Clean Get_Batt_debounce_count to make it check battery data stable or not?
	Get_Batt_debounce_count = 0;
}

/*
*******************************************************************************
* Function name: Battery1Out       
*
* Descriptoin: battery 1 out function.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Battery1Out(void)
{
	PORT_BYTE_PNTR BAT1_pntr;

	CLEAR_MASK(SEL_STATE0,PRESENT_A);  	//clear battery flag
	CLEAR_MASK(nBatteryStatus1,ENEXIST);	//clear battery flag
	BAT1_pntr = &SEL_STATE0;	// Base on address 0x04BF.
	for( ITempW01=0; ITempW01<=57; ITempW01++ )	// Clear 04BF~04F8.
	{
		*BAT1_pntr=0;
        BAT1_pntr++;
	}

	BAT1_pntr = &BATTMANUFACTURE;	// Base on address 0x048F.
	for( ITempW01=0; ITempW01<=16; ITempW01++ )	// Clear 048F~049F.
	{
		*BAT1_pntr=0;
        BAT1_pntr++;
	}


    if(SystemIsS0 && IS_MASK_SET(LENOVOPMFW,BATTERY_MAIN_CAL))
    {
        uVPCeventSource = General;
		uVPCeventSource2 = 0;
		ECSMI_SCIEvent(SDV_VPC_notify);
    }

	Battdata_ready = 0;
	BatSMbusFailCount = 0;
	nBattErrorCnt = 0;
	LENOVOBATT = 0;
	EM8_TEST = 0;
	LV_Authen_Step_CNT = 0;
	SHA1failCnt = 0;
    SHA1FailRetry=0x00;
	Bat1_FPChgFlag = 0;
	CellCount=0; 
	ECSMI_SCIEvent(ACPI_BAT1IN_SCI);
}


/*
*******************************************************************************
* Function name: LID_Pressed       
*
* Descriptoin: Lid pressed.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void LID_Pressed(void)
{
    CLEAR_MASK(SWI_EVENT,LID);
	ECQEvent(SCI_LID_Close);
	RamDebug(0x15);
	EC_TS_ON_LOW();//Add touch panel control signal. 
	
}

/*
*******************************************************************************
* Function name: LID_Released       
*
* Descriptoin: Lid released.
*
* Invoked by: EventManager()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void LID_Released(void)
{
	SET_MASK(SWI_EVENT,LID);
	if ((SystemIsS3))
	{
		PulseSBPowerButton();
	}
    ECQEvent(SCI_LID_Open);
	RamDebug(0x16);
	EC_TS_ON_HI();//Add touch panel control signal.  
}

// ----------------------------------------------------------------------------
// Device insert/remove debounce routine.
// input:   device_id
// ----------------------------------------------------------------------------

const struct sDebounce code Debounce_TBL[] =
{

  	{&POWER_FLAG1	,wait_PSW_off	  ,&DEBOUNCE_CONT1	 ,T_PSW_DEBOUNCE	   	,PSWPressed	    ,PSWReleased    ,PSWOverrided	},
    {&POWER_FLAG1	,adapter_in 	  ,&DEBOUNCE_CONT2 	 ,T_AC_DEBOUNCE		 	,AdapterIn	    ,AdapterOut	   	,NullEvent	    },
	{&BT1_STATUS1	,bat_in			  ,&DEBOUNCE_CONT3 	 ,T_BAT_DEBOUNCE	    ,Battery1In	    ,Battery1Out	,NullEvent		},
	{&POWER_FLAG1	,wati_NOVO_off	  ,&DEBOUNCE_CONT4 	 ,T_NOVO_DEBOUNCE	   	,NOVOPressed	,NOVOReleased   ,NOVOOverrided  }, //Modify debunce time define.
	{&POWER_FLAG1	,LAN_WAKE_Status  ,&DEBOUNCE_CONT5 	 ,T_LAN_WAKE_DEBOUNCE 	,LanWakeLow	    ,LanWakeHi      ,NullEvent		},
	{&POWER_FLAG1	,Lid_Act       	  ,&DEBOUNCE_CONT6 	 ,T_LID_DEBOUNCE	    ,LID_Pressed	,LID_Released	,NullEvent		},//change LID implement method.
	{&EVT_STATUS1	,DummyFlag		  ,&DEBOUNCE_CONT7 	 ,T_EXTEVT_DEBOUNCE	 	,NullEvent	    ,NullEvent	   	,NullEvent	    },
	{&EVT_STATUS1	,DummyFlag 		  ,&DEBOUNCE_CONT8 	 ,T_EXTEVT_DEBOUNCE   	,NullEvent	    ,NullEvent		,NullEvent	    },
	{&EVT_STATUS1	,DummyFlag	      ,&DEBOUNCE_CONT9 	 ,T_EXTEVT_DEBOUNCE   	,NullEvent	    ,NullEvent		,NullEvent	    },
	{&EVT_STATUS1	,DummyFlag		  ,&DEBOUNCE_CONT10  ,T_EXTEVT_DEBOUNCE	 	,NullEvent	    ,NullEvent		,NullEvent	    },
};


/*
*******************************************************************************
* Function name: EventManager       
*
* Descriptoin: Event manager.
*              If want to call the function of status not change, please set nochange_func = 1
*
* Invoked by: Hook_Timer1msEvent()
*
* TimeDiv：1ms
*
* Arguments: EventId   
*
* Return Values: NA
*******************************************************************************
*/
#pragma OT(8, SPEED)
void EventManager(BYTE device_id)
{
	BYTE new_state,old_state;
	BYTE device_flag;
	switch(device_id)
	{
	case 0 :
    	if (Read_EC_PWRBTN()&&Read_EC_NOVO()) 	return;  
        new_state = Read_EC_PWRBTN();                            
		break;

	case 1 :
		new_state = Read_AC_IN();
		break;

	case 2 :
		new_state=Read_BAT_IN();//ADD battery detect
		break;

	case 3 :	
        new_state = Read_EC_NOVO(); 
		break;

	case 4 :
		#if	Support_EC_LANWake
		new_state = Read_LAN_WAKE_IN();
		#else
		new_state = 0x00;
		#endif	// Support_EC_LANWake
		break;

	case 5 :
		if(IS_MASK_CLEAR(SysStatus,LidKBIgnore)) 
		{
			#if LID_OnlyQEvent_Support 
			new_state = !Read_LID_SW_IN(); //Open: LID_SW_IN: high else: low
			#else
			new_state = 0x00;
			#endif
		}
		else
		{
			new_state = 0x00;
		}
		break;
	case 6 :
		new_state = 0x00;
		break;

	case 7:
        new_state = 0x00;
		break;

	case 8:
		new_state = 0x00;
		break;

	case 9:
        new_state = 0x00;
		break;

	default :
		new_state = 0x00;
		break;
	}

	Tmp_XPntr = Debounce_TBL[device_id].REG;
	Tmp_XPntr1 = Debounce_TBL[device_id].Cunter;
	device_flag = Debounce_TBL[device_id].Flag;

	old_state = ((*Tmp_XPntr & device_flag) != 0x00);
	if(new_state != old_state)
	{
		if((*Tmp_XPntr1) == 0x00)
		{
			//LMLNANO008:S+ modify power button debunce time in S0.
			if((device_id==0) && (SystemIsS0) && (IS_MASK_CLEAR(cCmd, b6TestBtnEn))&&(IS_MASK_CLEAR(POWER_FLAG1, wait_PSW_off))) //ANGELAG038: add
				(*Tmp_XPntr1) = T_PSW_DEBOUNCE_S0;
			else
				(*Tmp_XPntr1) = Debounce_TBL[device_id].Time;
		}
		else
		{
			(*Tmp_XPntr1) --;
            
			if((*Tmp_XPntr1) == 0)
			{
				if(new_state)
				{
					(Debounce_TBL[device_id].press)();
					*Tmp_XPntr |= device_flag;
				}
				else
				{
					(Debounce_TBL[device_id].release)();
					*Tmp_XPntr  &= (~device_flag);
				}
			}
		}
	}
	else
	{
		*Tmp_XPntr1 = 0x00;
	}

	(Debounce_TBL[device_id].nochange)();
}

