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



/*****************************************************************************/
// Procedure: PWM_TimeCount									TimeDiv: 50mSec
// Description: PWM Beep function.
// GPIO:
// Referrals: Service short time PWM output, base on 50ms.
/*****************************************************************************/
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

/*****************************************************************************/
// Procedure: ThmIC_Temperature								TimeDiv: 500mSec
// Description: Read RAM temperature
// GPIO:
// Referrals:
/*****************************************************************************/
extern void TempProtect_VRAM(void);
void ThmIC_Temperature(void)
{
	BYTE BsData,BTmlCmd;

	if( SystemNotS0 || (PwrOnDly5Sec!=0) )	 //T089: add PwrOnDly5Sec judge.
	{ return; }
	//Tml_SMLink();
	//TempProtect_VRAM();   
    //return; 

	switch( TmlICStep & 0x03 )  //  Change TmlICStep from 7 to 3
	{
	case 0:
		BTmlCmd = VRAM_Temp;
		break;
	/*case 1:
	BTmlCmd = SSD_Temp; 
	BTmlCmd =TmlIC_Temp;
	break;*///Remove thermal IC read remote 2 temperature
	case 1:
		BTmlCmd =TmlIC_Temp;
		break;
	//case 3:  //change  2 to 3
		//Tml_SMLink();
		//return;
		//break;
	default:
		TmlICStep = 0;
		return;
	}
	//bWSMBusBlock(SMbusCh4, SMbusWBK, TmlIC_Addr, 0x01, &BATchallenger, 20, TRUE);

	if(!bRWSMBus(SMbusCh4, SMbusRB, TmlIC_Addr, BTmlCmd, &BsData, 0))  
	//if((!bRWSMBus(SMbusCh4, SMbusRB, TmlIC_Addr, BTmlCmd, &BsData, 0))&&MBID_READY&&(!((uMBID&0x10)&&(uMBID&0x20)))&&((uMBID&0x20)&&(!(uMBID&0x10))&&(VRAM_Temp==BTmlCmd)))
	{      //  1. SMBus fail; 2. MBID OK; 3.  no UMA 37W ; 4.  no UMA 47W VRAM sensor  
		TMErrCnt++;
		if( IS_MASK_CLEAR(ERR_THMSTS, b0ThmICError) )// Check bit0 is Error.
		{
			if ( TMErrCnt > 7 )		// 3 Sec.
			{
				SET_MASK(ERR_THMSTS, b0ThmICError);	// Set bit0 Thermal IC read temperature error.
			}
		}
		else
		{
			if ( TMErrCnt > 68 )	// 30 Sec.
			{
				ProcessSID(THERMALCOMMFAIL_ID);	// 0x0B shutdown ID.
				TMErrCnt = 0;					// Clear error count.
				//RSMRST_LOW();  
				//Delay1MS(1); //ANGELAS050:remove
				//RSMRST_HI();   
				//TmlICStep++;
			}
		}
		ResetSMBus(SMbusCh4);
	}
	else
	{ 
//ANGELAG023: remove start
	/*	if(BsData == 0x00)	// read 0'C  
		//if((BsData == 0x00)&&MBID_READY&&(!((uMBID&0x10)&&(uMBID&0x20)))&&((uMBID&0x20)&&(!(uMBID&0x10))&&(VRAM_Temp==BTmlCmd)))
		{
			TMErrCnt++;
			if( IS_MASK_CLEAR(ERR_THMSTS, b0ThmICError) )// Check bit0 is Error.
			{
				if ( TMErrCnt > 7 )		// 3 Sec.
				{
					SET_MASK(ERR_THMSTS, b0ThmICError);	// Set bit0 Thermal IC read temperature error.
				}
			}
			else
			{
				if ( TMErrCnt > 68 )	// 30 Sec.
				{
					ProcessSID(THERMALCOMMFAIL_ID);	// 0x0B shutdown ID.
					TMErrCnt = 0;					// Clear error count.
				    //RSMRST_LOW(); 
				    //Delay1MS(1); //ANGELAS050:remove
				    //RSMRST_HI();  
				    //TmlICStep++;
				}
			}
		}
		
		else
		
		{
		*/ //ANGELAG023: remove end
			switch( TmlICStep & 0x03 )
			{
			case 0:
				nVramTemp = BsData;	// Save VRAM temperature.
				TMErrCnt = 00;		// Clear error count.
				CLEAR_MASK(ERR_THMSTS,b0ThmICError);	// Clear bit0.
				TmlICStep++;		// next step 1.
				break;
			/*case 1:
				nNBTemp = BsData;	// Save SSD temperature.
				EXTVGA_TEMP=nNBTemp; //T034+
				TMErrCnt = 00;		// Clear error count.
				CLEAR_MASK(ERR_THMSTS,b0ThmICError);	// Clear bit0.
				TmlICStep++;		// next step 2.
				break;*/      //Remove thermal IC read remote 2 temperature
			case 1:
				nRamTemp = BsData;	// Save RAM temperature.
				TMErrCnt = 00;		// Clear error count.
				CLEAR_MASK(ERR_THMSTS,b0ThmICError);	// Clear bit0.
				TmlICStep++;		// next step 2.
				break;
			default:
				break;
			}
		// } //ANGELAG023: remove
	}

}
//-----------------------------------------------------------------------------
// Service short time PWM output, base on 50ms.
//-----------------------------------------------------------------------------
/* //ANGELAS050:remove start
void Tml_SMLink(void)
{
	if( !bRSMBusBlock(SMbusCh4, SMbusRBK, 0x96, 0x40, &MaxCPU_MCHTemp))//, 0x14, TRUE))
	{
		TMErrCnt++;
		if( IS_MASK_CLEAR(ERR_THMSTS, b0ThmICError) )// Check bit0 is Error.
		{
			if ( TMErrCnt > 35 )	//change 7 to 35	// 3 Sec.
			{
				SET_MASK(ERR_THMSTS, b0ThmICError);	// Set bit0 Thermal IC read temperature error.
			}
		}
		else
		{
			if ( TMErrCnt > 254 ) //change 68 to 254   	// 30 Sec.
			{
				//ProcessSID(THERMALCOMMFAIL_ID);	// 0x0B shutdown ID.
				TMErrCnt = 0;					// Clear error count.
				//RSMRST_LOW();
				//Delay1MS(1);
				//RSMRST_HI();
			}
		}
		ResetSMBus(SMbusCh4);
	}
	else
	{
		nOSShutdownTemp3 = MaxCPU_MCHTemp;		// Save PCH temperature.
		CLEAR_MASK(ERR_THMSTS,b0ThmICError);	// Clear bit0.
		TMErrCnt = 0;							// Clear error count.
		TmlICStep = 0;							// Reset Step.
	}
}
*///ANGELAS050:remove end
/*****************************************************************************/
// Procedure:  							TimeDiv: 500mSec
// Description:  
// GPIO:  
// Referrals:
/*****************************************************************************/
/* ANGELAS050:remove start
void TempProtect_VRAM(void)
{
		BYTE BsData,BTmlCmd;
		BTmlCmd = SSD_Temp;//change SSD_Temp to SSD_Temp
	


		//if((!bRWSMBus(SMbusCh4, SMbusRB, TmlIC_Addr, BTmlCmd, &BsData, 0))&&MBID_READY&&(!(uMBID&0x20)))
		if(!bRWSMBus(SMbusCh4, SMbusRB, TmlIC_Addr, BTmlCmd, &BsData, 0))// REMOVE MBID_READY judge
		{	 
			TMErrCnt++;
			if( IS_MASK_CLEAR(ERR_THMSTS, b0ThmICError) )// Check bit0 is Error.
			{
				if ( TMErrCnt > 7 ) 	// 3 Sec.
				{
					SET_MASK(ERR_THMSTS, b0ThmICError); // Set bit0 Thermal IC read temperature error.
				}
			}
			else
			{
				if ( TMErrCnt > 68 )	// 30 Sec.
				{
					//ProcessSID(THERMALCOMMFAIL_ID); // 0x0B shutdown ID.
					TMErrCnt = 0;					// Clear error count.
					//RSMRST_LOW();  
					//Delay1MS(1);
					//RSMRST_HI();
				}
			}
			ResetSMBus(SMbusCh4);
		}
		else
		{ 
		if(BsData == 0x00)// REMOVE MBID_READY judge
			//if((BsData == 0x00)&&MBID_READY&&(!(uMBID&0x20)))
			{
				TMErrCnt++;
				if( IS_MASK_CLEAR(ERR_THMSTS, b0ThmICError) )// Check bit0 is Error.
				{
					if ( TMErrCnt > 7 ) 	// 3 Sec.
					{
						SET_MASK(ERR_THMSTS, b0ThmICError); // Set bit0 Thermal IC read temperature error.
					}
				}
				else
				{
					if ( TMErrCnt > 68 )	// 30 Sec.
					{
						//ProcessSID(THERMALCOMMFAIL_ID); // 0x0B shutdown ID.
						TMErrCnt = 0;					// Clear error count.
						  // RSMRST_LOW(); 
						 //  Delay1MS(1);
						 //  RSMRST_HI();	
					}
				}
			}
			else
			{

						//nVramTemp = BsData; // Save VRAM temperature.  
						nNBTemp=BsData;  
						TMErrCnt = 00;		// Clear error count.
						CLEAR_MASK(ERR_THMSTS,b0ThmICError);	// Clear bit0.
						if(nNBTemp>=85) //change nVramTemp to nNBTemp
						{
						  // ProcessSID(ThermalICOVerTEPM_ID); // 0x0B shutdown ID.
						   //RSMRST_LOW(); 
						  // Delay1MS(1);
						  // RSMRST_HI();		
						}
			}
		  }
}

*///ANGELAS050:remove end
/*****************************************************************************/
// Procedure: VGA_Temperature								TimeDiv: 500mSec
// Description: Read VGA temperature(M/B)
// GPIO: GPIOF6(118)
// Referrals:
/*****************************************************************************/
void VGA_Temperature(void)
{
	BYTE BsData;
	//Add read AMD GPU thermal temperature  start
//	BYTE INIT_GPU_REG2[4] = {0x0f, 0x00, 0x01, 0xC5}; //ANGELAG016: remove
//ANGELAG016: add start
	 BYTE INIT_GPU_REG2[4] = {0x0f, 0x00, 0x00, 0x8f};
  	BYTE INIT_GPU_REG1[4] = {0x0f, 0x00, 0x00, 0x8e};
	BYTE INIT_GPU_REG0[4] = {0xc0, 0x30, 0x00, 0x14};
//ANGELAG016: add end
	if ( (CPU_TYPE & 0xC0) == 0x00 )	// Isn't check VGA support optimus?
	{ nHybridGraphicsDIS;}				// No optimus status.
 	else
 	{ nHybridGraphicsEN; }				// optimus status.

	//if( SystemNotS0 || (PwrOnDly5Sec!=0) )	 //T089: add PwrOnDly5Sec judge.

	// Check S0 and  VGA into.
	if( SystemNotS0 || !(nHybridGraphicsGET)|| (PwrOnDly5Sec!=0) ) //== to != // add PwrOnDly5Sec judge.modify nHybridGraphicsGET not take a reverse value 
	{  
        VGAErrCnt = 00;  
        //VGA_TEMP=0; //ANGELAG018: remove
        //VGA_TBuff3=0; //ANGELAG018: remove
	VGA_TEMP = VGA_TBuff3 = VGA_TBuff2 = VGA_TBuff1 = 0x00; //ANGELAG018: add
        CLEAR_MASK(ERR_THMSTS,b1VGATempEr); 
        return; 
    }
     
	if((CPU_TYPE &0xc0)==0x40)
	{
//ANGELAG016: add start
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
//ANGELAG016: add end
//ANGELAG016: remove start
	/*	if(!bWSMBusBlock(SMbusCh4, SMbusWBK, VGA_Addr, 0x01, &INIT_GPU_REG2, 4,0))
		{
			VGAErrCnt++;
		}*/
//ANGELAG016: remove end
			
		//vgaok=1;
		
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
		//sdAMDTH0=sdAMDTH0<<24;
		else
		{
			//VGA_TEMP=sdAMDTH0&0xff; //ANGELAG018: remove
			VGA_TEMP=((sdAMDTH2>>1)+((sdAMDTH3<<8)&0XFF)); //ANGELAG018: add
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
	//Add read AMD GPU thermal temperature end
	if((CPU_TYPE &0xc0)==0x80)
	{
		if(!bRWSMBus(SMbusCh4, SMbusRB, EXTVGA_Addr, 0x00, &BsData, 0))// REMOVE MBID_READY judge

		//if((!bRWSMBus(SMbusCh4, SMbusRB, VGA_Addr, TmlIC_Temp, &BsData, 0))&&MBID_READY&&(!(uMBID&0x20))) //add MBID judge.
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
		/* //ANGELAG023: remove start
			if(BsData == 0x00)	// read 0'C  // REMOVE MBID_READY judge
			//if((BsData == 0x00)&&MBID_READY&&(!(uMBID&0x20)))// read 0'C 
			{
            	VGA_TEMP = BsData;      
				VGAErrCnt++;
				if( IS_MASK_CLEAR(ERR_THMSTS, b1VGATempEr) )// Check bit1 is Error.
				{
					if ( VGAErrCnt > 7 )	// 3 Sec.
					{
						SET_MASK(ERR_THMSTS, b1VGATempEr);	// Set bit1 Thermal IC read temperature error.
					}
				}
				else
				{
					if ( VGAErrCnt > 68 )			// 30 Sec.
					{
						SET_MASK(SysStatus,ERR_ShuntDownFlag);
						ProcessSID(VGACOMMFAIL_ID);	// 0x20 shutdown ID.
						VGAErrCnt = 00;				// Clear error count.
						RSMRST_LOW();
						Delay1MS(1);
						RSMRST_HI();
					}
				}
			}
			else
			
			{
			*/ ////ANGELAG023: remove end
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
			// }  ////ANGELAG023: remove
		}
	}
    
}



/*****************************************************************************/
// Procedure: Oem_Thermal_Control								TimeDiv: 1 Sec
// Description: Read CPU temperature
// GPIO: GPIOF6(118)
// Referrals:
/*****************************************************************************/
void Oem_Thermal_Control(void)
{
	//if(SystemIsS0&& (PwrOnDly5Sec==0)) //add PwrOnDly5Sec judge.
	//if(SystemIsS0&& (PwrOnDly5Sec==0)&&Read_SLPS3() && Read_SLPS4()&&(PECIBIOSTEST==0)) //add PwrOnDly5Sec judge.//owhen enter S3/S4/CB,BIOS sent B4 CMD to EC from 66port for peci fail.// Remove PECI workaround.
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









/*****************************************************************************/
// Procedure: OEM_Throttling_Ctrl								TimeDiv: 1 Sec
// Description: System CPU/GPU Throttling Data
// GPIO:
// Referrals:  CPU/GPU Temperature
/*****************************************************************************/
//-----------------------------------------------------------------------------
/*const CPUTHRstruct code CPUTHRTable[]= //ANGELAG006: remove start
{ //      Thr_Off  Thr_On  Turbo_Rem   Turbo_Off    Thr_Shut	       Step  Tjxx  Throttling Table
//-----------------------------------------------------------------------------
	{	90, 	98, 		55, 	60, 		100 },	// 0 for DIS CPU 14'  //MEILING019:modify fan table start.  //MEILING024:change cpu turbo off from 70 to 65.  //MEILING040:modify turbo on/off start.
	{	92, 	99, 		57, 	61, 		105 },	// 1 for DIS GPU 14'
	
	{	90, 	98, 		59, 	64, 		100 },	// 2 for UMA CPU 14'  //MEILING024:change cpu turbo on from 60 to 65. //MEILING033:modify turbo on/off.
	{	100,	100,		100, 	100, 		100 },  // 3 for UMA GPU 14'

	{	90, 	98, 		55, 	60, 		100 },	// 4 for DIS CPU 15'  //MEILING024:change cpu turbo off from 70 to 65.  //MEILING033:modify turbo on/off.
	{	92, 	99, 		57, 	61, 		105 },	// 5 for DIS GPU 15'  //MEILING033:modify turbo on/off.
	
	{	90, 	98, 		58, 	62, 		100 },	// 6 for UMA CPU 15'  //MEILING024:change cpu turbo off from 60 to 65.  //MEILING033:modify turbo on/off.  //MEILING040:modify turbo on/off end.
	{	100,	100,		100, 	100, 		100 },  // 7 for UMA GPU 15'

	{	90,		98,			55,		60,			100	},  // 8 for DIS CPU 17'
	{	92,	    99,	    	57,		61,			105	},  // 9 for DIS GPU 17'

	{	90,		98,			58,		62,			100	},  // 10 for UMA CPU 17'
	{	100,	100,		100,	100,		100	}   // 11 for UMA GPU 17'  //MEILING019:modify fan table end.

};*/ //ANGELAG006: remove end

//ANGELAG006: add start
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
//ANGELAG006: add end

void OEM_Throttling_Ctrl(void)
{
	BYTE BTemp_Type,BTbStep;
	
	BTbStep = 0;//MEILING023:add.

	if( SystemNotS0 ) // modify cpu keep max p_state,when battery RSOC>5% under DC mode.
	//if( (SystemNotS0&&IS_MASK_SET(SYS_STATUS,AC_ADP))|| ((BAT1PERCL>0x05)&&IS_MASK_CLEAR(SYS_STATUS,AC_ADP))) 
	{
	    H_PROCHOT_OFF();
		return;
	}
#if FAN_TABLE_Already
	//MEILING019:modify fan table start.
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
	//MEILING019:modify fan table end.
	
	//if ( IS_MASK_CLEAR(SYS_MISC1, ACPI_OS) )
	//{ Thro_DOS_Mode(BTbStep); }
	//  always std mode.
    // else
    // { 
		Thro_Std_Mode(BTbStep); 
    // }  

 	/*  
	else if ( IS_MASK_SET(EM7FUNCTION, QIUETMODE) )			// bit5:QIUETMODE, Check Quite mode.
	{ Thro_Quite_Mode(BTbStep); } 						// Quite
	else if ( IS_MASK_SET(EM7FUNCTION, LSPRUNNING) )	// bit3:LSPRUNNING, Check Super performance mode.
	{ Thro_Super_Mode(BTbStep); } 				// Super performance Mode.
	else
	{ Thro_Std_Mode(BTbStep); }
 	*/  
	nOSThrottlingTemp = THR_PRO_ON;	// For OS throttling temperature.
	nOSShutdownTemp = THR_Tab_Shut;	// For OS Shutdown temperature.
	nShutDownTemp = THR_Tab_Shut;	// Shutdown temperature.
#endif 
}

//-----------------------------------------------------------------------------
// Throttling: Standard Mode.
//-----------------------------------------------------------------------------
void Thro_Std_Mode(BYTE BMBStep)
{
/* ANGELAG046: remove start
//	switch( (CPU_TYPE & 0x03) ) //ANGELAG018: remove start
//	{
//	#if Support_TJ100
//	case 2:	 //ANGELAG018: remove end	// Tj100 Step8 and VGA Step16.
		THRTab_ToRAM((0+BMBStep));	//:8 to 0  // Throttle Standard table save to RAM.
		Thro_Shut_Status((0+BMBStep),TEMP_TYPE_CPU); //:8 to 0  // Check Throttle and Shutdown status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(0+BMBStep)].Turbo_Rem,CPUTHRTable[(0+BMBStep)].Turbo_Off,TEMP_TYPE_CPU); //:8 to 0 
	//	break; //ANGELAG018: remove start
	//#endif	// Support_TJ100

	//default:	// Tj90 Step4 and VGA Step16 Default.
	//	THRTab_ToRAM((0+BMBStep));//T14H:4 to 2	// Throttle Standard table save to RAM.
	//	Thro_Shut_Status((0+BMBStep),TEMP_TYPE_CPU); 	//:4 to 2		// Check Throttle and Shutdown status.
	//	// Check CPU Turbo function.
	//	Thro_Turbo(CPUTHRTable[(0+BMBStep)].Turbo_Rem,CPUTHRTable[(0+BMBStep)].Turbo_Off,TEMP_TYPE_CPU); //:4 to 2	
	//	break;
	//} //ANGELAG018: remove end

	VTHRTab_ToRAM((1+BMBStep));//:16 to 4	// VGA Throttle Standard table save to RAM.
	Thro_Shut_Status((1+BMBStep),TEMP_TYPE_VGA); //:16 to 4  // Check Throttle and Shutdown status.
	// Check GPU Turbo function.
	Thro_Turbo(CPUTHRTable[(1+BMBStep)].Turbo_Rem,CPUTHRTable[(1+BMBStep)].Turbo_Off,TEMP_TYPE_VGA);//:16 to 4

    Thro_Shut_Status(0,TEMP_TYPE_local);//:Add thermal IC local and remote  over temperature protect.
    if(uMBGPU&0x02)//DIS  //ANGELAG012:add.
    {
    Thro_Shut_Status(0,TEMP_TYPE_remote);//:Add thermal IC local and remote  over temperature protect.
    }
*/ //ANGELAG046: remove end

	//ANGELAG046: add start
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

	////ANGELAG046: add end

}

//COKEYXU044: add disable CPU turbo for 90s function
void disCPUturbo90s(void)
{
	if(DisCPUTurboFor90s > 0) //COKEYXU029: add 
	{
		DisCPUTurboFor90s--; 
	}
}

/* //ANGELAS050:remove start
//-----------------------------------------------------------------------------
// Throttling: DOS Mode.
//-----------------------------------------------------------------------------
void Thro_DOS_Mode(BYTE BMBStep)
{
	switch( (CPU_TYPE & 0x03) )
	{
	#if Support_TJ100
	case 2:		// Tj100 Step10.
		THRTab_ToRAM((1+BMBStep));//T14H:10 to 1	// Throttle Standard table save to RAM.
		Thro_Shut_Status((1+BMBStep),TEMP_TYPE_CPU);  //T14H:10 to 1	
		break;
	#endif		// Support_TJ100

	default:	// Tj90 Step6 Default.
		THRTab_ToRAM((3+BMBStep));//:6 to 3	 // Throttle Standard table save to RAM.
		Thro_Shut_Status((3+BMBStep),TEMP_TYPE_CPU);  //:6 to 3
		break;
	}
	VTHRTab_ToRAM((5+BMBStep)); //:18 to 5	// VGA Throttle Standard table save to RAM.
	Thro_Shut_Status((5+BMBStep),TEMP_TYPE_VGA);//:18 to 5	// Check Throttle and Shutdown status.

}
//-----------------------------------------------------------------------------
// Throttling: Quite Mode.
//-----------------------------------------------------------------------------
void Thro_Quite_Mode(BYTE BMBStep)
{
	VTHRTab_ToRAM((17+BMBStep));	// VGA Throttle Standard table save to RAM.
	switch( (CPU_TYPE & 0x03) )
	{
	case 3:		// Tj105 Step13 and VGA Step 17.
		THRTab_ToRAM((13+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((13+BMBStep),TEMP_TYPE_CPU); 	// Check Throttle and Shutdown status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(13+BMBStep)].Turbo_Rem,CPUTHRTable[(13+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;

	#if Support_TJ100
	case 2:		// Tj100 Step9 and VGA Step 17.
		THRTab_ToRAM((9+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((9+BMBStep),TEMP_TYPE_CPU); 		// Check Throttle and Shutdown status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(9+BMBStep)].Turbo_Rem,CPUTHRTable[(9+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;
	#endif		// Support_TJ100

	case 1:		// Tj90 Step5 and VGA Step 17.
	#if Support_TJ85
	#else
	default:	// Tj90 Step5 and VGA Step 17 Default.
	#endif		// Support_TJ85
		THRTab_ToRAM((5+BMBStep));	// Throttle Standard table save to RAM.
		Thro_Shut_Status((5+BMBStep),TEMP_TYPE_CPU); 		// Check Throttle and Shutdown status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(5+BMBStep)].Turbo_Rem,CPUTHRTable[(5+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;

	#if Support_TJ85
	default:	// Tj85 Step1 and VGA Step 17 Default.
		THRTab_ToRAM((1+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((1+BMBStep),TEMP_TYPE_CPU); 		// Check Throttle and Shutdown status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(1+BMBStep)].Turbo_Rem,CPUTHRTable[(1+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;
		#endif	// Support_TJ85
	}
	Thro_Shut_Status((17+BMBStep),TEMP_TYPE_VGA);	// Check Throttle and Shutdown status.
	// Check GPU Turbo function.
	Thro_Turbo(CPUTHRTable[(17+BMBStep)].Turbo_Rem,CPUTHRTable[(17+BMBStep)].Turbo_Off,TEMP_TYPE_VGA);
}



//-----------------------------------------------------------------------------
// Throttling: Super performance Mode.
//-----------------------------------------------------------------------------
void Thro_Super_Mode(BYTE BMBStep)
{
	switch( (CPU_TYPE & 0x03) )
	{
	case 3:	// Tj105 Step15 and VGA Step18.

		THRTab_ToRAM((15+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((15+BMBStep),TEMP_TYPE_CPU);		// Check Throttle and Shutdown status.
		Thro_Street((15+BMBStep),TEMP_TYPE_CPU);		// Check VGA Street status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(15+BMBStep)].Turbo_Rem,CPUTHRTable[(15+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;

	#if Support_TJ100
	case 2:		// Tj100 Step11 and VGA Step18.
		THRTab_ToRAM((11+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((11+BMBStep),TEMP_TYPE_CPU);		// Check Throttle and Shutdown status.
		Thro_Street((11+BMBStep),TEMP_TYPE_CPU);		// Check VGA Street status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(11+BMBStep)].Turbo_Rem,CPUTHRTable[(11+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;
	#endif		// Support_TJ100

	case 1:		// Tj90 Step7 and VGA Step18.
	#if Support_TJ85
	#else
	default:	// Tj90 Step7 and VGA Step18 Default.
	#endif		// Support_TJ85

		THRTab_ToRAM((7+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((7+BMBStep),TEMP_TYPE_CPU);		// Check Throttle and Shutdown status.
		Thro_Street((7+BMBStep),TEMP_TYPE_CPU);		// Check VGA Street status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(7+BMBStep)].Turbo_Rem,CPUTHRTable[(7+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;

	#if Support_TJ85
	default:	// Tj85 Step3 and VGA Step18 Default.
		THRTab_ToRAM((3+BMBStep));			// Throttle Standard table save to RAM.
		Thro_Shut_Status((3+BMBStep),TEMP_TYPE_CPU);		// Check Throttle and Shutdown status.
		Thro_Street((3+BMBStep),TEMP_TYPE_CPU);		// Check VGA Street status.
		// Check CPU Turbo function.
		Thro_Turbo(CPUTHRTable[(3+BMBStep)].Turbo_Rem,CPUTHRTable[(3+BMBStep)].Turbo_Off,TEMP_TYPE_CPU);
		break;
		#endif	// Support_TJ85
	 }

	VTHRTab_ToRAM((19+BMBStep)); 	// VGA Throttle Standard table save to RAM.
	Thro_Shut_Status((19+BMBStep),TEMP_TYPE_VGA);	// Check Throttle and Shutdown status.
	Thro_Street((19+BMBStep),TEMP_TYPE_VGA);		// Check VGA Street status.
	// Check GPU Turbo function.
	Thro_Turbo(CPUTHRTable[(19+BMBStep)].Turbo_Rem,CPUTHRTable[(19+BMBStep)].Turbo_Off,TEMP_TYPE_VGA);
}
*///ANGELAS050:remove end
//-----------------------------------------------------------------------------
// Throttling: Check throttling and Shutdown status.
//-----------------------------------------------------------------------------
void Thro_Shut_Status(BYTE BStep, BYTE BTType)
{
	BYTE BAvgTemp;

	if ( BTType == 2 )
	{	// Set External FAN.
		//if ( EXT_VGA_Buff3 <= CPUTHRTable[BStep].Thr_Off )		// Check prochot turn OFF.
		if ( EXT_VGA_Buff3 < THR_PRO_OFF)		//T14K+
		{ CLR_MASK(Thro_Status, b2ProCH_EXTVGA); }
		//else if ( EXT_VGA_Buff3 >= CPUTHRTable[BStep].Thr_On )	// Check prochot turn ON.
		else if ( EXT_VGA_Buff3 >= THR_PRO_ON)	//T14K+
		{ SET_MASK(Thro_Status, b2ProCH_EXTVGA); }

		//if ( EXT_VGA_Buff3 >= CPUTHRTable[BStep].Thr_Shut )	// Check shutdown status.
		if ( EXT_VGA_Buff3 >=THR_Tab_Shut)	//T14K+
		{ Thro_Count_Shut(&EXTVGA_Shut_Cnt,TEMP_TYPE_EXT); }	// Check x times for shutdown protection.
		else
		{ EXTVGA_Shut_Cnt = 0; }
	}
	else if ( BTType == 1 )
	{	// Set Internal VGA.
		//if ( VGA_TBuff3 <= CPUTHRTable[BStep].Thr_Off )	 	// Check prochot turn OFF.
		 	
		/*if ( VGA_TBuff3 < THR_PRO_OFF)		//+
			{ CLR_MASK(Thro_Status, b1ProCH_VGA); }
			//else if ( VGA_TBuff3 >= CPUTHRTable[BStep].Thr_On )	// Check prochot turn ON.
			else if ( VGA_TBuff3 >= THR_PRO_ON )	//+
		 	 { SET_MASK(Thro_Status, b1ProCH_VGA); }*/
			 //ANGLEAG020: add start
			 if ( VGA_TBuff3 < VTHR_PRO_OFF)		
			{ CLR_MASK(GPU_Prochot, b0_Thermal); }
			else if ( VGA_TBuff3 >= VTHR_PRO_ON )	
		 	 { SET_MASK(GPU_Prochot, b0_Thermal); }
			 //ANGELAG020: add end

			//if ( VGA_TBuff3 >= CPUTHRTable[BStep].Thr_Shut )  	// Check shutdown status.
			if ( VGA_TBuff3 >= VTHR_Tab_Shut )  //T14K+ //G87:Modify the GPU shutdown temperature  registers
			{ Thro_Count_Shut(&VGA_Shut_Cnt,TEMP_TYPE_VGA); }	// Check x times for shutdown protection.
			else
			{ VGA_Shut_Cnt = 0; }
	}
   	else if ( BTType == 4 ) //G90:Add thermal IC local and remote  over temperature protect.
    {
      	// if ( nVramTemp >= nVramoverTemp )  //ANGELAS070:remove
        if ( ThermistorCPU_TEMP >= nCPUthermistoroverTemp )  //ANGELAS070:add
        { Thro_Count_Shut(&local_Shut_Cnt,TEMP_TYPE_local); }   // Check x times for shutdown protection.
        else
       	{ local_Shut_Cnt = 0; }
    }
    else if ( BTType == 5 )//G90:Add thermal IC local and remote  over temperature protect.
    {
       	//if ( nRamTemp >= nRamoverTemp )  //ANGELAS070:remove
	   	if ( EXTVGA_TEMP >= nGPUthermistoroverTemp )  //ANGELAS070:add
        { Thro_Count_Shut(&remote_Shut_Cnt,TEMP_TYPE_remote); }   // Check x times for shutdown protection.
        else
       	{ remote_Shut_Cnt = 0; }
    }
	else
	{	// Set Internal CPU.
		//if ( TEMP_Buff_3 <= CPUTHRTable[BStep].Thr_Off ) //-	// Check prochot turn OFF.
		if ( TEMP_Buff_3 < THR_PRO_OFF) //-
		{ CLR_MASK(Thro_Status, b0ProCH_CPU); }
		//else if ( TEMP_Buff_3 >= CPUTHRTable[BStep].Thr_On )  //- // Check prochot turn ON.
		else if ( TEMP_Buff_3 >= THR_PRO_ON)  //-
		{ SET_MASK(Thro_Status, b0ProCH_CPU); }

		//if ( TEMP_Buff_3 >= CPUTHRTable[BStep].Thr_Shut )	//-// Check shutdown status.
		if ( TEMP_Buff_3 >= THR_Tab_Shut )	//+
		{ Thro_Count_Shut(&CPU_Shut_Cnt,TEMP_TYPE_CPU); }	// Check x times for shutdown protection.
		else
		{ CPU_Shut_Cnt = 0; }
	}
	
}

//-----------------------------------------------------------------------------
// Throttling: Check CPU/VGA Turbo status.
//-----------------------------------------------------------------------------
void Thro_Turbo(BYTE BTurboRem, BYTE BTurboOff, BYTE BTType)
{
	BYTE BAvgTemp;

	if ( IS_MASK_CLEAR(SYS_MISC1, ACPI_OS) ) { return; }
	//if((CPU_TYPE &0xc0)!=0x80){return;}//G45:Add judge AMD GPU and NV GPU, AMD GPU not support torbo.//ANGELAS096:-Add CPU turbo enable/disable Q_event.

	if ( BTType == 2 )
	{	// Set External FAN.
		if ( EXT_VGA_Buff3 <= BTurboRem )				// Check turbo resume.
		{ CLR_MASK(Thro_Status, b5Turbo_EXTVGA); }
		else if ( EXT_VGA_Buff3 >= BTurboOff )			// Check turbo Off.
		{ SET_MASK(Thro_Status, b5Turbo_EXTVGA); }
	}
	else if ( BTType == 1 )
	{	// Set Internal VGA.
		//if ( TEMP_Buff_3 <= BTurboRem ) // Check turbo resume.  //MEILING024:remove.
		//ANGELAG032: add start
		if(Read_AC_IN())
		{
		//ANGELAG032: add end
			if ( VGA_TBuff3 <= BTurboRem ) // Check turbo resume.  //MEILING024:change to VGA temp.
			{ CLR_MASK(Thro_Status, b4Turbo_VGA); }
			//else if ( TEMP_Buff_3 >= BTurboOff )	  // Check turbo Off.  //MEILING024:remove.
			else if ( VGA_TBuff3 >= BTurboOff ) // Check turbo Off.  //MEILING024:change to VGA temp.
			{ SET_MASK(Thro_Status, b4Turbo_VGA); }
		//ANGELAG032: add start
		}
		else
		{
			if ( VGA_TBuff3 <= (BTurboRem-5) ) // Check turbo resume.  //MEILING024:change to VGA temp.
			{ CLR_MASK(Thro_Status, b4Turbo_VGA); }
			//else if ( TEMP_Buff_3 >= BTurboOff )	  // Check turbo Off.  //MEILING024:remove.
			else if ( VGA_TBuff3 >= (BTurboOff-5) ) // Check turbo Off.  //MEILING024:change to VGA temp.
			{ SET_MASK(Thro_Status, b4Turbo_VGA); }
		}
		//ANGELAG032: add end
	}
	else
	{	// Set Internal CPU.

//JERRYCH005:s+Add cpu turbo enable/disable event.		
		if (( TEMP_Buff_3 <= BTurboRem ) /*&&	( VGA_TBuff3 <=80)*/)// Check turbo resume. //COKEYXU027: enable CPU turbo when GPU temp <= 60
		{ CLR_MASK(Thro_Status, b3Turbo_CPU); }
		else
		{
			if (( TEMP_Buff_3 >= BTurboOff )/*||(VGA_TBuff3 >=84)*/)//COKEYXU027: disable CPU turbo when GPU temp > = 65.
			{
				SET_MASK(Thro_Status, b3Turbo_CPU);
			}
		}
	}
		
    if ( IS_MASK_CLEAR(Thro_Status, b3Turbo_CPU ) )/* && (DisCPUTurboFor90s == 0) && (DisCPUTurboForLTP == 0) && IS_MASK_CLEAR(Adapter90WWA,b1_DisCPUturbo)  //remove for L340 test
    && (nAtmFanSpeed > 5))*/ //COKEYXU029: add fan1 speed > 500 and system start after 90s,then enable CPU turbo. //COKEYXU046: when low temp protect disable CPU turbo.
	{                                                                                                                                          //COKEYXU030: add W/A for 90W adapter,cannot charge battery RSOC up when system consumption is very high.              
		if ( IS_MASK_SET(uVGATurboFun,uDisCPUTurboOK) )		
		{
			CLR_MASK(uVGATurboFun, uDisCPUTurboOK);	
            ECQEvent(EN_CPUTURBO_67);  //MEILING036:add.
		}
	}
	else
	{
	 	if ( IS_MASK_CLEAR(uVGATurboFun,uDisCPUTurboOK) )	// bit3.
	 	{
			SET_MASK(uVGATurboFun, uDisCPUTurboOK);			// Set bit3. 
            ECQEvent(DIS_CPUTURBO_66);  //MEILING036:add.
	 	}
	}
//JERRYCH005:e+Add cpu turbo enable/disable event.
}

//-----------------------------------------------------------------------------
// Throttling: Check VGA Street status.
//-----------------------------------------------------------------------------
/* //ANGELAS050:remove start
void Thro_Street(BYTE BStep, BYTE BTType)
{
	#if Support_Steering_Fun
	BYTE BAvgTemp;

	if ( IS_MASK_SET(SYS_MISC1, ACPI_OS) ) { return; }

	if ( BTType == 2 )
	{
		if ( EXT_VGA_Buff3 <= VGA_Steer_On )				// Check VGA steering On.
		{ CLR_MASK(Thro_Status2, b2Steer_EXTVGA); }
		else if ( EXT_VGA_Buff3 >= VGA_Steer_Off )			// Check VGA steering Off.
		{ SET_MASK(Thro_Status2, b2Steer_EXTVGA); }
	}
	else if ( BTType == 1 )
	{
		 if ( VGA_TBuff3 <= VGA_Steer_On )				// Check VGA steering On.
		{ CLR_MASK(Thro_Status2, b1Steer_VGA); }
		else if ( VGA_TBuff3 >= VGA_Steer_Off )
		{ SET_MASK(Thro_Status2, b1Steer_VGA); }	// Check VGA steering Off.
	}
	else
	{	// CPU Type.
		switch( (CPU_TYPE & 0x03) )
		{
		case 3:		// Tj105
		case 2:		// Tj100
			if ( TEMP_Buff_3 <= VGA_Steer_On )		// Check VGA steering On.
			{ CLR_MASK(Thro_Status2, b0Steer_CPU); }
			else if ( TEMP_Buff_3 >= VGA_Steer_Off )// Check VGA steering Off.
			{ SET_MASK(Thro_Status2, b0Steer_CPU); }
			break;
		case 1:		// Tj90
		default:	// Tj85 Default.
			if ( TEMP_Buff_3 <= VGA_Steer85_On )	// Check VGA steering On.
			{ CLR_MASK(Thro_Status2, b0Steer_CPU); }
			else if ( TEMP_Buff_3 >= VGA_Steer_Off )// Check VGA steering Off.
			{ SET_MASK(Thro_Status2, b0Steer_CPU); }
			break;
		}
	}

	if ( IS_MASK_CLEAR(Thro_Status2, (b0Steer_CPU+b1Steer_VGA+b2Steer_EXTVGA)) )
	{
		if ( IS_MASK_SET(uVGATurboFun,uDisPowerSteeringOK) )	// bit1.
		{
			CLR_MASK(uVGATurboFun, uDisPowerSteeringOK);		// Clear bit1.
			//ECQEvent(DIS_TURBO_63); 	// 0x63 Disable VGA Steering. G24:remove GPU turbo Q event
		}
	}
	else
	{
	 	if ( IS_MASK_CLEAR(uVGATurboFun,uDisPowerSteeringOK) )	// bit1.
		{
			SET_MASK(uVGATurboFun, uDisPowerSteeringOK);		// Set bit1.
	 		ECQEvent(EN_PWRSTEER_64); // 0x64 Enable VGA Steering.
	 	}
	}
	#endif	// Support_Steering_Fun
}
*/ //ANGELAS050:remove end
//-----------------------------------------------------------------------------
// Throttling: Shutdown count used.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Throttle table save to ram.
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
// VGA Throttle table save to ram.
//-----------------------------------------------------------------------------
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

/*****************************************************************************/
// Procedure: ODDPowerSaving									TimeDiv: 50mSec
// Description: ODD zero power control.
// GPIO: GPIOA4, GPIOD7
// Referrals:
/*****************************************************************************/
/*//ANGELAS050:remove start
void ODDPowerSaving(void)
{
	#if Support_ODDZeroPower
	// Check S0 status and ODD and ACPI Mode.
	if(IS_MASK_CLEAR(SYS_MISC1, ACPI_OS))
	{ return; }

	if( IS_MASK_SET(LENOVODEVICE, ODDPWRON) )	// Check bit6: ODD power on.
	{
		uVPCeventSource = 0x00;					// for Levono AP used.
		uVPCeventSource2 = VPCeventODD;			// for Levono AP used.
		ECQEvent(SDV_VPC_notify);				// 0x44 for Levono used.
		SET_MASK(uODDPWRsaving, uODD_PWRon);	// Set bit0: uODD_PWRon.
		CLEAR_MASK(LENOVODEVICE, ODDPWRON);		// Clear bit6: ODD power on.
	}

	if( SystemIsS0 )	// Check S0 status.
	{
		// Power off
		//if( IS_MASK_SET(LENOVODEVICE,ODDPWROFF) )
		//{
		//	uVPCeventSource = 0x00;					// for Levono AP used.
		//	uVPCeventSource2 = VPCeventODD;			// for Levono AP used.
		//	ECQEvent(SDV_VPC_notify);				// 0x44 for Levono used.
		//	SET_MASK(uODDPWRsaving, uODD_PWRoff1);	// Set bit2: uODD_PWRoff1.
		//	CLEAR_MASK(LENOVODEVICE, ODDPWROFF);	// Clear bit0: ODD power off.
		//	}

		// Power on by hotkey
		if( IS_MASK_SET(LENOVODEVICE, ODDPWRKEYON) )// Check bit2: ODD hotkey.
		{
			uVPCeventSource = 0x00;
			uVPCeventSource2 = VPCeventODDEject;
			ECQEvent(SDV_VPC_notify);				// 0x44 for Levono used.
			CLEAR_MASK(LENOVODEVICE, ODDPWRKEYON);	// Clear bit2: ODD hotkey status.
		}

		// Power on by button
		if( (ODD_DA()) && (IS_MASK_CLEAR(uODDPWRsaving, uODD_ejectbtn)) )	// open   // bit6: ODD button.
		{
		  	if (IS_MASK_CLEAR(SysStatus,FnF3PKeyUp))
		  	{
				uODDtimeCnt++;
				if( uODDtimeCnt >= 3)
				{
					SET_MASK(SysStatus,FnF3PKeyUp);
					uODDtimeCnt = 0;
				}
		  	}
		}
		else if ((!ODD_DA())&& (IS_MASK_CLEAR(uODDPWRsaving, uODD_ejectbtn)))
		{  
			if (IS_MASK_SET(SysStatus,FnF3PKeyUp))
		  	{
				uODDtimeCnt++;
				if( uODDtimeCnt >= 3 )
				{
					CLEAR_MASK(uODDPWRsaving, uODD_BtnLowPluse);	// Clear bit1: ODD button low pulse.
					CLEAR_MASK(uODDPWRsaving, uODD_low100ms);		// Clear bit4: ODD 100mSec.
					SET_MASK(uODDPWRsaving, uODD_PWRon);			// Set bit0: ODD power on.
					SET_MASK(uODDPWRsaving, uODD_ejectbtn);			// Set bit5: ODD eject button.
					ECQEvent(ODD_BTN_EVENT);						// 0x53 for Levono used.
					uODDtimeCnt = 0;
					CLEAR_MASK(SysStatus, FnF3PKeyUp);   
				}                                                               
		    	}
		}
		else
		{ uODDtimeCnt = 0; }

		if( IS_MASK_SET(LENOVODEVICE, ODDPWRStatus) )	// Check bit7: ODD Status.
		{
			CLEAR_MASK(uODDPWRsaving, uODD_ejectbtn);	// Set bit5: ODD eject button.
			CLEAR_MASK(LENOVODEVICE, ODDPWRStatus);		// Clear bit7: ODD Status.
		}

		if ( IS_MASK_SET(LENOVODEVICE, ODDPWRBTNON) )	// Check bit1: ODD power button.
		{
			if( IS_MASK_CLEAR(uODDPWRsaving, uODD_BtnLowPluse))	// Check bit1:ODD low pulse.
			{
				uVPCeventSource = 0x00;
				uVPCeventSource2 = VPCeventODD;
				ECQEvent(SDV_VPC_notify);	// 0x44 for Levono used.
				SET_MASK(uODDPWRsaving, uODD_BtnLowPluse);	// Set bit1: ODD button low pulse.

				ODD_DAOUT;                                                    
				ODD_DA_HI();         
				
			}
			else
			{
				if( IS_MASK_CLEAR(uODDPWRsaving, uODD_low100ms) )	// Check bit4:ODD 100mSec low pulse.
				{
					switch( uODDdelaytimeStep )
					{
					case 0:
						if( ChkTimeScale(&uODDlowCnt, (Timer_10 +Timer_1)) )	// Delay 830mSec. //:change Timer_7 to Timer_1
					       {
							ODD_DAOUT;              
							//ODD_DA_HI();
							ODD_DA_LOW();    
							uODDdelaytimeStep++;
						}
						break;
					case 1:
						if( ChkTimeScale(&uODDlowCnt, Timer_1) )	// Delay 100mSec.  // change Timer_3 to Timer_1
						{
							ODD_DAOUT;              
							//ODD_DA_LOW();
							ODD_DA_HI();      
							SET_MASK(uODDPWRsaving, uODD_low100ms);	// Set bit4: ODD 100mSec.
							uODDdelaytimeStep++;
						}
						break;
					default:
				 		break;
					}
				}
				else
				{
					uVPCeventSource = 0x00;
					uVPCeventSource2 = VPCeventODD;
					ECQEvent(SDV_VPC_notify);	// 0x44 for Levono used.
					ODD_DA_INPUT;    //T038 open 
					SET_MASK(uODDPWRsaving, uODD_InitEnd);		// Set bit6: ODD initial End.
					CLEAR_MASK(uODDPWRsaving, uODD_ejectbtn);	// Clear bit5: ODD eject button.
					CLEAR_MASK(LENOVODEVICE, ODDPWRBTNON);		// Clear bit1: ODD power button.
					uODDdelaytimeStep = 0;
				}
			}
		}
	}
	#endif	// Support_ODDZeroPower.
}*/


//martin0616 add start  //MARTINH179
//-----------------------------------------------------------------------------
// Smart Fan control
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Fan table 1   2009/08/18 Rev.01
//-----------------------------------------------------------------------------
const thermalstruct code ThermalTalbe1CPU[]= //UMA //MARTINH185 //MARTINH162 //MARTINH150 //MARTINH147;//MARTINH118: change content //MARTINH079:change content // martin0616: change 'ThermalTalbe1' to 'ThermalTalbe1CPU'//COKEYXU006:refer to Fan table v02_1220.
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
//MARTINH079:add start
const thermalstruct code ThermalTalbe1CPU_DIS[]= //DIS //MARTINH185 //MARTINH175 //MARTINH172 //MARTINH166 //MARTINH079:change content // martin0616: change 'ThermalTalbe1' to 'ThermalTalbe1CPU' //COKEYXU006:refer to Fan table v02_1220.
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
//martin0616 add start
const thermalstruct code ThermalTalbe1VGA[]=  //MARTINH185 //MARTINH172  //MARTINH175 //MARTINH166  //COKEYXU006:refer to Fan table v02_1220.
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
//martin0616 add end
//-----------------------------------------------------------------------------
// Fan table 2
//-----------------------------------------------------------------------------
const thermalstruct code ThermalTalbe2CPU[]= //MARTINH185 //MARTINH079:change content // martin0616: change 'ThermalTalbe2' to 'ThermalTalbe2CPU' //COKEYXU006:refer to Fan table v02_1220.
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
//MARTINH079:add start
const thermalstruct code ThermalTalbe2CPU_DIS[]= //DIS //MARTINH185  //MARTINH175 //MARTINH172  //MARTINH166 //MARTINH079:change content // martin0616: change 'ThermalTalbe1' to 'ThermalTalbe1CPU'
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
//MARTINH079:add end

//martin0616 add start
const thermalstruct code ThermalTalbe2VGA[]= //MARTINH185 //MARTINH175 //MARTINH166 //MARTINH079:change content
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
//martin0616 add end
//MARTINH146:Add start
//-----------------------------------------------------------------------------
// Check Fan RPM count used.
//-----------------------------------------------------------------------------
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
//MARTINH146:Add end
//-----------------------------------------------------------------------------
// The function of check fan rpm 1  //martin0624A:change this function to meet 2 thermal table 1 fan.
//-----------------------------------------------------------------------------	
void CheckFanRPM1(void)
{
	BYTE FanLevel_tcpu = FanLevelCPU;	
	BYTE FanLevel_tvga = FanLevelVGA;
	//BYTE Fan1RPM_t = Fan1RPM;

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
	   Fan1RPMTemp =*ThermalTalbe1CPU[FanLevel_tcpu].RPM;  //MARTINH146:Change 'Fan1RPM' to 'Fan1RPMTemp'
	else
	   Fan1RPMTemp =*ThermalTalbe1VGA[FanLevel_tvga].RPM;  //MARTINH146:Change 'Fan1RPM' to 'Fan1RPMTemp'
	   
//MARTINH146:Change:Add start
    //if(Fan1RPMTemp != Fan1RPM)  //MARTINH150:Remove
    if(Fan1RPMTemp == Fan1RPMBackUp)  //MARTINH150:Add
    {    
	    Check_FANRPM_Count(&Fan1RPMCount,TEMP_TYPE_FAN1RPM);
    }	
	else
	{
       Fan1RPMBackUp = Fan1RPMTemp;  //MARTINH150:Add
	   Fan1RPMCount = 0x00;
	}	
//MARTINH146:Change:Add end	
}
//-----------------------------------------------------------------------------
// The function of check fan rpm 2   //martin0624A:change this function to meet 2 thermal table 1 fan.
//-----------------------------------------------------------------------------
void CheckFanRPM2(void)
{
	BYTE FanLevel_tcpu = FanLevel2CPU;
	BYTE FanLevel_tvga = FanLevel2VGA;
	//BYTE Fan1RPM_t = Fan2RPM;

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
	   Fan2RPMTemp =*ThermalTalbe2CPU[FanLevel_tcpu].RPM;    //MARTINH146:Change 'Fan2RPM' to 'Fan2RPMTemp'
	else
	   Fan2RPMTemp =*ThermalTalbe2VGA[FanLevel_tvga].RPM;    //MARTINH146:Change 'Fan2RPM' to 'Fan2RPMTemp'

//MARTINH146:Change:Add start
    //if(Fan2RPMTemp != Fan2RPM)  //MARTINH150:Remove
    if(Fan2RPMTemp == Fan2RPMBackUp)  //MARTINH150:Add
    {
	    Check_FANRPM_Count(&Fan2RPMCount,TEMP_TYPE_FAN2RPM); 
    }
	else
	{
	   Fan2RPMBackUp = Fan2RPMTemp;  //MARTINH150:Add
	   Fan2RPMCount = 0x00;
	}
//MARTINH146:Change:Add end

}

void Fan1MainControl(void)
{
	//if(Fan1RPM>=Fan2RPM)  //martin0624A: remove
	//{	
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
	//}
//martin0624A remove start
/*	else
	{
		if(((Fan2RPM*100)-RPMACC)>RPM1)
		{
			if(FanValue<FAN_PWM_Max)
			FanValue++;	
		}
		else if(((Fan2RPM*100)+RPMACC)<RPM1)
		{
			if(FanValue>0x00)
			FanValue--;	
		}		
	} */
//martin0624A remove end	
}
void Fan2MainControl(void)
{
	//if(Fan2RPM>=Fan1RPM)     //martin0624A: remove
	//{
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
	//}
//martin0624A remove start	
/*    else
	{	
		if(((Fan1RPM*100)-RPMACC)>RPM1)
		{
			if(FanValue<FAN_PWM_Max)
			FanValue++;	
		}
		else if(((Fan1RPM*100)+RPMACC)<RPM1)
		{
			if(FanValue>0x00)
			FanValue--;	
		}
	} */
//martin0624A remove end	
}

//-----------------------------------------------------------------------------
// The function of fan rpm control
//-----------------------------------------------------------------------------
void FanControl(void)
{
	if(	RPM1==0x0000)
	{
		FAN_PWM = FanInitValue;
	}
	Fan1MainControl(); //MARTINH031:add
//MARTINH031:remove	start
/*	
    if(Fan1RPM!=0x00)
    {
        Fan1MainControl();
    }
    else
    {
        Fan2MainControl();
    }
*/  
//MARTINH031:remove end
}
//MARTINH031:add start
//-----------------------------------------------------------------------------
// The function of fan rpm control
//-----------------------------------------------------------------------------
void Fan2Control(void)
{
	if(	RPM2==0x0000)
	{
		FAN2_PWM = FanInitValue;
	}
	Fan2MainControl(); 
}
//MARTINH031:add end
//-----------------------------------------------------------------------------
// Init. thermal table 1
//-----------------------------------------------------------------------------
void InitThermalTable1(void)
{
	BYTE index;
	index=0x00;
//MARTINH079:add start	
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
//MARTINH079: add end		
	{
	    while(index<(sizeof(ThermalTalbe1CPU)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe1CPU[index].FanOn=ThermalTalbe1CPU[index].CFanOn;
		    *ThermalTalbe1CPU[index].FanOff=ThermalTalbe1CPU[index].CFanOff;
		    *ThermalTalbe1CPU[index].RPM=ThermalTalbe1CPU[index].CRPM;
		    index++;
	    }
	}
//MARTINH031: add start 
    index=0x00;
      while(index<(sizeof(ThermalTalbe1VGA)/sizeof(thermalstruct)))
      {
	     *ThermalTalbe1VGA[index].FanOn=ThermalTalbe1VGA[index].CFanOn;
	     *ThermalTalbe1VGA[index].FanOff=ThermalTalbe1VGA[index].CFanOff;
	     *ThermalTalbe1VGA[index].RPM=ThermalTalbe1VGA[index].CRPM;
	     index++;
      } 
//MARTINH031: add end 
}

//-----------------------------------------------------------------------------
// Init. thermal table 2
//-----------------------------------------------------------------------------
void InitThermalTable2(void)
{
	BYTE index;
	index=0x00;
//MARTINH079:add start	
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
//MARTINH079: add end
	{
	    while(index<(sizeof(ThermalTalbe2CPU)/sizeof(thermalstruct)))
	    {
	    	*ThermalTalbe2CPU[index].FanOn=ThermalTalbe2CPU[index].CFanOn;
		    *ThermalTalbe2CPU[index].FanOff=ThermalTalbe2CPU[index].CFanOff;
		    *ThermalTalbe2CPU[index].RPM=ThermalTalbe2CPU[index].CRPM;
		    index++;
	    }
	}	
//MARTINH031: add start 
    index=0x00;
    while(index<(sizeof(ThermalTalbe2VGA)/sizeof(thermalstruct)))
   {
	   *ThermalTalbe2VGA[index].FanOn=ThermalTalbe2VGA[index].CFanOn;
	   *ThermalTalbe2VGA[index].FanOff=ThermalTalbe2VGA[index].CFanOff;
	   *ThermalTalbe2VGA[index].RPM=ThermalTalbe2VGA[index].CRPM;
	   index++;
   }
//MARTINH031: add end 	
}

//-----------------------------------------------------------------------------
// Fan full off control
//-----------------------------------------------------------------------------
void FanFullOff(void)
{
	FAN_PWM = 0x00;
	nAtmFanSpeed = 0;  //COKEYXU028: add for not show 0 fan rpm after fan stop
}
//MARTINH031: add start
//-----------------------------------------------------------------------------
// Fan full off control
//-----------------------------------------------------------------------------
void Fan2FullOff(void)
{
	FAN2_PWM = 0x00;
	nAtmFan1Speed = 0;  //COKEYXU028: add for not show 0 fan rpm after fan stop
}
//MARTINH031: add end
//-----------------------------------------------------------------------------
// The function of get fan RPM
//-----------------------------------------------------------------------------
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
//MARTINH031:add start
//-----------------------------------------------------------------------------
// The function of get fan2 RPM
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
// Fan Speed Control When Fan Table Not Ready
//-----------------------------------------------------------------------------
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
//MARTINH031:add end
//-----------------------------------------------------------------------------
// The function of smart fan  //martin0624A:change this function to meet 2 fan control.
//-----------------------------------------------------------------------------
void FanManager(void)
{

	   // Check VGA and CPU Temperature error bit and full on bit.
	    if ( (IS_MASK_SET(ERR_THMSTS, b1VGATempEr)) ||  //MARTINH116:remove "b3PECITempEr of ERR_THMSTS"
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
			GetRPM2();  //MARTINH031:add
			return;
		}
	
		if(ManualFanPRM==0x00) 			// In normal control mode
		{
			//if((Fan1RPM!=0x00)||(Fan2RPM!=0x00))  //MARTINH031:remove
			if(Fan1RPM!=0x00)  //MARTINH031:add
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
//MARTINH031: add start
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

//MARTINH031: add end
}


//martin0616 add end

void Fan_Init(void)
{
	nFanStatus1 = Fan_Num;
	nFanStatus1 = (nFanStatus1 << 4) | Fan_Step;
	nOSThrottlingTemp = 97;
	nOSShutdownTemp = 100;
	nShutDownTemp = 100;
}

//ANGELAS050:remove end
/*****************************************************************************/
// Procedure: ThrottlingControl								TimeDiv: 100mSec
// Description: Chekc all throttling status.
// GPIO:
// Referrals:
/*****************************************************************************/
void ThrottlingControl(void)
{
	if ( SystemIsS0 )
	{
		//   Remove keep P1 states under battery mode.
		/*
	      if ( IS_MASK_CLEAR(SYS_STATUS,AC_ADP)&&(0x00==cBATTThrottling) )
	     { cBATTThrottling = 0x01; }
	     */
		//  Remove keep P1 states under battery mode.
		cThrottlingSet.byte &= 0xF0;	// Clear Throttling status.

		if( ( cThrottlingSet.byte & 0x0F ) < cADPThrottling )	// Compare current and ADP Throttling 
		// if( ( cThrottlingSet.byte & PSTATE_MAXStep ) < cADPThrottling )   //0ptimize CPU P_state (change 16 step to 8 step).//:Change MAX P_state from 7 step to 11 step.
		{
			cThrottlingSet.byte |= cADPThrottling;
		}
		if( ( cThrottlingSet.byte & 0x0F ) < cTHERMThrottling )	// Compare current and Thermal throttling./
		//if( ( cThrottlingSet.byte & PSTATE_MAXStep ) < cTHERMThrottling )//:0ptimize CPU P_state (change 16 step to 8 step).//-
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cTHERMThrottling;
		}
		if( ( cThrottlingSet.byte & 0x0F ) < cBATTThrottling )	// Compare current and BAT throttling.
		//if( ( cThrottlingSet.byte & PSTATE_MAXStep ) < cBATTThrottling )//:0ptimize CPU P_state (change 16 step to 8 step).//-
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cBATTThrottling;
		}
		
		//MEILING055:S+ add battery Low throttling level.
		if( ( cThrottlingSet.byte & 0x0F ) < cBATTLowThrottling )	// Compare current and BAT throttling.
		//if( ( cThrottlingSet.byte & PSTATE_MAXStep ) < cBATTThrottling )//:0ptimize CPU P_state (change 16 step to 8 step).//-
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= cBATTLowThrottling;
		}
		//MEILING055:E+
		
		if( ( cThrottlingSet.byte & 0x0F ) < nThrottlingAPSet )	// For AP or debug 
		//if( ( cThrottlingSet.byte & PSTATE_MAXStep ) < nThrottlingAPSet )//0ptimize CPU P_state (change 16 step to 8 step).//-
		{
			cThrottlingSet.byte &= 0xF0;
			cThrottlingSet.byte |= nThrottlingAPSet;
		}

		if( ( cThrottlingSet.byte & 0x0F ) != ( REAL_THROTTLING_INDEX & 0x0F ) )
		//if( ( cThrottlingSet.byte & PSTATE_MAXStep ) != ( REAL_THROTTLING_INDEX & PSTATE_MAXStep ) )//:0ptimize CPU P_state (change 16 step to 8 step).//-
		{	// Cpu throttling for power status change
			if( ( (nRealThermalPinGET) && (( cThrottlingSet.byte & 0x0F ) >= 0x01 ))|| (!nRealThermalPinGET) )
			//if( ( (nRealThermalPinGET) && (( cThrottlingSet.byte & PSTATE_MAXStep ) >= 0x01 ))|| (!nRealThermalPinGET) )//:0ptimize CPU P_state (change 16 step to 8 step).
			{
				cThrottlingSet.fbit.cTHRM_SW = 1;
                RamDebug(cADPThrottling);       
                RamDebug(cTHERMThrottling);       
                RamDebug(cBATTThrottling);        
                RamDebug(REAL_THROTTLING_INDEX);        
				REAL_THROTTLING_INDEX = (REAL_THROTTLING_INDEX & 0xF0) | (cThrottlingSet.byte & 0x0F);
                //REAL_THROTTLING_INDEX = (REAL_THROTTLING_INDEX & 0xF0) | (cThrottlingSet.byte & PSTATE_MAXStep);//:0ptimize CPU P_state (change 16 step to 8 step).//-
                RamDebug(REAL_THROTTLING_INDEX);      
			}
		}
		if(CPUThrottlingDelayTime == 0) //MEILING052:send the 0x1D event after delay time.
		{
			if( cThrottlingSet.fbit.cTHRM_SW )
			{
				cThrottlingSet.fbit.cTHRM_SW = 0;
				if ( IS_MASK_SET(SYS_MISC1, ACPI_OS) )	// Check OS mode.
				{
			    	RamDebug(0xEE);  RamDebug(CPU_SLOW_AD);        //T053+ 
					ECQEvent(CPU_SLOW_AD); 	// 0x1D inform bios.
				}
			}
		}
	}
}

//MEILING033:add start GPU throttling control function.
void GPUThrottlingControl(void)
{

	if ( SystemIsS0 && (CPU_TYPE&0x80) ) //ANGELAG031: modify 0xc0 to 0x80 //ANGELAG022: modify 80 to C0 //ANGELAG014: modify to only NV GPU //MEILING044:add.
	{

		cGPUThrottlingSet.byte &= 0xF0;	// Clear Throttling status.

		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattThrottling)	// Compare battery power Throttling 
		{
			cGPUThrottlingSet.byte |= cGPUBattThrottling;
		}

		//MEILING055:S+ add battery throttling level when PnP AC.
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUACtoBattThrottling)	// Compare PnP AC battery Throttling 
		{
			cGPUThrottlingSet.byte |= cGPUACtoBattThrottling;
		}
		//MEILING055:E+.

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
		if( ( cGPUThrottlingSet.byte & 0x0F ) < cGPUBattLTPThrottling)	// Compare battery OCP throttling
		{
			cGPUThrottlingSet.byte &= 0xF0;
			cGPUThrottlingSet.byte |= cGPUBattLTPThrottling;
		}
		//COKEYXU046: add end
		//ANGELAG019: add start
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
//MEILING033:add end.

/*****************************************************************************/
// Procedure: Chk_FAN_RPM_Control							TimeDiv: 50mSec
// Description: Check FAN is AP control.
// GPIO:
// Referrals:
/*****************************************************************************/
void Chk_FAN_RPM_Control(void)
{
	BYTE BRPM_Manual;
	if (uReserve07.fbit.nFanManual == 1)
	{
		FAN_PWM_ALT; //ANGELAS018:Open fan control function.
		if ( (FAN_PWM != FAN_PWM_Max) && (nAtmFanSpeed != ManualFanPRM) )
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
		//ANGELAS017:S+Open fan control function.
		if ( (FAN_PWM == FAN_PWM_Max) && (nAtmFanSpeed > ManualFanPRM) )
		{
			if( FAN_PWM > 0 )
			{ FAN_PWM--; }
		}
		//ANGELAS017:E+Open fan control function.
	}
	//ANGELAS018:S+Open fan control function.
	else
	{
		#if !FAN_TABLE_Already
		FAN_PWM_OUT;			// Set FAN_PWM OUTPUT.
		EC_FAN_PWM_HI();
        FAN2_PWM_OUT;           //ELMERZH002:FAN 2 running at full speed.
        EC_FAN2_PWM_HI();       //ELMERZH002:FAN 2 running at full speed.
		#endif	// FAN_TABLE_Already
	}
	//ANGELAS018:E+Open fan control function.
}




