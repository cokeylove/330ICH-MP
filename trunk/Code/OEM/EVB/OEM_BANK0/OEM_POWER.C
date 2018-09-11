/*----------------------------------------------------------------------------
 * Filename: OEM_Power.C  For Chipset: ITE.IT85XX
 *
 * Function: Power sequnce control function example
 *
 * Author  : Dino
 *
 * Copyright (c) 2009 - , ITE Tech. Inc. All Rights Reserved.
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>


/*
*******************************************************************************
* Function name: CheckResetSource      
*
* Descriptoin: To read variable [RSTStatus] to get EC reset source.
*
* Invoked by: STARTUP.A51
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void CheckResetSource(void)
{
//add debug information for EC reset status(1.external watch-dog, 2.internal watch-dog; 3. EC VSTBY or WRST reset,4.EC code reset).
    RamDebug(0xBB); 
    RamDebug(RSTStatus);
    RamDebug(GPCRA0);
    RamDebug(PWRSW8SRST);  
    RamDebug(0xBB);    
    switch (RSTStatus & 0x03)
    {
        case 0:
        case 1:
            if (GPCRA0 == 0x80)
            {
                ShutDnCause = SC_ECColdBoot;
            }
            else
            {
                ShutDnCause = SC_EC_0x0000;
            }
            break;

        case 2:
            if (GPCRA0 == 0x80)
            {
                ShutDnCause = SC_IntWatchDog;
            }
            else
            {
                ShutDnCause = SC_EC_0x0000;
            }
            break;

        case 3:
            if (GPCRA0 == 0x80)
            {
                ShutDnCause = SC_ExtWatchDog;
            }
            else
            {
                ShutDnCause = SC_EC_0x0000;
            }

            break;
    }

    if (IS_MASK_SET(PWRSW8SRST, BIT(1)))
    {
        ShutDnCause = SC_EC8SRESET;
    }
}


/*
*******************************************************************************
* Function name: Check_LPC_Reset      
*
* Descriptoin: Check LPC reset.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Check_LPC_Reset(void)
{
	if(!Read_LPC_RST())
	{
   		CLEAR_MASK(KBHISR,SYSF);
	    CLEAR_MASK(SYS_MISC1,ACPI_OS);
		OEM_LPC_Reset();	// initial OEM status.
		//fixed reboot systen TP device lost when disable TP without driver
		TPCLK_OUTPUT;
        TPCLK_HI();
        TPCLK_ALT;
	}
}

/*
*******************************************************************************
* Function name: OEM_LPC_Reset      
*
* Descriptoin: OEM LPC Reset.
*
* Invoked by: Check_LPC_Reset()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void OEM_LPC_Reset(void)
{
	BYTE BSLI_Det;
	uReserve07.fbit.nTPDriverIn = 0;
	uReserve07.fbit.nVPCDriverIn = 0;
	uReserve07.fbit.nFanManual = 0; //Clear thermal tool flag under S3/S4/S5. return EC control
	CPU_TYPE &= 0x3f;//CLEAR BIT6 and BIT7 Unit abnormal shuntdown will not clear GPU flag,so when enter S5,WB and S3 we clear GPU flag to not read GPU temperature.
	SET_MASK(pDevStus, pENABLE_TP);	// Turn on TouchPad.
	CLR_MASK(cCmd, b3BkOff);		// Turn on backlight.
	CLR_MASK(OEMControl, b1BkOff);	
	SMartNoise = 0x04;				// Stop Fan Dust mode.
	BackLight_En_Delay = 40;			//Increase lit backlight time for reboot.//:Panel background light delay about 1s after Lenovo logo show out when reboot.

	CLEAR_MASK(LENOVOPMFW,BATTERY_MAIN_CAL);
	CLEAR_MASK(LENOVOPMFW,BATTERY_CAL_END);
	LENOVOPMFW_Temp = 0;
	CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);

	CLEAR_MASK(pDevStatus3,b6EnDisTPSend);
	CLEAR_MASK(pDevStatus3,b7TPStateFormer);

	
	CLEAR_MASK(pDevStatus1,b0F6ScanCodeSend);
    CLEAR_MASK(pDevStatus1,b1F6DisableTP);

	CLEAR_MASK(pDevStatus1,b4LNVAPP_STATUS);
	CLEAR_MASK(pDevStatus1,b5TPDRIVER_STATUS);
	CLEAR_MASK(pDevStatus1,b6MODE_STATUS);
	CLEAR_MASK(pDevStatus1,b7DisableTP);
	DisCPUTurboFor90s = 90;                         //COKEYXU029:add
	CountOfWinAndX = 10;
	CLR_MASK(uVGATurboFun, uDisCPUTurboOK);	     //COKEYXU029: add
}



//============S5 -> DSX==========================================================

void S5DSX_RSMRST_LOW()
{
    RSMRST_LOW();
}

void S5DSX_PCH_PWR_EN_OFF()
{
    PCH_PWR_EN_OFF();

}

void S5DSX_EC_ON_LOW()
{
    EC_ON_LOW(); 
}

void S5DSX_PWR_BTN_LOW()
{
    PM_PWRBTN_LOW();
}

/*
*******************************************************************************
* Function name: Enter_ShipMode      
*
* Descriptoin: Enter Ship mode.
*
* Invoked by: Oem_S5DSXSequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Enter_ShipMode()
{
#if shipmodesupport
    if((ShipModeEn == 0xA5) || IS_MASK_SET(EMStatusBit2, b0SetBatteryShipMode)) 
    {
        SET_MASK(BATTUPDATEFW, PriBattInhib);
        Delay1MS(200); 
        Delay1MS(100); 
        Delay1MS(250); 
        Delay1MS(250); 
        Delay1MS(250); 
        Delay1MS(250); 
        Lock_ShipMode();

        Delay1MS(250);  
        Delay1MS(250);  
        Delay1MS(250);  
        Delay1MS(250);  
    }
#endif
}

/*
*******************************************************************************
* Function name: RecordenEnterShipMode      
*
* Descriptoin: Recorden Enter Ship mode.
*
* Invoked by: Oem_S5DSXSequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void RecordenEnterShipMode()
{
#if shipmodesupport
    if((ShipModeEn==0xA5)||IS_MASK_SET(EMStatusBit2,b0SetBatteryShipMode))
    {
        ShipModeEn=0x00; 
        ProcessSID(0xA6);
        if(nBattGasgauge < 95)
        {
            ProcessSID(0xA8);
            while(1);
        }
        CLEAR_MASK(BATTUPDATEFW,PriBattInhib);
    }
#endif
}

/*
*******************************************************************************
* Function name: S5DSX_Init_Status      
*
* Descriptoin: S5 to DSX init Status.
*
* Invoked by: Oem_S5DSXSequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S5DSX_Init_Status()
{
    PowerInitOK=0;
    SysPowState = SYSTEM_DSX;
    RamDebug(0x0C);
}

const sPowerSEQ code asPowerSEQS5DSX[]=
{
    {0x0000,                        0,        0,    },
    {S5DSX_RSMRST_LOW,              5,        0,    },
    {S5DSX_PCH_PWR_EN_OFF,          2,        0,    },
    {S5DSX_EC_ON_LOW,               0,        0,    },
    {S5DSX_PWR_BTN_LOW,             5,        0,    },    
    {Enter_ShipMode,              120,        0,    },
    {RecordenEnterShipMode,        20,        0,    },
    {S5DSX_Init_Status,             0,        0,    },
};

/*
*******************************************************************************
* Function name: Oem_S5DSXSequence      
*
* Descriptoin: OEM S5 to DSX sequence.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_S5DSXSequence(void)
{
    if(PowSeqDelay != 0x00 )
    {
        PowSeqDelay--;
        if(PowSeqDelay != 0x00 )
        {
            return;
        }
    }

    while(PWSeqStep<(sizeof(asPowerSEQS5DSX)/sizeof(sPowerSEQ)))
    {
        if(asPowerSEQS5DSX[PWSeqStep].checkstatus==0x00) // Do function
        {
            (asPowerSEQS5DSX[PWSeqStep].func)();
            PowSeqDelay = asPowerSEQS5DSX[PWSeqStep].delay;
            PWSeqStep++;
            if(PowSeqDelay!=0x00)
            {
                break;
            }
        }
        else    // Check input status pin
        {
            if((asPowerSEQS5DSX[PWSeqStep].func)())
            {
                PowSeqDelay = asPowerSEQS5DSX[PWSeqStep].delay;
                PWSeqStep++;
            }
            break;
        }
    }
}


//============DSX -> S5==========================================================
void DSXS5_EC_ON_HI(void)
{
    EC_ON_HI(); 
}

BYTE DSXS5_PMIC_Init(void)
{
    InitPMIC(); 
    return PowerInitOK;
 
}

void DSXS5_PWR_BTN_HI(void)
{
    PM_PWRBTN_HI(); 
}

void DSXS5_PCH_PWR_EN(void)
{
    PCH_PWR_EN_ON(); 
}

void DSXS5_RSMRST_HI(void)
{
    RSMRST_HI(); 
}

/*
*******************************************************************************
* Function name: DSXS5_Init_Status      
*
* Descriptoin: DSX to S5 init Status.
*
* Invoked by: Oem_DSXS5Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void DSXS5_Init_Status(void)
{  
    if(IS_MASK_SET(SYS_STATUS, AC_ADP))
    {
        if(SystemIsS0)
        {
            AC_PRESENT_HI();

        }
    }

    SysPowState = SYSTEM_S5;
    RamDebug(0x0B);
}

const sPowerSEQ code asPowerSEQDSXS5[]=
{
    { 0x0000,                    0,        0,    },
    { DSXS5_EC_ON_HI,            0,        0,    },
    { DSXS5_PMIC_Init,           0,        1,    },
    { DSXS5_PWR_BTN_HI,          1,        0,    },
    { DSXS5_PCH_PWR_EN,         30,        0,    },
    { DSXS5_RSMRST_HI,          20,        0,    },   
    { S5S0_AC_PRESENT_EN,        1,        0,    },  
    { DSXS5_Init_Status,         0,        0,    },
};

/*
*******************************************************************************
* Function name: Oem_DSXS5Sequence      
*
* Descriptoin: OEM DSX to S5 sequence.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_DSXS5Sequence(void)
{
    if(PowSeqDelay != 0x00 )
    {
        PowSeqDelay--;
        if(PowSeqDelay != 0x00 )
        {
            return;
        }
    }

    while(PWSeqStep<(sizeof(asPowerSEQDSXS5)/sizeof(sPowerSEQ)))
    {
        if(asPowerSEQDSXS5[PWSeqStep].checkstatus==0x00) // Do function
        {
            (asPowerSEQDSXS5[PWSeqStep].func)();
            PowSeqDelay = asPowerSEQDSXS5[PWSeqStep].delay;
            PWSeqStep++;
            if(PowSeqDelay!=0x00)
            {
                break;
            }
        }
        else    // Check input status pin
        {
            if((asPowerSEQDSXS5[PWSeqStep].func)())
            {
                PowSeqDelay = asPowerSEQDSXS5[PWSeqStep].delay;
                PWSeqStep++;
            }
            break;
        }
    }
}

/*
*******************************************************************************
* Function name: Oem_SysPowerContrl      
*
* Descriptoin: Handle System Power Control.
*
* Invoked by: Hook_Timer1msEvent()
*
* TimeDiv：1ms
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_SysPowerContrl(void)
{
    // Check current System PowerState to see what should we do.
    switch (SysPowState)
    {
   	case SYSTEM_S0:
		if(!Read_SLPS3())
		{
			if (S3S4DelayCnt > 10)
			{
              	PWSeqStep = 1;
				PowSeqDelay = 00;
                RamDebug(0xD0);        
              	SysPowState=SYSTEM_S0_S3;
				if (!Read_SLPS4())
				{
					PWSeqStep = 1;
					PowSeqDelay = 0x00;
                    RamDebug(0x0D);
    				SysPowState=SYSTEM_S0_S5;
				}
				else if (IS_MASK_SET(SysStatus,CloseLid))
				{
					SET_MASK(SysStatus,EnterS3Lid);
				}
			}
			else
			{	
				S3S4DelayCnt++;	
			}
		}
		else
		{	
			S3S4DelayCnt = 0;		
		}
		Check_LPC_Reset();
		break;

   	case SYSTEM_S3:   
        if(Read_SLPS3())  
        {
            PWSeqStep = 3;
		    PowSeqDelay = 20;
            RamDebug(0x33);   
           	SysPowState=SYSTEM_S3_S0;
        }    
        
        if(IS_MASK_SET(SYS_STATUS,b4IllegalAdp)&&Read_AC_IN())
        {
   		    SET_MASK(SYS_STATUS,AC_ADP);	
			BATT_LEN_OFF();
        }
        break;

    case SYSTEM_S4:
    case SYSTEM_S5:
		if((Read_SLPS3() && Read_SLPS4())) 
		{
			PWSeqStep = 1;
			PowSeqDelay = 0x00;
            RamDebug(0x53);
			SysPowState=SYSTEM_S5_S0;
		}
		CheckAutoPowerOn();
#if shipmodesupport
  		if((ShipModeEn==0xA5)||IS_MASK_SET(EMStatusBit2,b0SetBatteryShipMode))
  		{
  			CLEAR_MASK(ACPI_HOTKEY, b7BIOS_NoShut);
			CLEAR_MASK(ACPI_HOTKEY, b6Cmd_NoShut);
  		}
#endif
	
		if(PowerOTP_REG)
		{
			if ( IS_MASK_CLEAR(SysStatus, ERR_ShuntDownFlag) ) 
			{ 
			    ProcessSID(PMIC_Shutdown_ID);
			} 
			OTP_TEMP = PowerOTP_REG;
			PowerOTP_REG = 0;
			RSMRST_LOW();
			Delay1MS(12);
			EC_ON_LOW();
			PowerInitOK = 0;
			
		}

        if ((IS_MASK_CLEAR(ACPI_HOTKEY, b7BIOS_NoShut)) && (IS_MASK_CLEAR(ACPI_HOTKEY, b6Cmd_NoShut)) && (LOWBATT_3TIMES==0))
        {
            if ((!Read_AC_IN()) && IS_MASK_CLEAR(cCmd, bPCHPWR_Keep)) 
            {
                //COKEYXU007:Enable SYSTEM_S5_DSX.
                PWSeqStep = 1;
                PowSeqDelay = 0x40;
                SysPowState=SYSTEM_S5_DSX;
            }
        }
     
        if(IS_MASK_SET(SYS_STATUS,b4IllegalAdp)&&Read_AC_IN())
        {
   		    SET_MASK(SYS_STATUS,AC_ADP);	
			BATT_LEN_OFF();
        }

        break;
        
    case SYSTEM_DSX:
        CheckAutoPowerOn();
        if((Read_AC_IN()) || (LOWBATT_3TIMES != 0))
        {  
            PWSeqStep = 1;
            PowSeqDelay = 0x00;
            SysPowState=SYSTEM_DSX_S5;
        }
        break;
    case SYSTEM_DSX_S5:
        Oem_DSXS5Sequence();
        break;
    case SYSTEM_S5_DSX:
        Oem_S5DSXSequence();
        break;

    case SYSTEM_S5_S0:
		Oem_S5S0Sequence();
		break;

    case SYSTEM_S3_S0:
		Oem_S3S0Sequence();
        break;

    case SYSTEM_S0_S3:
		Oem_S0S3Sequence();
        break;

    case SYSTEM_S0_S4:
        break;

    case SYSTEM_S0_S5:
		Oem_S0S5Sequence();
        break;

	case SYSTEM_EC_WDTRST:
		break;

   	default:
        SysPowState=SYSTEM_DSX;     
		if(!Read_AC_IN()) 
	    {
            PWSeqStep = 1;
            PowSeqDelay = 0x00;
            SysPowState = SYSTEM_S5_DSX;
        }
		else 
	    {
            PWSeqStep = 1;
            PowSeqDelay = 0x00;
            SysPowState = SYSTEM_DSX_S5;
        }
        break;
    }
}

/*
*******************************************************************************
* Function name: SetS0GPIO      
*
* Descriptoin: Set S0 GPIO.
*
* Invoked by: S5S0_SUSP(),S3S0_SUSP()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void SetS0GPIO(void)
{
	SCI_HI();			  	//RUNSCI Hi
	RCING_HI();
	TPCLK_ALT;				// TP_CLK to alternate pin
	TP_DATA_ALT;			// TP_DATA to alternate pin
	SMBus2_CLK_ALT;			// SMBus2 CLK to alternate pin
	SMBus2_DAT_ALT;			// SMBus2 DATA to alternate pin

	#if FAN_TABLE_Already
	FAN_PWM_ALT;			// Set FAN_PWM Alt.
	FAN2_PWM_ALT;
	#else
	FAN_PWM_OUT;			// Set FAN_PWM OUTPUT.
	EC_FAN_PWM_HI();       
    FAN2_PWM_OUT;           //FAN 2 running at full speed.
    EC_FAN2_PWM_HI();       
	#endif	// FAN_TABLE_Already

	FAN_SPEED_ALT;			// Set FAN Speed Alt.
	InitSio();

	KBHISR = 0x00;
	KBHIKDOR = 0x00;
	CLEAR_MASK(KBHISR,SYSF);

	SPCTRL1 = 0x83;			//msmart test
	BRAM3E =0;
	BRAM3F =0;

	PECIDelayCnt = 0x04;

	Fan_Init();

	if ( IS_MASK_SET(SYS_MISC1, b1Num_LED) )
	{ NUMLED_ON(); }
	else
	{ NUMLED_OFF(); }
}


/*
*******************************************************************************
* Function name: SetS5GPIO      
*
* Descriptoin: Set S5 GPIO.
*
* Invoked by: S0S5_SUSP(),S0S3_SUSP()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void SetS5GPIO(void)
{
	SCI_LOW();			//RUNSCI Hi
	RCING_LOW();

	TP_CLK_INDW;		// TP_CLK to Input+pull_DW pin
	TP_DATA_INDW;		// TP_DATA to Input+pull_DW pin
	SMBus2_CLK_INDW;	// SMBus2 CLK to Input+pull_DW pin
	SMBus2_DAT_INDW;	// SMBus2 DATA to Input+pull_DW pin
	FAN_PWM_INDW;		// Set FAN_PWM input+pull_dw.
	FAN_SPEED_INDW;		// Set FAN Speed input+pull_dw.

	CAPLED_OFF();		// Turn off Cap LED.
	NUMLED_OFF();		// Turn off Num LED.
	if ( SysPowState == SYSTEM_S0_S5 ) 
	{
		USB_ON_INPUT;	// Disable USB wake.
	}
	else
	{
		USB_ON_OUTPUT;
		USB_ON_LOW();	// Turn on USB power.
	}
	AC_IN_INPUT;
	CLR_MASK(SYS_MISC1,b5Crisis_LED);	// Clear Crisis LED.
}

//============S5 -> S0==========================================================


/*
*******************************************************************************
* Function name: S5S0_STEP1      
*
* Descriptoin: S5 to S0 step1.
*
* Invoked by: Oem_S5S0Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
BYTE S5S0_STEP1(void)
{
    CLR_MASK(cCmd, bPCHPWR_Keep);    
    AutoTimer=0x00; 
    if(IS_MASK_SET(SysStatus2, b3EC8S_Reset)) 
	{
	  	CLEAR_MASK(SysStatus2, b3EC8S_Reset);
	}
	  
	EC_ON_HI(); 
   	PM_PWRBTN_HI();
    RamDebug(0x80);
	return(1);
}

/*
*******************************************************************************
* Function name: S5S0_PMIC_Initial      
*
* Descriptoin: S5 to S0 PMIC Initial.
*
* Invoked by: Oem_S5S0Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
BYTE S5S0_PMIC_Initial(void)
{
	InitPMIC(); 
    RamDebug(0x81);
	if(!PowerInitOK)
		return 0;
	else
		return 1;
}


/*
*******************************************************************************
* Function name: S5S0_AC_PRESENT_EN      
*
* Descriptoin: S5 to S0 AC PRESENT enable.
*
* Invoked by: Oem_S5S0Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S5S0_AC_PRESENT_EN(void) 
{
	uPGID= Read_Eflash_Byte(EEPROMA2,(EEPROMA1_B03 | 0x07) ,0xE0); //:Add the difference between G and Z project
    MBID_FB=uPGID;       
	PM_PWRBTN_HI();
    if(IS_MASK_SET(SYS_STATUS,AC_ADP)) 
    {
	   AC_PRESENT_HI();
    } 
    
    RamDebug(0x82);
}

void S5S0_PCH_PWR_EN(void)
{
	PCH_PWR_EN_ON();
    
    RamDebug(0x83);
}

void S5S0_RSMRST_EN(void)
{
	RSMRST_HI();
    RamDebug(0x84);
}

void S5S0_PM_PWER_BTN2(void)
{
	PM_PWRBTN_LOW();
	SET_MASK(CMOS_TEST,BIT2); 
    RamDebug(0x85);
}

void S5S0_PM_PWER_BTN3(void)
{
	PM_PWRBTN_HI();
    RamDebug(0x86);
}

BYTE S5S0_CHK_S4(void)
{
	if(Read_SLPS4())
	{
	    PM_PWRBTN_HI(); 
		EC_VCCST_EN_HI();
        RamDebug(0x87);
		return(1);
	}
    PM_PWRBTN_LOW(); 
    
    RamDebug(0x88);
	return(0);
}

BYTE S5S0_CHK_S3(void)
{
	if(Read_SLPS3())
	{		
        RamDebug(0x89);
		return(1);
	}
    RamDebug(0x8A);
	return(0);
}

void S5S0_EC_VPP_PWREN(void)
{
	EC_VPP_PWREN_HI();
    
    RamDebug(0x8C);
}

void S5S0_SYSON(void)
{
	SYSON_HI();
    RamDebug(0x8D);
}

void S5S0_SUSP(void)
{
	SUSP_ON();
	CURRENT_STATUS &= 0xC0; //Clear flags of entering and resuming from S3 & S4 &S5
	SetS0GPIO();
}

void S5S0_EC_VCCIO_EN(void)
{
	EC_VCCIO_EN_HI();
}


void S5S0_USBON(void)
{
	USB_ON_OUTPUT;
	USB_ON_LOW();
	SET_MASK(EMStatusBit, b7RdUSBChgEn);	// default support USB charger S4/S5.
}

void S5S0_VCCST_PWRGD(void)
{
	//EC_VCCST_PWRGD_HI();
}


BYTE S5S0_CHK_VDDQ_PWRGOOD(void)
{
	BYTE BPwrDate=0x03;
	if(Read_VDDQ_PGOOD())
	{
        RamDebug(0x8E);

        return(1);
	}
    RamDebug(0x8F);
	return(0);
}

void S5S0_VR_ON(void)
{
	Clr_UEFIStatus();	// Clear UEFI status for BIOS used.
	VR_ON_ON();
    if (IS_MASK_SET(SYS_STATUS,AC_ADP))   
    {
   		AdapterIDOn_Flag = 0x01; 
 		ADPIDON10MS_NUM=0xa0;  // Modify read adapter ID time, in order to enter S4 under DC mode ,then plug in AC and wake up from S4 not pop up limited support.
    }  
	MuteCount = 4;
}

BYTE S5S0_CHK_VRREADY(void)
{
	if(Read_CPU_VR_READY())
	{
		return(1);
	}
	return(0);
}

void S5S0_PCH_PWROK(void)
{
	PCH_PWROK_HI();
    PwrOnDly5Sec =5;	//optimize PwrOnDly5Sec,from 40 to 5.
}

void S5S0_SYS_PWROK(void)
{
	SYS_PWROK_HI();
}

/*
*******************************************************************************
* Function name: S5S0_Init_Status      
*
* Descriptoin: S5 to S0 init Status.
*
* Invoked by: Oem_S5S0Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S5S0_Init_Status(void)
{
	CLEAR_MASK(ACPI_HOTKEY, b7BIOS_NoShut); //support RTC wake and psw can wake during RTC wake enable  in DC mode except ship mode enable 
	CLEAR_MASK(ACPI_HOTKEY, b6Cmd_NoShut); 
	ShipModeEn=0x00; 
	Fan_ON_Count = FAN_Boot_On;	// Turn on Fan 3 Sec.
	disable_turbo_counter=60; 
	nBrightValue = 17;			// Default WIN8 brightness 110~130 nit.
	nCpuTemp = VGA_TEMP = EXTVGA_TEMP = nSKINTemp90 = 32;	// Default temperature value.
	ThermistorCPU_TEMP=32;		
  	if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
  	{
    	batteryFirstUsedDateL = batteryFirstUsedDateH = 0x00;
	  	if(bRWSMBus(SMbusChB,SMbusRW,SmBat_Addr,FirstUsedDate,&batteryFirstUsedDateL,SMBus_NeedPEC))
	  	{
			if(batteryFirstUsedDateL || batteryFirstUsedDateH )
		   		CLEAR_MASK(pProject0,b3uBatteryTimeNoOK);
			else
				SET_MASK(pProject0,b3uBatteryTimeNoOK);
			RamDebug(0xAE);
	  	}
  	}			
	InitThermalTable1();
    InitThermalTable2();
	LED_KB_PWM_Count = 10;	// Turn on KB BL LED.  //MEILING030:change 3 to 10(1.5s to 5s).
	ACPI_HOTKEY &= 0x1F;	// Clear bit6,7, can cut power in the battery mode.
	SYS_STATUS &= 0xF8;		// Clear OS mode.
	CLR_MASK(cCmd, b3BkOff);	// Turn on backlight.
	CLR_MASK(OEMControl, b1BkOff); 
	BackLight_En_Delay =16;	// Delay 280msec turn on backlight.//reduce light backlight time for clodboot.from 28 to 16.
    delayEDPTm = 5;                             // Delay 50ms to initial EDP. 
	SET_MASK(pDevStus, pENABLE_TP);	// Turn on TouchPad.
    //add lid flag to BIOS, whether need to open  backlight.
    if(Read_LID_SW_IN())
    	SET_MASK(SWI_EVENT,LID);
    else
        CLEAR_MASK(SWI_EVENT,LID);

    EC_TS_ON_HI();//Add touch panel control signal. 
    #if SupportReadTPID
    ReadTPid();
    #endif
    uReserve07.fbit.uACInOutBeep = 1;
	Lowpower_DISfunction();
	CPUThrottlingDelayTime = 60; //add delay time 1 minute to send 0x1D event.
	SysPowState = SYSTEM_S0;	//add set S0 power state.
}

/* follow ITE EVB Power Sequence 20111202.pdf
;===========================================================================================
; Power On (DC mode)
;===========================================================================================
;
;S5_ENABLE(O)   +----------------------------------------------------------------------------
;--------------+(1)Always power group 3,5VA (if Adapter in,this signal will exist always)
;
;PM_Power_BTN(O)            +----------------------+    ~       +-----------------------
;----------------------+                                       +--~----+
;
;RSMRST(O)                           +----------------------------------------
;-------------------------+
;
;AC_Present (Intel Only and if AC attached)(O)   +----------------------------------------
;----------------------------------------+
;
;                                                                                                        +---------------------
;SLP_S4#(I) SLP4(I)-------------------------------------------+
;
;                                                                                                               +---------------------
;IMVP_PWRGD (I)  ------------------------------------------------+
;                                                                                                                           +------------------
;S0_Power_Good(O)------------------------------------------------------+
;========================================================================
*/

//-------------------------------------------------------------------------------------
//  Do S5 -> S0 power sequnce
//-------------------------------------------------------------------------------------

const sPowerSEQ code asPowerSEQS5S0[]=
{
	{ 0x0000, 				0,		0,  },	//Dummy step always in the top of tabel
	{ S5S0_STEP1,			0,		1,	},  
	{ S5S0_PMIC_Initial,	1,		1,	},  
	{ S5S0_PCH_PWR_EN,		30,		0,  },
	{ S5S0_RSMRST_EN,		20,		0,  },
	{ S5S0_AC_PRESENT_EN,	76,		0,  }, // 20+76>95
	{ S5S0_PM_PWER_BTN2,    20,		0,  },
	{ S5S0_PM_PWER_BTN3,    0,      0,  },  
	{ S5S0_CHK_S4,			0, 		1,	}, 
	{ S5S0_EC_VPP_PWREN,    2,      0,  },
	{ S5S0_SYSON,			0, 		0,	}, 
	{ S5S0_CHK_VDDQ_PWRGOOD,0,	  	1,	},
	{ S5S0_CHK_S3,			0,     	1,  }, 
	{ SLPS3_Sleep_En,       0,      0,  }, 
	{ SLPS4_Sleep_En,       0,      0,  }, 
	{ S5S0_SUSP,			0, 		0,	},  
	{ S5S0_EC_VCCIO_EN,		8, 		0,	},  //   syson to vccio <25
    { S5S0_USBON,			50,		0,	},
    { S5S0_VCCST_PWRGD,		5,		0,	},
	{ S5S0_VR_ON,			0,		0,	},    
	{ S5S0_CHK_VRREADY,		10,		1,	},
	{ S5S0_PCH_PWROK,		100,	0,	},  // check VR_ON
	{ S5S0_SYS_PWROK,		49,		0,	},	
	{ S5S0_Init_Status,		0,		0,	},
};

/*
*******************************************************************************
* Function name: Oem_S5S0Sequence      
*
* Descriptoin: S5 -> S0 power sequnce.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_S5S0Sequence(void)
{
    if ( PowSeqDelay != 0x00 )
    {
        PowSeqDelay--; 									//1m Sec count base
        if ( PowSeqDelay != 0x00 )	return;
    }

	while(PWSeqStep<(sizeof(asPowerSEQS5S0)/sizeof(sPowerSEQ)))
	{
		if(asPowerSEQS5S0[PWSeqStep].checkstatus==0x00)	// Do function
		{
			(asPowerSEQS5S0[PWSeqStep].func)();
			PowSeqDelay = asPowerSEQS5S0[PWSeqStep].delay;
			PWSeqStep++;
			if(PowSeqDelay!=0x00)
			{
				break;
			}
		}
		else	// Check input status pin
		{
			if((asPowerSEQS5S0[PWSeqStep].func)())
			{
				PowSeqDelay = asPowerSEQS5S0[PWSeqStep].delay;
			    PWSeqStep++;
			}
			break;
		}
	}
}

//============S0 -> S5==========================================================
void S0S5_VR_ON(void)
{
	SYS_PWROK_LOW();
	PCH_PWROK_LOW();
	VR_ON_OFF();
	USB_ON_INPUT; 
	EC_VCCIO_EN_LOW();
	#if	Support_External_IO
	DGPU_HOLD_RST_LOW();
	S_DGPU_RST_LOW();
	#endif	// Support_External_IO
}

void S0S5_SUSP(void)
{
    EC_LID_OUT_LOW(); 
	SUSP_OFF();
	SetS5GPIO();
}

void S0S5_EC_VCCST_EN(void)
{
	EC_VCCST_EN_LOW();
}

void S0S5_EC_VPP_PWREN(void)
{
	EC_VPP_PWREN_LOW();
}

void S0S5_SYS_ON(void)
{
	SYSON_LOW();
	Write_NOVOS4();
}

/*
*******************************************************************************
* Function name: S0S5_Init_Status      
*
* Descriptoin: S0 to S5 init Status.
*
* Invoked by: Oem_S0S5Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S0S5_Init_Status(void)
{
	F2_Pressed=0; 
//Add CMD for press power button 4s don't shutdown when flash bios.
	#if chPWSW1WDT
       CLEAR_MASK(pProject4,pPWSWdisable);
    #endif  //chPWSW1WDT     
			
  	if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
  	{
    	if(IS_MASK_SET(pProject0,b1uFUdayClr))
    	{
      		batteryFirstUsedDateH = 0x00;
	  		batteryFirstUsedDateL = 0x00;
      		if(bRWSMBus(SMbusChB,SMbusWW,SmBat_Addr,FirstUsedDate,&batteryFirstUsedDateL,SMBus_NeedPEC))
      		{
        		CLEAR_MASK(pProject0,b1uFUdayClr);
        		RamDebug(0xAD);
      		}
    	}
  	}

	Bioswatchdog=0;
	uNovoVPCCount = 0;				// Clear NOVO VPC status.
	nCpuTemp = TEMP_Buff_1 = TEMP_Buff_2 = TEMP_Buff_3=0x00; //:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
	VGA_TEMP = VGA_TBuff3 = VGA_TBuff2 = VGA_TBuff1 = 0x00;	//:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
	nNBTemp = nRamTemp =0;//:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
	CLEAR_MASK(nBatteryStatH, CMBS_CRITICALLOW); 
	CPU_TYPE &= 0x3f;//:CLEAR BIT6 and BIT7 Unit abnormal shuntdown will not clear GPU flag,so when enter S5,WB and S3 we clear GPU flag to not read GPU temperature.
	CPU_Shut_Cnt = VGA_Shut_Cnt = EXTVGA_Shut_Cnt = 0;	// Clear shutdown conunt.
	ERR_THMSTS = 0;					// Clear thermal status fail bit.
	Thro_Status = Thro_Status2 = 0;	// clear Throttling status.
	ECSleepCount = 3;				// Delay entry EC sleep.
#if TouchPad_only
    PowerOffClearTPPendingData();
#endif
    if(Read_LID_SW_IN())
    	SET_MASK(SWI_EVENT,LID);
    else
        CLEAR_MASK(SWI_EVENT,LID);

    EC_TS_ON_LOW();//Turn off touch pad sensor power in s5/s3.  
       
	if (cSysActionFlag == SysAction_EC_Restart || cSysActionFlag == SysAction_SysShutdown_EC_AutoBoot)
	{
	    WinFlashMark = 0x53;
        WinFlashMark2 = 0x35;
		InternalWDTNow();
	}
	else if(cSysActionFlag == SysAction_EC_Shutdown) 
	{
		ProcessSID(FLASH_ID);
		WinFlashMark = 0xAA;
		WinFlashMark1=0x88;
		InternalWDTNow();
	}

	if(IS_MASK_SET(SysStatus,LidKBIgnore))
	{
		Ccb42_DISAB_KEY = 0;
		Flag.SCAN_INH = 0;	// not Inhibit scanner (internal keyboard).
		Unlock_Scan();		// UnLock scanner
		CLEAR_MASK(SysStatus,LidKBIgnore);
	}

	if(PowerOVP_REG|PowerUVP_REG|PowerOCP_REG|PowerOTP_REG)
	{
		if ( IS_MASK_CLEAR(SysStatus, ERR_ShuntDownFlag) )
		{ ProcessSID(PMIC_Shutdown_ID); }
	}

	PollingOTP();

	if ( IS_MASK_CLEAR(SysStatus, ERR_ShuntDownFlag) )
	{ ProcessSID(S5_ID); }
	CLEAR_MASK(SysStatus,ERR_ShuntDownFlag);	// Clear Shutdown ID bit.
	initMark = 0x00;	// CMW 20121203 for PS8625 translator
	TEMP_Error = 0x00; // CMW 20121203 for PS8625 translator
	PECI_Err_CNT=0;//When enter S3/S4/S4,clear PECI fail count.
	Lowpower_ENfunction(); 
	CLEAR_MASK(pDevStatus3,b6EnDisTPSend);
	CLEAR_MASK(pDevStatus3,b7TPStateFormer);
	CLEAR_MASK(pDevStatus1,b0F6ScanCodeSend);
	CLEAR_MASK(pDevStatus1,b1F6DisableTP);
	CLEAR_MASK(pDevStatus1,b4LNVAPP_STATUS);
	CLEAR_MASK(pDevStatus1,b5TPDRIVER_STATUS);
	CLEAR_MASK(pDevStatus1,b6MODE_STATUS);
	CLEAR_MASK(pDevStatus1,b7DisableTP);

	PM_PWRBTN_HI();
	SysPowState = SYSTEM_S5;	
	  
}

//-------------------------------------------------------------------------------------
//  Do S0 -> S5 power sequnce
//-------------------------------------------------------------------------------------
const sPowerSEQ code asPowerSEQS0S5[]=
{
	{ 0x0000, 				0,		0,  },	// Dummy step always in the top of tabel
	{ S0S5_VR_ON,			0,		0,	},  
	{ S0S5_SUSP, 		   	0,		0,  },
	{ S0S5_SYS_ON,			0,		0,	}, 
	{ S0S5_EC_VCCST_EN,		30,		0,	}, 
	{ S0S5_EC_VPP_PWREN,    10,     0,  }, 
	{ S0S5_Init_Status,		0,		0,	},
};


/*
*******************************************************************************
* Function name: Oem_S0S5Sequence      
*
* Descriptoin: S0 -> S5 power sequnce.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_S0S5Sequence(void)
{
	if ( PowSeqDelay != 0x00 )
	{
		PowSeqDelay--;				//1m Sec count base
		if ( PowSeqDelay != 0x00 )
		{
			return;
		}
	}

	while(PWSeqStep<(sizeof(asPowerSEQS0S5)/sizeof(sPowerSEQ)))
	{
		if(asPowerSEQS0S5[PWSeqStep].checkstatus==0x00) // Do function
		{
			(asPowerSEQS0S5[PWSeqStep].func)();
			PowSeqDelay = asPowerSEQS0S5[PWSeqStep].delay;
			PWSeqStep++;
			if(PowSeqDelay!=0x00)
			{
				break;
			}
		}
		else											// Check input status pin
		{
			if((asPowerSEQS0S5[PWSeqStep].func)())
			{
				PowSeqDelay = asPowerSEQS0S5[PWSeqStep].delay;
				PWSeqStep++;
			}
			break;
		}
	}
}

//============S0 -> S3==========================================================
void S0S3_VR_ON(void)
{
	SYS_PWROK_LOW();
	PCH_PWROK_LOW();
  	VR_ON_OFF();	
	EC_VCCIO_EN_LOW();
	#if	Support_External_IO	
	NVDD_PWR_EN_LOW();
	S_NVDD_PWR_EN_LOW();
	S_DGPU_PWR_EN_LOW();
	DGPU_PWR_EN_LOW();
	#endif	// Support_External_IO
}

void S0S3_SUSP(void)
{
    EC_LID_OUT_LOW(); 
	SUSP_OFF();
	#if	Support_External_IO
	DGPU_HOLD_RST_LOW();
	S_DGPU_RST_LOW();
	#endif	// Support_External_IO
	SetS5GPIO();
}

/*
*******************************************************************************
* Function name: S0S3_Init_Status      
*
* Descriptoin: S0 to S3 init Status.
*
* Invoked by: Oem_S0S3Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S0S3_Init_Status(void)
{
	#if	Support_External_IO
	Init_IT8302();  
	#endif	// Support_External_IO
	CPU_TYPE &= 0x3f;//:CLEAR BIT6 and BIT7 Unit abnormal shuntdown will not clear GPU flag,so when enter S5,WB and S3 we clear GPU flag to not read GPU temperature.
	CPU_Shut_Cnt = VGA_Shut_Cnt = EXTVGA_Shut_Cnt = 0;	// Clear shutdown conunt.
	nCpuTemp = TEMP_Buff_1 = TEMP_Buff_2 = TEMP_Buff_3=0x00; //:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
	VGA_TEMP = VGA_TBuff3 = VGA_TBuff2 = VGA_TBuff1 = 0x00;	//:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
	nNBTemp = nRamTemp =0;//:Clear CPU & VGA & thermal IC temp after system shutdown or enter s3.
#if TouchPad_only
    PowerOffClearTPPendingData();
#endif  
	ERR_THMSTS = 0;					// Clear thermal status fail bit.
	Thro_Status = Thro_Status2 = 0;	// clear Throttling status.  
	AOAC_STATUS &= 0x10;			// Clear ISCT status.
	initMark = 0x00;	
	TEMP_Error = 0x00;	
    EC_TS_ON_LOW();//Turn off touch pad sensor power in s5/s3.  
    PECI_Err_CNT=0;//:When enter S3/S4/S4,clear PECI fail count.
	Lowpower_ENfunction(); 

	CLEAR_MASK(pDevStatus3,b6EnDisTPSend);
	CLEAR_MASK(pDevStatus3,b7TPStateFormer);
	CLEAR_MASK(pDevStatus1,b0F6ScanCodeSend);
	CLEAR_MASK(pDevStatus1,b1F6DisableTP);
	CLEAR_MASK(pDevStatus1,b4LNVAPP_STATUS);
	CLEAR_MASK(pDevStatus1,b5TPDRIVER_STATUS);
	CLEAR_MASK(pDevStatus1,b6MODE_STATUS);

	CLEAR_MASK(pDevStatus1,b7DisableTP);

   	SysPowState=SYSTEM_S3; 
}

//-------------------------------------------------------------------------------------
//  Do S0 -> S3 power sequnce
//-------------------------------------------------------------------------------------
const sPowerSEQ code asPowerSEQS0S3[]=
{
  	{ 0x0000,         		0,    	0,  },  // Dummy step always in the top of tabel
  	{ S0S3_VR_ON,  	      	0,     	0,  }, 
  	{ S0S3_SUSP,     		0,    	0,  },  
  	{ S0S3_Init_Status,		0,		0,	},
};

/*
*******************************************************************************
* Function name: Oem_S0S3Sequence      
*
* Descriptoin: S0 -> S3 power sequnce.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_S0S3Sequence(void)
{
  	if(PowSeqDelay != 0x00 )
  	{
    	PowSeqDelay--;        //1m Sec count base
    	if(PowSeqDelay != 0x00 )
    	{
    		return;
    	}
  	}

  	while(PWSeqStep<(sizeof(asPowerSEQS0S3)/sizeof(sPowerSEQ)))
  	{
    	if(asPowerSEQS0S3[PWSeqStep].checkstatus==0x00) // Do function
    	{
      		(asPowerSEQS0S3[PWSeqStep].func)();
      		PowSeqDelay = asPowerSEQS0S3[PWSeqStep].delay;
      		PWSeqStep++;
      		if(PowSeqDelay!=0x00)
      		{
        		break;
      		}
    	}
    	else                      // Check input status pin
    	{
      		if((asPowerSEQS0S3[PWSeqStep].func)())
      		{
        		PowSeqDelay = asPowerSEQS0S3[PWSeqStep].delay;
        		PWSeqStep++;
      		}
      		break;
    	}
  	}
}

//============S3 -> S0==========================================================
void S3S0_PM_PWER_BTN1(void)
{
	PM_PWRBTN_LOW();
}

void S3S0_PM_PWER_BTN2(void)
{
	PM_PWRBTN_HI();
	SysPowState=SYSTEM_S3;
}

void S3S0_SUSP(void)
{
	PM_PWRBTN_HI();
	SUSP_ON();
	SetS0GPIO();
	USB_ON_OUTPUT;
	USB_ON_LOW();
	#if	Support_External_IO
	DGPU_PWR_EN_HI();
	S_DGPU_PWR_EN_HI();
	Delay1MS(7);
	NVDD_PWR_EN_HI();
	S_NVDD_PWR_EN_HI();
	#endif	// Support_External_IO
}

void S3S0_EC_VCCIO_EN(void)
{
	EC_VCCIO_EN_HI();
}

void S3S0_VCCST_PWRGD(void)
{
	//EC_VCCST_PWRGD_HI();
}

void S3S0_VR_ON(void)
{
	VR_ON_ON();
	MuteCount = 18;  
    if (IS_MASK_SET(SYS_STATUS,AC_ADP))   
    {
   		AdapterIDOn_Flag = 0x01; 
 		ADPIDON10MS_NUM=0x20;
    }
}

void PCH_PWROK(void)
{
	PCH_PWROK_HI();
}

/*
*******************************************************************************
* Function name: S3S0_Init_Status      
*
* Descriptoin: S3 to S0 init Status.
*
* Invoked by: Oem_S3S0Sequence()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void S3S0_Init_Status(void)
{
	Fan_ON_Count = FAN_Boot_On;	// Turn on Fan 3 Sec.
	disable_turbo_counter=60; 
    RamDebug(0x9B);   
	CLR_MASK(cCmd, b3BkOff);	// Turn on backlight.
	CLR_MASK(OEMControl, b1BkOff); 
	BackLight_En_Delay = 3;		// Delay 30msec turn on backlight.
    delayEDPTm = 40;                           // Delay 100ms to initial EDP.
	SET_MASK(pDevStus, pENABLE_TP);	// Turn on TouchPad.
	CURRENT_STATUS &= 0xC0; 		// Clear flags of entering and resuming from S3 & S4 &S5 
    EC_TS_ON_HI(); 
	Lowpower_DISfunction(); 
	SysPowState = SYSTEM_S0;   
}

//-------------------------------------------------------------------------------------
//  Do S3 -> S0 power sequnce
//-------------------------------------------------------------------------------------
const sPowerSEQ code asPowerSEQS3S0[]=
{
  	{ 0x0000,         		0,    	0,  },	// Dummy step always in the top of tabel
  	{ S3S0_PM_PWER_BTN1,	20,	  	0,  },
  	{ S3S0_PM_PWER_BTN2,	2,    	0,  },
	{ S3S0_SUSP,     		2,    	0,  },		
	{ S3S0_EC_VCCIO_EN,     50,    	0,  },		
	{ S3S0_VCCST_PWRGD,     5,    	0,  },		
  	{ S3S0_VR_ON,			0,		0,	},
  	{ SLPS3_Sleep_En,       0,      0,  }, 
  	{ SLPS4_Sleep_En,       0,      0,  }, 
 	{ S5S0_CHK_VRREADY,		10,		1,	},	//  there S5S0... same as S3S0... 
 	{ S5S0_PCH_PWROK,		100,	0,	},	//  there S5S0... same as S3S0...  //modify delay time from 85 to 100.
	{ S5S0_SYS_PWROK,		110,	0,	},	//  there S5S0... same as S3S0...
	{ S3S0_Init_Status,		0,		0,	},
};

/*
*******************************************************************************
* Function name: Oem_S3S0Sequence      
*
* Descriptoin: S3 -> S0 power sequnce.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_S3S0Sequence(void)
{
  	if(PowSeqDelay != 0x00 )
  	{
    	PowSeqDelay--;        //1m Sec count base
    	if(PowSeqDelay != 0x00 )
    	{
      		return;
    	}
  	}

  	while(PWSeqStep<(sizeof(asPowerSEQS3S0)/sizeof(sPowerSEQ)))
  	{
    	if(asPowerSEQS3S0[PWSeqStep].checkstatus==0x00) // Do function
    	{
      		(asPowerSEQS3S0[PWSeqStep].func)();
      		PowSeqDelay = asPowerSEQS3S0[PWSeqStep].delay;
      		PWSeqStep++;
      		if(PowSeqDelay!=0x00)
      		{
        		break;
      		}
    	}
    	else	// Check input status pin
    	{
      		if((asPowerSEQS3S0[PWSeqStep].func)())
      		{
        		PowSeqDelay = asPowerSEQS3S0[PWSeqStep].delay;
        		PWSeqStep++;
      		}
      		break;
    	}
  	}
}
//=============================================================================


/*
*******************************************************************************
* Function name: CheckAutoPowerOn      
*
* Descriptoin: Check auto power on.
*
* Invoked by: Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void CheckAutoPowerOn(void)
{
	if ((WinFlashMark == 0x53) && (WinFlashMark2 == 0x35) &&( SystemIsS5|| (SystemIsDSX && (!Read_AC_IN()) ) ))
	{
		AutoTimer = 0x83;
		WinFlashMark = 0;
       	WinFlashMark2 = 0;
	}
}


/*
*******************************************************************************
* Function name: check8sreset      
*
* Descriptoin: Check 8s reset
*
* Invoked by: Core_Initialization()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void check8sreset(void)
{
	RamDebug(ShutDnCause);
	Init_SMBus_Regs();
	if(IS_MASK_SET(PWRSW8SRST,BIT1))  
    {  
        SET_MASK(SysStatus2, b3EC8S_Reset);
        RamDebug(0xA8);
    }
    else 
    {
        CLEAR_MASK(SysStatus2, b3EC8S_Reset);
        RamDebug(0xA9);
    }
}

/*
*******************************************************************************
* Function name: SendSynapticsIDSequence      
*
* Descriptoin: Send Synaptics ID Sequence
*
* Invoked by: ReadTPid()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void SendSynapticsIDSequence()
{
    BYTE Temp;

	Temp=0;
	do 
	{
		Send2PortNWait(0x02, 0xE8, 1);
		Send2PortNWait(0x02, 0x00, 1);
		Temp++;
	}while(Temp<4);

	Send2PortNWait(0x02, 0xE9, 4);
}

/*
*******************************************************************************
* Function name: SendElanIDSequence      
*
* Descriptoin: Send Elan ID Sequence
*
* Invoked by: ReadTPid()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void SendElanIDSequence()
{
    Send2PortNWait(0x02, 0xE6, 1);
    Send2PortNWait(0x02, 0xE6, 1);
    Send2PortNWait(0x02, 0xE6, 1);
    Send2PortNWait(0x02, 0xE9, 4);
}

/*
*******************************************************************************
* Function name: SendALPSIDSequence      
*
* Descriptoin: Send ALPS ID Sequence
*
* Invoked by: ReadTPid()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void SendALPSIDSequence()
{
    Send2PortNWait(0x02, 0xE7, 1);
    Send2PortNWait(0x02, 0xE7, 1);
    Send2PortNWait(0x02, 0xE7, 1);
    Send2PortNWait(0x02, 0xE9, 4);
}

/*
*******************************************************************************
* Function name: ReadTPid      
*
* Descriptoin: Read TP ID
*
* Invoked by: S5S0_Init_Status()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void ReadTPid()
{
    SendSynapticsIDSequence();

    if(TPID3 == 0x47)
    {
        TP_Type = 0x02;		// Synatics TouchPad.
    }
    else
    {
        SendElanIDSequence();
        if(TPID2 == 0x3C)
        {
            TP_Type = 0x01;	// ELAN ToucnPad.
        }
       else
      {
           SendALPSIDSequence();
           if(TPID2==0x73)
          {
                TP_Type =0x03;
          }
           
       }  
   	}

}

/*
*******************************************************************************
* Function name: Write_NOVOS4      
*
* Descriptoin: Write NOVO S4
*
* Invoked by: S0S5_SYS_ON()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Write_NOVOS4(void)
{
	if(IS_MASK_CLEAR(LENOVOPMFW_Temp,EEPROM_Token))
	{
		EEPROM_PwrSts = Read_Eflash_Byte(EEPROMA2,(EEPROMA1_B03 | 0x07) ,0xDF);
		if(CURRENT_STATUS & ENTER_S4) // S4
	  	{	// Entering S4
			if (( (SYS_STATUS & 0x07) >= 0x04 )&&( (SYS_STATUS & 0x07) <= 0x06))	// Win8 or win10 // add win10 mode
			{	// Win8
				if( (EEPROM_PwrSts & 0x10) == 0 )		// check S4 bit to before.
			    {
					if( (EEPROM_PwrSts & 0x01) != 0 )	// check WIN8 bit to before.
					{
						eEEPROMAddrsss = 0xDF;
						eEEPROMBank = 0x07;
						eEEPROMData = EEPROM_PwrSts = 0xEE;
						Update_EEPROMMark();
					}
			    }
				else
				{
					eEEPROMAddrsss = 0xDF;
					eEEPROMBank = 0x07;
					eEEPROMData = EEPROM_PwrSts = 0xEE;
					Update_EEPROMMark();
				}
			}
			else
			{	// Win7 or other.
				if( (EEPROM_PwrSts & 0x10) == 0 )		// check S4 bit to before.
			    {
					if( (EEPROM_PwrSts & 0x01) == 0 )	// check WIN8 bit to before.
					{
						eEEPROMAddrsss = 0xDF;
						eEEPROMBank = 0x07;
						eEEPROMData = EEPROM_PwrSts = 0xEF;
						Update_EEPROMMark();
					}
			    }
				else
				{
					eEEPROMAddrsss = 0xDF;
					eEEPROMBank = 0x07;
					eEEPROMData = EEPROM_PwrSts = 0xEF;
					Update_EEPROMMark();
				}
			}
		}
	  	else // S5
	  	{
			if ( (EEPROM_PwrSts & 0x10) == 0 )		// check S4 bit to before.
			{
				eEEPROMAddrsss = 0xDF;
				eEEPROMBank = 0x07;
				eEEPROMData = EEPROM_PwrSts = 0xFF;
				Update_EEPROMMark();
			}
			else
			{
				if ( (EEPROM_PwrSts & 0x01) == 0 )	// check S4 bit to before.
				{
					if (!( (SYS_STATUS & 0x07) >= 0x04 )&&( (SYS_STATUS & 0x07) <= 0x06))	// check not Win8, win8.1  or win10 // add win10 mode
					{
						eEEPROMAddrsss = 0xDF;
						eEEPROMBank = 0x07;
						eEEPROMData = EEPROM_PwrSts = 0xFF;
						Update_EEPROMMark();
					}
				}
			}
	  	}
	}
}


/*
*******************************************************************************
* Function name: PulseSBPowerButton      
*
* Descriptoin:  pulsing S.B. power button 100ms
*
* Invoked by: service_scan(),ChkS3ResumeRSOC(),LID_Released()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void PulseSBPowerButton(void)
{
	PM_PWRBTN_LOW();
    SBSWReleaseCunt = 20;   // decrease timer base is 10ms total time 200ms
}


/*
*******************************************************************************
* Function name: CheckSBPowerButton      
*
* Descriptoin:  releasing S.B. power button
*
* Invoked by: Hook_Timer10msEventB()
*
* TimeDiv：10ms
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void CheckSBPowerButton(void)
{
	if(SBSWReleaseCunt!=0x00)
	{
		SBSWReleaseCunt--;
		if(SBSWReleaseCunt==0x00)
		{
			PM_PWRBTN_HI();
		}
	}
}

/*
*******************************************************************************
* Function name: Lowpower_ENfunction      
*
* Descriptoin:  Low power enable function.
*
* Invoked by: S0S5_Init_Status(),S0S3_Init_Status()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Lowpower_ENfunction(void)
{
	SET_MASK(CHGIC_WriteCmd0x12H,LOWPWR_EN);
  	if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) //ANGELAS059:0x12H to 0x12L 
    {
		CHGIC_SMbusFailCnt++;
			   
    }
}

/*
*******************************************************************************
* Function name: Lowpower_DISfunction      
*
* Descriptoin:  Low power disable function.
*
* Invoked by: S5S0_Init_Status(),S3S0_Init_Status()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Lowpower_DISfunction(void)
{
	CLEAR_MASK(CHGIC_WriteCmd0x12H,LOWPWR_EN);
  	if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) //ANGELAS059:0x12H to 0x12L 
    {
		 CHGIC_SMbusFailCnt++;
	}
}



const sPMICStruct code powerInit[] =
{//     command               REG               no.
	//{ SDWN,			&PowerSDWN			}, // remove
	{ EN_MASK,		&PowerEN_MASK		},
	{ EN_RW,		&PowerEN_RW			},	
	{ BUCK1CTRL,	&PowerBUCK1CTRL		},
	{ BUCK2CTRL,	&PowerBUCK2CTRL		},
	{ BUCK3CTRL,	&PowerBUCK3CTRL		},
	{ LDO1CTRL,		&PowerLDO1CTRL		},
	{ LDO2CTRL,		&PowerLDO2CTRL		},
	{ DISCHCTRL1,	&PowerDISCHCTRL1	},
	//{ DISCHCTRL2,	&PowerDISCHCTRL2	}, //remove
	{ OC_CTRL,		&PowerOC_CTRL		}, //remove
	{ BUCK_FSW,		&PowerBUCK_FSW		},
};

/*
*******************************************************************************
* Function name: CheckInitPMIC      
*
* Descriptoin:  Check init PMIC
*
* Invoked by: InitPMIC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
BYTE CheckInitPMIC(void)
{
	PowerReturnData=0xFE; 
	if(!PowerIICWriteByte(0xE2,0x00)) // 03 to 00
	{
		RamDebug(0xC1);
		return 0;
	}
	return 1;
}

/*
*******************************************************************************
* Function name: PMICID      
*
* Descriptoin:  PMIC ID
*
* Invoked by: InitPMICReg()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void PMICID(void)
{
	if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, VENDODID, &PowerVENDODID,SMBus_NoPEC))
	{
		PowerInitError++;
	}
}

/*
*******************************************************************************
* Function name: InitPMICReg      
*
* Descriptoin:  Init PMIC register
*
* Invoked by: InitPMIC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void InitPMICReg(void)
{

	if(PowerVENDODID == 0)
	{
		PMICID(); 
	}

	PowerInitIndex = 0;

	PowerEN_MASK = 0x80; 
	PowerEN_RW = 0x80;  //set to 0x80 allow EC write PMIC
	if(PowerVENDODID==0x53) 
	{ 
		PowerBUCK1CTRL = 0x26;  
		PowerBUCK2CTRL = 0x2C;  
		PowerBUCK3CTRL = 0x42; //from 0x34 to 0x36 to solve GPU noise issue.  //change to 1.08V
		PowerLDO1CTRL = 0x1E;  
		PowerLDO2CTRL = 0x02; 
		PowerDISCHCTRL1 = 0x15;
        
        PowerOC_CTRL = 0x35;
		PowerBUCK_FSW = 0x03; 
	}
	else  
	{

		PowerBUCK1CTRL = 0x26; 
		PowerBUCK2CTRL = 0x0C;  
		PowerBUCK3CTRL = 0x42;  //change to 1.08V
		PowerLDO1CTRL = 0x1E;  
		PowerLDO2CTRL = 0x04;  
		PowerDISCHCTRL1 = 0x15;
		
        PowerOC_CTRL = 0x35;
		PowerBUCK_FSW = 0x00; 
	} 
	
	while(PowerInitIndex < (sizeof(powerInit)/sizeof(sPMICStruct)))
	{
		if(PowerIICWriteByte(powerInit[PowerInitIndex].cmd, *powerInit[PowerInitIndex].smbdataA))
		{
			PowerInitIndex++;
		}
		else
		{
			PowerInitIndex++;
			RamDebug(0xC2);
			RamDebug(PowerInitErrorCMD);
			RamDebug(PowerInitErrorDATA);
		}
	}

	if(!PowerIICWriteByte(0xD1,0xBF))  // set to 0xBF, then PMIC run
	{
		RamDebug(0xC3);
		RamDebug(PowerInitErrorCMD);
		RamDebug(PowerInitErrorDATA);
	}
}

/*
*******************************************************************************
* Function name: InitPMIC      
*
* Descriptoin:  Init PMIC
*
* Invoked by: DSXS5_PMIC_Init(),S5S0_PMIC_Initial()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void InitPMIC(void)
{
	if(!PowerInitOK)
	{	
		if(CheckInitPMIC())
		{
			InitPMICReg();
			RamDebug(0xC0);
			PowerInitOK=1;
		}
	}
}

/*
*******************************************************************************
* Function name: PowerIICWriteByte      
*
* Descriptoin:  Power IIC write byte.
*
* Invoked by: CheckInitPMIC(),InitPMICReg()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
BYTE PowerIICWriteByte(BYTE Reg_offset, BYTE PS_data)
{
	PMIC_counter = 0;
	
	while(1)
	{
		if(!bRWSMBus(SMbusChA, SMbusWB, powerICaddress, Reg_offset, &PS_data, SMBus_NoPEC))
		{
			PowerInitError++;
			ResetSMBus(SMbusChA); 
		}
		if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, Reg_offset, &PowerReturnData,SMBus_NoPEC))
		{
			PowerInitError++;
			ResetSMBus(SMbusChA);
		}
		if(PowerReturnData == PS_data)
		{
			return 1;
		}
		else
		{
			if(PMIC_counter == 5)
			{
				return 0;
			}
			PowerInitErrorCMD=Reg_offset;
			PowerInitErrorDATA=PowerReturnData;
			PMIC_counter++;
		}
	}
}

/*
*******************************************************************************
* Function name: PollingOTP      
*
* Descriptoin:  Polling OTP.
*
* Invoked by: Hook_Timer1SecEventA(),S0S5_Init_Status()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void PollingOTP(void)
{
	if(PowerInitOK==1)
	{
		if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, OVP_REG, &PowerOVP_REG,SMBus_NoPEC))
		{
			PowerInitError++;
		}
		if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, UVP_REG, &PowerUVP_REG,SMBus_NoPEC))
		{
			PowerInitError++;
		}
		if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, OCP_REG, &PowerOCP_REG,SMBus_NoPEC))
		{
			PowerInitError++;
		}
		if(!bRWSMBus(SMbusChA, SMbusRB, powerICaddress, OTP_REG, &PowerOTP_REG,SMBus_NoPEC))
		{
			PowerInitError++;
		}
	}
}

