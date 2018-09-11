/*-----------------------------------------------------------------------------
 * TITLE: OEM_LED.C
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

/* ----------------------------------------------------------------------------
 * FUNCTION:   OEM_Write_Leds
 *
 * Write to SCROLL LOCK, NUM LOCK, CAPS LOCK, and any custom LEDs (including
 * an "Fn" key LED).
 *
 * Input: data (LED_ON bit) = Data to write to the LEDs
 *         bit 0 - SCROLL, Scroll Lock
 *         bit 1 - NUM,    Num Lock
 *         bit 2 - CAPS,   Caps Lock
 *         bit 3 - OVL,    Fn LED
 *         bit 4 - LED4,   Undefined
 *         bit 5 - LED5,   Undefined
 *         bit 6 - LED6,   Undefined
 *         bit 7 - LED7,   Undefined
 *
 * Updates Led_Data variable and calls send_leds if appropriate.
 * ------------------------------------------------------------------------- */
void OEM_Write_Leds(BYTE data_byte)
{
    Led_Data = data_byte;
	//if ( uMBID & 0x40 )
    //if ( uMBID & 0x01 ) 	// Check 15"G52:modify board id for MFG  from 0x04 to 0x01 follow new ec common spec
    //if( ( uMBID & 0x03 == 0x01 ) ||( uMBID & 0x03 == 0x02 ) )	//ANGELAS008: Modidy board ID for check 15" and 17"
    if ( uMBID & 0x01 ) 	// Check 15"G52:modify board id for MFG  from 0x04 to 0x01 follow new ec common spec //ANGELAS009 remove ANGELAS008
    {
	    if ( data_byte & BIT1 )
	    {
		   	Hook_NUMLED_ON();
			SET_MASK(SYS_MISC1, b1Num_LED);
	    }
	    else
	    {
			Hook_NUMLED_OFF();
			CLR_MASK(SYS_MISC1, b1Num_LED);
	     }
    }

    if ( data_byte & BIT2 )
    {
		Hook_CAPLED_ON();
		SET_MASK(SYS_MISC1, b2Cap_LED);
    }
	else
	{
		Hook_CAPLED_OFF();
		CLR_MASK(SYS_MISC1, b2Cap_LED);
	}
}

//MEILING030:S- remove old LED control function.
/*void Lenovo_LED(void)
{
	if ( SystemIsS5 )
	{ PWR_LED_OFF(); }
	else
	{
		LOWBATT_3TIMES =0;
		if ( SystemIsS3 || (IS_MASK_SET(uISCT_2, b5ISCT_PwrLED)) )	// Check S3 and ISCT status.
		{
			PowerLed_Count++;
			if ( PowerLed_Count < 20 )
			{
				PWR_LED_ON();
			}
			if( (PowerLed_Count > 20) && (PowerLed_Count <40) )
			{
				PWR_LED_OFF();
			}
			if ( PowerLed_Count > 40 )
			{
				PowerLed_Count = 0;
				PWR_LED_ON();
			}
		}
		else
		{
			if ( SystemIsS0 || (SysPowState==SYSTEM_S5_S0))	// Check S0 and power on sequence status.
				PWR_LED_ON();
		}
	}

	// Battery LED control
	GPCRA1 = ALT;
	GPCRA2 = ALT;
	if ( IS_MASK_SET(SYS_MISC1,b5Crisis_LED) )
	{
		if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
		{	// under 5%
			// 660mHz of Amber, 1000ms on/ 500ms off
			C4CPRS = 0xC0;	// set frequence
			C4MCPRS = 0x00;
			DCR2 = 0x55;	// On Amber
			DCR1 = 0xFF;	// Off Green
		}
		else
		{ DCR1 = DCR2 = 0xFF; }	// Off Amber and Green.
	}
	else
	{
		if( IS_MASK_SET(LV_Authen_Step_CNT,BIT(6)) )
		{	// authentication Fail
			C4CPRS = 0xFF;	// set frequence
			C4MCPRS = 0x00;

	        if(SystemIsS0 || Read_AC_IN())
	        {
	            DCR2 = 0x7F;	// On Amber
	    		DCR1 = 0xFF;	// Off Green
	        }
	        else // not S0 & Battery Only
	        { DCR1 = DCR2 = 0xFF; }	// Off Amber and Green.
		}
		else if( IS_MASK_SET(Bat1_FPChgFlag,BIT(0)) )
		{	//  abnormal pre-charge
			// Amber and Green xchange at 250ms
	    	C4CPRS = 0x3F;	// set frequence
	    	C4MCPRS = 0x00;

	        if(SystemIsS0 || Read_AC_IN())
	        {
	            DCR2 = 0x7F;	// On Amber
	    		DCR1 = 0xFF;	// Off Green
	        }
	        else // not S0 & Battery Only
	        { DCR1 = DCR2 = 0xFF; }	// Off Amber and Green.
		}
		else if( Read_AC_IN() )
		{	// AC IN
			LOWBATT_3TIMES =0;
			PWMPOL = 0x00;						// sync. Green and Amber
					
			if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
			{	// Battery IN, Charging mode
				if( BAT1PERCL >= 0x50 )
				{	// over 80%
					// Green always on
					DCR2 = 0xFF;	// Off Amber
					DCR1 = 0x00;	// On Green
				}
				else if( BAT1PERCL >= 0x14 )
				{	// over 20%
					// 196mHz of Green, 5000ms on/ 100ms off
					C4CPRS = 0x90;	// set frequence
					C4MCPRS = 0x02;
					DCR2 = 0xFF;	// Off Amber
					DCR1 = 0x05;	// On Green
				}
				else if( BAT1PERCL >= 0x05 )
				{	// over 5%
					// 303mHz of Amber, 3200ms on/ 100ms off
					C4CPRS = 0xA8;	// set frequence
					C4MCPRS = 0x01;
					DCR2 = 0x08;	// On Amber
					DCR1 = 0xFF;	// Off Green
				}
				else
				{	// under 5%
					// 660mHz of Amber, 1000ms on/ 500ms off
					C4CPRS = 0xC0;	// set frequence
					C4MCPRS = 0x00;
					DCR2 = 0x55;	// On Amber
					DCR1 = 0xFF;	// Off Green
				}
			}
			else
			{ DCR1 = DCR2 = 0xFF; }	// Off Amber and Green.
		}
		else
		{	// Dischage mode
			if ( SystemIsS5 )
			{
				DCR1 = 0xFF;		// Off Green
				if (LOWBATT_3TIMES ==0)
				{
					DCR2 = 0xFF;	// Off Amber
				}
				else
				{
					if (LOWBATT_3TIMES > 80)
						DCR2 = 0x00;					// On Amber
					else if (LOWBATT_3TIMES > 60)
						DCR2 = 0xFF;					// Off Amber
					else if (LOWBATT_3TIMES > 40)
						DCR2 = 0x00;					// On Amber
					else if (LOWBATT_3TIMES > 20)
						DCR2 = 0xFF;					// Off Amber
					else if (LOWBATT_3TIMES > 0)
						DCR2 = 0x00;					// On Amber

					LOWBATT_3TIMES--;
					if (LOWBATT_3TIMES==0)
						DCR2 = 0xFF;					// Off Amber
				}
			}
			else
			{
				PWMPOL = 0x00;		// sync. Green and Amber
				if( SystemIsS3 )
				{	// DC S3 mode
					DCR2 = 0xFF;	// Off Amber
					DCR1 = 0xFF;	// Off Green
				}
				else if( BAT1PERCL >= 0x14 )
				{
					// Green always on
					DCR2 = 0xFF;	// Off Amber
					DCR1 = 0x00;	// On Green
				}
				else if( BAT1PERCL >= 0x05 )
				{	// over 5%
					// Amber always on
					DCR2 = 0x00;	// On Amber
					DCR1 = 0xFF;	// Off Green
				}
				else
				{	// under 5%
					// 660mHz of Amber, 1000ms on/ 500ms off
					C4CPRS = 0xC0;	// set frequence
					C4MCPRS = 0x00;
					DCR2 = 0x55;	// On Amber
					DCR1 = 0xFF;	// Off Green
				}
			}
		}
	}
	if( cCHIPType==0x01 )	// IT8586EX
	{
		DCR0 = DCR1;
	}


	#if Support_TPLED
	if ( IS_MASK_SET(pDevStus, pENABLE_TP) )
	{// TPLED_OFF(); }	// TouchPad ON.
	else
	{ //TPLED_ON(); }		// TouchPad OFF.
	#endif	// Support_TPLED
}*/
//MEILING030:E- remove old LED control function.


//MEILING030:S+ add new LED control function.
void Battery_Charge_Discharge_LED_CONTROL(WORD LED_ON,BYTE LED_OFF)   
{
	if(!(BAT_LED_Cnt_OFF|BAT_LED_Cnt_ON))
	{
		BAT_LED_Cnt_ON = LED_ON;
		BAT_LED_Cnt_OFF = LED_OFF;
	}
	if(BAT_LED_Cnt_ON)
	{
		BAT_LED_Cnt_ON = BAT_LED_Cnt_ON - 1;
		if( (BAT1PERCL >= 0) && (BAT1PERCL < 20) )
		{     
			BAT_LOW_LED_ON();
			BAT_CHG_LED_OFF();
		}
		else
		{
			BAT_CHG_LED_ON();         
			BAT_LOW_LED_OFF();
		}
		
	}
	else if(BAT_LED_Cnt_OFF)
	{
		BAT_LED_Cnt_OFF = BAT_LED_Cnt_OFF - 1;
		BAT_CHG_LED_OFF();	         //LED OFF
		BAT_LOW_LED_OFF();	
	}
	
	
}

void Battery_LED_CONTROL(WORD LED_ON,BYTE LED_OFF)   
{
	BAT_CHG_LED_OFF();
	if(!(BAT_LED_Cnt_OFF|BAT_LED_Cnt_ON))
	{
		BAT_LED_Cnt_ON = LED_ON;
		BAT_LED_Cnt_OFF = LED_OFF;
	}
	
	if(BAT_LED_Cnt_ON)
	{
		BAT_LED_Cnt_ON = BAT_LED_Cnt_ON - 1;
		BAT_LOW_LED_ON(); 	
	}
	else if(BAT_LED_Cnt_OFF)
	{
		BAT_LED_Cnt_OFF = BAT_LED_Cnt_OFF - 1;
		BAT_LOW_LED_OFF();       
	}
	
	
}

void Battery_LED_Reset(void)
{
	
	BAT_LED_Cnt_ON = 0;
	BAT_LED_Cnt_OFF = 0;

	BAT_LOW_LED_OFF();         //LED OFF
	BAT_CHG_LED_OFF();

		
}

void Lenovo_LED(void)
{
	if ( SystemIsS5||SystemIsDSX|| (SysPowState==SYSTEM_DSX_S5))
	{ 
		PWR_LED_OUTPUT;
		CLR_MASK(PWM0LHE,DIMMING_ENABLE);
		PWR_LED_OFF();
   	}
	else
	{
		LOWBATT_3TIMES =0;
		if ( SystemIsS3 || (IS_MASK_SET(uISCT_2, b5ISCT_PwrLED)) )	// Check S3 and ISCT status.
		{
			PWR_LED_ALT;
			SET_MASK(PWM0LHE,DIMMING_ENABLE);
		}
		else
		{
			if ( SystemIsS0 || (SysPowState==SYSTEM_S5_S0))	// Check S0 and power on sequence status.
			{
				PWR_LED_OUTPUT;
				CLR_MASK(PWM0LHE,DIMMING_ENABLE);
				PWR_LED_ON();
			}
		}
	}

  	if ( IS_MASK_SET(SYS_MISC1,b5Crisis_LED) )
	{
		if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
		{	 
		 	Battery_LED_CONTROL(20,10);//Amber, 1000ms on/ 500ms off
		}
		else
		{  
		 	Battery_LED_Reset();
		}	
	}
	else
	{
		if( IS_MASK_SET(LV_Authen_Step_CNT,BIT(6)) )
		{	// authentication Fail
	        if(SystemIsS0 || Read_AC_IN())
	        {
				Battery_LED_CONTROL(5,5);//Amber, 250ms on/ 250ms off
	        }
	        else // not S0 & Battery Only
	        { 
		      	Battery_LED_Reset();
		   	}	
		}
		else if( IS_MASK_SET(Bat1_FPChgFlag,BIT(0)) )
		{	
	        if(SystemIsS0 || Read_AC_IN())
	        {
			   	Battery_LED_CONTROL(5,5);//Amber, 250ms on/ 250ms off
	        }
	        else // not S0 & Battery Only
	        { 
				Battery_LED_Reset();
			}	
		}
		else if( Read_AC_IN() )
		{	// AC IN
			LOWBATT_3TIMES =0;
			
			if(IS_MASK_SET(SEL_STATE0,PRESENT_A))
			{	// Battery IN, Charging mode
				if( BAT1PERCL >= 0x50 )
				{	// over 80%
				  	Battery_Charge_Discharge_LED_CONTROL(1,0);//White solid on
				}
				else if( BAT1PERCL >= 0x14 )
				{	// over 20%
					Battery_Charge_Discharge_LED_CONTROL(100,2);//White 0.1s off/5s on
				}
				else
				{	// under 20%  Amber 0.25s On/ 0.25s Off for total 1.25s, remain off for 3s
					BAT_LOW_LED_Cnt = BAT_LOW_LED_Cnt + 1;   
					
					if(BAT_LOW_LED_Cnt <= 25)
					{
						Battery_Charge_Discharge_LED_CONTROL(5,5);
					}
					else 
					{
						Battery_LED_Reset();
						if(BAT_LOW_LED_Cnt==65) //ANGELAG003: Modify 85 to 65
						{
							BAT_LOW_LED_Cnt=0;
						}
					}
				}
			}
			else
			{ 
			 	Battery_LED_Reset();
			}	// Off Amber and Green.
		}
		else
		{	// Dischage mode
			if ( SystemIsS5 ||SystemIsDSX ||(SysPowState==SYSTEM_DSX_S5))
			{
				if (LOWBATT_3TIMES ==0)
				{
				   	Battery_LED_Reset();
				}
				else
				{	//low battery flash 3 times
					if (LOWBATT_3TIMES > 80)
					{
					   	Battery_LED_CONTROL(1,0);// On Amber
					}
					else if (LOWBATT_3TIMES > 60)
					{
					   	Battery_LED_CONTROL(0,1);// Off Amber
					}
					else if (LOWBATT_3TIMES > 40)
					{
					  	Battery_LED_CONTROL(1,0);// On Amber
					}
					else if (LOWBATT_3TIMES > 20)
					{
					  	Battery_LED_CONTROL(0,1);//// Off Amber
					}
					else if (LOWBATT_3TIMES > 0)
					{
					    Battery_LED_CONTROL(1,0);// On Amber
                    }
					LOWBATT_3TIMES--;
					if (LOWBATT_3TIMES==0)
					{
					  
					   	Battery_LED_Reset(); // Off Amber
					}
				}
			}
			else
			{
				if( SystemIsS3 )
				{	// DC S3 mode
					Battery_LED_Reset();
				}
				else if( BAT1PERCL >= 0x14 )
				{	//over 20%
				  	Battery_LED_Reset();//off
				}
				else if( BAT1PERCL >= 0x05 )
				{	//over 5%
				    Battery_Charge_Discharge_LED_CONTROL(1,0);//Amber solid on
				}
				else
				{	// under 5%
					Battery_Charge_Discharge_LED_CONTROL(5,5);//Amber 0.25s on/0.25s off
				}
			}
		}
	}
	/*
	if ( SystemIsS0 )
	{
		if ( (DEVICEMODULE & 0x43) != 0 )
		{ RF_LED_LOW(); }	// RF LED ON.
		else
		{ RF_LED_HI(); }	// RF LED OFF.
	}
	else
	{ RF_LED_HI(); }		// RF LED OFF.
	*/

	#if Support_TPLED
	if ( IS_MASK_SET(pDevStus, pENABLE_TP) )
	{
		//TPLED_OFF();
	}	// TouchPad ON.
	else
	{ 
		//TPLED_ON();
	}	// TouchPad OFF.
	#endif	// Support_TPLED
}

//MEILING030:E+ add new LED control function.

void MFG_LED(void)
{
	if(cOsLedCtrl.fbit.cOL_PwrLed == 1)
		PWR_LED_ON();
	else
		PWR_LED_OFF();

	if(cOsLedCtrl.fbit.cOL_ChgLed == 1)
		//DCR1=0;  //MEILING030:remove.
		BAT_CHG_LED_ON(); //MEILING030:add.
	else
		//DCR1=0xFF;  //MEILING030:remove.
		BAT_CHG_LED_OFF(); //MEILING030:add.

	if(cOsLedCtrl.fbit.cOL_DisChgLed == 1)
		//DCR2=0;  //MEILING030:remove.
		BAT_LOW_LED_ON(); //MEILING030:add.
	else
		//DCR2=0xFF;  //MEILING030:remove.
		BAT_LOW_LED_OFF(); //MEILING030:add.  //MEILING056:modify led on to off.

	/*
	if(cOsLedCtrl.fbit.cOL_WLANLed == 1)
		RF_LED_LOW();
	else
		RF_LED_HI();
	*/

    if( uMBID & 0x01 ) // Check 15"G52:modify board id for MFG  from 0x04 to 0x01 follow new ec common spec
	{
		if( cOsLedCtrl.fbit.cOL_NumlockLed == 1 )
			NUMLED_ON();
		else
			NUMLED_OFF();
	}

	if( cOsLed1Ctrl.fbit.cOL1_CapslockLed == 1 )
		CAPLED_ON();
	else
		CAPLED_OFF();

	if( cOsLed1Ctrl.fbit.cOL1_KBBacklight == 1 )
		DCR3 = 0xFF; //ELMERZH001:add PWM control Keyboard backlight
		//EC_BKL_EN_HI(); //ELMERZH001:add PWM control Keyboard backlight
	else
		DCR3 = 0; //ELMERZH001:add PWM control Keyboard backlight
		//EC_BKL_EN_LOW(); //ELMERZH001:add PWM control Keyboard backlight

	#if Support_TPLED
	if( cOsLedCtrl.fbit.cOL_TPActLed == 1 )
		//TPLED_ON();
	else
		//TPLED_OFF();
	#endif	// Support_TPLED
}

//ELMERZH001:S+ add PWM control Keyboard backlight ,transplant code from 720s.  
void SetKeyboardLED(void)
{
    BYTE BKBMax, BKBHalf;

    if ( (SystemNotS0) || (cOsLedCtrl.fbit.cOL_CtrlRight == 1) || (!Read_LID_SW_IN()) )
    {
        EC_BKL_PWM = 0;  //ELMERZH001:add
        //EC_BKL_EN_LOW(); //ELMERZH001:remove
        return;
    }

    //if ( uMBID & 0x40 )
    //{ // QIQY6.
    //  BKBMax = 0x4B;
    //  BKBHalf = 0x33;
    //}
    //else
    //{ // QIQY5.
    BKBMax = 0xF0;//ELMERZH001:modify
    BKBHalf = 0x28;//ELMERZH001:modify
    //}
    if(UpdateLEDBL_delay != 0)
    {
        UpdateLEDBL_delay--;
        return;
    }


    if(uReserve07.fbit.nVPCDriverIn == 1)
    {
        if ( IS_MASK_SET(EMStatusBit, b0SetKBLEDON) )
        {
            if ( IS_MASK_SET(EMStatusBit, b2KBLEDChk) )
            {
                LED_KB_PWM_Step = 2;   //COKEYXU010:Modify KB BKL to just on/off
            }
        }
        else
        {
            LED_KB_PWM_Step = 0;
        }
    }


    if ( (LED_KB_PWM_Count & 0x7F) != 0 )
    {
        if ( 0 == EC_BKL_PWM )  //MEILING001:remove.
            //if(!Read_EC_BKL_EN()) //MEILING001:add.
        {
            EC_BKL_PWM = BKBMax; //MEILING001:remove.
            //EC_BKL_EN_HI(); //MEILING001:add.
            LED_KB_PWM_Count |= 0x80;
        }
        LED_KB_PWM_Count--;
    }
    else
    {
        if ( IS_MASK_SET(LED_KB_PWM_Count, BIT7) ) //REJERRY007:modify backlight control.
        {
            EC_BKL_PWM = 0;
            LED_KB_PWM_Count = 0;
        }
        else
        {
            switch(LED_KB_PWM_Step)
            {
            case 0:
                EC_BKL_PWM = 0;
                SET_MASK(EMStatusBit, b2KBLEDChk);//72JERRY036: Modify KB backlight control for lenovo app setting.
                break;
            case 1:
				//COKEYXU010:Modify KB BKL to just on/off 
                //EC_BKL_PWM = BKBHalf;
                //CLEAR_MASK(EMStatusBit, b2KBLEDChk);//72JERRY036: Modify KB backlight control for lenovo app setting.
                break;

            case 2:
                EC_BKL_PWM = BKBMax;
                CLEAR_MASK(EMStatusBit, b2KBLEDChk);//72JERRY036: Modify KB backlight control for lenovo app setting.
                break;

            default:
                EC_BKL_PWM = 0;
                SET_MASK(EMStatusBit, b2KBLEDChk);//72JERRY036: Modify KB backlight control for lenovo app setting.
                break;

            }

        }
    }

}

//ELMERZH001:E+ add PWM control Keyboard backlight,transplant code from 720s.  


