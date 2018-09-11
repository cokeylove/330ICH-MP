/*-----------------------------------------------------------------------------
 * TITLE: OEM_BATTERY.C
 *
 * Author : Dino
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>



/*
*******************************************************************************
* Function name: UpdateNameSpace       
*
* Descriptoin: This function is used to Update name space about battery.
*
* Invoked by: FirstGetBatData() and OEM_PollingBatData_TASK()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void UpdateNameSpace(void)
{
	nBattGasgauge=BAT1PERCL;
       
	nCycleCounter=EC_oCCBQl;
	EC_BatteryStatusL = nBattery0x16L;
	EC_BatteryStatusH = nBattery0x16H;

	if(IS_MASK_SET(nBattery0x16L,FullyChg))
    {
		SET_MASK(nBatteryStatus1,ENFULL);

        if(nBattGasgauge == 100) // 100%
        {
            if(IS_MASK_CLEAR(LENOVOPMFW_Temp,BATTERY_FULLED_100))
            {
                SET_MASK(LENOVOPMFW_Temp,BATTERY_FULLED_100);
                ECSMI_SCIEvent(ACPI_BAT1IN_SCI); // Notify BIOS Update Batt Status
            }
        }
   	}
	else
    {
		CLEAR_MASK(nBatteryStatus1,ENFULL);
        CLEAR_MASK(LENOVOPMFW_Temp,BATTERY_FULLED_100);
	}

	Lenovo_Battery_EM80();

#if turboboostandstorage //storage mode action with support turbo boost charger

	if(testtoolflag==0)//Add flag for test tool to keep the battery RSOC at 60%
	{
		if(IS_MASK_SET(LENOVOPMFW,BATTERY_STORAGE) && IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))
		{
			if(IS_MASK_SET(System_Status,b0_System_Is_Heavy_Loading) || IS_MASK_SET(System_Status,b1_EC_S3_Auto_Wake_Up))
			{
				CalcBatRCC = (nFullChgCapH << 8) + nFullChgCapL;
				CalcBatRCC = (CalcBatRCC / 20) + (CalcBatRCC / 200);
				nRemainingCapH = (CalcBatRCC >> 8);
				nRemainingCapL = (CalcBatRCC & 0xFF);
			}
			else
			{
				nRemainingCapL = Bat0x0FTempL;
				nRemainingCapH = Bat0x0FTempH;
			} 
		
			SET_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);
		
			if(nBattGasgauge >= 55)
			{
				if(IS_MASK_SET(ACOFF_SOURCE, BATTLEARN) )
				{ 
			    	ECSMI_SCIEvent(ACPI_BAT1IN_SCI); 
            	}
				CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
				BATT_LEN_OFF();
            	if (IS_MASK_CLEAR(StatusKeeper, BatteryProtectCHG))
            	{
                	SET_MASK(nStopChgStat3H, EmStopChgarg);
            	}
            	else
            	{
                	CLEAR_MASK(nStopChgStat3H, EmStopChgarg);
                	if(nBattGasgauge == 60)
		        	{
						if(KeepBattRemineCap == 0)
						{ 
					    	KeepBattRemineCap = (WORD)(nRemainingCapH << 8) + nRemainingCapL; 
                    	}
						else
						{
							CalcBatRCC =(nFullChgCapH<<8)+nFullChgCapL;
							if(((WORD)(nRemainingCapH << 8) + nRemainingCapL) > (KeepBattRemineCap + (CalcBatRCC/200)))
							{
								KeepBattRemineCap = 0;
								CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
								ECSMI_SCIEvent(ACPI_BAT1IN_SCI); // Notify BIOS Update Batt Status
							}
						}
					}
            	}			
			}
			else if (nBattGasgauge < 55)
			{
				CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
				BATT_LEN_OFF(); 
            	SET_MASK(StatusKeeper, BatteryProtectCHG);
				CLR_MASK(nStopChgStat3H,EmStopChgarg);
			}
		}
		else 
		{
			if (IS_MASK_SET(EC_BatteryStatusL,FullyChg)&&(IS_MASK_SET(SYS_STATUS,AC_ADP))&&(nBattGasgauge==100)) 
			{
				nRemainingCapL = nFullChgCapL;
				nRemainingCapH = nFullChgCapH;
			}
			else
			{
				if(IS_MASK_SET(System_Status,b0_System_Is_Heavy_Loading) || IS_MASK_SET(System_Status,b1_EC_S3_Auto_Wake_Up))
				{
					CalcBatRCC = (nFullChgCapH << 8) + nFullChgCapL;
					CalcBatRCC = (CalcBatRCC / 20) + (CalcBatRCC / 200);
					nRemainingCapH = (CalcBatRCC >> 8);
					nRemainingCapL = (CalcBatRCC & 0xFF);
				}
				else
				{
					nRemainingCapL = Bat0x0FTempL;
					nRemainingCapH = Bat0x0FTempH;
				}
			}

        	if(IS_MASK_SET(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING))
        	{
            	CLEAR_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);

            	if(IS_MASK_SET(ACOFF_SOURCE,BATTLEARN))
            	{
                	CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
	 				BATT_LEN_OFF(); 
            	}
        	}
        	CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
	 		CLR_MASK(nStopChgStat3H,EmStopChgarg);
    	}
	}
	else if(testtoolflag==1)
	{
		if (IS_MASK_SET(LENOVOPMFW,BATTERY_STORAGE) && IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL)) 
		{
			if(IS_MASK_SET(System_Status,b0_System_Is_Heavy_Loading) || IS_MASK_SET(System_Status,b1_EC_S3_Auto_Wake_Up))
			{
				CalcBatRCC = (nFullChgCapH << 8) + nFullChgCapL;
				CalcBatRCC = (CalcBatRCC / 20) + (CalcBatRCC / 200);
				nRemainingCapH = (CalcBatRCC >> 8);
				nRemainingCapL = (CalcBatRCC & 0xFF);
				
			}
			else
			{
				nRemainingCapL = Bat0x0FTempL;
				nRemainingCapH = Bat0x0FTempH;
			} 
		
			SET_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);

			if (nBattGasgauge > 60)
			{
				SET_MASK(ACOFF_SOURCE, BATTLEARN);
				BATT_LEN_ON();	
			}
			else if (nBattGasgauge >= 55 && nBattGasgauge <= 60)
			{
				if ( IS_MASK_SET(ACOFF_SOURCE, BATTLEARN) )
				{ 
			    	ECSMI_SCIEvent(ACPI_BAT1IN_SCI); 
            	}
				CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
				BATT_LEN_OFF(); 
            	if (IS_MASK_CLEAR(StatusKeeper, BatteryProtectCHG))
            	{
                	SET_MASK(nStopChgStat3H, EmStopChgarg);
            	}
            	else
            	{
                	CLEAR_MASK(nStopChgStat3H, EmStopChgarg);
                	if (nBattGasgauge == 60)
		        	{
						if (KeepBattRemineCap == 0)
						{ 
					    	KeepBattRemineCap = (WORD)(nRemainingCapH << 8) + nRemainingCapL; 
                    	}
						else
						{
							CalcBatRCC =(nFullChgCapH<<8)+nFullChgCapL;
			     			if (((WORD)(nRemainingCapH << 8) + nRemainingCapL) > (KeepBattRemineCap + (CalcBatRCC/200)))
							{
								KeepBattRemineCap = 0;
								CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
								ECSMI_SCIEvent(ACPI_BAT1IN_SCI); // Notify BIOS Update Batt Status
							}
						}
					}
            	}
			}
			else if (nBattGasgauge < 55)
			{
				CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
				BATT_LEN_OFF(); 
            	SET_MASK(StatusKeeper, BatteryProtectCHG);
				CLR_MASK(nStopChgStat3H,EmStopChgarg);
			}
		}	
		else
		{
			if (IS_MASK_SET(EC_BatteryStatusL,FullyChg)&&(IS_MASK_SET(SYS_STATUS,AC_ADP))&&(nBattGasgauge==100))
			{
				nRemainingCapL = nFullChgCapL;
				nRemainingCapH = nFullChgCapH;
			}
			else
			{
				if(IS_MASK_SET(System_Status,b0_System_Is_Heavy_Loading) || IS_MASK_SET(System_Status,b1_EC_S3_Auto_Wake_Up))
				{
					CalcBatRCC = (nFullChgCapH << 8) + nFullChgCapL;
					CalcBatRCC = (CalcBatRCC / 20) + (CalcBatRCC / 200);
					nRemainingCapH = (CalcBatRCC >> 8);
					nRemainingCapL = (CalcBatRCC & 0xFF);
				}
				else
				{
					nRemainingCapL = Bat0x0FTempL;
					nRemainingCapH = Bat0x0FTempH;
				}
			}

    		if(IS_MASK_SET(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING))
    		{
        		CLEAR_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);

        		if(IS_MASK_SET(ACOFF_SOURCE,BATTLEARN))
        		{
            		CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
	 				BATT_LEN_OFF(); 
        		}
    		}
    		CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
			CLR_MASK(nStopChgStat3H,EmStopChgarg);
 		}
	}

#else

	if (IS_MASK_SET(LENOVOPMFW,BATTERY_STORAGE) && IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))
	{	
		nRemainingCapL = Bat0x0FTempL;
		nRemainingCapH = Bat0x0FTempH;

    	SET_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);

		if (nBattGasgauge > 60)
		{
			SET_MASK(ACOFF_SOURCE, BATTLEARN);
			BATT_LEN_ON();	
        	if(IS_MASK_SET(CHGIC_ReadCmd0x37L, TurboBoost)&&(nNowCurrentH & 0x80))
        	{			
				CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
			}
		}
		else if (nBattGasgauge >= 55 && nBattGasgauge <= 60)
		{
			if ( IS_MASK_SET(ACOFF_SOURCE, BATTLEARN) )
			{ 
				ECSMI_SCIEvent(ACPI_BAT1IN_SCI); 
        	}
			CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
			BATT_LEN_OFF(); 
        	if (IS_MASK_CLEAR(StatusKeeper, BatteryProtectCHG))
        	{
            	SET_MASK(nStopChgStat3H, EmStopChgarg);
        	}
        	else
        	{
           		CLEAR_MASK(nStopChgStat3H, EmStopChgarg);
            	if (nBattGasgauge == 60)
		    	{
					if (KeepBattRemineCap == 0)
					{ 
						KeepBattRemineCap = (WORD)(nRemainingCapH << 8) + nRemainingCapL; 
                	}
					else
					{
						CalcBatRCC =(nFullChgCapH<<8)+nFullChgCapL; 
						if (((WORD)(nRemainingCapH << 8) + nRemainingCapL) > (KeepBattRemineCap + (CalcBatRCC/200)))
						{
							KeepBattRemineCap = 0;
							CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
							ECSMI_SCIEvent(ACPI_BAT1IN_SCI); // Notify BIOS Update Batt Status
						}
					}
				}
        	}
		}
		else if (nBattGasgauge < 55)
		{
			CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
			BATT_LEN_OFF();
        	SET_MASK(StatusKeeper, BatteryProtectCHG);
			CLR_MASK(nStopChgStat3H,EmStopChgarg);
		}
	}	
	else 
	{
		if (IS_MASK_SET(EC_BatteryStatusL,FullyChg)&&(IS_MASK_SET(SYS_STATUS,AC_ADP))&&(nBattGasgauge==100))
		{
			nRemainingCapL = nFullChgCapL;
			nRemainingCapH = nFullChgCapH;
		}
		else
		{
			// return RSOC(0x0d)=0 to BIOS if battery error
			if (ChkBattery_abnormal_status == ChkBattery_abnormal_status_error)
			{

			}
			else
			{
				nRemainingCapL = Bat0x0FTempL;
				nRemainingCapH = Bat0x0FTempH;
			}
		}

    	if(IS_MASK_SET(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING))
    	{
        	CLEAR_MASK(LENOVOPMFW_Temp,BATTERY_CYCLE_RUNNING);

        	if(IS_MASK_SET(ACOFF_SOURCE,BATTLEARN))
        	{
            	CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
	 			BATT_LEN_OFF(); 
        	}
    	}
    	CLEAR_MASK(StatusKeeper, BatteryProtectCHG);
		CLR_MASK(nStopChgStat3H,EmStopChgarg);
	}

#endif

}

/*
*******************************************************************************
* Function name: ChkLENOVOPMFW       
*
* Descriptoin: This function is used to check Lenovo PMFW.
*
* Invoked by: ChkGoTarget() and OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void ChkLENOVOPMFW(void)
{
	if (IS_MASK_SET(LENOVOPMFW,BATTERY_MAIN_CAL))			// check Calibration Cycle enable
	{
		if (cBattFlag0.fbit.cBF0_FullToTarget == 0)
		{
		   	if (IS_MASK_CLEAR(LENOVOPMFW,BATTERY_CAL_END))
		   	{
				cBattFlag0.fbit.cBF0_FullToTarget = 1;		// charge to 100% then discharge to 1%
				cBattFlag0.fbit.cBF0_Full = 0;
				cBattFlag0.fbit.cBF0_GoTarget = 0;
				cTargetGauge = 1;
		   	}
		}
	}
	else if (IS_MASK_SET(LENOVOPMFW,BATTERY_CAL_END))
	{
		CLR_MASK(LENOVOPMFW,BATTERY_CAL_END);
		cBattFlag0.fbit.cBF0_FullToTarget = 0;
		cBattFlag0.fbit.cBF0_Full = 0;
		cBattFlag0.fbit.cBF0_GoTarget = 0;
        CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
		BATT_LEN_OFF(); 
	}
}

/*
*******************************************************************************
* Function name: ChkGoTarget       
*
* Descriptoin: This function is used to check RSOC go target.
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void ChkGoTarget(void)
{
	if (cBattFlag0.fbit.cBF0_GoTarget == 1)
	{
		if (cTargetGauge >= nBattGasgauge)
		{
			CLEAR_MASK(ACOFF_SOURCE, BATTLEARN);
			BATT_LEN_OFF();
			cBattFlag0.fbit.cCmdAcOff = 0;
			if (IS_MASK_SET(LENOVOPMFW,BATTERY_MAIN_CAL))
			{
				SET_MASK(LENOVOPMFW,BATTERY_CAL_END);
				CLR_MASK(LENOVOPMFW,BATTERY_MAIN_CAL);
                cTargetGauge=0;
                SET_MASK(LENOVOPMFW_Temp,BATTERY_CALIBRATION_OK);   // release Calibration mode
                uVPCeventSource = General;
                uVPCeventSource2 = 0;
                ECSMI_SCIEvent(SDV_VPC_notify);
                ChkLENOVOPMFW();
			}
			return;
		}
		if (nBattGasgauge > cTargetGauge)
		{
			SET_MASK(ACOFF_SOURCE, BATTLEARN);
			BATT_LEN_ON();	
			cBattFlag0.fbit.cCmdAcOff = 1;
		}
	}
	else if (cBattFlag0.fbit.cBF0_FullToTarget == 1)
	{
		if (cBattFlag0.fbit.cBF0_Full ==0)
		{
			if (IS_MASK_SET(nBattery0x16L,FullyChg))
			{
				cBattFlag0.fbit.cBF0_Full=1;
				cBattFlag0.fbit.cBF0_GoTarget = 1;					// Discharge or Charge to cTargetGauge%
			}
		}
	}
}

/*
*******************************************************************************
* Function name: ChkS3ResumeRSOC       
*
* Descriptoin: This function is used to check S3 resume RSOC.
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void ChkS3ResumeRSOC(void)
{
	if(IS_MASK_SET(battery_critical,DCdischargeto5ins3))
	{
        RamDebug(0x37);  
        PulseSBPowerButton(); 
		CLEAR_MASK(battery_critical,DC0ver5enterS3);
		CLEAR_MASK(battery_critical,DCdischargeto5ins3);
		SET_MASK(System_Status,b1_EC_S3_Auto_Wake_Up);
	}

	if(Read_AC_IN() || (nBattGasgauge > 7 || nBattGasgauge < 6))
	{
		CLR_MASK(System_Status,b1_EC_S3_Auto_Wake_Up);
	}
}

/*
*******************************************************************************
* Function name: ChkS3DCRSOC       
*
* Descriptoin: This function is used to check S3 DC RSOC.
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void ChkS3DCRSOC(void)
{
	if(SystemIsS3)
	{
   		if(nBattGasgauge > 7 || nBattGasgauge < 5)
		{
			SET_MASK(battery_critical, DC0ver5enterS3);
		}
   		else if(nBattGasgauge == 7)
   		{
			if(IS_MASK_CLEAR(SYS_STATUS,AC_ADP) && IS_MASK_SET(battery_critical,DC0ver5enterS3))
			{
				SET_MASK(battery_critical,DCdischargeto5ins3);
			}
			else
			{
				CLEAR_MASK(battery_critical,DCdischargeto5ins3);
				SET_MASK(battery_critical, b2_DC0ver5enterS3);
			}
		}
		else if(nBattGasgauge == 6)
   		{
			if(IS_MASK_CLEAR(SYS_STATUS,AC_ADP) && IS_MASK_SET(battery_critical,b2_DC0ver5enterS3))
			{
				SET_MASK(battery_critical,DCdischargeto5ins3);
			}
			else
			{
				CLEAR_MASK(battery_critical,DCdischargeto5ins3);
				SET_MASK(battery_critical, b3_DC0ver5enterS3);
			}
		}
		else if(nBattGasgauge == 5)
   		{
			if(IS_MASK_CLEAR(SYS_STATUS,AC_ADP) && IS_MASK_SET(battery_critical,b3_DC0ver5enterS3))
			{
				SET_MASK(battery_critical,DCdischargeto5ins3);
			}
			else
			{
				CLEAR_MASK(battery_critical,DCdischargeto5ins3);
			}
		}
		
	}
	else
	{
		battery_critical = 0;
	}
}

/*
*******************************************************************************
* Function name: RSOC1Pto0P_ShutdownCheck       
*
* Descriptoin: This function is used to check RSOC 1P to 0P shutdown.
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void RSOC1Pto0P_ShutdownCheck(void)
{
    if(IS_MASK_SET(SYS_STATUS,AC_ADP) || Read_AC_IN() || (nBattGasgauge > 0x01) || SystemNotS0)
    {
       RSOC1PTO0PCount = 0x00;
	   RSOC1PTO0PSaveSpace = 0x00;
    }
	else
	{
	   if(nBattGasgauge == 0x01)
	   {
	      SET_MASK(RSOC1PTO0PSaveSpace,RSOCIs1P);
	   }
	   else if((nBattGasgauge == 0x00) && IS_MASK_SET(RSOC1PTO0PSaveSpace,RSOCIs1P))
	   {
	      	if ( RSOC1PTO0PCount >= RSOC_Shut_Cnt )	// Read 30 times.
	        {
				ProcessSID(RSOC_1Pto0P_ID);  //Shutdown ID 0x40		
		
			    SET_MASK(SysStatus,ERR_ShuntDownFlag);	
		
				RSOC1PTO0PCount = 0x00;
	            RSOC1PTO0PSaveSpace = 0x00;
				SET_MASK(USB_Charger, b2USB_TmlDis);	// Disable USB charger.
				RSMRST_LOW();
				Delay1MS(1);
				RSMRST_HI();				
	        }
	        else
	        { 
	            RSOC1PTO0PCount++; 
			}
	   }
	}
}

/*
*******************************************************************************
* Function name: RSMRST_shutdown       
*
* Descriptoin: This function is used to shutdown.
*
* Invoked by:  ChkBattery_OTP() and Chk_BatSMbusFailCount()
*
* TimeDiv：100ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void RSMRST_shutdown(void)					//shutdown (RSMRST HI-->low-->HI)

{
	RSMRST_LOW();					//shutdown (remark first)
	Delay1MS(1);
	RSMRST_HI();
	RSMshutdownCnt++;
}

/*
*******************************************************************************
* Function name: DownACtoBatteryGPUState       
*
* Descriptoin: This function is used to decrease GPU throttling.
*
* Invoked by:  Hook_Timer500msEventB()
*
* TimeDiv：500ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
//S+ decrease GPU throttling every 2s after PnP AC.
void DownACtoBatteryGPUState()
{
	if((cGPUACtoBattThrottling !=0)&&(cGPUACtoBattTime==0))
	{
		if(IS_MASK_SET(SYS_STATUS,AC_ADP))
		{
			cGPUACtoBattThrottling=0;
		}
		else
		{
			cGPUACtoBattThrottling--;
			cGPUACtoBattTime = 4;//2s to do an action
			if((nBattGasgauge > 30) && (cGPUACtoBattThrottling == 1))
			{
				cGPUACtoBattThrottling=0;
				cGPUACtoBattTime = 0;  //2s to do an action
			}
			else if((nBattGasgauge <= 30) && (cGPUACtoBattThrottling == 2))
			{
				cGPUACtoBattThrottling=0;
				cGPUACtoBattTime = 0;  //2s to do an action
			}
		}
	}
}
//E+.

/*
*******************************************************************************
* Function name: CPUProchotCtrl       
*
* Descriptoin: This function is used to control CPU prochot.
*
* Invoked by:  Hook_Timer10msEventA()
*
* TimeDiv：10ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
//add start CPU prochot control function.
void CPUProchotCtrl()
{
	if ( IS_MASK_CLEAR(Thro_Status, (b0ProCH_CPU+b1ProCH_VGA+b2ProCH_EXTVGA+b6ProCH_TURBO+b7ProCH_BATT))
		&&IS_MASK_CLEAR(Thro_Status2, (b2Batt_LTP+b3Batt_OTP+b6BattRSOC_Low+b7ProCH_Psys))  //COKEYXU039: add b2Batt_LTP
		&&IS_MASK_CLEAR(LENOVOBATT2, (BATTERY_OCP))  
		&&IS_MASK_CLEAR(Thro_Status4, (b0ProCH_ADP))  
		&&IS_MASK_CLEAR(nBatteryStatH, CMBS_CRITICALLOW)&&(CPUProchotONCnt == 0))
	{	// turn off prochot.  //add prochot on 2S when AC out and battery over temperatrue and battery RSOC low.
	    H_PROCHOT_OFF(); 
	    nRealThermalPinDIS;
	    CLR_MASK(pPROCHOT, b0Thermal_PRCOHOTon);	// for AP display.
	}
	else
 	{	// turn on prochot.
	    H_PROCHOT_ON();
	    nRealThermalPinEN;
		SET_MASK(pPROCHOT, b0Thermal_PRCOHOTon);	// for AP display.
	}     
}
//add end.

/*
*******************************************************************************
* Function name: ChkBattery_LTP
*
* Descriptoin: 
*          DC mode and low temprature condition,pull CPU and GPU prochot. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv: 100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/

void ChkBattery_LTP()
{
	if ((SystemIsS0)&&(!Read_AC_IN())) //COKEYXU045: only DC mode to check Battery LTP.
	{
		if(nBattAverTemp <= 5)   
		{
			cGPUBattLTPThrottling = 4; //COKEYXU046: battery cell temp < 5, set GPU D5.
			DisCPUTurboForLTP = 0xFF;   //COKEYXU046: disable CPU turbo
			SET_MASK(Thro_Status2, b2Batt_LTP);
			SET_MASK(GPU_Prochot, b2_battery_LTP);
		}
		else if(nBattAverTemp <= 10)
		{
			if(nBattAverTemp > 6) //COKEYXU049: LTP condition,when battery cell temp> 6, cancle CPU prochot.
			{
				CLEAR_MASK(Thro_Status2, b2Batt_LTP);
			}
			DisCPUTurboForLTP = 0xFF;  //COKEYXU046: disable CPU turbo
			cGPUBattLTPThrottling = 2; //COKEYXU046: 5 < battery cell temp < 20, set GPU D3.
			if(Psys_AvgData > 341) //COKEYXU049: Power consumption>50W, pull GPU prochot
			{
				SET_MASK(GPU_Prochot, b2_battery_LTP);
			}
			else
			{
				CLEAR_MASK(GPU_Prochot, b2_battery_LTP);
			}
		}
		else if(nBattAverTemp <= 20)
		{
			CLEAR_MASK(Thro_Status2, b2Batt_LTP);
			CLEAR_MASK(GPU_Prochot, b2_battery_LTP);
			DisCPUTurboForLTP = 0xFF;
			cGPUBattLTPThrottling = 2;
		}
		else
		{
			CLEAR_MASK(Thro_Status2, b2Batt_LTP);
			CLEAR_MASK(GPU_Prochot, b2_battery_LTP);
			cGPUBattLTPThrottling = 0;
			if(nBattAverTemp > 21)  //COKEYXU049:  LTP condition,when battery cell temp> 21,enable CPU turbo.
			{
				DisCPUTurboForLTP = 0;
			}
		}

		if(nBattAverTemp <= 20)
	    {
    		CHGIC_ReadCmd0x3DL=0x39;    
    	  	CHGIC_ReadCmd0x3DH=0x25; 
	    }
    	else
    	{
    		CHGIC_ReadCmd0x3DL=0x39;    
      		CHGIC_ReadCmd0x3DH=0x51;
    	}
    	
	}
	else
	{	
		DisCPUTurboForLTP = 0;
		cGPUBattLTPThrottling = 0; //COKEYXU048: clear cGPUBattLTPThrottling.
		CLEAR_MASK(Thro_Status2, b2Batt_LTP);
		CLEAR_MASK(GPU_Prochot, b2_battery_LTP);
		CHGIC_ReadCmd0x3DL=0x39;    
      	CHGIC_ReadCmd0x3DH=0x51;
	}

    if(CHGIC_ReadCmd0x3DH != CHGIC_ReadCmd0x3DH_Temp)
	{
		if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerProchot1, &CHGIC_ReadCmd0x3DL,SMBus_NoPEC))
    	{
			CHGIC_SMbusFailCnt++;
    	}
		CHGIC_ReadCmd0x3DH_Temp = CHGIC_ReadCmd0x3DH;
	}
}


/*
*******************************************************************************
* Function name: ChkBattery_OTP
*
* Descriptoin: 
*          Check battery over temperature protect. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ChkBattery_OTP()
{
	ITempW01 = (WORD)((EC_oCBTh<<8)+EC_oCBTl);		// get temperature
	if (ITempW01 > 2730)
	{
		nBattAverTemp = (ITempW01 - 2730) / 10;
	}
	else
	{
		return;
	}

	//S+ add battery OTP shutdown protect in AC mode.
	if((SystemIsS0)&&(nBattAverTemp >= BatteryOTPShutdown))  //65
	{
		#if !EN_PwrSeqTest
        SET_MASK(SysStatus,ERR_ShuntDownFlag);//Add battery over temperature protect.
		ProcessSID(BATTOVERTEMP_ID);
		RSMRST_shutdown();
		#endif
	}
	//E+.
	  
	if ((SystemIsS0)&&(IS_MASK_SET(nBatteryStatH,CMBS_DISCHARGE)))     
	{		
		if ((nBattGasgauge>25 && nBattAverTemp >= 55) || (nBattGasgauge<=25 && nBattAverTemp >= 48))//COKEYXU005: nBattGasgauge 40 to 25
		{                                                                                            
			SET_MASK(nStopChgStat3L,ENOVERTEMP);
			SET_MASK(BatteryAlarm,BATOTP);
            SET_MASK(Thro_Status2, b3Batt_OTP);
			if(uPJID == 0X02) //for I+N
			{
				cGPUBattOTPThrottling = 4;
				SET_MASK(GPU_Prochot, b3_battery_OTP); 

			}
			else //for I+A and UMA
			{ 
				SET_MASK(GPU_Prochot, b3_battery_OTP); 
			} 			
		}
		else if((nBattGasgauge>25 && nBattAverTemp < 53) || (nBattGasgauge<=25 && nBattAverTemp < 46)) //COKEYXU005: nBattGasgauge 40 to 25
		{                                                                                             
			CLR_MASK(BatteryAlarm,BATOTP);
			CLR_MASK(nStopChgStat3L,ENOVERTEMP);
			cGPUBattOTPThrottling = 0;  //psys will set D2 
			CLR_MASK(GPU_Prochot, b3_battery_OTP); 
            CLEAR_MASK(Thro_Status2, b3Batt_OTP);			
		}

		if(nBattGasgauge<=25) //COKEYXU005: nBattGasgauge 40 to 25
		{
			if(nBattAverTemp > 48)
			{
				SET_MASK(Fan_full_status, b0_Battery_OTP);
			}
			else if(nBattAverTemp < 46)
			{
				CLR_MASK(Fan_full_status, b0_Battery_OTP);
			}
		}
		else
		{
			CLR_MASK(Fan_full_status, b0_Battery_OTP);
		}		
	}
	else
	{
		CLR_MASK(BatteryAlarm,BATOTP);
		CLR_MASK(nStopChgStat3L,ENOVERTEMP);
		CLEAR_MASK(Thro_Status2, b3Batt_OTP); 
		cGPUBattOTPThrottling = 0; 
		CLR_MASK(GPU_Prochot, b3_battery_OTP); 
		CLR_MASK(Fan_full_status, b0_Battery_OTP); 
	}
}

/*
*******************************************************************************
* Function name: ChkBattery_LowVoltage
*
* Descriptoin: 
*          Check battery low voltage. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ChkBattery_LowVoltage()
{
    WORD CV1,CV2,CV3,CV4;
	
	if (!Read_AC_IN())
	{
		if (nNowCurrentH & 0x80)
		{
			if(bRSMBusBlock(SMbusChB, SMbusRBK, SmBat_Addr, C_Mdata, &BAT1_MD_1))
            {
				
		    	CV4 = (BAT1_MD_6 << 8)+BAT1_MD_5;   // Get Main battery desgin voltage   //MEILING016:remove.  //MEILING017:add.
		        CV3 = (BAT1_MD_8 << 8)+BAT1_MD_7;   // Get Main battery desgin voltage   //MEILING016:remove.  //MEILING017:add.
		        CV2 = (BAT1_MD_A << 8)+BAT1_MD_9;   // Get Main battery desgin voltage  //MEILING016:change CV3 to CV2.  
		        CV1 = (BAT1_MD_C << 8)+BAT1_MD_B;   // Get Main battery desgin voltage  //MEILING016:change CV4 to CV1.  
				if ((CV1 < 3000) || (CV2 < 3000) || ((CellCount>=3) && (CV3 < 3000)) || ((CellCount==4) && (CV4 < 3000)) )
				{
					if(SystemIsS0)
                    {
                        RamDebug(0xB1);                          
                        SET_MASK(nBatteryStatH, CMBS_CRITICALLOW);                            
                    }            
				}
              	else
                {
                   	CLR_MASK(nBatteryStatH, CMBS_CRITICALLOW);  
                }
 

		        if ((CV1 < 3000) || (CV2 < 3000) || ((CellCount>=3) && (CV3 < 3000)) || ((CellCount==4) && (CV4 < 3000)) ) //MEILING017: add
				{
					SET_MASK(BatteryAlarm,BATLOWVOL);
				}
                else 
				{
					CLR_MASK(BatteryAlarm,BATLOWVOL);
				}      
            }   
		}
	}
	else
	{
		CLR_MASK(BatteryAlarm,BATLOWVOL);
        CLR_MASK(nBatteryStatH, CMBS_CRITICALLOW);     
	}
}


/*
*******************************************************************************
* Function name: CkkBattery_RSOCLow
*
* Descriptoin: 
*          Check battery low RSOC. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void CkkBattery_RSOCLow()
{
	if ((!Read_AC_IN()) || (IS_MASK_SET(CHGIC_ReadCmd0x12L,BatLearnEnable)))
	{
		if(nBattGasgauge <= 7) //turn on prochot and dGPU throttling to D5.
		{
			if(uPJID == 0X02) //I+N
			{
				cGPUBattLowThrottling = 4; 
			}
			else
			{ 
				SET_MASK(GPU_Prochot, b4_battery_low); 
			} 
            SET_MASK(Thro_Status2, b6BattRSOC_Low); 
		}
		else if(nBattGasgauge > 7) //turn off prochot and dGPU throttling to D1.
		{
			cGPUBattLowThrottling = 0;  //psys will set D2
			CLR_MASK(GPU_Prochot, b4_battery_low); 
            CLEAR_MASK(Thro_Status2, b6BattRSOC_Low); 
		}
	}
	else
	{
		cGPUBattLowThrottling = 0; 
		CLR_MASK(GPU_Prochot, b4_battery_low); 
        CLEAR_MASK(Thro_Status2, b6BattRSOC_Low); 
	}
}

/*
*******************************************************************************
* Function name: ChkBattery_FCCchg
*
* Descriptoin: 
*          Check battery FCC change. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ChkBattery_FCCchg()
{
	ChkBattery_FCCchg_count++;
	if (ChkBattery_FCCchg_count >= 100) //10 Sec(100ms 100times)
	{
		ChkBattery_FCCchg_count = 0;
		//COKEYXU019:S+ remove for RSOC drop from 100% to 85% issue.
		//ChkBattery_FCCchg_count2++;
		//if(ChkBattery_FCCchg_count2 < 30)
		//{
		//  return;
		//}
		//ChkBattery_FCCchg_count2 = 0x00;
		//COKEYXU019:E+
		if ((ChkBattery_FCCchg_lastFCCL == 0) && (ChkBattery_FCCchg_lastFCCH == 0))
		{
			ChkBattery_FCCchg_lastFCCL = nFullChgCapL;
			ChkBattery_FCCchg_lastFCCH = nFullChgCapH;
		}

		//Check FCC and Notify OS if FCC change for each 10 sec
		if ((ChkBattery_FCCchg_lastFCCL != nFullChgCapL)
			|| (ChkBattery_FCCchg_lastFCCH != nFullChgCapH))
		{
			ECSMI_SCIEvent(ACPI_BAT1IN_SCI);
			ChkBattery_FCCchg_lastFCCL = nFullChgCapL;
			ChkBattery_FCCchg_lastFCCH = nFullChgCapH;
		}
	}
}

/*
*******************************************************************************
* Function name: ChkAvgCurrent
*
* Descriptoin: 
*          Check battery average current. 
*
* Invoked by:  OEM_PollingBatData_TASK()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ChkAvgCurrent()
{
	//Average current report to OS
	//Timer<=60 seconds(The timer starts counting when AC adapter plug out.)
	//Report "0x00" to EC name space 0xd2, 0xd3 by one time, and then
	//Report battery Current(0x0a) to EC name space 0xd2, 0xd3
	//Reset condition:When Timer>60 seconds,Report battery AverageCurrent(0x0b) to EC name space 0xd2, 0xd3
	if (Bat0x0BTempH & 0x80)
	{
		XWTemp1 = 0xFFFF - (WORD)((Bat0x0BTempH<<8)+Bat0x0BTempL);	
		if (XWTemp1 < 400)
		{
			return;
		}
	}	
	// Filter the dirty data because when AC not exit, there will only be discharge current.
  	if(IS_MASK_CLEAR(SYS_STATUS,AC_ADP) && SystemIsS0 && IS_MASK_CLEAR(Bat0x0BTempH,BIT7))
    {
   
         if(IS_MASK_SET(nNowCurrentH,BIT7))
        {
             nAvgCurrentL = nNowCurrentL;
		     nAvgCurrentH = nNowCurrentH;
        }  		 
        return;
    }
  	// Filter the dirty data because when AC not exit, there will only be discharge current.
	if ( Bat0x0BFakeCnt == 0)
	{
		nAvgCurrentL = Bat0x0BTempL;
		nAvgCurrentH = Bat0x0BTempH;
	}
	else if(Bat0x0BFakeCnt < 60)
	{
		nAvgCurrentL = nNowCurrentL;
		nAvgCurrentH = nNowCurrentH;
	}
	else if(Bat0x0BFakeCnt == 60)
	{
		nAvgCurrentL = 0;
		nAvgCurrentH = 0;
	}
}

/*
*******************************************************************************
* Function name: RST_ChgTimeOutCnt
*
* Descriptoin: 
*          Reset charge timeout count. 
*
* Invoked by:  Battery1Sec() and Chk_BatSMbusFailCount()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void RST_ChgTimeOutCnt(void)
{
	TrickleChgTimeOutCnt = 0;
	FastChgTimeOutCnt = 0;
	CLEAR_MASK(nStopChgStat3L,ENSTOPCHG);
	CLEAR_MASK(nStopChgStat3H,ENTRITIMEOUT); 
}

/*
*******************************************************************************
* Function name: Battery100ms
*
* Descriptoin: 
*          Battery 100ms task. 
*
* Invoked by:  Hook_Timer100msEventC()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Battery100ms(void)
{
	if (inhibit2sec!=0)
		inhibit2sec--;
	else
		CLEAR_MASK(nStopChgStat3L,ENCHARGESUSP);		// use for detect battery charging suspend
}

/*
*******************************************************************************
* Function name: Battery1Sec
*
* Descriptoin: 
*          Battery 1sec task. 
*
* Invoked by:  Hook_Timer1SecEventA()
*
* TimeDiv：1s
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Battery1Sec(void)
{
	if (Bat0x0BFakeCnt != 0)
		Bat0x0BFakeCnt--;

  	if ((BatSMbusFailCount != 0)&&IS_MASK_CLEAR(BATTUPDATEFW, PriBattInhib))   //add 'PriBattInhib' judge 
	{
    	SET_MASK(Bat1_FPChgFlag,BIT(0));
    	nBattErrorCnt++;
        if (nBattErrorCnt > 250)
    		nBattErrorCnt = 251;
    	if (BatSMbusFailCount > 220)
    		BatSMbusFailCount = 221;
	}
	else
	{
		nBattErrorCnt = 0;
		if (nNowCurrentL|nNowCurrentH)			// if  nNowCurrent != 0
		{
			if ((nNowCurrentH&0x80) == 0)		// nNowCurrent > 0, charging
			{								//current(0x0A)>0mA&polling time10cycles
				nBattTempCnt ++;
				if (nBattTempCnt >= 10)
					nBattTempCnt = 11;			// nBattTempCnt > 10
			}
			else								// discharging
			{
				nBattTempCnt = 0;
				RST_ChgTimeOutCnt();
			}
		}
		else
		{
			nBattTempCnt = 0;
		}
	}
}


const sRSmbusBStruct code ABatCommandTable [] =
{//     command               REG                      no.
	{ C_Date,		&nManufactureDateL		,0x00},				//Batpollstep1 = 0    	WORD
	{ C_Dchem,		&batteryChemistry		,0x04},				//Batpollstep1 = 1    	BLOCK
	{ C_Mname,		&BATTMANUFACTURE		,0x0A},				//Batpollstep1 = 2    	BLOCK
	{ C_Dname,		&BATTDEVICENAME			,0x0A},				//Batpollstep1 = 3		BLOCK
	{ C_OpMfgFun5,  &BAT1_Bar_Code          ,0x0A},             //Batpollstep1 = 4       BLOCK  //MEILING031:add. 
	{ C_Date,		&nManufactureDateL		,0x00},				//Batpollstep1 = 5  	WORD
	{ C_DCap,	    &nDesignCapL			,0x00},				//Batpollstep1 = 6  	WORD    0~8 initial only
	{ C_DVolt,		&nDesignVoltL			,0x00},				//Batpollstep1 = 7  	WORD
	{ C_SerialNo,	&nSerialNumL			,0x00},				//Batpollstep1 = 8  	WORD
	{ C_mode,		&EC_C_modeL				,0x00},				//Batpollstep1 = 9		WORD
	{ C_ChargingV,  &nChargingVoltL			,0x00},				//Batpollstep1 = 10  	WORD
	{ C_CycleCount,	&EC_oCCBQl				,0x00},				//Batpollstep1 = 11  	WORD
	{ C_volt,		&nPresentVoltL			,0x00},				//Batpollstep1 = 12  	WORD
	{ C_current,	&nNowCurrentL			,0x00},				//Batpollstep1 = 13  	WORD
	{ C_ChargingI,  &nBattCharCurrentL      ,0x00},				//Batpollstep1 = 14  	WORD
	{ C_BatStatus,	&nBattery0x16L			,0x00},				//Batpollstep1 = 15  	WORD
	{ C_RMcap,		&Bat0x0FTempL			,0x00},				//Batpollstep1 = 16  	WORD    9~19 normal polling
	{ C_current,	&nNowCurrentL			,0x00},				//Batpollstep1 = 17  	WORD
	{ C_FCcap,		&nFullChgCapL			,0x00},				//Batpollstep1 = 18 	WORD
	{ C_temp,		&EC_oCBTl				,0x00},				//Batpollstep1 = 19 	WORD
	{ C_RSOC,	 	&BAT1PERCL  			,0x00},				//Batpollstep1 = 20 	WORD
	{ C_current,	&nNowCurrentL			,0x00},				//Batpollstep1 = 21  	WORD
	{ C_AVcurrent,	&Bat0x0BTempL			,0x00}, 			//Batpollstep1 = 22 	WORD
	{ C_LVMfgFun2,	&Bat0x3ETempL			,0x00},				//Batpollsetp1 = 23     WORD
	{ C_access,		&Bat0x00TempL			,0x00}, 			//Batpollsetp1 = 24 	WORD
	{ C_current,	&nNowCurrentL			,0x00},				//Batpollstep1 = 25  	WORD
	{ C_RSOC,	 	&BAT1PERCL  			,0x00},				//Batpollstep1 = 26 	WORD  //Add read battery RSOC at bottom of table.
	//{ C_D_FET,	&SHIPMODE_L			,0x00}, 				//Batpollstep1 = 27 	WORD

};

/*
*******************************************************************************
* Function name: GetBatData
*
* Descriptoin: 
*          Get Battery Data. 
*
* Invoked by:  FirstGetBatData() and OEM_PollingBatData_TASK()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void GetBatData(BYTE _STEP)
{
	switch (_STEP)
    {
    	case 1:	//block data
		case 2: //block data
		case 3: //block
		case 4: //block data
			if(	!bRSMBusBlock(SMbusChB, SMbusRBK, SmBat_Addr,
				ABatCommandTable[_STEP].cmd,
				ABatCommandTable[_STEP].smbdataA))
			{	// SMBUS FAIL
				BatSMbusFailCount++;
			}
			else
			{	// SMBUS OK
				BatSMbusFailCount=0;
			}
			break;

		default: //word data
			if(!bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr,
			   ABatCommandTable[_STEP].cmd,
			   ABatCommandTable[_STEP].smbdataA,
			   SMBus_NoPEC))
			{	// SMBUS FAIL
				BatSMbusFailCount++;
			}
			else
			{	// SMBUS OK
				BatSMbusFailCount=0;
			}

		        break;
	}
}

/*
*******************************************************************************
* Function name: ChkBattery_Percl
*
* Descriptoin: 
*          Check Battery Percl. 
*
* Invoked by:  WakeUp_DO_Function()
*
* TimeDiv：10ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ChkBattery_Percl()
{
    bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr,C_RSOC,&BAT1PERCL ,SMBus_NoPEC);
}

/*
*******************************************************************************
* Function name: FirstGetBatData
*
* Descriptoin: 
*          battery plug in first fast update all information. 
*
* Invoked by:  Battery1In() and Hook_Timer500msEventA()
*
* TimeDiv：10ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void FirstGetBatData(void)
{
  	BYTE i,j;
	WORD DesignVoltage; 

  	Batpollstep1 = 0;
  	nBatteryStatL = 0;
  	for (i=0;i<(sizeof(ABatCommandTable)/4);i++)
  	{
    	GetBatData(Batpollstep1);
    	Batpollstep1++;
  	}
    Batpollstep1=8;
    SHA1_SEED = (WORD)((TL0+Bat0x0FTempL)<<8);
	
 	//Add code for GBSI function.
    if((batteryChemistry[0]=='L')&&(batteryChemistry[1]=='I')&&(batteryChemistry[2]=='O')&&(batteryChemistry[3]=='N'))
		SET_MASK(nBatteryStatL,CMBS_BATTTYPE);	
	else
	    CLEAR_MASK(nBatteryStatL,CMBS_BATTTYPE);

	OCPCapacity = (WORD)((nDesignCapH<<8)+nDesignCapL);
	OCPCapacity = OCPCapacity *8;

    OCPCapacity =(WORD)((LWORD)OCPCapacity*1000 / (WORD)((nDesignVoltH<<8)+nDesignVoltL));

	OCPCapacityRelease = (WORD)((nDesignCapH<<8)+nDesignCapL);
	OCPCapacityRelease = OCPCapacityRelease *7;

    OCPCapacityRelease =(WORD)((LWORD)OCPCapacityRelease*1000 / (WORD)((nDesignVoltH<<8)+nDesignVoltL));

   	DesignVoltage=(WORD)((nDesignVoltH<<8)+ nDesignVoltL);
   	if((DesignVoltage >= 14000) && (DesignVoltage <= 15200))  //Cell: 4series   14V~15.2V
		CellCount=4;
	else if((DesignVoltage >= 10500) && (DesignVoltage <= 11400)) //Cell: 3series   10.5V~11.4V
		CellCount=3;
	else if((DesignVoltage >= 7000) && (DesignVoltage <= 7600)) //Cell: 2series   7V~7.6V
		CellCount=2;

	S3ResumeRSOC = S3RSOCPercentage;		// Set S3 resuem in the battery under 5%.
	BatteryOTP = BatteryOTPHi;
	BatteryOTPRelease = BatteryOTPLow;
	BatteryOTPShutdown = BatteryOTPSD;


	UpdateNameSpace();
	Chk_Battery_Full();
	nBattErrorCnt = 0;
	SHA1_SEED = SHA1_SEED + (WORD)(TL0);
	srand(SHA1_SEED);

	if (BatSMbusFailCount==0)
	{
		Battdata_ready = 1;
		WSMbusTemp01 = EC_C_modeH;
		if ((WSMbusTemp01^0xE0)&0xE0)				
		{
			WSMbusTemp01=EC_C_modeL;
			WSMbusTemp02=EC_C_modeH;
			WSMbusTemp02|=0xE0;
			if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_mode, &WSMbusTemp01,SMBus_NoPEC))
			{
				BatSMbusFailCount++;
			}
		}
	}
}

/*
*******************************************************************************
* Function name: Chk_Battery_Full
*
* Descriptoin: 
*          Check battery full. 
*
* Invoked by:  FirstGetBatData(),OEM_PollingBatData_TASK(),AdapterIn() and AdapterOut()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Chk_Battery_Full(void)
{
	//COKEYXU018: 65W adapter stop charging when S0
	if (IS_MASK_CLEAR(SYS_STATUS,AC_ADP)||(IS_MASK_SET(CHGIC_ReadCmd0x37L, BoostIndication))||(IS_MASK_SET(SYS_STATUS,AC_ADP)&&IS_MASK_SET(ACOFF_SOURCE,BATTLEARN)))
	{
    	CLEAR_MASK(SEL_STATE0,CHARGE_A);		//clear  battery charging flag
    	CLEAR_MASK(nBatteryStatH,CMBS_CHARGE);	//clear  battery charging flag
    	SET_MASK(nBatteryStatH,CMBS_DISCHARGE);	//set battery discharging flag
    	return;
	}

	if ((cBattFlag0.fbit.cCmdAcOff==1)||IS_MASK_SET(EC_BatteryStatusL,FullyChg)||((nStopChgStat3L|nStopChgStat3H)!=0))
	{
		CLEAR_MASK(SEL_STATE0,CHARGE_A);			        //clear  battery charging flag
		CLEAR_MASK(nBatteryStatH,CMBS_CHARGE);		//clear  battery charging flag
		CLEAR_MASK(nBatteryStatH,CMBS_DISCHARGE);   	//clear battery discharging flag
		return;
	}
	else
	{
		if ((cBattFlag0.fbit.cBF0_GoTarget ==1) && (cTargetGauge == nBattGasgauge))
		{
			CLEAR_MASK(SEL_STATE0,CHARGE_A);			//clear  battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_CHARGE);		//clear  battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_DISCHARGE);	//clear battery discharging flag
			return;
		}
	}

	if (IS_MASK_CLEAR(LENOVOPMFW,BATTERY_STORAGE))  
	{
		if (IS_MASK_SET(nBattery0x16L,FullyChg))	// || IS_MASK_SET(CHGIC_ReadCmd0x12L,ChargeInhibit))
		{
			CLEAR_MASK(SEL_STATE0,CHARGE_A);  			//clear  battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_CHARGE);  	//clear  battery charging flag
			SET_MASK(nBatteryStatH,CMBS_DISCHARGE); 	//set battery discharging flag
		}
		else
		{
			SET_MASK(SEL_STATE0,CHARGE_A);  			//set battery charging flag
			SET_MASK(nBatteryStatH,CMBS_CHARGE);  		//set battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_DISCHARGE); 	//clear battery discharging flag
		}
	}
	else
	{
		if (IS_MASK_SET(EC_BatteryStatusL,FullyChg))	// || IS_MASK_SET(CHGIC_ReadCmd0x12L,ChargeInhibit))
		{
			CLEAR_MASK(SEL_STATE0,CHARGE_A);  			//clear  battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_CHARGE);  	//clear  battery charging flag
		}
		else
		{
			SET_MASK(SEL_STATE0,CHARGE_A);  			//set battery charging flag
			SET_MASK(nBatteryStatH,CMBS_CHARGE);  		//set battery charging flag
			CLEAR_MASK(nBatteryStatH,CMBS_DISCHARGE); 	//clear battery discharging flag
		}
	}
}

/*
*******************************************************************************
* Function name: Unlock_ShipMode
*
* Descriptoin: 
*          Unlock battery shipmode. 
*
* Invoked by:  Hook_64Port(),Hook_66Port(),Hook_6CPort(),AdapterIn() and Hook_Timer1SecEventC()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Unlock_ShipMode(void)
{
	// first command = 0x34, data1 = 0x2000, data2 = 0x4000
 	// change all SMBus_NoPEC to SMBus_NeedPEC
    if(IS_MASK_SET(BATTUPDATEFW,BIT0))
        return;
	ShipModeEn=0x00;   
	CLEAR_MASK(EMStatusBit2,b0SetBatteryShipMode); //ANGELAS079:add
	WSMbusTemp01=0x00;
	WSMbusTemp02=0x20;

	if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC)) 
	{
		SMbusFailCnt3++;
	}

	WSMbusTemp01=0x00;
	WSMbusTemp02=0x40;

	if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC))
	{
		SMbusFailCnt2++;
	}

	if(bRWSMBus(SMbusChB, SMbusRW, SmBat_Addr, C_D_FET, &ShipModeACK,SMBus_NoPEC))
	{
	    if(ShipModeACK!=0x00)
	    {
           ShipModeCnt=1;
		}	
	}
	else
	{
	    SMbusFailCnt4++;
        ShipModeCnt=1;
	}
}

/*
*******************************************************************************
* Function name: Lock_ShipMode
*
* Descriptoin: 
*          Lock battery shipmode. 
*
* Invoked by:  Enter_ShipMode()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Lock_ShipMode(void)
{
	// first command = 0x34, data1 = 0x0000, data2 = 0x1000
 	//change all SMBus_NoPEC to SMBus_NeedPEC

	//s+Add time delay for Clear First Used Date to Enable ship mode following the requirements of the battery and re-try mechanism.
	BYTE retryNum = 0x00; 
    SMbusFailCnt3 = 0x00; 
    SMbusFailCnt2 = 0x00;
	
	WSMbusTemp01=0x00;
	WSMbusTemp02=0x00;
	if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC))
	{
		SMbusFailCnt3++;
	}
	
	Delay1MS(250);

	WSMbusTemp01=0x00;
	WSMbusTemp02=0x10;
	if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC))
	{
	  	SMbusFailCnt2++;
	}

	for(retryNum=0x00;retryNum<0x06;retryNum++)
    {
    	if((SMbusFailCnt3 == 0x00)&&(SMbusFailCnt2 == 0x00))
        	break;

        ProcessSID(0xA5);
        SMbusFailCnt3 = 0x00;
        SMbusFailCnt2 = 0x00;
           
        Delay1MS(0x0A);
		Delay1MS(250); 
           
        WSMbusTemp01=0x00;
        WSMbusTemp02=0x00;
        if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC))
        {
        	SMbusFailCnt3++;
        }
           
       	Delay1MS(250);
       
        WSMbusTemp01=0x00;
        WSMbusTemp02=0x10;
        if(!bRWSMBus(SMbusChB, SMbusWW, SmBat_Addr, C_D_FET, &WSMbusTemp01,SMBus_NeedPEC))
        {
        	SMbusFailCnt2++;
        }
	}
	//e+Add time delay for Clear First Used Date to Enable ship mode following the requirements of the battery and re-try mechanism.

}

/*
*******************************************************************************
* Function name: OEM_PollingBatData_TASK
*
* Descriptoin: 
*          OEM polling Battery Data Task 
*
* Invoked by:  Hook_Timer100msEventA()
*
* TimeDiv：100ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void OEM_PollingBatData_TASK(void)
{
	XBYTE i,j;
    XBYTE *ptr;

  	if(IS_MASK_CLEAR(BATTUPDATEFW,BIT0))
  	{
    	GetBatData(Batpollstep1);
    	Batpollstep1++;

    	if(Batpollstep1 >= (sizeof(ABatCommandTable)/4))
    	{
			////////////////////////
			//Battery Debounce Block
			//Get 9 entry from battery table, loop and XOR each byte, calculate a hash byte
			//If hash result same as previous, then counter +1
			//If counter > 3 times, mean battery data stable, keep Batpollstep1 = 9 to skip first 9 entry in battery table
			//otherwise, clean counter and loop the table again.
			if (Get_Batt_debounce_count < 3)
			{
				Get_Batt_debounce_hash2 = Get_Batt_debounce_hash1;
				Get_Batt_debounce_hash1 = 0x00;
				for (i=0; i<9; i++)
				{
					if(ABatCommandTable[i].block_limit == 0x00)
						Get_Batt_debounce_hash1 ^= *ABatCommandTable[i].smbdataA;
					else
					{
						ptr = ABatCommandTable[i].smbdataA;
						for(j=0; j<ABatCommandTable[i].block_limit; j++)
						{
							Get_Batt_debounce_hash1 ^= *ptr;
							ptr++;
						}
					}
				}

				if (Get_Batt_debounce_hash2 == Get_Batt_debounce_hash1)
					Get_Batt_debounce_count++;
				else
					Get_Batt_debounce_count = 0;

				Batpollstep1=0; //revert Batpollstep1 for start over
				//Fill nBatteryStatL after Battery Data stable.
				if ( Get_Batt_debounce_count >= 3 )
				{
					if( ( BATTMANUFACTURE[0] == 'S' ) && ((BATTMANUFACTURE[1] == 'A' )||( BATTMANUFACTURE[1] == 'a' )) )
				    	nBatteryStatL |= 0x10 ;	// SANYO
					else if( (BATTMANUFACTURE[0] == 'S' ) && ( BATTMANUFACTURE[1] == 'O' ) )
				    	nBatteryStatL |= 0x20 ;	// Sony
					else if( (BATTMANUFACTURE[0] == 'P' ) && (( BATTMANUFACTURE[1] == 'A' )|| ( BATTMANUFACTURE[1] == 'a' )) )
						nBatteryStatL |= 0x40 ;	// Panasonic
					else if( (BATTMANUFACTURE[0] == 'S' ) && ( BATTMANUFACTURE[1] == 'U' ) )
						nBatteryStatL |= 0x50 ;	// Samsung
					else if( (BATTMANUFACTURE[0] == 'L' ) && ( BATTMANUFACTURE[1] == 'G' ) )
						nBatteryStatL |= 0x30 ;	// LG
					else if( (BATTMANUFACTURE[0] == 'C' ) && (( BATTMANUFACTURE[1] == 'P' ) || ( BATTMANUFACTURE[1] == 'p' )) )
						nBatteryStatL |= 0x60;	// CPT Celxpert
					else if( (BATTMANUFACTURE[0] == 'S' ) && (BATTMANUFACTURE[1] == 'M' ) )
						nBatteryStatL |= 0x70;	// Simplo*/
				}
			}
			else
			{
    			Batpollstep1=9;
			}
    	}
		UpdateNameSpace();
		ChkLENOVOPMFW();
		ChkGoTarget();
	  	Chk_Battery_Full();
		if(GPUProchotONCnt == 0) 
		{ 
			ChkBattery_OTP();
			ChkBattery_LTP();//COKEYXU039:Add Battery Low Temperature Protect
			CkkBattery_RSOCLow(); 
		} 
        ChkBattery_LowVoltage();
		ChkBattery_FCCchg();
		ChkAvgCurrent();
		ChkS3DCRSOC();//Modify battery RSOC below 5% can't into S3.
		ChkS3ResumeRSOC();
		RSOC1Pto0P_ShutdownCheck();
  	}
}


/*
*******************************************************************************
* Function name: WriteSmartChgIC
*
* Descriptoin: 
*          Write smart charge IC 
*
* Invoked by:  AdapterIn(),Cmd_B4(),Hook_Timer500msEventA(),Charge_inhibit2sec()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void WriteSmartChgIC(void)
{
  	BYTE sCmd, *sData;

	if (!Read_AC_IN())
		return;

  	if( CHGIC_ptr>=4 ) 
		CHGIC_ptr=0;
  	else 
		CHGIC_ptr++;
	switch( CHGIC_ptr )
    {
    case 0x00:
    	sCmd = C_ChargeCurrent;
        sData = &nBattCharCurrentL;
        break;
    case 0x01:
        sCmd = C_ChargeVoltage;
        sData = &nChargingVoltL;
        break;
    case 0x02:
		
		//COKEYXU041: S+ S+Setting charge IC
        if( AdapterID==AdapterID_170W || AdapterID==AdapterID_135W )
		{									// 6.144A 1800	(4096+2048)
			CHGIC_InputCurrentH = 0x18; 	
			CHGIC_InputCurrentL = 0x00;		
		}
		else if(AdapterID==AdapterID_90W)	
		{									// 4.1A 1000	(4096)
			CHGIC_InputCurrentH = 0x10;		
			CHGIC_InputCurrentL = 0x00;		
		}
		else if(AdapterID==AdapterID_65W)		
		{									// 3A 0C00 (2048+1024)
			CHGIC_InputCurrentH = 0x0C; 	
			CHGIC_InputCurrentL = 0x00;		
		}
		else if(AdapterID==AdapterID_45W)
		{									// 2.05A 0800 (2048)
			CHGIC_InputCurrentH = 0x08;	 	
			CHGIC_InputCurrentL = 0x00;		
		}
		//COKEYXU041:E+Setting charge IC 
			
        sCmd = C_InputCurrent;
        sData = &CHGIC_InputCurrentL;
        break;
	case 0x03:
	  	if ((CHGIC_ReadCmd0x12L!=0) || (CHGIC_ReadCmd0x12H!=0))
	  	{
			if ((cBattFlag0.fbit.cCmdAcOff==1)||IS_MASK_SET(EC_BatteryStatusL,FullyChg))	
			{
				SET_MASK(CHGIC_WriteCmd0x12L,ChargeInhibit);	//Inhibit Charge 
			}
			else
			{
				if ((cBattFlag0.fbit.cBF0_GoTarget ==1) && (cTargetGauge == nBattGasgauge))
				{
					SET_MASK(CHGIC_WriteCmd0x12L,ChargeInhibit);	//Inhibit Charge 
				}
				else
				{
					CLR_MASK(CHGIC_WriteCmd0x12L,ChargeInhibit);	//Enable Charge 
				}
			}
			//Due to must be stop charger before set AC OFF. add ACOFF_SOURCE in condition.
			if (nStopChgStat3L|nStopChgStat3H|inhibit2sec|ACOFF_SOURCE)					// nStopChgStat3L,nStopChgStat3H,Charge_inhibit2sec != 0
			{
				SET_MASK(CHGIC_WriteCmd0x12L,ChargeInhibit); 	//Inhibit Charge 
			}
			
        	sCmd = C_ChargerMode;
        	sData = &CHGIC_WriteCmd0x12L; 
	  	}
		else
		{
			return;
		}
	  	break;
		
	case 0x04:
    	sCmd = C_ChargerMode3;
    	sData = &CHGIC_WriteCmd0x37L;
	  	break;
    default:
		return;
    }
    if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, sCmd, sData,SMBus_NoPEC))
    {
		CHGIC_SMbusFailCnt++;
    }
}


const sRSmbusBStruct code ReadChgIcCmdTable [] =
{//     command                   REG 			 no.
  { C_ChargeCurrent,    &CHGIC_ReadCmd0x14L    	,0x00},
  { C_ChargeVoltage,    &CHGIC_ReadCmd0x15L    	,0x00},
  { C_InputCurrent,    	&CHGIC_ReadCmd0x3FL     ,0x00},
  { C_ChargerMode,     	&CHGIC_ReadCmd0x12L		,0x00},
  { C_ChargerMode3,     &CHGIC_ReadCmd0x37L		,0x00},
};

/*
*******************************************************************************
* Function name: ReadSmartChgIC
*
* Descriptoin: 
*          Read smart charge IC 
*
* Invoked by:  AdapterIn(),Hook_Timer500msEventA()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void ReadSmartChgIC(void)
{
	if(!bRWSMBus(SMbusChB, SMbusRW, Charger_Addr,
         ReadChgIcCmdTable[CHGIC_ptr].cmd,
         ReadChgIcCmdTable[CHGIC_ptr].smbdataA,
           SMBus_NoPEC))
	{  // SMBUS FAIL
  		CHGIC_SMbusFailCnt++;
	}
}

/*
*******************************************************************************
* Function name: Chk_BAT1PERCL_5
*
* Descriptoin: 
*          Check battery percent 5
*
* Invoked by:  Chk_BatSMbusFailCount()
*
* TimeDiv：500ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Chk_BAT1PERCL_5(void)
{
	if ((BAT1PERCL <= 0)&&(IS_MASK_SET(nBattery0x16L,Dsg)))	// BAT1PERCL <= 5%  //change 5 to 0
	{
		SET_MASK(BatteryAlarm,BATPercl_5);
	}
	else
	{
		if (IS_MASK_SET(BatteryAlarm,BATPercl_5))
		{
			CLEAR_MASK(BatteryAlarm,BATPercl_5);
		}
	}
}


/*
*******************************************************************************
* Function name: Chk_BatSMbusFailCount
*
* Descriptoin: 
*          Check battery SMbus fail count
*
* Invoked by:  Hook_Timer500msEventA()
*
* TimeDiv：500ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Chk_BatSMbusFailCount(void)
{
	BYTE sCmd, *sData;

	if (nBattErrorCnt==0)
	{
		CLEAR_MASK(nStopChgStat3L,ENCOMMFAIL);	// clear bat communication fail and clear STOP charge
		CLEAR_MASK(Bat1_FPChgFlag,BIT(0));
		Chk_BAT1PERCL_5();
		return;
	}

	if (IS_MASK_CLEAR(SYS_STATUS,AC_ADP))		// discharge
	{

		RST_ChgTimeOutCnt();

		if (nBattErrorCnt==30)
		{
			SMbusFailCnt2++;
		}
		if (nBattErrorCnt>=150)
		{
			#if !EN_PwrSeqTest
			SMbusFailCnt3++;
			nBattErrorCnt = 151;
			if ((SysPowState==SYSTEM_S0)||(SysPowState==SYSTEM_S3))
			{
                SET_MASK(SysStatus,ERR_ShuntDownFlag);//Add battery smbus fail shutdown ID.
				ProcessSID(BATTCOMMFAIL_ID);
				RSMRST_shutdown();
			}
			#endif
		}
	}
	else
	{
		if (IS_MASK_SET(SYS_STATUS,AC_ADP))					// battery charge
		{
			if ((nBattErrorCnt != 0)&&(nBattErrorCnt < 30))
			{
			//Modify charger IC config.
				nBattCharCurrentL = 0x00;			// 256ma
				nBattCharCurrentH = 0x01;           //Modify power charge IC setting for battery current.
				nChargingVoltL = 0x40;				//12.6V  
				nChargingVoltH = 0x31;            
				SMbusFailCnt3++;
			}
			if ((nBattErrorCnt >= 30))				// disable charge
			{
				RST_ChgTimeOutCnt();
				SET_MASK(nStopChgStat3L,ENCOMMFAIL);	// Set bat communication fail and STOP charge.
															//	charge inhibit
				SMbusFailCnt2++;
			}
		}
	}
}


/*
*******************************************************************************
* Function name: Chk_CHG_TimeOut
*
* Descriptoin: 
*          Check charge timeout
*
* Invoked by:  Hook_Timer1MinEvent()
*
* TimeDiv：1min
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Chk_CHG_TimeOut(void)			
{
    if (nBattTempCnt < 10)
		return;
	if (IS_MASK_SET(CHGIC_ReadCmd0x12L,ChargeInhibit))
		return;

	if((nBattCharCurrentH > 1) || ((nBattCharCurrentH == 1) && (nBattCharCurrentL >= 0xF4))) // nBattCharCurrent >= 0x01F4  (500mA)
	{
		TrickleChgTimeOutCnt = 0;
		FastChgTimeOutCnt++;
		if (FastChgTimeOutCnt >= 720) //12hour
		{
			FastChgTimeOutCnt = 721;
			SET_MASK(nStopChgStat3L,ENSTOPCHG);
		}
	}
	else // nBattCharCurrent < 0x01F4  (500mA)
	{
		FastChgTimeOutCnt = 0;
		TrickleChgTimeOutCnt++;
		if (TrickleChgTimeOutCnt >= 480) //COKEYXU025:change to 8hour from 6hour.
		{
			TrickleChgTimeOutCnt = 481;
			SET_MASK(nStopChgStat3H,ENTRITIMEOUT); //MARTINO006:change 'nStopChgStat3L' to 'nStopChgStat3H'
		}
	}
}


/*
*******************************************************************************
* Function name: Compare_data
*
* Descriptoin: 
*          Compare data
*
* Invoked by:  Compare_Auth_Result()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
BYTE Compare_data(XBYTE *buf1, XBYTE *buf2, BYTE count)
{
	do 
	{
		if( *buf1 != *buf2 ) return FALSE;
		buf1++;
		buf2++;
		count--;
	}while(count);
	return TRUE;
}

/*
*******************************************************************************
* Function name: Compare_Auth_Result
*
* Descriptoin: 
*          Compare authentication result
*
* Invoked by:  LV_BAT_Authentication()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void Compare_Auth_Result(void)
{
	if(Compare_data(&K, &RBATH0, 20) )
	{
		CLEAR_MASK(nStopChgStat3H,NotLenovoBattery); 	// SHA1 pass,legal
		CLEAR_MASK(LENOVOBATT,BATTERY_LEGAL);			// SHA1 pass,legal
		CLEAR_MASK(LV_Authen_Step_CNT,BIT(6)); 		// authentication ok
		SHA1failCnt=0x00;
		SHA1FailRetry=0x00;
	}
	else
	{
		SHA1failCnt ++;
		if (SHA1failCnt < 4)
		{
			Service_Auth_Step=23;						//if  SHA1failCnt <3, retry
		}
		else
		{
		    SHA1FailRetry++;
		    if((SHA1FailRetry!= 3)&&(SHA1FailRetry<11)) 
		    {
		        Service_Auth_Step = 1;
		    }
            else if(SHA1FailRetry == 3)  //add judge         
		    {
			    SET_MASK(nStopChgStat3H,NotLenovoBattery);	// SHA1 no pass, battery illegal
			    SET_MASK(LENOVOBATT,BATTERY_LEGAL);			// SHA1 no pass, battery illegal
			    SET_MASK(LV_Authen_Step_CNT,BIT(6)); 		// authentication Fail
			    Service_Auth_Step = 1;  
		    }
       	}  
	}
}

/*
*******************************************************************************
* Function name: SendChallengeToBat
*
* Descriptoin: 
*          Send challenge to battery
*
* Invoked by:  LV_BAT_Authentication()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
BYTE SendChallengeToBat(void)
{
	BYTE SMBus_work;
	SMBus_work = bWSMBusBlock(SMbusChB, SMbusWBK, BatteryAddress, 0x27, &BATchallenger, 20, TRUE);
	if( SMBus_work ) 
		return TRUE;
	
	return FALSE;
}


/*
*******************************************************************************
* Function name: GetChallengeFromBat
*
* Descriptoin: 
*          Get challenge from battery
*
* Invoked by:  LV_BAT_Authentication()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
BYTE GetChallengeFromBat(void)
{
	if( bRSMBusBlock(SMbusChB, SMbusRBK, BatteryAddress, 0x28, &RBATH0) ) 
		return TRUE;
	
	return FALSE;
}

/*
*******************************************************************************
* Function name: SetRandomKey
*
* Descriptoin: 
*          Set random Key
*
* Invoked by:  LV_BAT_Authentication()
*
* TimeDiv：NA
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void SetRandomKey(BYTE N)
{
	char *ptr;
	BYTE i;

	if( N==1 )
	{
		ptr = BATchallenger;
		for(i=0;i<20;i++)
		{
			*ptr = (char)(rand()&0xff);
			ptr++;
		}

		/* bq20zxx two pass example key */
		//SHA1_W[4] = 0x20212223;
		//SHA1_W[5] = 0x24252627;
		//SHA1_W[6] = 0x28292A2B;
		//SHA1_W[7] = 0x2C2D2E2F;
		//SHA1_W[8] = 0x30313233;

		/* LV Sample example key */
		//SHA1_W[4] =0xC82CA3CA;
		//SHA1_W[5] =0x10DEC726;
		//SHA1_W[6] =0x8E070A7C;
		//SHA1_W[7] =0xF0D1FE82;
		//SHA1_W[8] =0x20AAD3B8;


		/* SHA1 one pass example key */
		//SHA1_W[4] = 0x00000000;
		//SHA1_W[5] = 0x00000000;
		//SHA1_W[6] = 0x00000000;
		//SHA1_W[7] = 0x00000000;
		//SHA1_W[8] = 0x00000000;
	}
    else 
    {
		// move first time result to challenge
		//SHA1_W[4]=Hn0;
		N_Bit_Shift = 0;
		ITempW01 = &Hn0;
		AddrX_High = (ITempW01 >> 8);
		AddrX_Low  = (ITempW01 & 0xFF);
		ITempW01 = &SHA1_W[4];
		AddrZ_High = (ITempW01 >> 8);
		AddrZ_Low  = (ITempW01 & 0xFF);
		Cal_CTRL = Rotate_Shift_right_BIT;

		//SHA1_W[5]=Hn1;
		AddrX_Low+=4;
		AddrZ_Low+=4;
		Cal_CTRL = Rotate_Shift_right_BIT;

		//SHA1_W[6]=Hn2;
		AddrX_Low+=4;
		AddrZ_Low+=4;
		Cal_CTRL = Rotate_Shift_right_BIT;

		//SHA1_W[7]=Hn3;
		AddrX_Low+=4;
		AddrZ_Low+=4;
		Cal_CTRL = Rotate_Shift_right_BIT;

		//SHA1_W[8]=Hn4;
		AddrX_Low+=4;
		AddrZ_Low+=4;
		Cal_CTRL = Rotate_Shift_right_BIT;
	}
}


/*
*******************************************************************************
* Function name: LV_BAT_Authentication
*
* Descriptoin: 
*          Lenovo battery authentication
*
* Invoked by:  Hook_Timer500msEventA()
*
* TimeDiv：500ms
*
* Arguments: none
*
* Return Values: none 
*******************************************************************************
*/
void LV_BAT_Authentication(void)
{
	unsigned long xdata * byte_sha1_pntr;
    int j;

	//===================================================================
	// Service SHA1 algorithm
	//===================================================================
	switch(Service_Auth_Step)
	{
	case 1:
		SetRandomKey(1);		// initial W(4~8)
		Service_Auth_Step=2;
		break;
	case 21:					// SHA1 start send data delay 10 sec
		SendChallengeToBat();	// send to battery
		Service_Auth_Step=22;
		break;
	case 22:
		byte_sha1_pntr = sha1_auth(&BATchallenger);
		for (j=0; j<5 ;j++)
		{
			K[j] = *byte_sha1_pntr;
			byte_sha1_pntr++;
		}
		Service_Auth_Step=23;
		break;
	case 23:
		Service_Auth_Step=24;
		break;
	case 24:
		Service_Auth_Step=25;	// wait  for battery authentication
		break;
	case 25:
		GetChallengeFromBat();
		Service_Auth_Step=0;
		Compare_Auth_Result();
		break;
	case 0:
		Service_Auth_Step=0;
		break;
	default:
		Service_Auth_Step ++;
		break;
	}
}

/*
*******************************************************************************
* Function name: Lenovo_Battery_EM80       
*
* Descriptoin: Lenovo Battery EM80.
*
* Invoked by: UpdateNameSpace()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Lenovo_Battery_EM80(void)
{
	if (EM8_TEST == 0)
	{
		if (IS_MASK_SET(Bat0x3ETempH,BIT0)&&(IS_MASK_SET(SYS_STATUS,AC_ADP))&&IS_MASK_CLEAR(LENOVOPMFW,BATTERY_STORAGE))		// under storage mode Battery has been running over 72hrs ,EM can't pop out a message.
		{ 
			SET_MASK(LENOVOBATT,BATTERY_USAGE);
        }
		else
		{ 
		    CLEAR_MASK(LENOVOBATT,BATTERY_USAGE); 
        }

		if (Bat0x00TempL&0x03)		
		{ 
		    SET_MASK(LENOVOBATT,BAD_BATT); 
        }	
		else
		{ 
		    CLEAR_MASK(LENOVOBATT,BAD_BATT); 
        }

		if (IS_MASK_SET(Bat0x3ETempH,BIT1))	
		{ 
		    SET_MASK(LENOVOBATT,BATTERY_Exhaustion); 
        }	// Battery poor  (Exhaustion)
		else
		{ 
		    CLEAR_MASK(LENOVOBATT,BATTERY_Exhaustion);   
        }
	}
	else
	{ 
	    LENOVOBATT = EM8_TEST ; 
    }
}

//Through the charger IC option control battery learn mode.
/*
*******************************************************************************
* Function name: BATT_LEN_OFF       
*
* Descriptoin: Battery Learn mode off.
*
* Invoked by: Cmd_59(),UpdateNameSpace(),ChkLENOVOPMFW(),ChkGoTarget(),AdapterOut(),and Oem_SysPowerContrl()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void BATT_LEN_OFF(void)
{
	if(IS_MASK_SET(CHGIC_WriteCmd0x12L,BatLearnEnable)) //COKEYXU001:Add for S3 cannot wake from PS2 KBD. 
	{
		CLEAR_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable); 
   	 	if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) 
   		{
			CHGIC_SMbusFailCnt++;
			RamDebug(0x13);
  		}
	}
}


/*
*******************************************************************************
* Function name: BATT_LEN_ON       
*
* Descriptoin: Battery Learn mode on.
*
* Invoked by: Cmd_59(),UpdateNameSpace(),ChkGoTarget()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void BATT_LEN_ON(void)
{
	if(IS_MASK_CLEAR(CHGIC_WriteCmd0x12L,BatLearnEnable)) //COKEYXU001:Add for S3 cannot wake from PS2 KBD.
	{
 		SET_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable); 
		if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) 
   		{
			CHGIC_SMbusFailCnt++;
			RamDebug(0x12);
   		}
	}
}

/*
*******************************************************************************
* Function name: Charger_3D_Config       
*
* Descriptoin: Config charger 3D register
*
* Invoked by: Cmd_59(),UpdateNameSpace(),ChkGoTarget()
*
* TimeDiv：NA
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Charger_3D_Config(void)
{
	CHGIC_ReadCmd0x3DL=0x39;    
	CHGIC_ReadCmd0x3DH=0x51;   //COKEYXU020: 71 to 51
    if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerProchot1, &CHGIC_ReadCmd0x3DL,SMBus_NoPEC))
    {
		CHGIC_SMbusFailCnt++;
    }
}

const SRTVTstruct code RTVTTable [] =
{
	{20,0x040E},
	{21,0x0409},
	{22,0x0404},
	{23,0x03FF},
	{24,0x03F9},
	{25,0x03F4},
	{26,0x03EE},
	{27,0x03E8},
	{28,0x03E2},
	{29,0x03DB},
	{30,0x03D5},
	{31,0x03CE},
	{32,0x03C7},
	{33,0x03C0},
	{34,0x03B9},
	{35,0x03B1},
	{36,0x03AA},
	{37,0x03A2},
	{38,0x039A},
	{39,0x0392},
	{40,0x0389},
	{41,0x0381},
	{42,0x0378},
	{43,0x0370},
	{44,0x0367},
	{45,0x035E},
	{46,0x0354},
	{47,0x034B},
	{48,0x0342},
	{49,0x0338},
	{50,0x032E},
	{51,0x0324},
	{52,0x031A},
	{53,0x0310},
	{54,0x0306},
	{55,0x02FC},
	{56,0x02F2},
	{57,0x02E7},
	{58,0x02DD},
	{59,0x02D2},
	{60,0x02C8},
	{61,0x02BD},
	{62,0x02B2},
	{63,0x02A8},
	{64,0x029D},
	{65,0x0292},
	{66,0x0287},
	{67,0x027D},
	{68,0x0272},
	{69,0x0267},
	{70,0x025D},
	{71,0x0252},
	{72,0x0247},
	{73,0x023D},
	{74,0x0232},
	{75,0x0228},
	{76,0x021E},
	{77,0x0213},
	{78,0x0209},
	{79,0x01FF},
	{80,0x01F5},
	{81,0x01EB},
	{82,0x01E1},
	{83,0x01D7},
	{84,0x01CE},
	{85,0x01C4},
	{86,0x01BB},
	{87,0x01B1},
	{88,0x01A8},
	{89,0x019F},
	{90,0x0196},
	{91,0x018E},
	{92,0x0185},
	{93,0x017C},
	{94,0x0174},
	{95,0x016C},
	{96,0x0164},
	{97,0x015C},
	{98,0x0154},
	{99,0x014C},
	{100,0x0145},
	{101,0x013D},
	{102,0x0136},
	{103,0x012F},
	{104,0x0128},
	{105,0x0121},
	{106,0x011A},

};

/*
*******************************************************************************
* Function name: PollingCPURT       
*
* Descriptoin: Polling CPU RT
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
void PollingCPURT(void)
{
	BYTE i;
	BYTE j=0;

	for (i=0;i<(sizeof(RTVTTable)/sizeof(SRTVTstruct));i++)
	{
		if(NTC_V2<RTVTTable[j].VOL)
		{
			j++;
		}
		else if(NTC_V2>=RTVTTable[j].VOL)
		{
			ThermistorCPU_TEMP=RTVTTable[j].TEM;//Change fan table follow thermal team.
			return;
		}	
	}
}


/*
*******************************************************************************
* Function name: PollingGPURT       
*
* Descriptoin: Polling GPU RT
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
void PollingGPURT(void)
{
	BYTE i;
	BYTE j=0;

	for (i=0;i<(sizeof(RTVTTable)/sizeof(SRTVTstruct));i++)
	{
		if(NTC_V1<RTVTTable[j].VOL)
		{
			j++;
		}
		else if(NTC_V1>=RTVTTable[j].VOL)
		{
			EXTVGA_TEMP=RTVTTable[j].TEM;//JERRYCRZ024:Change fan table follow thermal team.
			return;
		}		
	}
}

/*
*******************************************************************************
* Function name: Clear_Batt_First_Used_Date       
*
* Descriptoin: Clear battery first used date
*
* Invoked by: Hook_Timer500msEventC()
*
* TimeDiv：500ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Clear_Batt_First_Used_Date(void)
{
	if(IS_MASK_SET(SEL_STATE0,PRESENT_A)) 
	  {
	    	if(IS_MASK_SET(pProject0,b1uFUdayClr))
	    	{
			if(!bRWSMBus(SMbusChB,SMbusRW,SmBat_Addr,FirstUsedDate,&batteryFirstUsedDateL,SMBus_NeedPEC))
	      		{
	        		RamDebug(0xAE);
				return;
	      		}
			if(batteryFirstUsedDateH != 0x00 || batteryFirstUsedDateL != 0x00)
			{
		      		batteryFirstUsedDateH = 0x00;
			  	batteryFirstUsedDateL = 0x00;
		      		if(!bRWSMBus(SMbusChB,SMbusWW,SmBat_Addr,FirstUsedDate,&batteryFirstUsedDateL,SMBus_NeedPEC))
		      		{
		        		RamDebug(0xAD);
					return;
		      		}
			}
			else
			{
				CLEAR_MASK(pProject0,b1uFUdayClr);
			}
	    	}

	  }
}

//COKEYXU022:move Battery_Expresscharge function from OEM_MAIN. 
//COKEYXU011: S+ Add express charge method.
/*
*******************************************************************************
* Function name: Battery_Expresscharge       
*
* Descriptoin: Battery express charge
*
* Invoked by: Hook_Timer500msEventA()
*
* TimeDiv：500ms
*
* Arguments: NA   
*
* Return Values: NA
*******************************************************************************
*/
void Battery_Expresscharge(void)
{
	//check battery quick charge support.
	if(IS_MASK_CLEAR(EC_C_modeL,b4QuickChargeMode))
	{
		return;
	}
	
	if (IS_MASK_SET(OEMControl,Expresschargemode))
	{
		if(IS_MASK_CLEAR(Bat0x3ETempH,Expresscharge_mode))
		{
			//COKEYXU024:S+ only S5 support quick charge
			if(SystemIsS5)
			{
				SET_MASK(Bat0x3ETempH,Expresscharge_mode);
			}
			else
			{
				CLEAR_MASK(Bat0x3ETempH,Expresscharge_mode);
			}
			//COKEYXU024:E+
			
	 		if(bRWSMBus(SMbusChB,SMbusWW,SmBat_Addr,C_LVMfgFun2,&Bat0x3ETempL,SMBus_NeedPEC))
	 		{
	 			RamDebug(0xD1);
				RamDebug(Bat0x3ETempH);
	 		}
		}
	}
	else
	{
		if(IS_MASK_SET(Bat0x3ETempH,Expresscharge_mode))
		{
    		CLEAR_MASK(Bat0x3ETempH,Expresscharge_mode);
   			if(bRWSMBus(SMbusChB,SMbusWW,SmBat_Addr,C_LVMfgFun2,&Bat0x3ETempL,SMBus_NeedPEC))
	 		{
	 			RamDebug(0xD2);
				RamDebug(Bat0x3ETempH);
	 		}
		}
	}
}
//COKEYXU011: E+ Add express charge methodg.


