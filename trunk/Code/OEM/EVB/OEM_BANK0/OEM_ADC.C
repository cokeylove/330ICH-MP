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
// The function of scaning ADC channel
//----------------------------------------------------------------------------
void ScanADCFixChannel(void)
{
	BYTE i; 
	LWORD Psys_SUM = 0; 

   	if(IS_MASK_SET(VCH5CTL, BIT7))
    {
        VCH5CTL |= 0x80;        // write clear data vaild flag
	 	ADP_I= ((WORD)VCH5DATM << 8) + VCH5DATL;
    }


    // read API_ID.
	if(IS_MASK_SET(VCH6CTL, BIT7))
	{
    	VCH6CTL |= 0x80;        // write clear data vaild flag  
 	    API_ID = ((WORD)VCH6DATM << 8) + VCH6DATL; 
	}
	
	//read Psys
	if(IS_MASK_SET(VCH7CTL, BIT7))
    {
    	VCH7CTL |= 0x80;        // write clear data vaild flag
	 	Psys= ((WORD)VCH7DATM << 8) + VCH7DATL;
    }

	for (i=3; i>0; i--) 
	{
		Psys_Data[i] = Psys_Data[i-1];
		Psys_SUM += Psys_Data[i];
	}
	Psys_Data[0] = Psys;
	Psys_SUM += Psys_Data[0];
	Psys_AvgData = Psys_SUM / 4; //calculate 4 times average value

	psys_throttle(); //COKEYXU008: Add
	
}

void ScanADCDyChannel1(void)
{
	BYTE i;
	LWORD ADP_I_SUM = 0;

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
    
    if (SysPowState == SYSTEM_S0 && Read_AC_IN())//add ACIN condition.
    {
		ADPI2Sec++;
		for (i=3; i>0; i--)
		{
			ADPI_Data[i] = ADPI_Data[i-1];
			ADP_I_SUM += ADPI_Data[i];
		}
		ADPI_Data[0] = ADP_I;
		ADP_I_SUM += ADPI_Data[0];
		ADPI_AvgData = ADP_I_SUM / 4;
   
        
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
		
		if (ADPI2Sec > 200)
		{
			if (IS_MASK_SET(SEL_STATE0, PRESENT_A))
			{
				Chk_HybridFORBQ24780_STPP(); //ANGELAS047:add
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
	VCH5CTL=0x90;   // ADP_I
	VCH6CTL=0x90;   // ADAPTER_ID_R
	VCH7CTL=0x90;   //ANGELAG019: add
}

//---------------------------------------------------------------------------
// Init Voltage Compare Function
// Input: 1 Group. choice the init VC channel  0: VC0 1: VC1 2:VC2 3:All VC Channel
//---------------------------------------------------------------------------
void Init_VC(BYTE Group)
{
	int tGroup = (Group&0x03);
	if( (VCMP0CTL&0x80) || (VCMP1CTL&0x80) || (VCMP2CTL&0x80) )
	{
		IER18 |= 0x80;
	}
}

void Chk_HybridFORBQ24780_STPP(void)
{
 	if(Read_AC_IN() && (nBattGasgauge > Chk_Hybrid_STPP_min_BattGasgauge))   //COKEYXU016:>=10%,change to >10%.   
	{
		SET_MASK(CHGIC_WriteCmd0x37L, TurboBoost);
	 	if(IS_MASK_CLEAR(LENOVOPMFW,BATTERY_MAIN_CAL))  
		{
			cBattFlag0.fbit.cCmdAcOff=0; 			//Resume Charger
 		}
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
		CLEAR_MASK(CHGIC_WriteCmd0x12L,BatLearnEnable); 
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

void psys_throttle() //COKEYXU008: modify as chkpsys
{
	if ((SystemIsS0) && (!Read_AC_IN()))
	{
		if((IS_MASK_SET(SEL_STATE0,PRESENT_A)) && (IS_MASK_CLEAR(BATTUPDATEFW,BIT0)))
		{
			if (nBattGasgauge > 25)
			{
				if (Psys_AvgData >= 546) //COKEYXU003:Modify to 80W(1.6V)
				{
					SET_MASK(GPU_Prochot, b1_Psys);
					SET_MASK(Thro_Status2, b7ProCH_Psys);
				}
				else if ((Psys_AvgData >= 409) && (Psys_AvgData < 546))//COKEYXU014:60~80W, cancel prochot, and NV SKU set D2
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
			else if (nBattGasgauge > 7) //battery capacity is 7-10
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
	}
	else
	{
		cGPUBattPsysThrottling = 0;
		CLR_MASK(GPU_Prochot, b1_Psys);
		CLR_MASK(Thro_Status2, b7ProCH_Psys);
	}
}

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


