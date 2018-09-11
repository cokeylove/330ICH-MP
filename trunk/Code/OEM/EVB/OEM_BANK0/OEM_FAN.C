/*-----------------------------------------------------------------------------
 * TITLE: OEM_FAN.C
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
* Function name: PWM_TimeCount      
*
* Descriptoin: PWM Beep function.Service short time PWM output, base on 50ms.
*
* Invoked by: Hook_Timer50msEventB()
*
* TimeDiv：50ms
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void PWM_TimeCount(void)
{
	if (uReserve07.fbit.uACInOutBeep == 0)
	{
		if( PWM_LEDBeep_CNT == 0x00 )
		{
			BEEP_INIT;
			PWM_BEEP = 0x00;
			return;
		}

		BEEP_ALT;
		PWM_LEDBeep_CNT--;		// decrease timer
		// AC IN/OUT beep time
		if( PWM_LEDBeep_CNT == 0x00 )
		{
			BEEP_INIT;
			PWM_BEEP = 0x00;
		}
	}
	else
	{
		PWM_LEDBeep_CNT = 0;
		BEEP_INIT;
		PWM_BEEP = 0x00;
	}
}

/*
*******************************************************************************
* Function name: VGA_Temperature      
*
* Descriptoin: Read VGA temperature(M/B)
*
* Invoked by: Hook_Timer1SecEventA()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void VGA_Temperature(void)
{
	BYTE BsData;
	BYTE INIT_GPU_REG2[4] = {0x0f, 0x00, 0x00, 0x8f};
  	BYTE INIT_GPU_REG1[4] = {0x0f, 0x00, 0x00, 0x8e};
	BYTE INIT_GPU_REG0[4] = {0xc0, 0x30, 0x00, 0x14};

	if ( (CPU_TYPE & 0xC0) == 0x00 )	// Isn't check VGA support optimus?
	{ nHybridGraphicsDIS;}				// No optimus status.
 	else
 	{ nHybridGraphicsEN; }				// optimus status.

	if( SystemNotS0 || !(nHybridGraphicsGET)|| (PwrOnDly5Sec!=0) ) 
	{  
        VGAErrCnt = 00;  
	    VGA_TEMP = VGA_TBuff3 = VGA_TBuff2 = VGA_TBuff1 = 0x00; 
        CLEAR_MASK(ERR_THMSTS,b1VGATempEr); 
        return; 
    }
     
	if((CPU_TYPE &0xc0)==0x40)
	{
	if(!bWSMBusBlock(SMbusCh4, SMbusWBK, VGA_Addr, 0x01,( BYTE* )&INIT_GPU_REG1,4,0))//load GPU register offset to SMB_ADDR
		{
		VGAErrCnt++;
		}
 	if(!bWSMBusBlock(SMbusCh4, SMbusWBK, VGA_Addr, 0x02,( BYTE* )&INIT_GPU_REG0,4,0))//sent data to be written into the DGPU rgister
		{
		VGAErrCnt++;
		}

	if(!bWSMBusBlock(SMbusCh4, SMbusWBK, VGA_Addr, 0x01,( BYTE* )&INIT_GPU_REG2,4,0))//load GPU register offset to SMB_ADDR
		{
		VGAErrCnt++;
		}
		
		if(!bRSMBusBlock(SMbusCh4, SMbusRBK, VGA_Addr, 0x03, &sdAMDTH0))
		{
			VGAErrCnt++;
			if( IS_MASK_CLEAR(ERR_THMSTS, b1VGATempEr) )// Check bit1 is Error.
			{
				if ( VGAErrCnt > 35 )	//change 7 to 35  // 3 Sec.
				{
					SET_MASK(ERR_THMSTS, b1VGATempEr);	// Set bit1 Thermal IC read temperature error.
				}
			}
			else
			{
				if ( VGAErrCnt > 254 )		// from 68 to 255	// 30 Sec.
				{
					SET_MASK(SysStatus,ERR_ShuntDownFlag);
					ProcessSID(VGACOMMFAIL_ID);	// 0x20 shutdown ID.
					VGAErrCnt = 00;				// Clear error count.
					RSMRST_LOW();
					Delay1MS(1);
					RSMRST_HI();
				}
			}
			ResetSMBus(SMbusCh4);
		}
		else
		{
			VGA_TEMP=((sdAMDTH2>>1)+((sdAMDTH3<<8)&0XFF)); 
			VGAErrCnt = 00;		// Clear error count.
			CLEAR_MASK(ERR_THMSTS,b1VGATempEr);	// Clear bit1,bit5.

			if ( IS_MASK_CLEAR(Fan_Debug_Temp,b1VGA_Temp) )	// Debug VGA Temperature, Engineer myself control.
			{
		    	if((VGA_TEMP&0x80)==0x80)//Filter the value of the GPU temperature greater than 128 // optimize G69.
         		{
         			return;
         		}  
        		else
				{
					VGA_TBuff3 = VGA_TBuff2;
					VGA_TBuff2 = VGA_TBuff1;
					VGA_TBuff1 = VGA_TEMP;
					VGA_TBuff3 = (VGA_TBuff1 + VGA_TBuff2 + VGA_TBuff3)/3;		// VGA average temperature.
            	}
			}
		}
	}
	if((CPU_TYPE &0xc0)==0x80)
	{
		if(!bRWSMBus(SMbusCh4, SMbusRB, EXTVGA_Addr, 0x00, &BsData, 0))
		{
        	VGAErrCnt++;
			if( IS_MASK_CLEAR(ERR_THMSTS, b1VGATempEr) )// Check bit1 is Error.
			{
				if ( VGAErrCnt > 35 )	//change 7 to 35  // 3 Sec.
				{
					SET_MASK(ERR_THMSTS, b1VGATempEr);	// Set bit1 Thermal IC read temperature error.
				}
			}
			else
			{
				if ( VGAErrCnt > 254 )		// from 68 to 255	// 30 Sec.
				{
					SET_MASK(SysStatus,ERR_ShuntDownFlag);
					ProcessSID(VGACOMMFAIL_ID);	// 0x20 shutdown ID.
					VGAErrCnt = 00;				// Clear error count.
					RSMRST_LOW();
					Delay1MS(1);
					RSMRST_HI();
				}
			}
			ResetSMBus(SMbusCh4);
		}
		else
		{
				VGA_TEMP = BsData;
				VGAErrCnt = 00;		// Clear error count.
				CLEAR_MASK(ERR_THMSTS,b1VGATempEr);	// Clear bit1,bit5.

				if ( IS_MASK_CLEAR(Fan_Debug_Temp,b1VGA_Temp) )	// Debug VGA Temperature, Engineer myself control.
				{
					if((VGA_TEMP&0x80)==0x80)//Filter the value of the GPU temperature greater than 128// optimize G69
                	{
                 		return;
                	}  
           			else
					{
						VGA_TBuff3 = VGA_TBuff2;
						VGA_TBuff2 = VGA_TBuff1;
						VGA_TBuff1 = VGA_TEMP;
						VGA_TBuff3 = (VGA_TBuff1 + VGA_TBuff2 + VGA_TBuff3)/3;		// VGA average temperature.
					}
				}
		}
	}
    
}

/*
*******************************************************************************
* Function name: ReadPCHTemp      
*
* Descriptoin: Read PCH temperature(M/B)
*
* Invoked by: Hook_Timer1SecEventB()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void ReadPCHTemp(void) 
{
	BYTE BsData;
	if( SystemNotS0 && (PwrOnDly5Sec==0)) 
		return;

	if(!bRSMBusBlock(SMbusChD, SMbusRBK, PCH_Temp, 0x40, &BsData))
	{
		PCHErrCnt++;  
		if( IS_MASK_CLEAR(ERR_THMSTS, b4PCHTempEr) ) 
		{
			if ( PCHErrCnt > 3 )    
			{
				SET_MASK(ERR_THMSTS, b4PCHTempEr); 
			}
		}
		else
		{
			if ( PCHErrCnt > 34 )   
			{

				SET_MASK(SysStatus,ERR_ShuntDownFlag);
				ProcessSID(CPUCOMMFAIL_ID);  
				PCHErrCnt = 0;      
				ETWCFG=EWDKEYEN;			 // enable external WDT key
				EWDKEYR=0xFF;			// external WDT reset	
				while(1);						   // Wait for watch dog time-out
			} 
		}
		ResetSMBus(SMbusChD); 
	}
	else
	{
		nNBTemp = BsData;   // Get PCH temperature.
		PCHErrCnt = 0;   // Clear error count.
		CLEAR_MASK(ERR_THMSTS, b4PCHTempEr); // Clear bit3 PCH read temperature error.
		if (IS_MASK_CLEAR(Fan_Debug_Temp,b3PCH_Temp))  // Debug PCH Temperature, Engineer myself control.
		{
			PCHTEMP_Buff_3 = PCHTEMP_Buff_2;
			PCHTEMP_Buff_2 = PCHTEMP_Buff_1;
			PCHTEMP_Buff_1 = nNBTemp;
			PCHTEMP_Buff_3 = (PCHTEMP_Buff_1 + PCHTEMP_Buff_2 + PCHTEMP_Buff_3)/3;  // PCH average temperature.
		}
	} 
}


/*
*******************************************************************************
* Function name: Oem_Thermal_Control      
*
* Descriptoin: OEM thermal control
*
* Invoked by: Hook_Timer1SecEventB()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Oem_Thermal_Control(void)
{
	if(SystemIsS0&& (PwrOnDly5Sec==0)&&Read_SLPS3() && Read_SLPS4())// PECI workaround.
	{
		if(TJMAX == 0x00)
		{
			PECI_ReadCPUTJMAX();
			CPU_TYPE = CPU_TYPE & 0xFC; //
			if (TJMAX == 105)			//105
				CPU_TYPE |= 0x03;
			else if (TJMAX == 100)		//100
				CPU_TYPE |= 0x02;
			else if (TJMAX == 90)		//90
				CPU_TYPE |= 0x01;
		}
		else
		{ PECI_ReadCPUTemp(); }
		//Optimize read CPU temperature function.
		if( IS_MASK_SET(ERR_THMSTS, b3PECITempEr) )
		{
			DTS_ReadCPUTemp();
		}
		//0ptimize read CPU temperature function.
	}
}

const CPUTHRstruct code CPUTHRTable[]=
{ //      Thr_Off  Thr_On  Turbo_Rem   Turbo_Off    Thr_Shut	       Step  Tjxx  Throttling Table
//-----------------------------------------------------------------------------
	{	92, 	97, 		80, 		85, 		99 },	// 0 for DIS CPU 14' //ANGELAG028: 68 to 65 //ANGELAG027: modify to 60/68 //MEILING019:modify fan table start.  //MEILING024:change cpu turbo off from 70 to 65.  //MEILING040:modify turbo on/off start.//COKEYXU006:refer to Fan table v02_1220.
	{	92, 	97, 		60, 		65, 		99 },	// 1 for DIS GPU 14' //ANGELAG046: modify 65/70 to 62/67 //ANGELAG043: 60/65 to 65/70  //COKEYXU006:refer to Fan table v02_1220.
	
	{	88, 	97, 		77, 		82, 		100 },	// 2 for UMA CPU 14'  //MEILING024:change cpu turbo on from 60 to 65. //MEILING033:modify turbo on/off.
	{	100,	100,		100, 		100, 		100 },  // 3 for UMA GPU 14' 

	{	92, 	97, 		80, 		85, 		99 },	// 4 for DIS CPU 15' //ANGELAG028: 68 to 65  //ANGELAG027: modify to 60/68 //MEILING024:change cpu turbo off from 70 to 65.  //MEILING033:modify turbo on/off. //COKEYXU006:refer to Fan table v02_1220.
	{	92, 	97, 		60, 		65, 		99 },	// 5 for DIS GPU 15'  //ANGELAG046: modify 65/70 to 62/67  //ANGELAG043: 60/65 to 65/70 //MEILING033:modify turbo on/off. //COKEYXU006:refer to Fan table v02_1220.
	
	{	88, 	97, 		77, 		82, 		100 },	// 6 for UMA CPU 15'  //MEILING024:change cpu turbo off from 60 to 65.  //MEILING033:modify turbo on/off.  //MEILING040:modify turbo on/off end.
	{	100,	100,		100, 		100, 		100 },  // 7 for UMA GPU 15'

	{	92, 	97, 		80, 		85, 		99 },  // 8 for DIS CPU 17' //ANGELAG028: 68 to 65  //ANGELAG027: modify to 60/68   //COKEYXU006:refer to Fan table v02_1220.
	{	92, 	97, 		60, 		65, 		99 },  // 9 for DIS GPU 17' //ANGELAG046: modify 65/70 to 62/67  //ANGELAG043: 60/65 to 65/70   //COKEYXU006:refer to Fan table v02_1220.

	{	88,		97,			77,			82,			100	},  // 10 for UMA CPU 17'
	{	100,	100,		100,		100,		100	}   // 11 for UMA GPU 17'  //MEILING019:modify fan table end.

};

/*
*******************************************************************************
* Function name: OEM_Throttling_Ctrl      
*
* Descriptoin: OEM throttling control
*
* Invoked by: Hook_Timer1SecEventB()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void OEM_Throttling_Ctrl(void)
{
	BYTE BTemp_Type,BTbStep;
	
	BTbStep = 0;

	if( SystemNotS0 ) 
	{
	    H_PROCHOT_OFF();
		return;
	}
#if FAN_TABLE_Already
	if(uMBID == 0x14) //for 14'
	{
		if(uMBGPU&0x02) //DIS
	  	{
	  		BTbStep = 0;
	  	}
	  	else
	  	{
	  		BTbStep = 2;
		}
	}
	else if(uMBID == 0x15)//for 15'
	{
	    if(uMBGPU&0x02)
	  	{
	  		BTbStep = 4;
	  	}
	  	else
	  	{
	  		BTbStep = 6;
	  	}
	}
	else if(uMBID == 0x17)//for 17'
	{
	    if(uMBGPU&0x02)
	  	{
	  		BTbStep = 8;
	  	}
	  	else
	  	{
	  		BTbStep = 10;
	  	}
	}
	Thro_Std_Mode(BTbStep); 
	nOSThrottlingTemp = THR_PRO_ON;	// For OS throttling temperature.
	nOSShutdownTemp = THR_Tab_Shut;	// For OS Shutdown temperature.
	nShutDownTemp = THR_Tab_Shut;	// Shutdown temperature.
#endif 
}

/*
*******************************************************************************
* Function name: Thro_Std_Mode      
*
* Descriptoin: Thro_Std_Mode
*
* Invoked by: OEM_Throttling_Ctrl()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Thro_Std_Mode(BYTE BMBStep)
{
	THRTab_ToRAM((0+BMBStep));	//load cpu table to RAM
	VTHRTab_ToRAM((1+BMBStep)); //load VGA table to RAM
	
	Thro_Shut_Status((0+BMBStep),TEMP_TYPE_CPU); // Check CPU Throttle and Shutdown status.
	Thro_Turbo(CPUTHRTable[(0+BMBStep)].Turbo_Rem,CPUTHRTable[(0+BMBStep)].Turbo_Off,TEMP_TYPE_CPU); //Check CPU turbo
    Thro_Shut_Status(0,TEMP_TYPE_local); //check cpu thermistor shutdown

	if(uMBGPU&0x02) //only DIS do these action
    {
		Thro_Shut_Status((1+BMBStep),TEMP_TYPE_VGA); // Check VGA Throttle and Shutdown status.
		Thro_Turbo(CPUTHRTable[(1+BMBStep)].Turbo_Rem,CPUTHRTable[(1+BMBStep)].Turbo_Off,TEMP_TYPE_VGA); //check VGA turbo
    	Thro_Shut_Status(0,TEMP_TYPE_remote); //check VGA thermistor shutdown
    }
}

//COKEYXU044: add disable CPU turbo for 90s function
/*
*******************************************************************************
* Function name: disCPUturbo90s      
*
* Descriptoin: Disable CPU turbo 90s
*
* Invoked by: Hook_Timer1SecEventA()
*
* TimeDiv：1s
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void disCPUturbo90s(void)
{
	if(DisCPUTurboFor90s > 0) //COKEYXU029: add 
	{
		DisCPUTurboFor90s--; 
	}
}

/*
*******************************************************************************
* Function name: Thro_Shut_Status      
*
* Descriptoin: Check throttling and Shutdown status.
*
* Invoked by: Thro_Std_Mode()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Thro_Shut_Status(BYTE BStep, BYTE BTType)
{
	BYTE BAvgTemp;

	if ( BTType == 2 )
	{	// Set External FAN.       
		if ( EXT_VGA_Buff3 < THR_PRO_OFF)	 // Check prochot turn OFF.	
		{ CLR_MASK(Thro_Status, b2ProCH_EXTVGA); }
		else if ( EXT_VGA_Buff3 >= THR_PRO_ON)	// Check prochot turn ON.
		{ SET_MASK(Thro_Status, b2ProCH_EXTVGA); }
		if ( EXT_VGA_Buff3 >=THR_Tab_Shut)	  // Check shutdown status.
		{ Thro_Count_Shut(&EXTVGA_Shut_Cnt,TEMP_TYPE_EXT); }	// Check x times for shutdown protection.
		else
		{ EXTVGA_Shut_Cnt = 0; }
	}
	else if ( BTType == 1 )
	{
			 if ( VGA_TBuff3 < VTHR_PRO_OFF)		
			{ CLR_MASK(GPU_Prochot, b0_Thermal); }
			else if ( VGA_TBuff3 >= VTHR_PRO_ON )	
		 	 { SET_MASK(GPU_Prochot, b0_Thermal); }
			if ( VGA_TBuff3 >= VTHR_Tab_Shut )  //Modify the GPU shutdown temperature  registers
			{ Thro_Count_Shut(&VGA_Shut_Cnt,TEMP_TYPE_VGA); }	// Check x times for shutdown protection.
			else
			{ VGA_Shut_Cnt = 0; }
	}
   	else if ( BTType == 4 ) //Add thermal IC local and remote over temperature protect.
    {
        if ( ThermistorCPU_TEMP >= nCPUthermistoroverTemp )  
        { Thro_Count_Shut(&local_Shut_Cnt,TEMP_TYPE_local); }   // Check x times for shutdown protection.
        else
       	{ local_Shut_Cnt = 0; }
    }
    else if ( BTType == 5 )//Add thermal IC local and remote  over temperature protect.
    {
	   	if ( EXTVGA_TEMP >= nGPUthermistoroverTemp ) 
        { Thro_Count_Shut(&remote_Shut_Cnt,TEMP_TYPE_remote); }   // Check x times for shutdown protection.
        else
       	{ remote_Shut_Cnt = 0; }
    }
	else
	{	// Set Internal CPU.
		if ( TEMP_Buff_3 < THR_PRO_OFF) 
		{ CLR_MASK(Thro_Status, b0ProCH_CPU); }
		else if ( TEMP_Buff_3 >= THR_PRO_ON) 
		{ SET_MASK(Thro_Status, b0ProCH_CPU); }

		if ( TEMP_Buff_3 >= THR_Tab_Shut )	
		{ Thro_Count_Shut(&CPU_Shut_Cnt,TEMP_TYPE_CPU); }	// Check x times for shutdown protection.
		else
		{ CPU_Shut_Cnt = 0; }
	}
	
}

/*
*******************************************************************************
* Function name: Thro_Turbo      
*
* Descriptoin: Check CPU/VGA Turbo status.
*
* Invoked by: Thro_Std_Mode()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Thro_Turbo(BYTE BTurboRem, BYTE BTurboOff, BYTE BTType)
{
	BYTE BAvgTemp;

	if ( IS_MASK_CLEAR(SYS_MISC1, ACPI_OS) ) { return; }

	if ( BTType == 2 )
	{	// Set External FAN.
		if ( EXT_VGA_Buff3 <= BTurboRem )				// Check turbo resume.
		{ CLR_MASK(Thro_Status, b5Turbo_EXTVGA); }
		else if ( EXT_VGA_Buff3 >= BTurboOff )			// Check turbo Off.
		{ SET_MASK(Thro_Status, b5Turbo_EXTVGA); }
	}
	else if ( BTType == 1 )
	{	
		if(Read_AC_IN())
		{
			if ( VGA_TBuff3 <= BTurboRem ) // Check turbo resume. 
			{ CLR_MASK(Thro_Status, b4Turbo_VGA); } 
			else if ( VGA_TBuff3 >= BTurboOff ) // Check turbo Off. 
			{ SET_MASK(Thro_Status, b4Turbo_VGA); }
		}
		else
		{
			if ( VGA_TBuff3 <= (BTurboRem-5) ) // Check turbo resume. 
			{ CLR_MASK(Thro_Status, b4Turbo_VGA); }
			else if ( VGA_TBuff3 >= (BTurboOff-5) ) // Check turbo Off. 
			{ SET_MASK(Thro_Status, b4Turbo_VGA); }
		}
	}
	else
	{	// Set Internal CPU.

      //Add cpu turbo enable/disable event.		
		if (( TEMP_Buff_3 <= BTurboRem )&&	( VGA_TBuff3 <=60))// Check turbo resume. //COKEYXU027: enable CPU turbo when GPU temp <= 60
		{ CLR_MASK(Thro_Status, b3Turbo_CPU); }
		else
		{
			if (( TEMP_Buff_3 >= BTurboOff )||(VGA_TBuff3 >=65))//COKEYXU027: disable CPU turbo when GPU temp > = 65.
			{
				SET_MASK(Thro_Status, b3Turbo_CPU);
			}
		}
	}
		
    if ( IS_MASK_CLEAR(Thro_Status, b3Turbo_CPU ) &&
    (DisCPUTurboFor90s == 0) && (DisCPUTurboForLTP == 0) &&
    IS_MASK_CLEAR(Adapter90WWA,b1_DisCPUturbo) && (nAtmFanSpeed > 5))//COKEYXU029: add fan1 speed > 500 and system start after 90s,then enable CPU turbo. //COKEYXU046: when low temp protect£¬disable CPU turbo.
	{                                                                                                                                          //COKEYXU030: add W/A for 90W adapter,cannot charge battery RSOC up when system consumption is very high.              
		if ( IS_MASK_SET(uVGATurboFun,uDisCPUTurboOK) )		
		{
			CLR_MASK(uVGATurboFun, uDisCPUTurboOK);	
            ECQEvent(EN_CPUTURBO_67);  
		}
	}
	else
	{
		 	if ( IS_MASK_CLEAR(uVGATurboFun,uDisCPUTurboOK) )
		 	{
				SET_MASK(uVGATurboFun, uDisCPUTurboOK);		
	            ECQEvent(DIS_CPUTURBO_66); 
		 	}
	}
}

/*
*******************************************************************************
* Function name: Thro_Count_Shut      
*
* Descriptoin: Throttling shutdown count used.
*
* Invoked by: Thro_Shut_Status()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Thro_Count_Shut(XBYTE *BShut_Cnt, BYTE TempType)
{
	if ( *BShut_Cnt >= Thro_Shut_Cnt )	// Read 3 times.
	{
		if ( TempType == 2 )
		{ ProcessSID(EXTVGAOVERTEMP_ID); }		// EXT VGAShutdown ID 0x1C.
		else if ( TempType == 1 )
		{ ProcessSID(VGAOVERTEMP_ID); }	// VGA Shutdown ID 0x12.
		else if ( TempType == 4 )
		{ ProcessSID(CPUthermistoroverTemp_ID); }	//ANGELAS082: ThermalIClocalOVerTEPM_ID to CPUthermistoroverTemp_ID//30 to33
		else if ( TempType == 5 )
		{ ProcessSID(GPUthermistoroverTemp_ID); }	//ANGELAS082: ThermalICremoteOVerTEPM_ID to GPUthermistoroverTemp_ID//31 to 34
		else
		{ ProcessSID(DTSOVERTEMP_ID); }	// CPU Shutdown ID 0x11.

        SET_MASK(SysStatus,ERR_ShuntDownFlag);
	    CKCON &= 0x3F;			// set 26 bit counter // bit7-6 00=14ms,01=113ms, 10=911ms, 11(1/9.2MHz)*2'26=7.2S
	    CKCON |= 0x10;			// set 26 bit counter // bit7-6 00=14ms,01=113ms, 10=911ms, 11(1/9.2MHz)*2'26=7.2S
        WDTCON|=0x01;   		/* WDTRST = 1 Reset watch dog timer.*/
        WDTCON|=0x02;   		/* WDTEN  = 1 Enable watch dog.     */
        while(1);       		/* Wait for watch dog time-out      */
		PWSeqStep = 1;
    	PowSeqDelay = 0x00;
        RamDebug(0x0F);  
		SysPowState = SYSTEM_S0_S5;			// EC force Shutdown.

		*BShut_Cnt = 0;
		//RSMRST_LOW();
		//Delay1MS(1);
		//RSMRST_HI();
		SET_MASK(USB_Charger, b2USB_TmlDis);	// Disable USB charger.
	}
	else
	{ (*BShut_Cnt)++; }
}

/*
*******************************************************************************
* Function name: THRTab_ToRAM      
*
* Descriptoin: Throttle table save to ram.
*
* Invoked by: Thro_Std_Mode()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void THRTab_ToRAM(BYTE BStep)
{
    if(IS_MASK_SET(TEST_FLAG, Fan_Control_Auto))
    {
       return;
    }
	THR_PRO_OFF 	= CPUTHRTable[BStep].Thr_Off;
	THR_PRO_ON		= CPUTHRTable[BStep].Thr_On;
	THR_Turo_Rem	= CPUTHRTable[BStep].Turbo_Rem;
	THR_Turo_OFF	= CPUTHRTable[BStep].Turbo_Off;
	THR_Tab_Shut	= CPUTHRTable[BStep].Thr_Shut;
}

/*
*******************************************************************************
* Function name: VTHRTab_ToRAM      
*
* Descriptoin: VGA Throttle table save to ram.
*
* Invoked by: Thro_Std_Mode()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void VTHRTab_ToRAM(BYTE BStep)
{
    if(IS_MASK_SET(TEST_FLAG, Fan_Control_Auto))
    {
       return;
    }
	VTHR_PRO_OFF 	= CPUTHRTable[BStep].Thr_Off;
	VTHR_PRO_ON		= CPUTHRTable[BStep].Thr_On;
	VTHR_Turo_Rem	= CPUTHRTable[BStep].Turbo_Rem;
	VTHR_Turo_OFF	= CPUTHRTable[BStep].Turbo_Off;
	VTHR_Tab_Shut	= CPUTHRTable[BStep].Thr_Shut;
}

//-----------------------------------------------------------------------------
// Smart Fan control
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Fan table 1   2009/08/18 Rev.01
//-----------------------------------------------------------------------------
const thermalstruct code ThermalTalbe1CPU[]= //UMA //COKEYXU006:refer to Fan table v02_1220.
{
    {0,	44,	40,	0,	&Fan1On_Step1,	&Fan1Off_Step1,	&Fan1RPM_Step1	},	// Level 0 	
    {1,	48,	40,	18,	&Fan1On_Step2,	&Fan1Off_Step2,	&Fan1RPM_Step2	},	// Level 1 	
    {2,	60,	44,	20,	&Fan1On_Step3,	&Fan1Off_Step3,	&Fan1RPM_Step3	},	// Level 2 	
    {3,	65,	56,	22,	&Fan1On_Step4,	&Fan1Off_Step4,	&Fan1RPM_Step4	},	// Level 3 	
    {4,	70,	61,	25,	&Fan1On_Step5,	&Fan1Off_Step5,	&Fan1RPM_Step5	},	// Level 4 	
    {5, 74,	66,	28,	&Fan1On_Step6,	&Fan1Off_Step6,	&Fan1RPM_Step6	},	// Level 5 	
    {6,128,	70,	32,	&Fan1On_Step7,	&Fan1Off_Step7,	&Fan1RPM_Step7	},	// Level 6  	
    //{7,127, 63, 56, &Fan1On_Step8,	&Fan1Off_Step8,	&Fan1RPM_Step8	},	// Level 7 		
};

const thermalstruct code ThermalTalbe1CPU_DIS[]= //DIS//COKEYXU006:refer to Fan table v02_1220.
{
    {0,	44,	40,	0,	&Fan1On_Step1,	&Fan1Off_Step1,	&Fan1RPM_Step1	},	// Level 0 	
    {1,	48,	40,	18,	&Fan1On_Step2,	&Fan1Off_Step2,	&Fan1RPM_Step2	},	// Level 1 	
    {2,	60,	44,	20,	&Fan1On_Step3,	&Fan1Off_Step3,	&Fan1RPM_Step3	},	// Level 2 	
    {3,	65,	56,	22,	&Fan1On_Step4,	&Fan1Off_Step4,	&Fan1RPM_Step4	},	// Level 3 	
    {4,	70,	61,	25,	&Fan1On_Step5,	&Fan1Off_Step5,	&Fan1RPM_Step5	},	// Level 4 	
    {5, 74,	66,	28,	&Fan1On_Step6,	&Fan1Off_Step6,	&Fan1RPM_Step6	},	// Level 5 	
    {6,128,	70,	32,	&Fan1On_Step7,	&Fan1Off_Step7,	&Fan1RPM_Step7	},	// Level 6 	
    //{7,127, 55, 48, &Fan1On_Step8,	&Fan1Off_Step8,	&Fan1RPM_Step8	},	// Level 7 		
};

const thermalstruct code ThermalTalbe1VGA[]=   //COKEYXU006:refer to Fan table v02_1220.
{
    {0,	45,	41,	0,	&Fan1OnVGA_Step1,	&Fan1OffVGA_Step1,	&Fan1RPMVGA_Step1	},	// Level 0 	
    {1,	48,	41,	18,	&Fan1OnVGA_Step2,	&Fan1OffVGA_Step2,	&Fan1RPMVGA_Step2	},	// Level 1 	
    {2,	52,	44,	20,	&Fan1OnVGA_Step3,	&Fan1OffVGA_Step3,	&Fan1RPMVGA_Step3	},	// Level 2 	
    {3,	56,	48,	22,	&Fan1OnVGA_Step4,	&Fan1OffVGA_Step4,	&Fan1RPMVGA_Step4	},	// Level 3 	
    {4,	60,	52,	25,	&Fan1OnVGA_Step5,	&Fan1OffVGA_Step5,	&Fan1RPMVGA_Step5	},	// Level 4 	
    {5, 64,	56,	28,	&Fan1OnVGA_Step6,	&Fan1OffVGA_Step6,	&Fan1RPMVGA_Step6	},	// Level 5 	
    {6,128, 60, 32,	&Fan1OnVGA_Step7,	&Fan1OffVGA_Step7,	&Fan1RPMVGA_Step7	},	// Level 6 	
    //{7,127, 55, 48, &Fan1OnVGA_Step8,	&Fan1OffVGA_Step8,	&Fan1RPMVGA_Step8	},	// Level 7 		
};

//-----------------------------------------------------------------------------
// Fan table 2
//-----------------------------------------------------------------------------
const thermalstruct code ThermalTalbe2CPU[]=   //COKEYXU006:refer to Fan table v02_1220.
{
	{0,	44,	40,	0,	&Fan2On_Step1,	&Fan2Off_Step1,	&Fan2RPM_Step1	},	// Level 0 	
	{1,	48,	40,	20,	&Fan2On_Step2,	&Fan2Off_Step2,	&Fan2RPM_Step2	},	// Level 1 	
	{2,	60,	44,	23,	&Fan2On_Step3,	&Fan2Off_Step3,	&Fan2RPM_Step3	},	// Level 2 	
	{3,	65,	56,	26,	&Fan2On_Step4,	&Fan2Off_Step4,	&Fan2RPM_Step4	},	// Level 3 	
	{4,	70,	61,	31,	&Fan2On_Step5,	&Fan2Off_Step5,	&Fan2RPM_Step5	},	// Level 4 	
	{5, 74,	66,	36,	&Fan2On_Step6,	&Fan2Off_Step6,	&Fan2RPM_Step6	},	// Level 5 	
	{6,128,	70,	42,	&Fan2On_Step7,	&Fan2Off_Step7,	&Fan2RPM_Step7	},	// Level 6 	
	//{7,127, 63,	56,	&Fan2On_Step8,	&Fan2Off_Step8,	&Fan2RPM_Step8	},	// Level 7 		
};

const thermalstruct code ThermalTalbe2CPU_DIS[]= //DIS 
{
	{0,	44,	40,	0,	&Fan2On_Step1,	&Fan2Off_Step1,	&Fan2RPM_Step1	},	// Level 0 	
	{1,	48,	40,	20,	&Fan2On_Step2,	&Fan2Off_Step2,	&Fan2RPM_Step2	},	// Level 1 	
	{2,	60,	44,	23,	&Fan2On_Step3,	&Fan2Off_Step3,	&Fan2RPM_Step3	},	// Level 2 	
	{3,	65,	56,	26,	&Fan2On_Step4,	&Fan2Off_Step4,	&Fan2RPM_Step4	},	// Level 3 	
	{4,	70,	61,	31,	&Fan2On_Step5,	&Fan2Off_Step5,	&Fan2RPM_Step5	},	// Level 4 	
	{5, 74,	66,	36,	&Fan2On_Step6,	&Fan2Off_Step6,	&Fan2RPM_Step6	},	// Level 5 	
	{6,128,	70,	42,	&Fan2On_Step7,	&Fan2Off_Step7,	&Fan2RPM_Step7	},	// Level 6 	
	//{7,127, 55,	48,	&Fan2On_Step8,	&Fan2Off_Step8,	&Fan2RPM_Step8	},	// Level 7 	
};

const thermalstruct code ThermalTalbe2VGA[]= 
{
    {0,	45,	41,	0,	&Fan2OnVGA_Step1,	&Fan2OffVGA_Step1,	&Fan2RPMVGA_Step1	},	// Level 0 	
    {1,	48,	41,	20,	&Fan2OnVGA_Step2,	&Fan2OffVGA_Step2,	&Fan2RPMVGA_Step2	},	// Level 1 	
    {2,	52,	44,	23,	&Fan2OnVGA_Step3,	&Fan2OffVGA_Step3,	&Fan2RPMVGA_Step3	},	// Level 2 	
    {3,	56,	48,	26,	&Fan2OnVGA_Step4,	&Fan2OffVGA_Step4,	&Fan2RPMVGA_Step4	},	// Level 3 	
    {4,	60,	52,	31,	&Fan2OnVGA_Step5,	&Fan2OffVGA_Step5,	&Fan2RPMVGA_Step5	},	// Level 4 	
    {5, 64,	56,	36,	&Fan2OnVGA_Step6,	&Fan2OffVGA_Step6,	&Fan2RPMVGA_Step6	},	// Level 5 	
    {6,128,	60,	42,	&Fan2OnVGA_Step7,	&Fan2OffVGA_Step7,	&Fan2RPMVGA_Step7	},	// Level 6 	
    //{7,127, 55, 48, &Fan2OnVGA_Step8,	&Fan2OffVGA_Step8,	&Fan2RPMVGA_Step8	},	// Level 7 		
};


/*
*******************************************************************************
* Function name: Check_FANRPM_Count      
*
* Descriptoin: Check Fan RPM count used.
*
* Invoked by: CheckFanRPM1(),CheckFanRPM2()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Check_FANRPM_Count(BYTE *BRPM_Cnt, BYTE TempType)
{
		if ( *BRPM_Cnt >= Fan_RPM_Cnt )	// Read 500ms*8 times: 4S
		{
                 if ( TempType == TEMP_TYPE_FAN1RPM )
				 { 
					Fan1RPM = Fan1RPMTemp; 
				 }	
				 else if( TempType == TEMP_TYPE_FAN2RPM ) 
				 { 
				     Fan2RPM = Fan2RPMTemp;
				 }	

				 *BRPM_Cnt = 0;
		}
		else
		{ (*BRPM_Cnt)++; }
}


/*
*******************************************************************************
* Function name: CheckFanRPM1      
*
* Descriptoin: Check Fan RPM 1.
*
* Invoked by: Hook_Timer50msEventC()
*
* TimeDiv：50ms
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void CheckFanRPM1(void)
{
	BYTE FanLevel_tcpu = FanLevelCPU;	
	BYTE FanLevel_tvga = FanLevelVGA;

	if((Debugtemperature1==0x00)&&(Debugtemperature2==0x00))
	{
		temperature1 = TEMP_Buff_3;
		temperature2 = VGA_TBuff3;
	}
	else
	{
		temperature1 = Debugtemperature1;
		temperature2 = Debugtemperature2;
	}
// check CPU fan table1:		
	if(FanLevel_tcpu <((sizeof(ThermalTalbe1CPU)/sizeof(thermalstruct))-1))
	{
		if(temperature1>= *ThermalTalbe1CPU[FanLevel_tcpu].FanOn)          //COKEYXU031: Modify to >= as Thermal request.
		{
			FanLevel_tcpu += 1;
		}
	}
	
	if (FanLevel_tcpu>0)
	{
		if (temperature1<= *ThermalTalbe1CPU[FanLevel_tcpu].FanOff)        //COKEYXU031: Modify to <= as Thermal request.
		{			
			FanLevel_tcpu-=1;
		}
	}
// check VGA fan table1:
	if(FanLevel_tvga <((sizeof(ThermalTalbe1VGA)/sizeof(thermalstruct))-1))
	{
		if(temperature2>= *ThermalTalbe1VGA[FanLevel_tvga].FanOn)           //COKEYXU031: Modify to >= as Thermal request.
		{
			FanLevel_tvga += 1;
		}
	}
	
	if (FanLevel_tvga>0)
	{
		if (temperature2<= *ThermalTalbe1VGA[FanLevel_tvga].FanOff)          //COKEYXU031: Modify to <= as Thermal request.
		{			
			FanLevel_tvga-=1;
		}
	}
	
	FanLevelCPU = FanLevel_tcpu;	
	FanLevelVGA = FanLevel_tvga;	
	if((*ThermalTalbe1CPU[FanLevel_tcpu].RPM)>(*ThermalTalbe1VGA[FanLevel_tvga].RPM))
	   Fan1RPMTemp =*ThermalTalbe1CPU[FanLevel_tcpu].RPM;  
	else
	   Fan1RPMTemp =*ThermalTalbe1VGA[FanLevel_tvga].RPM;  
	   
    if(Fan1RPMTemp == Fan1RPMBackUp)  
    {    
	    Check_FANRPM_Count(&Fan1RPMCount,TEMP_TYPE_FAN1RPM);
    }	
	else
	{
       Fan1RPMBackUp = Fan1RPMTemp;  
	   Fan1RPMCount = 0x00;
	}	
}

/*
*******************************************************************************
* Function name: CheckFanRPM2      
*
* Descriptoin: Check Fan RPM 2.
*
* Invoked by: Hook_Timer50msEventC()
*
* TimeDiv：50ms
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void CheckFanRPM2(void)
{
	BYTE FanLevel_tcpu = FanLevel2CPU;
	BYTE FanLevel_tvga = FanLevel2VGA;

	if((Debugtemperature1==0x00)&&(Debugtemperature2==0x00))
	{
		temperature1 = TEMP_Buff_3;
		temperature2 = VGA_TBuff3;
	}
	else
	{
		temperature1 = Debugtemperature1;
		temperature2 = Debugtemperature2;
	}

// check CPU fan table2:		
	if(FanLevel_tcpu <((sizeof(ThermalTalbe2CPU)/sizeof(thermalstruct))-1))
	{
		if(temperature1>= *ThermalTalbe2CPU[FanLevel_tcpu].FanOn)                  //COKEYXU031: Modify to >= as Thermal request.
		{
			FanLevel_tcpu+= 1;
		}
	}
	
	if (FanLevel_tcpu>0)
	{
		if (temperature1<= *ThermalTalbe2CPU[FanLevel_tcpu].FanOff)                 //COKEYXU031: Modify to <= as Thermal request.
		{			
			FanLevel_tcpu-=1;
		}
	}
// check VGA fan table2:
	if(FanLevel_tvga <((sizeof(ThermalTalbe2VGA)/sizeof(thermalstruct))-1))
	{
		if(temperature2>= *ThermalTalbe2VGA[FanLevel_tvga].FanOn)                  //COKEYXU031: Modify to >= as Thermal request.
		{
			FanLevel_tvga+= 1;
		}
	}
	
	if (FanLevel_tvga>0)
	{
		if (temperature2<= *ThermalTalbe2VGA[FanLevel_tvga].FanOff)               //COKEYXU031: Modify to <= as Thermal request.
		{			
			FanLevel_tvga-=1;
		}
	}


	FanLevel2CPU = FanLevel_tcpu;	
	FanLevel2VGA= FanLevel_tvga;	
	if((*ThermalTalbe2CPU[FanLevel_tcpu].RPM)>(*ThermalTalbe2VGA[FanLevel_tvga].RPM))
	   Fan2RPMTemp =*ThermalTalbe2CPU[FanLevel_tcpu].RPM;   
	else
	   Fan2RPMTemp =*ThermalTalbe2VGA[FanLevel_tvga].RPM;    

    if(Fan2RPMTemp == Fan2RPMBackUp)  
    {
	    Check_FANRPM_Count(&Fan2RPMCount,TEMP_TYPE_FAN2RPM); 
    }
	else
	{
	   Fan2RPMBackUp = Fan2RPMTemp;  
	   Fan2RPMCount = 0x00;
	}
}

/*
*******************************************************************************
* Function name: Fan1MainControl      
*
* Descriptoin: Fan1 main control.
*
* Invoked by: FanControl()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Fan1MainControl(void)
{

	if(((Fan1RPM*100)-100)>RPM1)
	{
		if(FAN_PWM<FAN_PWM_Max)
		FAN_PWM++;	
	}
	else if(((Fan1RPM*100)+100)<RPM1)
	{
		if(FAN_PWM>0x00)
		FAN_PWM--;	
	}
}

/*
*******************************************************************************
* Function name: Fan2MainControl      
*
* Descriptoin: Fan2 main control.
*
* Invoked by: Fan1Control()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Fan2MainControl(void)
{
	if(((Fan2RPM*100)-100)>RPM2)
	{
		if(FAN2_PWM<FAN_PWM_Max)
		FAN2_PWM++;	
	}
	else if(((Fan2RPM*100)+100)<RPM2)
	{
		if(FAN2_PWM>0x00)
		FAN2_PWM--;	
	}			
}


/*
*******************************************************************************
* Function name: FanControl      
*
* Descriptoin: Fan1 RTM control.
*
* Invoked by: FanManager()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void FanControl(void)
{
	if(	RPM1==0x0000)
	{
		FAN_PWM = FanInitValue;
	}
	Fan1MainControl(); 
}

/*
*******************************************************************************
* Function name: Fan2Control      
*
* Descriptoin: Fan2 RTM control.
*
* Invoked by: FanManager()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Fan2Control(void)
{
	if(	RPM2==0x0000)
	{
		FAN2_PWM = FanInitValue;
	}
	Fan2MainControl(); 
}

/*
*******************************************************************************
* Function name: InitThermalTable1      
*
* Descriptoin: Initialize thermal table1.
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
void InitThermalTable1(void)
{
	BYTE index;
	index=0x00;

	if(IS_MASK_CLEAR(pProject0,b4VGAType)) 
	{
	     while(index<(sizeof(ThermalTalbe1CPU_DIS)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe1CPU[index].FanOn=ThermalTalbe1CPU_DIS[index].CFanOn;
		    *ThermalTalbe1CPU[index].FanOff=ThermalTalbe1CPU_DIS[index].CFanOff;
		    *ThermalTalbe1CPU[index].RPM=ThermalTalbe1CPU_DIS[index].CRPM;
		    index++;
	    }
	}
	else		
	{
	    while(index<(sizeof(ThermalTalbe1CPU)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe1CPU[index].FanOn=ThermalTalbe1CPU[index].CFanOn;
		    *ThermalTalbe1CPU[index].FanOff=ThermalTalbe1CPU[index].CFanOff;
		    *ThermalTalbe1CPU[index].RPM=ThermalTalbe1CPU[index].CRPM;
		    index++;
	    }
	}
    index=0x00;
      while(index<(sizeof(ThermalTalbe1VGA)/sizeof(thermalstruct)))
      {
	     *ThermalTalbe1VGA[index].FanOn=ThermalTalbe1VGA[index].CFanOn;
	     *ThermalTalbe1VGA[index].FanOff=ThermalTalbe1VGA[index].CFanOff;
	     *ThermalTalbe1VGA[index].RPM=ThermalTalbe1VGA[index].CRPM;
	     index++;
      } 
}

/*
*******************************************************************************
* Function name: InitThermalTable1      
*
* Descriptoin: Initialize thermal table2.
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
void InitThermalTable2(void)
{
	BYTE index;
	index=0x00;	
	if(IS_MASK_CLEAR(pProject0,b4VGAType)) 
	{
	    while(index<(sizeof(ThermalTalbe2CPU)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe2CPU[index].FanOn=ThermalTalbe2CPU_DIS[index].CFanOn;
		    *ThermalTalbe2CPU[index].FanOff=ThermalTalbe2CPU_DIS[index].CFanOff;
		    *ThermalTalbe2CPU[index].RPM=ThermalTalbe2CPU_DIS[index].CRPM;
		    index++;
	    }
	}
	else
	{
	    while(index<(sizeof(ThermalTalbe2CPU)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe2CPU[index].FanOn=ThermalTalbe2CPU[index].CFanOn;
		    *ThermalTalbe2CPU[index].FanOff=ThermalTalbe2CPU[index].CFanOff;
		    *ThermalTalbe2CPU[index].RPM=ThermalTalbe2CPU[index].CRPM;
		    index++;
	    }
	}	
    index=0x00;
    while(index<(sizeof(ThermalTalbe2VGA)/sizeof(thermalstruct)))
   {
	   *ThermalTalbe2VGA[index].FanOn=ThermalTalbe2VGA[index].CFanOn;
	   *ThermalTalbe2VGA[index].FanOff=ThermalTalbe2VGA[index].CFanOff;
	   *ThermalTalbe2VGA[index].RPM=ThermalTalbe2VGA[index].CRPM;
	   index++;
   }
}

/*
*******************************************************************************
* Function name: FanFullOff      
*
* Descriptoin: Fan full off control.
*
* Invoked by: FanManager()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void FanFullOff(void)
{
	FAN_PWM = 0x00;
	nAtmFanSpeed = 0;  //COKEYXU028: add for not show 0 fan rpm after fan stop
}

/*
*******************************************************************************
* Function name: Fan2FullOff      
*
* Descriptoin: Fan2 full off control.
*
* Invoked by: FanManager()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Fan2FullOff(void)
{
	FAN2_PWM = 0x00;
	nAtmFan1Speed = 0;  //COKEYXU028: add for not show 0 fan rpm after fan stop
}

/*
*******************************************************************************
* Function name: GetRPM1      
*
* Descriptoin: Get Fan1 RPM
*
* Invoked by: FanManager(),FanSpeedConWhenFanTableNotReady()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void GetRPM1(void)
{
WORD Curr_Fan1Tachometer;
	if(SystemIsS0)
	{
		if ( (F1TMRR == 0) && (F1TLRR == 0) )
		{
			nAtmFanSpeed = 0;
		}
		else
		{
			Curr_Fan1Tachometer = (36000/((WORD)(F1TMRR<<8) + F1TLRR))*60;
			nAtmFanSpeed = ( Curr_Fan1Tachometer / 100 );
            RPM1=Curr_Fan1Tachometer;  
		}
		
	}
	
}

/*
*******************************************************************************
* Function name: GetRPM2      
*
* Descriptoin: Get Fan2 RPM
*
* Invoked by: FanManager(),FanSpeedConWhenFanTableNotReady()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void GetRPM2(void)
{
  WORD Curr_Fan2Tachometer;
	if(SystemIsS0)
	{
		if ( (F2TMRR == 0) && (F2TLRR == 0) )
		{
			nAtmFan1Speed = 0;
		}
		else
		{
			Curr_Fan2Tachometer = (36000/((WORD)(F2TMRR<<8) + F2TLRR))*60;
			nAtmFan1Speed = ( Curr_Fan2Tachometer / 100 );
            RPM2=Curr_Fan2Tachometer;  
		}
		
	}  
}

/*
*******************************************************************************
* Function name: FanSpeedConWhenFanTableNotReady      
*
* Descriptoin: Fan speed control when fan table not ready.
*
* Invoked by: Hook_Timer100msEventC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void FanSpeedConWhenFanTableNotReady(void)
{
  if(ManualFanPRM!=0x00) 	
  {         
            FAN_PWM_ALT;
			GetRPM1();
			if( nAtmFanSpeed==0x0000)
			{
				FAN_PWM = FanInitValue;
			}
			else
			{
				if( nAtmFanSpeed > ManualFanPRM )
			{
				if( FAN_PWM > 0 )
				{ FAN_PWM--; }
			}
			else
			{
				if(FAN_PWM < FAN_PWM_Max)
				{ FAN_PWM++; }
				else
				{ FAN_PWM = FAN_PWM_Max; }
			}
			}
  }

 else
 	{
 	#if !FAN_TABLE_Already
		FAN_PWM_OUT;			// Set FAN_PWM OUTPUT.
		EC_FAN_PWM_HI();
		#endif	
 	}
  
  if(ManualFan2PRM!=0x00) 	
  {
            FAN2_PWM_ALT;
			GetRPM2();
			if( nAtmFan1Speed==0x0000)
			{
				FAN2_PWM = FanInitValue;
			}
			else
			{
				if( nAtmFan1Speed > ManualFan2PRM )
			{
				if( FAN2_PWM > 0 )
				{ FAN2_PWM--; }
			}
			else
			{
				if(FAN2_PWM < FAN_PWM_Max)
				{ FAN2_PWM++; }
				else
				{ FAN2_PWM = FAN_PWM_Max; }
			}
				}
  }
  else
  	{
  	#if !FAN_TABLE_Already
		FAN2_PWM_OUT;			// Set FAN_PWM OUTPUT.
		EC_FAN2_PWM_HI();
		#endif	
  	}
}

/*
*******************************************************************************
* Function name: FanManager      
*
* Descriptoin: Fan Manager.
*
* Invoked by: Hook_Timer100msEventC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void FanManager(void)
{

   // Check VGA and CPU Temperature error bit and full on bit.
    if ( (IS_MASK_SET(ERR_THMSTS, b1VGATempEr)) || 
		      (IS_MASK_SET(Thro_Status2, b4FAN1_FullOn)) )
    {
	    FAN_PWM = FAN2_PWM = Target_Duty = FAN_PWM_Max;	// Fan turn on max.
	    return;
    }
	
	if ( Fan_ON_Count != 0 )					// Turn on Fan x sec.
	{
		Fan_ON_Count--;
		FAN_PWM = FAN2_PWM = Target_Duty = FAN_PWM_Min;	// PWM min. 30%
		return;
	}
	
	if( SystemNotS0 )	// Not S0 status.
	{
		FAN_PWM = FAN2_PWM = Target_Duty = 0;	// Turn off FAN.
		return;
	}

	if(FanFullOnCont!=0x00)
	{
		FanFullOnCont--;
		GetRPM1();
		GetRPM2(); 
		return;
	}

	if(ManualFanPRM==0x00) 			// In normal control mode
	{
		if(Fan1RPM!=0x00)  
		{
			GetRPM1();
			FanControl();
		}
		else
		{
			if(FAN_PWM != 0x00)
			{
				RPM1=0x0000;
				FanFullOff();
			}
		}
	}
	else											// In RPM debug mode
	{
		GetRPM1();
		if( RPM1==0x0000)
		{
			FAN_PWM = FanInitValue;
		}
		else
		{
			if(((ManualFanPRM*100)-100)>RPM1)
			{
				if(FAN_PWM<FAN_PWM_Max)
				FAN_PWM++; 
			}
			else if(((ManualFanPRM*100)+100)<RPM1)
			{
				if(FAN_PWM>0x00)
				FAN_PWM--; 
			}
		}
	}
   if(ManualFan2PRM==0x00) 			// In normal control mode
	{
		if(Fan2RPM!=0x00)
		{
			GetRPM2();
			Fan2Control();
		}
		else
		{
			if(FAN2_PWM != 0x00)
			{
				RPM2=0x0000;
				Fan2FullOff();
			}
			
		}
	}
	else											// In RPM debug mode
	{
		GetRPM2();
		if( RPM2==0x0000)
		{
			FAN2_PWM = FanInitValue;
		}
		else
		{
			if(((ManualFan2PRM*100)-100)>RPM2)
			{
				if(FAN2_PWM<FAN_PWM_Max)
				FAN2_PWM++; 
			}
			else if(((ManualFan2PRM*100)+100)<RPM2)
			{
				if(FAN2_PWM>0x00)
				FAN2_PWM--; 
			}
		}
	}
}

/*
*******************************************************************************
* Function name: Fan_Init      
*
* Descriptoin: Fan Initialization.
*
* Invoked by: SetS0GPIO()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void Fan_Init(void)
{
	nFanStatus1 = Fan_Num;
	nFanStatus1 = (nFanStatus1 << 4) | Fan_Step;
	nOSThrottlingTemp = 97;
	nOSShutdownTemp = 100;
	nShutDownTemp = 100;
}

/*
*******************************************************************************
* Function name: ThrottlingControl      
*
* Descriptoin: Chekc all throttling status.
*
* Invoked by: Hook_Timer100msEventC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void ThrottlingControl(void)
{
	if ( SystemIsS0 )
	{
		cThrottlingSet.byte &= 0xF0;	// Clear Throttling status.

		if( ( cThrottlingSet.byte & 0x0F ) < cADPThrottling )	// Compare current and ADP Throttling 
		{
			cThrottlingSet.byte |= cADPThrottling;
		}
		if( ( cThrottlingSet.byte & 0x0F ) < cTHERMThrottling )	// Compare current and Thermal throttling./
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cTHERMThrottling;
		}
		if( ( cThrottlingSet.byte & 0x0F ) < cBATTThrottling )	// Compare current and BAT throttling.
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cBATTThrottling;
		}
		
		if( ( cThrottlingSet.byte & 0x0F ) < cBATTLowThrottling )	// Compare current and BAT throttling.
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cBATTLowThrottling;
		}
		
		if( ( cThrottlingSet.byte & 0x0F ) < nThrottlingAPSet )	// For AP or debug 
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= nThrottlingAPSet;
		}

		if( ( cThrottlingSet.byte & 0x0F ) != ( REAL_THROTTLING_INDEX & 0x0F ) )
		{	// Cpu throttling for power status change
			if( ( (nRealThermalPinGET) && (( cThrottlingSet.byte & 0x0F ) >= 0x01 ))|| (!nRealThermalPinGET) )
			{
				cThrottlingSet.fbit.cTHRM_SW = 1;
                RamDebug(cADPThrottling);       
                RamDebug(cTHERMThrottling);       
                RamDebug(cBATTThrottling);        
                RamDebug(REAL_THROTTLING_INDEX);        
				REAL_THROTTLING_INDEX = (REAL_THROTTLING_INDEX & 0xF0) | (cThrottlingSet.byte & 0x0F);
                RamDebug(REAL_THROTTLING_INDEX);      
			}
		}
		
		if(CPUThrottlingDelayTime == 0) //send the 0x1D event after delay time.
		{
			if( cThrottlingSet.fbit.cTHRM_SW )
			{
				cThrottlingSet.fbit.cTHRM_SW = 0;
				if ( IS_MASK_SET(SYS_MISC1, ACPI_OS) )	// Check OS mode.
				{
			    	RamDebug(0xEE);  RamDebug(CPU_SLOW_AD);       
					ECQEvent(CPU_SLOW_AD); 	// 0x1D inform bios.
				}
			}
		}
	}
}

/*
*******************************************************************************
* Function name: ThrottlingControl      
*
* Descriptoin: GPU throttling control function.
*
* Invoked by: Hook_Timer100msEventC()
*
* TimeDiv：NA
*
* Arguments: NA
*
* Return Values: NA
*******************************************************************************
*/
void GPUThrottlingControl(void)
{

	if ( SystemIsS0 && (CPU_TYPE&0x80) )
	{

		cGPUThrottlingSet.byte &= 0xF0;	// Clear Throttling status.

		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattThrottling)	// Compare battery power Throttling 
		{
			cGPUThrottlingSet.byte |= cGPUBattThrottling;
		}

		//add battery throttling level when PnP AC.
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUACtoBattThrottling)	// Compare PnP AC battery Throttling 
		{
			cGPUThrottlingSet.byte |= cGPUACtoBattThrottling;
		}

		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattOTPThrottling)	// Compare battery OTP Throttling 
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUBattOTPThrottling;
		}
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattLowThrottling)	// Compare battery RSOC low Throttling 
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUBattLowThrottling;
		}
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUThermalThrottling)	// Compare Thermal throttling
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUThermalThrottling;
		}
		//COKEYXU046: add start
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattLTPThrottling)	// Compare battery LTP throttling
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUBattLTPThrottling;
		}
		//COKEYXU046: add end
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattPsysThrottling)	// Compare system power consumption throttling
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUBattPsysThrottling;
		}


		if( ( cGPUThrottlingSet.byte & 0x0F ) != ( GPU_REAL_THROTTLING_INDEX & 0x0F ) )
		{	// send GPU throttling event.
			GPU_REAL_THROTTLING_INDEX = (GPU_REAL_THROTTLING_INDEX & 0xF0) | (cGPUThrottlingSet.byte & 0x0F);
			RamDebug(0xEE); 
			RamDebug(GPU_SLOW);  
			ECQEvent(GPU_SLOW); 	// 0x1E inform bios.
		}
	}

}

