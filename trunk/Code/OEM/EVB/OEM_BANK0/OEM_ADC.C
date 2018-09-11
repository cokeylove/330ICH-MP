/*-----------------------------------------------------------------------------
 * TITLE: OEM_ADC.C
 *
 * Author : Dino
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#include <CORE_INCLUDE.H>
#include <OEM_INCLUDE.H>

//----------------------------------------------------------------------------
// The function of disabling ADC channel
//----------------------------------------------------------------------------
/* //ANGELAS073:remove start
void DisableADCChannel(void)
{
      VCH0CTL=0x1F;   // NTC_V1
	VCH1CTL=0x1F;   // NTC_V2
	VCH2CTL=0x1F;   // BATT_TEMP
	VCH3CTL=0x00;   // SD_PWR_EN#
	VCH4CTL=0x00; 	//CPU_VR_READY
	VCH5CTL=0x00;   // ADP_I
	VCH6CTL=0x00;   // B+_Track
	VCH7CTL=0x00;   // ADAPTER_ID
	CLEAR_MASK(ADCCFG, BIT0);	// Disable ADC module
}

//----------------------------------------------------------------------------
// The function of Enabling ADC channel
//----------------------------------------------------------------------------
void EnableADCChannel(void)
{
	SET_MASK(ADCCFG, BIT0);		// Enable ADC module
}
*/ //ANGELAS073:remove end
//----------------------------------------------------------------------------
// The function of scaning ADC channel
//----------------------------------------------------------------------------
void ScanADCFixChannel(void)
{
	BYTE i; //ANGELAG046: add
	LWORD Psys_SUM = 0; //ANGELAG046: add
	/*  
   	if(IS_MASK_SET(VCH0CTL, BIT7))
    	{
        	VCH0CTL |= 0x80;        // write clear data vaild flag
	 	FixData0 = ((WORD)VCH0DATM << 8) + VCH0DATL;
    	}
	*/   
   	if(IS_MASK_SET(VCH5CTL, BIT7))
    {
        VCH5CTL |= 0x80;        // write clear data vaild flag
	 	ADP_I= ((WORD)VCH5DATM << 8) + VCH5DATL;
    }
    /* Read ADC value change to read level
  	if(IS_MASK_SET(VCH4CTL, BIT7))
    {
        VCH4CTL |= 0x80;        // write clear data vaild flag
		Read_VR_CPU_PWROK= ((WORD)VCH4DATM << 8) + VCH4DATL;
    }
    */
  	/*
    	if(IS_MASK_SET(VCH6CTL, BIT7))
    	{
        	VCH6CTL |= 0x80;        // write clear data vaild flag
	 	API_ID= ((WORD)VCH6DATM << 8) + VCH6DATL;
    	}
    	*/
    // read API_ID.
	if(IS_MASK_SET(VCH6CTL, BIT7))
	{
    	VCH6CTL |= 0x80;        // write clear data vaild flag  //COKEYXU001:VCH7CTL->VCH6CTL
 	    API_ID = ((WORD)VCH6DATM << 8) + VCH6DATL; //COKEYXU001:VCH7DATM->VCH6DATM,VCH7DATL->VCH6DATL
	}
	
	//ANGELAG019: add start
	if(IS_MASK_SET(VCH7CTL, BIT7))
    {
    	VCH7CTL |= 0x80;        // write clear data vaild flag
	 	Psys= ((WORD)VCH7DATM << 8) + VCH7DATL;
    }
		//ANGELAG019: add end
		//ANGELAG028: add start
/*	if ((SystemIsS0) && (!Read_AC_IN())) //ANGELAG046: remove start
	{
		if((IS_MASK_SET(SEL_STATE0,PRESENT_A)) && (IS_MASK_CLEAR(BATTUPDATEFW,BIT0)))
		{
			if (nBattGasgauge > 10)
			{
				if (Psys >= 368) //1.08V, 56W
				{
					SET_MASK(GPU_Prochot, b1_Psys);
					SET_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else
				{
					CLR_MASK(GPU_Prochot, b1_Psys);
					CLR_MASK(Thro_Status2, b7ProCH_Psys);
				}
			}
			else
			{
				if (Psys >= 191) //0.56V, 28W
				{
					SET_MASK(GPU_Prochot, b1_Psys);
					SET_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else
				{
					CLR_MASK(GPU_Prochot, b1_Psys);
					CLR_MASK(Thro_Status2, b7ProCH_Psys);
				}
			}
		}
		
	}
	else
	{
		CLR_MASK(GPU_Prochot, b1_Psys);
		CLR_MASK(Thro_Status2, b7ProCH_Psys);
	}
	//ANGELAG028: add end
	*/ ////ANGELAG046: remove end
	//ANGELAG046: add start
	for (i=3; i>0; i--) 
	{
		Psys_Data[i] = Psys_Data[i-1];
		Psys_SUM += Psys_Data[i];
	}
	Psys_Data[0] = Psys;
	Psys_SUM += Psys_Data[0];
	Psys_AvgData = Psys_SUM / 4; //calculate 4 times average value

	psys_throttle(); //COKEYXU008: Add
	//ANGELAG046: add end
}

void ScanADCDyChannel1(void)
{
	BYTE i;
	LWORD ADP_I_SUM = 0;
	/*  
    	if( IS_MASK_SET(VCH1CTL,BIT7) )
    	{
        	VCH1CTL |= 0x80;        // write clear data vaild flag
        	// CMW 20121022 +
        	if( VCH1CTL == (ADCChannel_0&0x0F) )
        	{
			DyData0 = ((WORD)VCH1DATM << 8) + VCH1DATL;
			VCH1CTL = ADCChannel_1;
        	}
	 	else
	 	{
			TURBO_V = ((WORD)VCH1DATM << 8) + VCH1DATL;
			VCH1CTL = ADCChannel_0;
	 	}
		// CMW 20121022 -
    	}
    	if( IS_MASK_SET(VCH2CTL, BIT7) )
    	{
             VCH2CTL |= 0x80;        // write clear data vaild flag
		DyData1 = ((WORD)VCH2DATM << 8) + VCH2DATL;
    	}

    	if(IS_MASK_SET(VCH3CTL, BIT7))
    	{
        	VCH3CTL |= 0x80;        // write clear data vaild flag
		if( VCH3CTL == (ADCChannel_2&0x0F) )
		{
			DyData2 = ((WORD)VCH3DATM << 8) + VCH3DATL;
			VCH3CTL = ADCChannel_6;
		}
		else
		{
			API_ID = ((WORD)VCH3DATM << 8) + VCH3DATL;
			VCH3CTL = ADCChannel_2;
		}
    	}
    	if( IS_MASK_SET(VCH5CTL, BIT7) )
    	{
              VCH5CTL |= 0x80;        // write clear data vaild flag
		DyData1 = ((WORD)VCH5DATM << 8) + VCH5DATL;
    	}
    	*/  
    if(IS_MASK_SET(VCH0CTL, BIT7))
    {
        VCH0CTL |= 0x80;        // write clear data vaild flag
	 	NTC_V1= ((WORD)VCH0DATM << 8) + VCH0DATL;
    }
    if(IS_MASK_SET(VCH1CTL, BIT7))
    {
        VCH1CTL |= 0x80;        // write clear data vaild flag
	 	NTC_V2= ((WORD)VCH1DATM << 8) + VCH1DATL;
    }
	//ANGELAS078:remove start
    //if(IS_MASK_SET(VCH2CTL, BIT7))
    //{
       	//VCH2CTL |= 0x80;        // write clear data vaild flag
	 	//BATTEMP= ((WORD)VCH2DATM << 8) + VCH2DATL;
    //}
	//ANGELAS078:remove end

	//MEILING001:S- remove EC_SD_PWR_EN
    /*if(IS_MASK_SET(VCH3CTL, BIT7))
    	{
        	VCH3CTL |= 0x80;        // write clear data vaild flag
	 	SD_PWR_EN= ((WORD)VCH3DATM << 8) + VCH3DATL;
    	}*/
    //MEILING001:E-
    
    if (SysPowState == SYSTEM_S0&& Read_AC_IN())//MEILING001:add ACIN condition.
    {
		//Chk_ACOP();
		ADPI2Sec++;
		for (i=3; i>0; i--)
		{
			ADPI_Data[i] = ADPI_Data[i-1];
			ADP_I_SUM += ADPI_Data[i];
		}
		ADPI_Data[0] = ADP_I;
		ADP_I_SUM += ADPI_Data[0];
		ADPI_AvgData = ADP_I_SUM / 4;
   //ANGELAG019: remove start
     /*   if (!(ADPI2Sec%10))
        {
            //if(AdapterID==AdapterID_65W) // add protect poin for 65w and 45w adpter //ANGELA075:ADD  //MEILING001:remove
            if(uMBGPU == 0x02) //juged by uMBGPU(0x01:UMA(45W), 0x02:DIS(65W)).  //MEILING001:add.
            {
                if(ADPI_AvgData>ECprochot_65W)
                {
                    SET_MASK(Thro_Status2, b7ProCH_ADP);
                }
				else 
			    {
			        if(IS_MASK_SET(Thro_Status2, b7ProCH_ADP))
			        {
			         	CLR_MASK(Thro_Status2, b7ProCH_ADP);
			        }
			    }
            }
			//else if(AdapterID==AdapterID_45W) //// add protect poin for 65w and 45w adpter //ANGELAS075:ADD //MEILING001:remove.
			else if(uMBGPU == 0x01)//MEILING001:add.
			{
             	if(ADPI_AvgData>ECprochot_45W) //ANGELAS075: add
                {
                    SET_MASK(Thro_Status2, b7ProCH_ADP);
                }
				else 
			    {
			        if(IS_MASK_SET(Thro_Status2, b7ProCH_ADP))
			        {
			         	CLR_MASK(Thro_Status2, b7ProCH_ADP);
			        }
			    }
			}
        }*/ //ANGELAG019: remove end
        //ANGELAG046: add start
        
        if(IS_MASK_CLEAR(SEL_STATE0, PRESENT_A) || (IS_MASK_SET(SEL_STATE0, PRESENT_A) && (nBattGasgauge <= 10))) //COKEYXU043: AC or AC+DC(RSOC<10%)
        {                                                                                                         
            if(uMBGPU == 0x02) //juged by uMBGPU(0x01:UMA(NA), 0x02:DIS(135W)).
            {
                	if(ADPI_AvgData>ECprochot_132W)    //COKEYXU041: change to ECprochot_132W because change AC adapter to 135W.
               	 	{
						SET_MASK(GPU_Prochot, b5_adapter);
                  	    SET_MASK(Thro_Status4, b0ProCH_ADP);
               		}
					else 
			   		{
						CLR_MASK(GPU_Prochot, b5_adapter);
		         		CLR_MASK(Thro_Status4, b0ProCH_ADP);
			    	}
					
            }
			else if(uMBGPU == 0x01)
			{
             	if(ADPI_AvgData>ECprochot_45W)
                {
					SET_MASK(GPU_Prochot, b5_adapter);
                    SET_MASK(Thro_Status4, b0ProCH_ADP);
                }
				else 
			    {
					CLR_MASK(GPU_Prochot, b5_adapter);
		         	CLR_MASK(Thro_Status4, b0ProCH_ADP);
			    }
			}
        }
		else
		{
			CLR_MASK(GPU_Prochot, b5_adapter);
			CLR_MASK(Thro_Status4, b0ProCH_ADP);

			if(uMBGPU == 0x02) //COKEYXU030: add when system consumption > 82W disable CPU turbo.
			{
				if(ADPI_AvgData>ECprochot_82W)   
				{
					SET_MASK(Adapter90WWA,b1_DisCPUturbo);
				}
				else if(ADPI_AvgData<ECprochot_78W)
				{
					CLEAR_MASK(Adapter90WWA,b1_DisCPUturbo);
				}
			}	
		}
		//ANGELAG046: add end
		
		if (ADPI2Sec > 200)
		{
			if (IS_MASK_SET(SEL_STATE0, PRESENT_A))
			{
				//Chk_Hybrid_STPP();  //ANGELAS047:remove
				Chk_HybridFORBQ24780_STPP(); //ANGELAS047:add
			}
			else
			{
				//Chk_AC_STPP(); //ANGELAG019: remove
			}
			ADPI2Sec=0;
		}
	}
}

//----------------------------------------------------------------------------
// Init ADC module
//----------------------------------------------------------------------------
void Init_ADC(void)
{
	VCH0CTL=ADCChannel_0;   // NTC_V1
	VCH1CTL=ADCChannel_1;   // NTC_V2
	//VCH2CTL=ADCChannel_2;   // BATT_TEMP //ANGELAS078:remove
	//VCH3CTL=0x90;   // SD_PWR_EN#  //MEILING001:remove.
	//VCH4CTL=0x90;	//CPU_VR_READY
	VCH5CTL=0x90;   // ADP_I (enable voltage channel for channel 5)
	VCH6CTL=0x90;   // ADAPTER_ID_R
	//VCH7CTL=0x90;   // ADAPTER_ID  //MEILING001:remove.
	VCH7CTL=0x90;   //ANGELAG019: add
}

// CMW 20121022 +
//---------------------------------------------------------------------------
// Init Voltage Compare Function
// Input: 1 Group. choice the init VC channel  0: VC0 1: VC1 2:VC2 3:All VC Channel
//---------------------------------------------------------------------------
void Init_VC(BYTE Group)
{
	int tGroup = (Group&0x03);
	/*//ANGELAS073:remove start
	#if Support_VC0_Function
	// Set the CPU Thermal VC Setting
	if( (tGroup == 0x00) || (tGroup == 0x03) )
	{
		VCMP0CTL &= 0x3F;	// Disable VC0 & Interrupt.
		CMP0THRDATM = (VC0TriggerValue>>8);		// threshold value buffer
		CMP0THRDATL = VC0TriggerValue;
		VCMP0CTL &= 0xF8;	// Clear ADC Channel & set to Channel 0.
		VCMP0CTL &= 0xDF;	// Set the Less Then Voltage trigger.
		VCMP0CTL &= 0xF7;	// Set Active-Low. //T18 change Active Low to Active high
		GCR15 |= 0x01;		// Enable GPJ3 to be VC1 GPO.
		GPCRJ3 = 0x00;
		VCMP0CTL |= 0x40;	// Enable Interrupt.
		VCMP0CTL |= 0x80;
	}
	#endif

	#if Support_VC1_Function
	// Set the ADP_I VC setting
	if( (tGroup == 0x01) || (tGroup == 0x03) )
	{
		VCMP1CTL &= 0x3F;	// Disable VC1 & Interrupt.
		//CMP1THRDATM = (VC1TriggerValue>>8);	// threshold value buffer
		//CMP1THRDATL = VC1TriggerValue;
		SetTurbo_V_Thro();	// Set Turbo_V trigger point.
		VCMP1CTL &= 0xF8;	// Clear ADC Channel
		VCMP1CTL |= 0x05;	 //T051A change 0x01 to 0x05   // Connect to ADC Channel 1.  
		VCMP1CTL |= 0x20;	// Set the Over Voltage trigger.
		VCMP1CTL &= 0xF7;	// Set Active-High.
		GCR15 |= 0x02;		// Enable GPJ4 to be VC1 GPO.
		//GPCRJ4 = 0x00;    
		VCMP1CTL |= 0x40;	// Enable Interrupt.
		VCMP1CTL |= 0x80;	// Enable VC1
	}
	#endif

	#if Support_VC2_Function
	// Set the xxx VC setting.
	if( (tGroup == 0x02) || (tGroup == 0x03) )
	{
	}
	#endif
	*///ANGELAS073:remove start
	if( (VCMP0CTL&0x80) || (VCMP1CTL&0x80) || (VCMP2CTL&0x80) )
	{
		IER18 |= 0x80;
	}
}
// CMW 20121022 -

/*****************************************************************************/
// Procedure: Lenovo_CPU_auto_mode								TimeDiv: 1Sec
// Description: CPU current less than 7A in 20 seconds,
//				 set the max CPU processor rate to 70.
// GPIO:
// Referrals: Lenovo CPU auto mode start from here
// 				1.23V / 3.00V = 41%
// 				1023 * 41% = 419.43 => 0x01A3
// 				(7A/76A)*419 = 38.5 => 0x27
// 				(3.5A/76A)*419=19.2 => 0x13
// 				Readlly test result voltage: 7A=>0x26 3.5A=>0x13
/*****************************************************************************/
/* //ANGELAS073:remove start
void Lenovo_CPU_auto_mode(void)
{
	#if Support_CPU_Auto
	if ( SystemNotS0 )	{ return; }

	ITempB01 = Auto_mode_EnPtr & 0x1F;			// mask enable bit => counter
	// 1. read ADC data
	//Auto_mode[ITempB01] = DyData2;
	//Mos: Get CPU IMon
	Auto_mode[ITempB01] = DyData1;

	if (ITempB01 == 19)
	{
		// 2. avg
		ITempW01 = 0;
		for( ITempB02=0; ITempB02<20; ITempB02++)
		{ ITempW01 += Auto_mode[ITempB02]; }
		Auto_mode_AVG = ITempW01/20;

		if( IS_MASK_SET(LENOVOBATT,CPU_AUTO_MODE) && !Read_AC_IN() &&
			(IS_MASK_SET(SYS_MISC1, ACPI_OS)) )	// check function enable
		{
			// 3. check statuse
			if( Auto_mode_AVG <= 0x002E )	// Imon < 7A
			{	// Set 70%.
				if ( IS_MASK_CLEAR(StatusKeeper, b3CPU_Auto) )
				{
					SET_MASK(StatusKeeper,b3CPU_Auto);
					SET_MASK(StatusKeeper,b2CPU_Auto70);
					CLR_MASK(Auto_mode_EnPtr,BIT6);
					//ECQEvent(AUTO_MODE_ON_EVENT);		// notify host on
		       		SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Off);
		        	       SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Change);
					uVPCeventSource = General;
					uVPCeventSource2 = 0;
					ECSMI_SCIEvent(SDV_VPC_notify);
				}
				else
				{
					if ( Auto_mode_AVG > 0x0016 )	// Imon > 3.5A
					{
						if ( IS_MASK_SET(Auto_mode_EnPtr,BIT6) )
						{
							CLR_MASK(StatusKeeper,b3CPU_Auto);
							CLR_MASK(StatusKeeper,b2CPU_Auto70);
						}
					}
					else
					{
						if ( IS_MASK_CLEAR(StatusKeeper,b2CPU_Auto70) )
						{
							SET_MASK(StatusKeeper,b3CPU_Auto);
							SET_MASK(StatusKeeper,b2CPU_Auto70);
							CLR_MASK(Auto_mode_EnPtr,BIT6);
							//ECQEvent(AUTO_MODE_OFF_EVENT);		// send ad disable for host
				            SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Off);
							SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Change);
						    uVPCeventSource = General;
						    uVPCeventSource2 = 0;
						    ECSMI_SCIEvent(SDV_VPC_notify);
						}
					}
				}
			}
			else
			{	// Set 100%
				if ( IS_MASK_CLEAR(Auto_mode_EnPtr,BIT6) )
				{
					SET_MASK(StatusKeeper,b3CPU_Auto);
					CLR_MASK(StatusKeeper,b2CPU_Auto70);
					SET_MASK(Auto_mode_EnPtr,BIT6);
					//ECQEvent(AUTO_MODE_OFF_EVENT);	// notify host off
			        CLR_MASK(LENOVOPMFW_Temp,Auto_Mode_Off);
			        SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Change);
					uVPCeventSource = General;
					uVPCeventSource2 = 0;
					ECSMI_SCIEvent(SDV_VPC_notify);
				}
			}
		}
		else
		{	// check function disable
			if( (IS_MASK_SET(Auto_mode_EnPtr, BIT6)) || (IS_MASK_SET(LENOVOPMFW_Temp,Auto_Mode_Off)) )	// check before disable CPU auto mode already On event
			{
				CLR_MASK(StatusKeeper,b3CPU_Auto);
				CLR_MASK(StatusKeeper,b2CPU_Auto70);
				CLR_MASK(Auto_mode_EnPtr,BIT6);
				//ECQEvent(AUTO_MODE_OFF_EVENT);		// send ad disable for host
	            CLR_MASK(LENOVOPMFW_Temp,Auto_Mode_Off);
				SET_MASK(LENOVOPMFW_Temp,Auto_Mode_Change);
			    uVPCeventSource = General;
			    uVPCeventSource2 = 0;
			    ECSMI_SCIEvent(SDV_VPC_notify);
			}
		}
	}

	// 4. check counter 20 seconds
	ITempB01++;
	if( ITempB01 >= 20 )
	{ ITempB01=0;  }
	Auto_mode_EnPtr = (Auto_mode_EnPtr&0xE0) | ITempB01;
	#endif	// Support_CPU_Auto
}

void Chk_Wrong_ADP(void)
{
	if( SysPowState == SYSTEM_S0 )
	{

		if((ACIN_FallINT_Count > 0) && (Chk_Wrong_ADP_Status == 0))
		{
			Chk_Wrong_ADP_Status = Chk_Wrong_ADP_Status_wait_2sec;
		}

		switch(Chk_Wrong_ADP_Status)
		{
		case Chk_Wrong_ADP_Status_wait_2sec:
			Chk_Wrong_10ms_Count++;
			if (Chk_Wrong_10ms_Count  > 200)
			{
				CLEAR_MASK(ACOFF_SOURCE, ADPOVP);
				//ACOFF_LOW();//JERRYCRZ030?那oremove AC_OFF function follow charge IC bq24780. 
			BATT_LEN_OFF();//JERRYCRZ030?那o
				cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
				//CHGIC_ptr = 2;						//reminder: call to write to SmartChg
				//WriteSmartChgIC();
				Chk_Wrong_10ms_Count = 0;		//
				ACIN_FallINT_Count = 0;
				Chk_Wrong_ADP_Status = Chk_Wrong_ADP_Status_wait_10sec;
			}
			else
			{
				cBattFlag0.fbit.cCmdAcOff=1; 			//Stop charger
				//CHGIC_ptr = 2;						//reminder: call to write to SmartChg
				//WriteSmartChgIC();
				SET_MASK(ACOFF_SOURCE, ADPOVP);
				//ACOFF_HI();
				BATT_LEN_ON();//JERRYCRZ030:+remove AC_OFF function follow charge IC bq24780. 
			}
			break;
		case Chk_Wrong_ADP_Status_wait_10sec:
			if (Chk_Wrong_10ms_Count > 1000)	//Over 10s not detect 5 times ACIN drop.
			{
				Chk_Wrong_10ms_Count = 0; 		//Reset all counter
				ACIN_FallINT_Count = 0;
				Chk_Wrong_ADP_Status=0; 			//Reset Status
			}
			else
			{
				if (ACIN_FallINT_Count >= 5)			//falling 5 times
				{
					Chk_Wrong_ADP_Status = Chk_Wrong_ADP_Status_ACOFF_Lock;
					ACIN_FallINT_Count = 0;
					Chk_Wrong_10ms_Count = 0;
					cBattFlag0.fbit.cCmdAcOff=1; 		//Stop charger 
					//CHGIC_ptr = 2;					//reminder: call to write to SmartChg
					//WriteSmartChgIC();
					SET_MASK(ACOFF_SOURCE, ADPOVP);
					//ACOFF_HI();	//JERRYCRZ030?那oremove AC_OFF function follow charge IC bq24780. 
					BATT_LEN_ON();//JERRYCRZ030+remove AC_OFF function follow charge IC bq24780. 

				}
				Chk_Wrong_10ms_Count++;
			}
			break;
		case Chk_Wrong_ADP_Status_ACOFF_Lock:
			//Reset AC OFF Lock after Plug out ACIN 20ms
			if (!Read_AC_IN())
			{
				if (Chk_Wrong_10ms_Count >2)
				{
					Chk_Wrong_ADP_Status = 0; 			//Reset Status and counter
					Chk_Wrong_10ms_Count = 0;
					ACIN_FallINT_Count =0;
					CLEAR_MASK(ACOFF_SOURCE, ADPOVP);
					//ACOFF_LOW();//JERRYCRZ030?那oremove AC_OFF function follow charge IC bq24780. 
			BATT_LEN_OFF();//JERRYCRZ030?那o
					cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
					//CHGIC_ptr = 2;						//reminder: call to write to SmartChg
					//WriteSmartChgIC();
				}
				Chk_Wrong_10ms_Count ++;
			}
			break;
		default:
			Chk_Wrong_ADP_Status = 0;
			Chk_Wrong_10ms_Count = 0;
			ACIN_FallINT_Count = 0;
			break;
		}
	}
	else
	{
		Chk_Wrong_ADP_Status = 0;
		Chk_Wrong_10ms_Count = 0;
		ACIN_FallINT_Count = 0;
	}
}
*/ //ANGELAS073:remove end
/* Mos: The memory used in Chk_ACOP function.
typedef enum
{
	Chk_ACOP_Status_wait_10sec,
	Chk_ACOP_Status_ACOFF_Lock,
}Chk_ACOP_Status_Type;
extern XBYTE	Chk_ACOP_Status;					//_at_(ECRAM1+0x94);
extern XWORD Chk_ACOP_10ms_Count;				//_at_(ECRAM1+0x95);*/
/* //ANGELAS073:remove start
void Chk_ACOP(void)
{
	//Mos: Does not check ACOP when TurboBoost enable
	if (IS_MASK_SET(CHGIC_ReadCmd0x37L, TurboBoost)) //ANGELAS016:Modify charge IC option setting. 12 to 37
	{
		return;
	}

	if (IS_MASK_SET(nNowCurrentH, BIT7))
	{
		Chk_ACOP_Bat_Chg_Current = 0xFFFF - (WORD)((nNowCurrentH << 8) + nNowCurrentL);//mA
	}
	else
	{
		Chk_ACOP_Bat_Chg_Current = 0;
	}

	//Mos: When AC_IN and Battery is discharging, the current > 25mA
	if (IS_MASK_SET(nNowCurrentH, BIT7)) //Mos: Battery is discharging
	{
		if(Read_AC_IN() && (Chk_ACOP_Status == 0) && Chk_ACOP_Bat_Chg_Current > 25)
		{
			Chk_ACOP_Status = Chk_ACOP_Status_wait_10sec;
		}
	}

	switch(Chk_ACOP_Status)
	{
		case Chk_ACOP_Status_wait_10sec:
			Chk_ACOP_10ms_Count++;
			if (Chk_ACOP_10ms_Count >= 1000) 				//ACOP over 10s
			{
				Chk_ACOP_Status = Chk_ACOP_Status_ACOFF_Lock;
				Chk_ACOP_10ms_Count = 0;
			}
			else
			{
				if (!(Read_AC_IN() && (Chk_ACOP_Bat_Chg_Current < 25)))
				{
					Chk_ACOP_Status = 0;
				}
			}
			break;
		case Chk_ACOP_Status_ACOFF_Lock:
			if (!Read_AC_IN())
			{
				Chk_ACOP_10ms_Count++;
				if (Chk_ACOP_10ms_Count >= 20) 					//ACOP over 200ms
				{
					Chk_ACOP_Status = Chk_ACOP_Status_Null;
					Chk_ACOP_10ms_Count = 0;
					CLEAR_MASK(ACOFF_SOURCE, ACOP);
					//ACOFF_LOW();//JERRYCRZ030?那oremove AC_OFF function follow charge IC bq24780. 
			BATT_LEN_OFF();//JERRYCRZ030?那o
				}
			}
			else
			{
				SET_MASK(ACOFF_SOURCE, ACOP);
				//ACOFF_HI();//JERRYCRZ030?那oremove AC_OFF function follow charge IC bq24780. 
				BATT_LEN_ON();//JERRYCRZ030+remove AC_OFF function follow charge IC bq24780. 
			}
			break;
		default:
			Chk_ACOP_Status = 0;
			Chk_ACOP_10ms_Count = 0;
			break;
	}
}

void Chk_Hybrid_STPP(void)
{
	LWORD BattPresentVolt;
	LWORD BattNowCurrent;
	//LWORD Batt_Output_Power;   //T069-
       BYTE  Batt_Output_Power;   // (unit: W)
	//if(Read_AC_IN() && (nBattGasgauge > Chk_Hybrid_STPP_min_BattGasgauge))  //Check Battery over 20%
       if(Read_AC_IN() && (nBattGasgauge >= Chk_Hybrid_STPP_min_BattGasgauge))    
       
	{
	SET_MASK(CHGIC_WriteCmd0x37L, TurboBoost); //ANGELAS016:Modify charge IC option setting. CHGIC_ReadCmd0x12L to CHGIC_WriteCmd0x37L
		switch(Chk_Hybrid_STPP_Status)
		{
			case Chk_Hybrid_STPP_Status_CP:
				if (ADPI_AvgData > TurboBoostCP)
				{
					//Mos: Stop charger
					SET_MASK(nStopChgStat3H, HybridModeStopChg);
					cBattFlag0.fbit.cCmdAcOff=1; 			//Stop Charger
					SET_MASK(nStopChgStat3H, ENADPTHROTTLING);
                                 if(nBattGasgauge >(Chk_Hybrid_STPP_min_BattGasgauge-5))    
                                { 
					Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_Charger_Turbo;
                                }
                                else
                                {
                                		//Mos: Clean all flag and status
		                            Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CP;
		                            CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
		                            cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
		                            CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);
		                            //CLEAR_MASK(CHGIC_ReadCmd0x37L, TurboBoost); //REMOVE DISABLE TURBOBOOST CONTROL POINT
		                            //Mos: Consider Battery gasguage less then 20%
		                            if (ADPI_AvgData > ACModeSTPPEn)
		                            {
		                            	SET_MASK(BatteryAlarm,HybridSTPP);
		                            	UpBatteryPState();
		                            }
		                            if (ADPI_AvgData < ACModeSTPPDis)
		                            {
		                            	CLR_MASK(BatteryAlarm,HybridSTPP);
		                            }
                                    
                                }                             
				}
				//else 
				else if(IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))  
				{
					CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
					cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
					CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);
					//CLEAR_MASK(CHGIC_ReadCmd0x37L, TurboBoost);//G39:REMOVE DISABLE TURBOBOOST CONTROL POINT
				}
				break;
			case Chk_Hybrid_STPP_Status_Charger_Turbo:
				if (ADPI_AvgData > TurboBoostEn)
				{
					//Mos: TurboBoost enable
					SET_MASK(CHGIC_WriteCmd0x37L, TurboBoost); //ANGELAS016:Modify charge IC option setting. CHGIC_ReadCmd0x12L to CHGIC_WriteCmd0x37L
					Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_Disable_CPUTurbo;
					BattPresentVolt = (WORD)((nPresentVoltH << 8) + nPresentVoltL);
					//Mos: Assume battrey discharge voltage is 10v, limit to 20W output, so limit current at 2A
					Chk_Hybrid_STPP_Turboboost_Battery_Current_limit = 0x0800;//2048mA
				}
				//else if (ADPI_AvgData < TurboBoostDis) 
				else if((ADPI_AvgData < TurboBoostDis)||(nBattGasgauge < (Chk_Hybrid_STPP_min_BattGasgauge-9)))  //T051A+//G32:change disableTurboBoost capacity from 40 to 36
				{
					//Mos: TurboBoost disable
					//CLEAR_MASK(CHGIC_ReadCmd0x37L, TurboBoost); //G39:REMOVE DISABLE TURBOBOOST CONTROL POINT
					//Mos: Charger resume
					CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
					cBattFlag0.fbit.cCmdAcOff=0;
					CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);
					Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CP;
					//Mos: Set PL2 != PL1, enable CPU turbo.
					PECI_SETPL2Watts(2);
				}
				break;
			case Chk_Hybrid_STPP_Status_Disable_CPUTurbo:
				BattPresentVolt = (WORD)((nPresentVoltH << 8) + nPresentVoltL);//mV
				if (IS_MASK_SET(nNowCurrentH, BIT7))
				{
					BattNowCurrent = 0xFFFF - (WORD)((nNowCurrentH << 8) + nNowCurrentL);//mA
				}
				else
				{
					BattNowCurrent = 0;//(WORD)((nNowCurrentH << 8) + nNowCurrentL);//mA
				}
				Batt_Output_Power = (BattPresentVolt * BattNowCurrent);
				//Batt_Output_Power / 1000000 / 20 / IOUT / (3 / 1024)
				// = Batt_Output_Power / 292968.75
				Batt_Output_Power=(BYTE) ((BattPresentVolt*0.1/100) *( BattNowCurrent*0.1/100)); 
				//Chk_Hybrid_STPP_Batt_Output_Power = Batt_Output_Power / 292968;                   
				Chk_Hybrid_STPP_Batt_Output_Power =(LWORD)( Batt_Output_Power*1000000 /292968.75);  

				//If enable TurboBoost, But Power consumption still reach TurboBoostEn point.
				//if (ADPI_AvgData > TurboBoostEn)
				if ((TurboBoostCP + Chk_Hybrid_STPP_Batt_Output_Power)
					> (TurboBoostEn + Chk_Hybrid_STPP_Batt_Output_Power_Limit))
				{
					//Mos: Set PL2 = PL1, disable CPU turbo.
					PECI_SETPL2Watts(1);
                                   SET_MASK(Thro_Status, b3Turbo_CPU);
	                            cTHERMThrottling &= 0xF0;	// Clear thermal throttling status.
		                     cTHERMThrottling |= 0x01;	// Set P-State level                
					Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CPU_throttling;
				}
				//else if (ADPI_AvgData < TurboBoostDis) 
				else if((ADPI_AvgData < TurboBoostDis)||(nBattGasgauge < (Chk_Hybrid_STPP_min_BattGasgauge-5)))  //T051A+
				{
					//Mos: TurboBoost disable
					//CLEAR_MASK(CHGIC_ReadCmd0x37L, TurboBoost);//REMOVE DISABLE TURBOBOOST CONTROL POINT
					//Mos: Charger resume
					CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
					cBattFlag0.fbit.cCmdAcOff=0;
					CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);
					Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CP;
					//Mos: Set PL2 != PL1, enable CPU turbo.
					PECI_SETPL2Watts(2);
                                   //T051B- SET_MASK(Thro_Status, b3Turbo_CPU);
                                   CLEAR_MASK(Thro_Status, b3Turbo_CPU); //T051B+
	                            cTHERMThrottling &= 0xF0;	// Clear thermal throttling status.
		                     // cTHERMThrottling |= 0x01;	// Set P-State level                      
				}
				break;
			case Chk_Hybrid_STPP_Status_CPU_throttling:
				BattPresentVolt = (WORD)((nPresentVoltH << 8) + nPresentVoltL);//mV
				if (IS_MASK_SET(nNowCurrentH, BIT7))
				{
					BattNowCurrent = 0xFFFF - (WORD)((nNowCurrentH << 8) + nNowCurrentL);//mA
				}
				else
				{
					BattNowCurrent = 0;//(WORD)((nNowCurrentH << 8) + nNowCurrentL);//mA
				}
				Batt_Output_Power = (BattPresentVolt * BattNowCurrent);
				//Batt_Output_Power / 1000000 / 20 / IOUT / (3 / 1024)
				// = Batt_Output_Power / 292968.75
				Batt_Output_Power=(BYTE) ((BattPresentVolt*0.1/100) *( BattNowCurrent*0.1/100));  
				//Chk_Hybrid_STPP_Batt_Output_Power = Batt_Output_Power / 292968;                    
				Chk_Hybrid_STPP_Batt_Output_Power =(LWORD)( Batt_Output_Power*1000000 /292968.75);  

				//If enable TurboBoost, But Power consumption still reach TurboBoostEn point.
				//if (ADPI_AvgData > TurboBoostEn)
				if ((TurboBoostCP + Chk_Hybrid_STPP_Batt_Output_Power)
					> (TurboBoostthrottlEn + Chk_Hybrid_STPP_Batt_Output_Power_Limit))
				{
					//Mos: CPU throttling to decrease power consumption
					SET_MASK(BatteryAlarm, HybridSTPP);
					UpBatteryPState();
				}
				//else if (ADPI_AvgData < TurboBoostthrottlDis)
				//else if ((TurboBoostCP + Chk_Hybrid_STPP_Batt_Output_Power) < TurboBoostthrottlDis)
				else if((BattNowCurrent == 0) && (ADPI_AvgData < TurboBoostthrottlDis))
				{
					CLEAR_MASK(BatteryAlarm, HybridSTPP);
					//Mos: Keep DownBatteryPState in Battery1Sec();
					//DownBatteryPState();
					if (cBATTThrottling == 0)
					{
						Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_Disable_CPUTurbo;
					}
				}
				break;
			case Chk_Hybrid_STPP_Status_Recover_CPU_throttling:
				break;
			case Chk_Hybrid_STPP_Status_Recover_Charger_Turbo:
				break;
			default:
				Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CP;
				//CLEAR_MASK(CHGIC_ReadCmd0x37L, TurboBoost);// REMOVE DISABLE TURBOBOOST CONTROL POINT
		}
	}
	else
	{
		//Mos: Clean all flag and status
		Chk_Hybrid_STPP_Status = Chk_Hybrid_STPP_Status_CP;
		CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
		cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
		CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);
		if(nBattGasgauge <= (Chk_Hybrid_STPP_min_BattGasgauge-5))//change disableTurboBoost capacity from 40 to 36//G82:Change battery capacity from 39 to 40 for disable TurboBoost follow SPEC.
			{
				CLEAR_MASK(CHGIC_WriteCmd0x37L, TurboBoost); //ANGELAS016:Modify charge IC option setting. CHGIC_ReadCmd0x12L to CHGIC_WriteCmd0x37L
			}
		//Mos: Consider Battery gasguage less then 20%
		//if (ADPI_AvgData > ACModeSTPPEn)
		{
			//SET_MASK(BatteryAlarm,HybridSTPP);
			//UpBatteryPState();
		}
		//if (ADPI_AvgData < ACModeSTPPDis)
		{
		//	CLR_MASK(BatteryAlarm,HybridSTPP);
		} //Remove battery mode power on CPU keep MAX P_state. 
		// Add plug in AC and Battery capacity less then 40%,at the same time system power is higher than set value,let CPU keep MAX P_state.
		if(Read_AC_IN() && (ACModeSTPPEn!=0) && (nBattGasgauge <= (Chk_Hybrid_STPP_min_BattGasgauge-5)))
		{
			if (ADPI_AvgData > ACModeSTPPEn)
			{
			SET_MASK(BatteryAlarm,HybridSTPP);
			UpBatteryPState();
			}
			if (ADPI_AvgData < ACModeSTPPDis)
			{
			CLR_MASK(BatteryAlarm,HybridSTPP);
			}
		}
		else
			{
			CLR_MASK(BatteryAlarm,HybridSTPP);
			}
	}
}
*/ //ANGELAS073:remove end

//ANGELAS047:add start
void Chk_HybridFORBQ24780_STPP(void)
{
 	if(Read_AC_IN() && (nBattGasgauge > Chk_Hybrid_STPP_min_BattGasgauge))   //COKEYXU016:>=10%,change to >10%.   
	{
		SET_MASK(CHGIC_WriteCmd0x37L, TurboBoost);
	 	if(IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))  
		{
			cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
 		}
		//MEILING022:S- Remove MEILING012.
		//MEILING012:S+ Add adapter current protect function.
		/*if (ADPI_AvgData > TurboBoostCP)
		{
			SET_MASK(nStopChgStat3H, HybridModeStopChg);
			cBattFlag0.fbit.cCmdAcOff=1; 			//Stop Charger
			SET_MASK(nStopChgStat3H, ENADPTHROTTLING);
		}
        	else
        	{
		    CLEAR_MASK(nStopChgStat3H, HybridModeStopChg);
		    cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
		    CLEAR_MASK(nStopChgStat3H, ENADPTHROTTLING);                       
        	}*/
		//MEILING012:E+ Add adapter current protect function.
		//MEILING022:E-.
 	}
 	else
 	{
 		if(IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))   
 		{
			cBattFlag0.fbit.cCmdAcOff=0;	
 		}
		
 		if(nBattGasgauge <= Chk_Hybrid_STPP_min_BattGasgauge)//G32:change disableTurboBoost capacity from 40 to 36//G82:Change battery capacity from 39 to 40 for disable TurboBoost follow SPEC.
		{													 //COKEYXU016:Modify nBattGasgauge to <=10% for issue Battery turbo charge RSOC not stop at 10%
			CLEAR_MASK(CHGIC_WriteCmd0x37L, TurboBoost);
		}
 		if(Read_AC_IN() && (ACModeSTPPEn!=0) && (nBattGasgauge <= Chk_Hybrid_STPP_min_BattGasgauge))//COKEYXU015:Modify to <=10%
		{
			if (ADPI_AvgData > ACModeSTPPEn)
			{
				SET_MASK(BatteryAlarm,HybridSTPP);
				//UpBatteryPState();  //MEILING048:remove.//MEILING051:-Add cpu P_STATE function.
			}
			if (ADPI_AvgData < ACModeSTPPDis)
			{
				CLR_MASK(BatteryAlarm,HybridSTPP);
			}
		}
		else
		{
			CLR_MASK(BatteryAlarm,HybridSTPP);
		}
 	}
}
//ANGELAS047:add end
//ANGELAG019: remove start
/*void Chk_AC_STPP(void)
{
	if (Read_AC_IN())
	{
		if (1)
		{
			if (ADPI_AvgData > ACModeSTPPEn)
			{
				SET_MASK(BatteryAlarm,ACSTPP);
				//UpBatteryPState();  //MEILING048:remove.//MEILING051:-Add cpu P_STATE function.
			}
			if (ADPI_AvgData < ACModeSTPPDis)
			{
				CLR_MASK(BatteryAlarm,ACSTPP);
			}
		}
	}
}
*/ //ANGELAG019: remove end

/* 
void SetPowerBatteryparameter(void)
{
    //for EM9 adapter detection and protection    //0.0029296875
    if((API_ID > 0x2DE) && (API_ID <= 0x374))  // >2.149  <=2.590
    {
        AdapterID = AdapterID_170W;
    }
    else if((API_ID > 0x238) && (API_ID <= 0x2D0))  // 1.663(0x238) 2.109(0x2D0)  1.663x1024/3=0x238
    {
        AdapterID = AdapterID_135W;
    }
    else if((API_ID > 0x190) && (API_ID <= 0x228)) //should change to >1.172(0x190) <=1.618(0x228) for 90W  ">0.693(0xEC) <=1.134(0x183) for 65W ",  //0.0029296875
    {
        AdapterID = AdapterID_90W;
    }
    else if ((API_ID > 0xEC) && (API_ID <= 0x183))//0.693<API_ID<1.134
    {
        AdapterID = AdapterID_65W;
    }
    else if ((API_ID > 0x50) && (API_ID <= 0xE2))//0.234<API_ID<0.663
    {
        AdapterID = AdapterID_45W;
    }
    else if((API_ID > 0x380) || (API_ID <= 0x46)) // >2.626 or <0.207
    {
        AdapterID = AdapterID_NON_SUPPORT;
    }
    //if(IS_MASK_SET(pProject0,b4VGAType))
    if(uMBGPU & 0x02)
    {
        if(AdapterID == AdapterID_65W || AdapterID == AdapterID_90W || AdapterID == AdapterID_135W || AdapterID == AdapterID_170W)
        {
            CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);
            CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
            //ANGELAS075 remove start
            TurboBoostCP = A65WCP;
            TurboBoostEn = A65Wchargerturbo;
            TurboBoostthrottlEn = A65Wthrottling;
            TurboBoostthrottlDis = A65Wthrottlingrecove;
            TurboBoostDis = A65Wturborecover;

            ACModeSTPPEn = A65Wthrottling; //62w
            ACModeSTPPDis = A65Wthrottlingrecove;  // 58W //55w
            //ANGELAS075 remove end
            Chk_Hybrid_STPP_min_BattGasgauge = 45;  //T051A: 20 to 45
            //Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024) //ANGELAS075 remove

            //Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            //Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }
        else if(AdapterID == AdapterID_45W)                 //120W -> 102w
        {
            SET_MASK(SYS_STATUS, b5UnSuitAdpPow); //Modify pop up Adapter has limited support when plug in 45W adapter to 65W supported units for ME.
            //CLEAR_MASK(SYS_STATUS,b5UnSuitAdpPow);
            CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
            //ANGELAS075 remove start
            TurboBoostCP = A45WCP;
            TurboBoostEn = A45Wchargerturbo;
            TurboBoostthrottlEn = A45Wthrottling;
            TurboBoostthrottlDis = A45Wthrottlingrecove;
            TurboBoostDis = A45Wturborecover;

            ACModeSTPPEn = A45Wthrottling;
            ACModeSTPPDis = A45Wthrottlingrecove;
            //ANGELAS075 remove end
            Chk_Hybrid_STPP_min_BattGasgauge = 45;  //20 to 45
            //Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)  //ANGELAS075 remove

            //Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            //Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }
        else if(AdapterID == AdapterID_NON_SUPPORT)
        {
            SET_MASK(SYS_STATUS, b4IllegalAdp);
            CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);

            if(SystemIsS0)
            {
                // CLEAR_MASK(SYS_STATUS,AC_ADP);   //Clear AC in  flag
                // ACOFF_HI();
                //ADPIDON10MS_NUM=0xFF;                    //     change From 0x0A to 0xFF
                // ACOFF_HI();         //T078+
                BATT_LEN_ON();//JERRYCRZ030+remove AC_OFF function follow charge IC bq24780.
                //ECSMI_SCIEvent(ACPI_ACOUT_SCI);
            }

            // Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            // Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }

    }
    // if(IS_MASK_CLEAR(pProject0,b4VGAType))
    if(uMBGPU & 0x01)
    {
        if(AdapterID == AdapterID_65W || AdapterID == AdapterID_90W || AdapterID == AdapterID_135W || AdapterID == AdapterID_170W)
        {
            CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);
            CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
            //ANGELAS075 remove start
            TurboBoostCP = A65WCP;
            TurboBoostEn = A65Wchargerturbo;
            TurboBoostthrottlEn = A65Wthrottling;
            TurboBoostthrottlDis = A65Wthrottlingrecove;
            TurboBoostDis = A65Wturborecover;

            ACModeSTPPEn = A65Wthrottling; //62w
            ACModeSTPPDis = A65Wthrottlingrecove;  // 58W //55w
            //ANGELAS075 remove
            Chk_Hybrid_STPP_min_BattGasgauge = 45;  // 20 to 45
            //Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)  //ANGELAS075 remove

            //Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            //Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }
        else if(AdapterID == AdapterID_45W)                 //120W -> 102w
        {
            //SET_MASK(SYS_STATUS,b5UnSuitAdpPow);// Adapter power is  suitable for ME.
            CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);
            CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
            //ANGELAS075 remove start
            TurboBoostCP = A45WCP;
            TurboBoostEn = A45Wchargerturbo;
            TurboBoostthrottlEn = A45Wthrottling;
            TurboBoostthrottlDis = A45Wthrottlingrecove;
            TurboBoostDis = A45Wturborecover;

            ACModeSTPPEn = A45Wthrottling;
            ACModeSTPPDis = A45Wthrottlingrecove;
            //ANGELAS075 remove end
            Chk_Hybrid_STPP_min_BattGasgauge = 45;  // 20 to 45
            //Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)  //ANGELAS075 remove

            // Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            // Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }
        else if(AdapterID == AdapterID_NON_SUPPORT)
        {
            SET_MASK(SYS_STATUS, b4IllegalAdp);
            CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);

            if(SystemIsS0)
            {
                // CLEAR_MASK(SYS_STATUS,AC_ADP);   //Clear AC in  flag
                // ACOFF_HI();
                //ADPIDON10MS_NUM=0xFF;                              //  change From 0x0A to 0xFF
                // ACOFF_HI();
                BATT_LEN_ON();//JERRYCRZ030+remove AC_OFF function follow charge IC bq24780.
                //ECSMI_SCIEvent(ACPI_ACOUT_SCI);
            }

            //Chk_Trickle_Current_Now_Current = 700;//700mA  //ANGELAS075 remove
            // Chk_Trickle_Current_Chg_Current = 500;//500mA  //ANGELAS075 remove
        }
    }

    //SetTurbo_V_Thro();
    //
    if(IS_MASK_CLEAR(SYS_STATUS, b4IllegalAdp))
    {
        ACOFF_LOW();
    }
    //
    //for EM9 adapter detection and protection


    //
    if ( SLI_Status == 0x44 )
    {
        AdapterID = AdapterID_170W;
    }
    else
    {
        if (API_ID >=  0x2FF) //martin0205 should change to >1.172 <=1.618 for 90W  ">0.693 <=1.134 for 65W ",
        {
            AdapterID = AdapterID_90W;
        }
        else if ((API_ID > 0xFF) && (API_ID < 0x2FF))
        {
            AdapterID = AdapterID_120W;
        }
        else if (API_ID < 0x7F && API_ID > 0x00)
        {
            AdapterID = AdapterID_170W;
        }
        else
        {
            AdapterID = AdapterID_NON_SUPPORT;
        }
    }

    if (AdapterID == AdapterID_90W || AdapterID == AdapterID_NON_SUPPORT)                       //95W -> 75w
    {
        TurboBoostCP = A90WCP;
        TurboBoostEn = A90Wchargerturbo;
        TurboBoostthrottlEn = A90Wthrottling;
        TurboBoostthrottlDis = A90Wthrottlingrecove;
        TurboBoostDis = A90Wturborecover;

        ACModeSTPPEn = A90WACmodethrottling;
        ACModeSTPPDis = A90WACmodethrottlingrecove;
        Chk_Hybrid_STPP_min_BattGasgauge = 20;
        Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)

        Chk_Trickle_Current_Now_Current = 700;//700mA
        Chk_Trickle_Current_Chg_Current = 500;//500mA
    }
    else if (AdapterID == AdapterID_120W)                   //120W -> 102w
    {
        TurboBoostCP = A120WCP;
        TurboBoostEn = A120Wchargerturbo;
        TurboBoostthrottlEn = A120Wthrottling;
        TurboBoostthrottlDis = A120Wthrottlingrecove;
        TurboBoostDis = A120Wturborecover;

        ACModeSTPPEn = A120WACmodethrottling;
        ACModeSTPPDis = A120WACmodethrottlingrecove;
        Chk_Hybrid_STPP_min_BattGasgauge = 20;
        Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)

        Chk_Trickle_Current_Now_Current = 700;//700mA
        Chk_Trickle_Current_Chg_Current = 500;//500mA
    }
    else if (AdapterID == AdapterID_170W)                   //170W -> 145w
    {
        AdpID170WAmode();
    }
}
*/


/*//ANGELAS073:remove start
void AdpID170WAmode(void)
{
	TurboBoostCP = A170WCP;
	TurboBoostEn = A170Wchargerturbo;
	TurboBoostthrottlEn = A170Wthrottling;
	TurboBoostthrottlDis =A170Wthrottlingrecove;
	TurboBoostDis = A170Wturborecover;

	ACModeSTPPEn = A170WACmodethrottling;
	ACModeSTPPDis = A170WACmodethrottlingrecove;
	Chk_Hybrid_STPP_min_BattGasgauge = 20;
	Chk_Hybrid_STPP_Batt_Output_Power_Limit = 68; // 20W / 20 / IOUT /( 3/1024)

	Chk_Trickle_Current_Now_Current = 700;//700mA
	Chk_Trickle_Current_Chg_Current = 500;//500mA
}

void RestoreTurbo_V_Thro(void)
{
	if (AdapterID==AdapterID_65W || AdapterID==AdapterID_NON_SUPPORT)	
	{
        	       CMP1THRDATM = (VC1RestoreValue_65W>>8);	// threshold value buffer
			CMP1THRDATL = VC1RestoreValue_65W;
	 }
	  else if (AdapterID==AdapterID_90W)			
		   {
	        	       CMP1THRDATM = (VC1RestoreValue_90W>>8);		// threshold value buffer
				CMP1THRDATL = VC1RestoreValue_90W;
		    }
} 

void SetTurbo_V_Thro(void)
{

	if (AdapterID==AdapterID_65W || AdapterID==AdapterID_NON_SUPPORT)	
	{
        	       CMP1THRDATM = (VC1TriggerValue_65W>>8);	// threshold value buffer
			CMP1THRDATL = VC1TriggerValue_65W;
	 }
	  else if (AdapterID==AdapterID_90W)			
		   {
	        	       CMP1THRDATM = (VC1TriggerValue_90W>>8);		// threshold value buffer
				CMP1THRDATL = VC1TriggerValue_90W;
		    }


	//if (AdapterID==AdapterID_90W || AdapterID==AdapterID_NON_SUPPORT)	//90W
	  	{
        //	       CMP1THRDATM = (VC1TriggerValue_90W>>8);	// threshold value buffer
	//		CMP1THRDATL = VC1TriggerValue_90W;
	  	}
	//	else if (AdapterID==AdapterID_120W)			//120W
			 {
	  //      	CMP1THRDATM = (VC1TriggerValue_120W>>8);		// threshold value buffer
	//			CMP1THRDATL = VC1TriggerValue_120W;
			 }
	//		 else if (AdapterID==AdapterID_170W)	//170W
				  {
	    //    		CMP1THRDATM = (VC1TriggerValue_170W>>8);	// threshold value buffer
		//			CMP1THRDATL = VC1TriggerValue_170W;
				  }
			  
}
*/ //ANGELAS073:remove end

//MEILING001:S+ define another function for SetPowerBatteryparameter().
void SetPowerBatteryparameter2(void)
{
	Chk_Hybrid_STPP_min_BattGasgauge = 10;  //COKEYXU005:45 to 10
	CLEAR_MASK(SYS_STATUS,b5UnSuitAdpPow);
	CLEAR_MASK(SYS_STATUS,b4IllegalAdp);
	if((API_ID > 0x2DE) && (API_ID <= 0x374))  // >2.149  <=2.590
    {
        AdapterID = AdapterID_170W;
    }
    else if((API_ID > 0x238) && (API_ID <= 0x2D0))  // 1.663(0x238) 2.109(0x2D0)  1.663x1024/3=0x238
    {
        AdapterID = AdapterID_135W;
    }
    else if((API_ID > 0x190) && (API_ID <= 0x228)) //should change to >1.172(0x190) <=1.618(0x228) for 90W  ">0.693(0xEC) <=1.134(0x183) for 65W ",  //0.0029296875
    {
        AdapterID = AdapterID_90W;
    }
    else if ((API_ID > 0xEC) && (API_ID <= 0x183))//0.693<API_ID<1.134
    {
        AdapterID = AdapterID_65W;
    }
    else if ((API_ID > 0x50) && (API_ID <= 0xE2))//0.234<API_ID<0.663
    {
        AdapterID = AdapterID_45W;
    }
    else if((API_ID > 0x380) || (API_ID <= 0x46)) // >2.626 or <0.207
    {
        AdapterID = AdapterID_NON_SUPPORT;
    }

	if(AdapterID == AdapterID_135W || AdapterID == AdapterID_170W)
    {
        CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);
        CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
		//COKEYXU018:S+ if suit Adapter,clear stop charge flag  
		CLEAR_MASK(ACOFF_SOURCE,BATTLEARN);
		CLEAR_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable); //REJERRY051:Modify read addr to write.
		if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) //REJERRY051:Modify read addr to write.
		{
			CHGIC_SMbusFailCnt++;
			RamDebug(0x14);  
		}
		//COKEYXU018:E+ 
	}
	else if(AdapterID == AdapterID_90W)  //COKEYXU042: For 90W adapter,set unsuit adapter.
	{
		SET_MASK(SYS_STATUS, b5UnSuitAdpPow);
        CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
	}
	else if(AdapterID == AdapterID_45W || AdapterID == AdapterID_65W )    //COKEYXU018: add 60W unsuit Adapter
    {
        SET_MASK(SYS_STATUS, b5UnSuitAdpPow); 
        CLEAR_MASK(SYS_STATUS, b4IllegalAdp);
		
		//COKEYXU018:S+ if unsuit Adapter,stop charge when S0.
		if(SystemIsS0)
		{
			
			CLEAR_MASK(SYS_STATUS,AC_ADP); //COKEYXU040: For 65W and 45W adapter,will not show AC in state when S0.	
			SET_MASK(ACOFF_SOURCE,BATTLEARN);//JERRYCR087: Modify non-support adapter can't charge under s3/s4/s4 and non-support adapter show charge under S0.
			SET_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable); //REJERRY051:Modify read addr to write.
			if (!bRWSMBus(SMbusChB, SMbusWW, Charger_Addr, C_ChargerMode, &CHGIC_WriteCmd0x12L,SMBus_NoPEC)) //REJERRY051:Modify read addr to write.
			{	
				CHGIC_SMbusFailCnt++;
				RamDebug(0x14);
			}
		}
	//COKEYXU018:E+
  	}
    else if(AdapterID == AdapterID_NON_SUPPORT)
    {
        SET_MASK(SYS_STATUS, b4IllegalAdp);
        CLEAR_MASK(SYS_STATUS, b5UnSuitAdpPow);
   	}
	
}
//MEILING001:E+

//ANGELAG046: add start

void psys_throttle() //COKEYXU008: modify as chkpsys
{
	if ((SystemIsS0) && (!Read_AC_IN()))
	{
		if((IS_MASK_SET(SEL_STATE0,PRESENT_A)) && (IS_MASK_CLEAR(BATTUPDATEFW,BIT0)))
		{
			if (nBattGasgauge > 25)
			{
				if (Psys_AvgData >= 648) //COKEYXU003:Modify to 80W(1.6V)//change to 95W(1.9V)
				{
					SET_MASK(GPU_Prochot, b1_Psys);
					SET_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else if ((Psys_AvgData >= 409) && (Psys_AvgData < 648))//COKEYXU014:60~80W, cancel prochot, and NV SKU set D2 //change to 95W(1.9V)
				{
					cGPUBattPsysThrottling = 1; //only for NV//COKEYXU014:change to D2.
					CLR_MASK(GPU_Prochot, b1_Psys);
					CLR_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else if (Psys_AvgData < 409) //COKEYXU003: <60W, cancel prochot, and NV SKU set D2
				{
					cGPUBattPsysThrottling = 1; //only for NV
					CLR_MASK(GPU_Prochot, b1_Psys);
					CLR_MASK(Thro_Status2, b7ProCH_Psys);
				}
			}
			else if (nBattGasgauge > 7) //battery capacity is 7-10 //COKEYXU047: RSOC is 7~25, power consumption > 30W,pull CPU/GPU prochot.
			{
				if (Psys_AvgData >= 204) //COKEYXU003:Modify to 30W(0.6V)
				{
					SET_MASK(GPU_Prochot, b1_Psys);
					SET_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else if (Psys_AvgData < 204) //COKEYXU003:Modify to 30W(0.6V)
				{
					cGPUBattPsysThrottling = 3; //D4
					CLR_MASK(GPU_Prochot, b1_Psys);
				 	CLR_MASK(Thro_Status2, b7ProCH_Psys);
				}
			}
			else // battery capacity is 0-7
			{
				SET_MASK(GPU_Prochot, b1_Psys);
				SET_MASK(Thro_Status2, b7ProCH_Psys);
			}
		}

		//COKEYXU038:remove
	/*	if(nBattAverTemp <= 15)   //COKEYXU036: DC mode limit GPU consumption
		{
			cGPUBattPsysThrottling = 4; //D5
		} 
	*/	
	}
	else
	{
		cGPUBattPsysThrottling = 0;
		CLR_MASK(GPU_Prochot, b1_Psys);
		CLR_MASK(Thro_Status2, b7ProCH_Psys);
	}
}
//ANGELAG046: add end



//MEILING001:S-
/*void Set_SD_PWR_EN(void)
{
	//if(SD_PWR_EN < 0x199)              // 1.2V
	if((SystemIsS0) && (SD_PWR_EN < 0x266))   // 1.2V //ANGELAS013:Modify SD power function.//ANGELAS098:Modify SD_POWER voltage value.chang1.2v to 1.8v
	{
		EC_SD_PWR_EN_OUTPUT;
		EC_SD_PWR_EN_LOW();
	}
	else
	{
		EC_SD_PWR_EN_HI();
		EC_SD_PWR_EN_INPUT;
	}
}*/
//MEILING001:E-

//ANGELAG056: add start
void Check_heavy_loading_500ms(void)
{

	if(((!Read_AC_IN()) || (IS_MASK_SET(CHGIC_ReadCmd0x12L,BatLearnEnable))) 
		&& (nBattGasgauge <= 7) && (nBattGasgauge >= 6)
		&& (IS_MASK_CLEAR(BATTUPDATEFW,BIT0)) && (IS_MASK_SET(SEL_STATE0,PRESENT_A)))
	{
		Psys_SUM_500ms_Cnt++;
		if(Psys_SUM_500ms_Cnt >= 5) //delay 200ms
		{
			Psys_SUM_500ms += Psys;
		}
		if(Psys_SUM_500ms_Cnt == 14)
		{
			Psys_SUM_500ms_Cnt = 4;
			Psys_SUM_500ms /= 10;
			if(Psys_SUM_500ms >= 191) //28W, 0.56V
			{
				SET_MASK(System_Status,b0_System_Is_Heavy_Loading);
			}
			Psys_SUM_500ms = 0;
		}
	}
	else
	{
		Psys_SUM_500ms = 0;
		Psys_SUM_500ms_Cnt = 0;
		CLR_MASK(System_Status,b0_System_Is_Heavy_Loading);
	}
}
//ANGELAG056: add end


