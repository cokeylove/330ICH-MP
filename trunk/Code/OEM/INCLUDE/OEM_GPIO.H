/*-----------------------------------------------------------------------------
 * TITLE: OEM_GPIO.H
 *
 * Author : Dino
 *
 * Note : These functions are reference only.
 *        Please follow your project software specification to do some modification.
 *---------------------------------------------------------------------------*/

#ifndef OEM_GPIO_H
#define OEM_GPIO_H

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
#define HighActive  	1
#define LowActive     	0

//-----------------------------------------------------------------
// GPIO Registers define
//-----------------------------------------------------------------
/*  General Control Register (GCR)  */
// BIT 7     -  GPIOB5 HIGH Follow LPCRST# HIGH-to-LOW
// BIT 6     -  WUI7 Enabled
// BIT 5     -  WUI6 Enabled
// BIT [2:1] -  [01] LPCRST# in GPIOB7
//                   [10] LPCRST# in GPIOD2
//                   [11] LPCRST# is dsiabled
#define GCR_Init		0x06    //
#define GCR2_Init		0x20	// Turn on SMBus4 function.

#define GCR10_Init      0x01 	//ANGELAS014: Modify EC reset time to 8s. 00 to 01
#define GPOTA_Init      0x00    // none openDrain
#define GPOTD_Init      0x00    //
#define GPOTH_Init      0x00    //
#define GPOTE_Init      0x00    //MEILING039:add set GPE0 is OpenDrain.

//-----------------------------------------------------------------
// GPIO Port A Registers define
//-----------------------------------------------------------------
#define PortA_Init		0x07            // PORT Data //ANGELAS040:0x06 to 0x07

#define PortA0_Ctrl		OUTPUT			//O   24  	PWR_LED#(G_Skylake)
#define PortA1_Ctrl		OUTPUT			//O   25  	BATT_CHG_LED#
#define PortA2_Ctrl		OUTPUT			//O   28  	BATT_LOW_LED#
//#define PortA3_Ctrl		OUTPUT			//O   29        EC_VCCST_PWRGD(G_Skylake)
//#define PortA4_Ctrl		OUTPUT          //O   30  	SYS_PWROK          	
#define PortA3_Ctrl		ALT			//O   29        EC_BKL_EN 
#define PortA4_Ctrl		OUTPUT          //O   30        EC_FAN2_PWM
#define PortA5_Ctrl		OUTPUT       	//O   31        EC_FAN1_PWM
#define PortA6_Ctrl		INPUT+PULL_DW	//O   32        BEEP#
#define PortA7_Ctrl		OUTPUT	        //O   34   	EC_VCCST_EN(G_Skylake)

#define PWR_LED_ON()				CLEAR_MASK(GPDRA,BIT(0)) //ANGELAS023:Modify UMA LED function.
#define PWR_LED_OFF()				SET_MASK(GPDRA,BIT(0)) //ANGELAS023:Modify UMA LED function.
#define PWR_LED_ALT                 GPCRA0 = ALT  //MEILING030:add.
#define PWR_LED_OUTPUT		  		GPCRA0 = OUTPUT	//MEILING030:add.
#define PWR_LED_INPUT		    	GPCRA0= INPUT   //MEILING030:add.

/*ANGELAS038:remove start
#define EC_ON_HI()  					SET_MASK(GPDRA,BIT(0))
#define EC_ON_LOW() 				CLEAR_MASK(GPDRA,BIT(0))
#define Read_EC_ON()				IS_MASK_SET(GPDRA,BIT(0))
*///ANGELAS038:remove end
#define BAT_CHG_LED_ON()			CLEAR_MASK(GPDRA,BIT(1))
#define BAT_CHG_LED_OFF()			SET_MASK(GPDRA,BIT(1))

#define BAT_LOW_LED_ON()			CLEAR_MASK(GPDRA,BIT(2))
#define BAT_LOW_LED_OFF()			SET_MASK(GPDRA,BIT(2))

//#define EC_VCCST_PWRGD_HI() 		SET_MASK(GPDRA,BIT(3))
//#define EC_VCCST_PWRGD_LOW()	 	CLEAR_MASK(GPDRA,BIT(3))

//#define SYS_PWROK_HI()				SET_MASK(GPDRA,BIT(4))
//#define SYS_PWROK_LOW()				CLEAR_MASK(GPDRA,BIT(4))

//ELMERZH001:S+ add PWM control Keyboard backlight.
#define EC_BKL_PWM					DCR3
#define EC_BKL_PWM_INDW				GPCRA3 = INPUT+PULL_DW	// Set EC_BKL_PWM input+pull down.
#define EC_BKL_PWM_ALT				GPCRA3 = ALT			// Set EC_BKL_PWM Alt.
#define EC_BKL_PWM_OUT				GPCRA3 = OUTPUT			// Set EC_BKL_PWM OUTPUT.
#define EC_BKL_PWM_HI()				SET_MASK(GPDRA,BIT(3))

/*
#define EC_BKL_EN_HI()      SET_MASK(GPDRA,BIT(3))
#define EC_BKL_EN_LOW()     CLR_MASK(GPDRA,BIT(3))
#define Read_EC_BKL_EN()    IS_MASK_SET(GPDRA,BIT(3))
*/
//ELMERZH001:E+ add PWM control Keyboard backlight.


#define FAN2_PWM					DCR4
#define FAN2_PWM_INDW				GPCRA4 = INPUT+PULL_DW	// Set FAN_PWM input+pull down.
#define FAN2_PWM_ALT				GPCRA4 = ALT			// Set FAN_PWM Alt.
#define FAN2_PWM_OUT				GPCRA4 = OUTPUT			// Set FAN_PWM OUTPUT.
#define EC_FAN2_PWM_HI()			SET_MASK(GPDRA,BIT(4))


#define FAN_PWM					DCR5
#define FAN_PWM_INDW				GPCRA5 = INPUT+PULL_DW	// Set FAN1_PWM input+pull down.
#define FAN_PWM_ALT				GPCRA5 = ALT			// Set FAN1_PWM Alt.
#define FAN_PWM_OUT				GPCRA5 = OUTPUT			// Set FAN1_PWM OUTPUT.
#define EC_FAN_PWM_HI()			SET_MASK(GPDRA,BIT(5))

#define PWM_BEEP					DCR6
#define BEEP_INIT					GPCRA6 = PortA6_Ctrl	// init PWM Beep.
#define BEEP_ALT					GPCRA6 = ALT			// Set PWM BEEP ALT.

#define EC_VCCST_EN_HI()			SET_MASK(GPDRA,BIT(7))
#define EC_VCCST_EN_LOW()			CLEAR_MASK(GPDRA,BIT(7))

//-----------------------------------------------------------------
// GPIO Port B Registers define
//-----------------------------------------------------------------
#define PortB_Init		0x00            // PORT Data //martin0313 set EC_ON to high, or EC will loss power.

#define PortB0_Ctrl		INPUT			//O    	108 	   ACIN(G_Skylake)
#define PortB1_Ctrl		INPUT			//O    	109 	   LID_SW#
#define PortB2_Ctrl		OUTPUT         	//O    	123 	   EC_VCCIO_EN(G_Skylake)
#define PortB3_Ctrl		INPUT			//A    	110 	   PWRSW#
#define PortB4_Ctrl		INPUT			//A    	111 	   EC_ON(reseved)(G_Skylake)
#define PortB5_Ctrl		OUTPUT			//O    	126    EC_RTCRST#_ON(G_Skylake) //ANGELAS052:INPUT TO OUTPUT
#define PortB6_Ctrl		OUTPUT			//O    	4    	  KBRST#
#define PortB7_Ctrl		INPUT			//O    	112 	  3VLP

#define Read_AC_IN()		IS_MASK_SET(GPDRB,BIT(0))
#define AC_IN_ALT			GPCRB0 = ALT	// Set AC_IN Alternate.
#define AC_IN_INPUT			GPCRB0 = INPUT	// Set AC_IN INPUT.

#define Read_LID_SW_IN()	IS_MASK_SET(GPDRB,BIT(1))

#define EC_VCCIO_EN_HI()	SET_MASK(GPDRB,BIT(2))
#define EC_VCCIO_EN_LOW()	CLEAR_MASK(GPDRB,BIT(2))

#define Read_EC_PWRBTN() 	IS_MASK_CLEAR(GPDRB,BIT(3))



//MEILING001:S-
/*//no function
#define GATEA20_ON()		SET_MASK(GPDRB,BIT(5))
#define GATEA20_OFF()		CLEAR_MASK(GPDRB,BIT(5))
#define GATEA20_HI()		SET_MASK(GPDRB,BIT(5))
#define GATEA20_LOW()		CLEAR_MASK(GPDRB,BIT(5))*/
//MEILING001:E-

#define RTCRST_ON_HI()		SET_MASK(GPDRB,BIT(5))
#define RTCRST_ON_LOW()		CLEAR_MASK(GPDRB,BIT(5))

#define KBRST_ON()			CLEAR_MASK(GPDRB,BIT(6))
#define KBRST_OFF()			SET_MASK(GPDRB,BIT(6))
#define RCING_LOW()			CLEAR_MASK(GPDRB,BIT(6))
#define RCING_HI()			SET_MASK(GPDRB,BIT(6))

//-----------------------------------------------------------------
// GPIO Port C Registers define
//-----------------------------------------------------------------
#define PortC_Init		0x28            // PORT Data  //change to 0x28 for LAN_WAKE although it is reserved

#define PortC0_Ctrl		OUTPUT	        //O    	119	 	BKOFF#
#define PortC1_Ctrl		ALT			    //IO   	115		EC_SMB_CK1
#define PortC2_Ctrl		ALT			    //IO   	116		EC_SMB_CK1
#define PortC3_Ctrl		ALT+PULL_UP	    //O	      56		KSO16_SMOSI
#define PortC4_Ctrl		INPUT	        //I	      120		LAN_WAKE#
#define PortC5_Ctrl		ALT+PULL_UP	    //O	      57		KSO17_SMISO
#define PortC6_Ctrl		OUTPUT		    //O       124		SUSP#
#define PortC7_Ctrl		OUTPUT		    //O       16		EC_RX

#define BKOFF_OFF()			CLEAR_MASK(GPDRC,BIT(0))
#define BKOFF_ON()			SET_MASK(GPDRC,BIT(0))
#define Read_BKOFF()		GET_MASK(GPDRC, BIT(0))

//LMLKBL0015:add start.
#define SMBUS_CK1_ALT       GPCRC1 = ALT;
#define SMBUS_DA1_ALT       GPCRC2 = ALT;
#define SMBUS_CK1_IN        GPCRC1 = INPUT;
#define SMBUS_DA1_IN        GPCRC2 = INPUT;
#define SMBUS_CK1_HIGH      SET_MASK(GPDRC,BIT(1))
#define SMBUS_DA1_HIGH      SET_MASK(GPDRC,BIT(2))
//LMLKBL0015:add end.

#define Read_LAN_WAKE_IN()	IS_MASK_CLEAR(GPDRC,BIT(4))

#define SUSP_OFF()			CLEAR_MASK(GPDRC,BIT(6))
#define SUSP_ON() 			SET_MASK(GPDRC,BIT(6))
#define Read_SUSP()			IS_MASK_SET(GPDRC,BIT(6))

#define EC_RX_ALT			GPCRC7 = ALT	// Set EC_RX Alternate.
#define EC_RX_OUTPUT		GPCRC7 = OUTPUT	// Set EC_RX OUTPUT.
#define EC_RX_INPUT			GPCRC7 = INPUT	// Set EC_RX INPUT.

//no function
#define O_DEBUG_CLK_HI()    SET_MASK(GPDRC,BIT(7))
#define O_DEBUG_CLK_LO()    CLEAR_MASK(GPDRC,BIT(7))


//-----------------------------------------------------------------
// GPIO Port D Registers define
//-----------------------------------------------------------------
#define PortD_Init		0x00            // PORT Data

#define PortD0_Ctrl		INPUT			//I     18       PM_SLP_S3#
#define PortD1_Ctrl		INPUT			//I     21       PM_SLP_S4#
#define PortD2_Ctrl		INPUT			//I     22 	PLT_RST#
#define PortD3_Ctrl		OUTPUT			//O    23 	EC_SCI#
#define PortD4_Ctrl		OUTPUT			//O    15 	EC_SMI#(reserved)  //JERRYCH003:Enable SMI for hang debug.
#define PortD5_Ctrl		INPUT		    //O    33	 	USB_ON#
#define PortD6_Ctrl		ALT	//A    47 	EC_FAN1_SPEED//JERRYCH00:Modify the fan control from linear to the ladder.
#define PortD7_Ctrl		ALT   //A    48 	EC_FAN2_SPEED  //JERRYCH00:Modify the fan control from linear to the ladder.
//#define PortD7_Ctrl		OUTPUT          //O    48 	EC_TS_ON(BG_Skylake)     //change OUTPUT to INPUT as init time.  //MEILING001:change INPUT to OUTPUT	

#define Read_SLPS3()		IS_MASK_SET(GPDRD,BIT(0))

#define Read_SLPS4()		IS_MASK_SET(GPDRD,BIT(1))

#define Read_LPC_RST()		IS_MASK_SET(GPDRD,BIT(2))

#define SCI_ON()			CLEAR_MASK(GPDRD,BIT(3)) // pull high after sus_p
#define SCI_OFF()			SET_MASK(GPDRD,BIT(3))
#define SCI_LOW()			CLEAR_MASK(GPDRD,BIT(3))
#define SCI_HI()			SET_MASK(GPDRD,BIT(3))

#define SMI_ON()		CLEAR_MASK(GPDRD,BIT(4)) //JERRYCH003:Enable SMI for hang debug.
#define SMI_OFF()		SET_MASK(GPDRD,BIT(4))//JERRYCH003:Enable SMI for hang debug.

#define USB_ON_INPUT		GPCRD5 = INPUT			// SET USB_ON# INPUT.
#define USB_ON_OUTPUT		GPCRD5 = OUTPUT			// SET USB_ON# OUTPUT.
#define USB_ON_LOW()		CLR_MASK(GPDRD,BIT(5))  // Turn On USB_ON#.

#define FAN_SPEED_ALT		GPCRD6 = ALT			// Set FAN1 Speed Alt.
#define FAN_SPEED_INDW		GPCRD6 = INPUT+PULL_DW	// Set FAN1 Speed input+pull down.

#define FAN2_SPEED_ALT		GPCRD7 = ALT			// Set FAN2 Speed Alt.
#define FAN2_SPEED_INDW		GPCRD7 = INPUT+PULL_DW	// Set FAN2 Speed input+pull down.

/* 
#define EC_TS_ON_HI()       SET_MASK(GPDRD,BIT(7)) // (first mask)
#define EC_TS_ON_LOW()      CLEAR_MASK(GPDRD,BIT(7))
#define EC_TS_ON_INPUT	    GPCRD7 = INPUT	   
#define EC_TS_ON_OUTPUT		GPCRD7 = OUTPUT	 
*/
//-----------------------------------------------------------------
// GPIO Port E Registers define
//-----------------------------------------------------------------
#define PortE_Init		0x81                    //T04B: set  0xA9 to 0xA8, GPE0 is not used now //T06:change oxA1 to 0xA9 for set GPE3(Adapter_ID_ON#) to high   // PORT Data //T02: change 0x81 to 0xA1 to set DPWROK_EC high //MEILING001:change 0x88 to 0x80. //MEILING039:change GPE0 to high.

#define PortE0_Ctrl		INPUT			//O     19       GPU_PROCHOT_ON#  //MEILING039:change GPE0 to GPU_PROCHOT_ON#.
#define PortE1_Ctrl		INPUT			//I      82        no use, Reserve for AOAC_ON#
#define PortE2_Ctrl		INPUT           //I      83       VDDQ_PGOOD
#define PortE3_Ctrl		OUTPUT			//O     84   	 EC_VPP_PWREN(BG_Skylake)
#define PortE4_Ctrl		INPUT			//I      125  	 reserved
#define PortE5_Ctrl		INPUT			//O 	    35   	 DPWROK_EC(reserved)
#define PortE6_Ctrl		INPUT           //O	    17      EC_TX             //set to Input for judge whether debug card input or not   
#define PortE7_Ctrl		OUTPUT			//O	    20      NUM_LED#

//MEILING039:S+ add GPE0 for GPU prochot.
#define GPU_PROCHOT_ON()        CLR_MASK(GPDRE,BIT(0))
#define GPU_PROCHOT_OFF()       SET_MASK(GPDRE,BIT(0))
#define GPU_PROCHOT_OUTPUT      GPCRE0 = OUTPUT;
#define GPU_PROCHOT_INPUT       GPCRE0 = INPUT;
//MEILING039:E+.

#define EC_FAN_ANTI_ON()		SET_MASK(GPDRE,BIT(1))      
#define EC_FAN_ANTI_OFF()		CLR_MASK(GPDRE,BIT(1))  
#define EC_FAN_ANTI_OUTPUT		GPCRE1 = OUTPUT	// JERRYCRZ014
#define EC_FAN_ANTI_INPUT		GPCRE1= INPUT	// JERRYCRZ014

#define Read_VDDQ_PGOOD()		IS_MASK_SET(GPDRE,BIT(2))

//MEILING001:S-
/*#define ADAPTERIDON_HI()		SET_MASK(GPDRE,BIT(3))      
#define ADAPTERIDON_LOW()		CLR_MASK(GPDRE,BIT(3))     
#define Read_ADAPTERID_ON()	IS_MASK_SET(GPDRE,BIT(3))*/
//MEILING001:E-

//MEILING001:S+
#define EC_VPP_PWREN_HI()       SET_MASK(GPDRE,BIT(3))
#define EC_VPP_PWREN_LOW()      CLR_MASK(GPDRE,BIT(3))
//MEILING001:E+

/* GPE4 change to GPI4
#define VR_ON_OFF() 			CLEAR_MASK(GPDRE,BIT(4))
#define VR_ON_ON()  			SET_MASK(GPDRE,BIT(4))
#define Read_VR_ON()			IS_MASK_SET(GPDRE,BIT(4))
*/
#define DPWROK_EC_LOW()			CLEAR_MASK(GPDRE,BIT(5))
#define DPWROK_EC_HI()			SET_MASK(GPDRE,BIT(5))


#define EC_TX_ALT			GPCRE6 = ALT	// Set EC_TX Alternate.
#define EC_TX_OUTPUT		GPCRE6 = OUTPUT	// Set EC_TX OUTPUT.
#define EC_TX_INPUT			GPCRE6 = INPUT	// Set EC_RX INPUT.

//no function
#define O_DEBUG_DAT_HI()    SET_MASK(GPDRE,BIT(6))
#define O_DEBUG_DAT_LO()    CLEAR_MASK(GPDRE,BIT(6))
#define Read_EC_TX()		IS_MASK_SET(GPDRE,BIT(6))   

#define NUMLED_ON()			CLEAR_MASK(GPDRE,BIT(7))
#define NUMLED_OFF()		SET_MASK(GPDRE,BIT(7))

//-----------------------------------------------------------------
// GPIO Port F Registers define
//-----------------------------------------------------------------
#define PortF_Init		0x00           

#define PortF0_Ctrl		OUTPUT			//O     85	EC_ON //ANGELAS020: INPUT to OUTPUT
#define PortF1_Ctrl		OUTPUT		    //O     86  	PBTN_OUT#
#define PortF2_Ctrl		ALT			    //I      87  	EC_SMB_CK3(BG_Skylake)  //MEILING001:change INPUT to ALT
#define PortF3_Ctrl		ALT             //IO	    88  	EC_SMB_DA3(BG_Skylake)  //MEILING001:change INPUT to ALT
#define PortF4_Ctrl		INPUT+PULL_DW   //IO    89  	TP_CLK
#define PortF5_Ctrl		OUTPUT    //EC_LID_OUT# //ANGELAG008: modify to OUTPUT //INPUT+PULL_DW 	//IO    90  	TP_DATA
#define PortF6_Ctrl		INPUT	        //A     117 	PECI_EC
#define PortF7_Ctrl		INPUT           //I	   118  	LAN_PWR_ON#(reserved)

//ANGELAS038:add start
#define EC_ON_HI()  		SET_MASK(GPDRF,BIT(0))
#define EC_ON_LOW() 		CLEAR_MASK(GPDRF,BIT(0))
//ANGELAS038:add end

#define PM_PWRBTN_LOW()		CLEAR_MASK(GPDRF,BIT(1))
#define PM_PWRBTN_HI()		SET_MASK(GPDRF,BIT(1))
#define Read_PM_PWRBTN()	IS_MASK_SET(GPDRF,BIT(1))

//MEILING001:S-
/*#define Read_PM_SLP_SUS()	IS_MASK_SET(GPDRF,BIT(2))

#define Read_SUSACK()			IS_MASK_SET(GPDRF,BIT(3))
#define SUSACK_LOW()			CLEAR_MASK(GPDRF,BIT(3))
#define SUSACK_HI()				SET_MASK(GPDRF,BIT(3))*/
//MEILING001:E-

//ANGELAG008: remove start
//#define TPCLK_LOW()		// CLEAR_MASK(GPDRF,BIT(4))
#define TPCLK_HI()		//	SET_MASK(GPDRF,BIT(4))
//#define TPCLK_INPUT		 //  GPCRF4 = INPUT
#define TPCLK_OUTPUT	 //  GPCRF4 = OUTPUT	
#define TPCLK_ALT	     //   GPCRF4 = ALT	
#define TP_CLK_INDW		 //	GPCRF4 = INPUT+PULL_DW	// Set TP CLK Input+Pull down.

#define TP_DATA_INDW	 //	GPCRF5 = INPUT+PULL_DW	// Set TP DATA Input+Pull down.
#define TP_DATA_ALT		 //	GPCRF5 = ALT			// Set TP DATA Alternate.
//ANGELAG008: remove end
//ANGELAG008: ADD START
#define EC_LID_OUT_LOW()	CLEAR_MASK(GPDRF,BIT(5))
#define EC_LID_OUT_HI()		SET_MASK(GPDRF,BIT(5))
#define Read_EC_LID_OUT()   IS_MASK_SET(GPDRF,BIT(5)) 
//ANGELAG008: ADD END

#define PECI_INDW			GPCRF6 = INPUT			// Set PECI INPUT.
#define PECI_ALT			GPCRF6 = ALT			// Set PECI Alternate.

//no function
/*#define LAN_PWR_ON_HI()    	SET_MASK(GPDRF,BIT(7))
#define LAN_PWR_ON_LO()  	CLEAR_MASK(GPDRF,BIT(7))
#define LAN_PWR_OUT         GPCRF7 = OUTPUT                         
#define LAN_PWR_IN          GPCRF7 = INPUT */                          

//-----------------------------------------------------------------
// GPIO Port G Registers define
//-----------------------------------------------------------------
#define PortG_Init		0x00            // PORT Data     //ANGELAS035: 01 to 00

#define PortG0_Ctrl		INPUT		   //O     106 	//ANGELAN012 NC
#define PortG1_Ctrl		OUTPUT         //O     107 	SYSON
#define PortG2_Ctrl		INPUT		   //A     100 	GPG2 for mirror
#define PortG3_Ctrl		INPUT          //A     101	EC_SPI_CS1# // change OUTPUT to INPUT
#define PortG4_Ctrl		INPUT          //A     102 	EC_SPI_SI     // change OUTPUT to INPUT        
#define PortG5_Ctrl		INPUT          //A     103      EC_SPI_SO                       		
#define PortG6_Ctrl     INPUT+PULL_DW  //O    104	      ME_FLASH//ANGELAS102:-Modify GPG6 to input+pullDW in initial.
#define PortG7_Ctrl		INPUT          //A    105       EC_SPI_CLK   //T11B change OUTPUT to INPUT

//#define EC_MUTE_ACT()		CLEAR_MASK(GPDRG,BIT(0)) //ANGELAN012 remove
//#define EC_MUTE_INACT()		SET_MASK(GPDRG,BIT(0)) //ANGELAN012 remove

#define SYSON_LOW()			CLEAR_MASK(GPDRG,BIT(1))
#define SYSON_HI() 			SET_MASK(GPDRG,BIT(1))
#define Read_SYSON()		IS_MASK_SET(GPDRG,BIT(1))

#define pchME_FLAS_HI()		SET_MASK(GPDRG,BIT(6))		// ME Lock
#define pchME_FLAS_LOW()	CLEAR_MASK(GPDRG,BIT(6))	// ME unlock
#define pchME_FLAS_INDW		GPCRG6 = INPUT+PULL_DW		// SET ME Input+Pull down.
#define pchME_FLAS_OUT		GPCRG6 = OUTPUT				// SET ME Output.

//-----------------------------------------------------------------
// GPIO Port H Registers define
//-----------------------------------------------------------------
#define PortH_Init		0x08                     //T048: 0x08 to 0x09        // PORT Data//JERRYSK001:modify RSMRST signal to low when GPIO init.change 09 to 08

#define PortH0_Ctrl		OUTPUT			//O   	93   EC_RSMRST#
#define PortH1_Ctrl		INPUT+PULL_DW	//IO  	94 	EC_SMB_CK2
#define PortH2_Ctrl		INPUT+PULL_DW	//IO  	95 	EC_SMB_DA2
#define PortH3_Ctrl		OUTPUT		    //A		96 	CAPS_LED#
#define PortH4_Ctrl		OUTPUT		    //O    	97 	PCH_PWR_EN#
#define PortH5_Ctrl		OUTPUT		    //O    	98 	SYS_PWROK
#define PortH6_Ctrl		OUTPUT			//A  	99	PCH_PWROK


#define RSMRST_LOW()		CLEAR_MASK(GPDRH,BIT(0))
#define RSMRST_HI()			SET_MASK(GPDRH,BIT(0))
#define Read_RSMRST()		IS_MASK_SET(GPDRH,BIT(0))

#define SMBus2_CLK_INDW 	GPCRH1 = INPUT+PULL_DW	// Set SMBus2 CLK input+pull down.
#define SMBus2_CLK_ALT		GPCRH1 = ALT			// Set SMBus2 CLK Alternate.

#define SMBus2_DAT_INDW 	GPCRH2 = INPUT+PULL_DW	// Set SMBus2 DATA input+pull down.
#define SMBus2_DAT_ALT		GPCRH2 = ALT			// Set SMBus2 DATA Alternate.

#define CAPLED_ON()			CLEAR_MASK(GPDRH,BIT(3))
#define CAPLED_OFF()		SET_MASK(GPDRH,BIT(3))

#define PCH_PWR_EN_ON()		SET_MASK(GPDRH,BIT(4))
#define PCH_PWR_EN_OFF()	CLEAR_MASK(GPDRH,BIT(4))

//MEILING001:S-
//no function
/*#define ACOFF_LOW()		CLEAR_MASK(GPDRH,BIT(5))
#define ACOFF_HI()			{								\
								CHGIC_ptr = 2;				\
								WriteSmartChgIC();			\
								SET_MASK(GPDRH,BIT(5));		\
							}
#define Read_ACOFF()		IS_MASK_SET(GPDRH,BIT(5))*/
//MEILING001:E-

#define SYS_PWROK_HI()				SET_MASK(GPDRH,BIT(5))
#define SYS_PWROK_LOW()				CLEAR_MASK(GPDRH,BIT(5))



#define PCH_PWROK_LOW()		CLEAR_MASK(GPDRH,BIT(6))
#define PCH_PWROK_HI() 		SET_MASK(GPDRH,BIT(6))
#define Read_PCH_PWROK()	IS_MASK_SET(GPDRH,BIT(6))

//-----------------------------------------------------------------
// GPIO Port I Registers define
//-----------------------------------------------------------------
#define PortI_Init		0x00            // PORT Data

#define PortI0_Ctrl		INPUT         //A    	66 	NTC_V1
#define PortI1_Ctrl		INPUT	      //A    	67 	NTC_V2(G_Skylake)
#define PortI2_Ctrl		INPUT		  //A    	68   ADD battery detect
#define PortI3_Ctrl		INPUT	      //O    	69 	SD_PWR_EN#(G_Skylake) 1.2V
#define PortI4_Ctrl		OUTPUT        //O  	    70 	VR_ON
#define PortI5_Ctrl		INPUT	      //A   	71 	ADP_I
#define PortI6_Ctrl		INPUT		  //I  	    72 	ADAPTER_ID_R  ADC
#define PortI7_Ctrl		INPUT		  //I   	73 	Test Point(BG_Skylake)

#define Read_BAT_IN()		IS_MASK_CLEAR(GPDRI,BIT(2))  //battery detect pin

//GPI4 change to GPJ3
//#define Read_CPU_VR_READY()		IS_MASK_SET(GPDRI,BIT(4))

//GPE4 change to GPI4
#define VR_ON_OFF() 			CLEAR_MASK(GPDRI,BIT(4))
#define VR_ON_ON()  			SET_MASK(GPDRI,BIT(4))
#define Read_VR_ON()			IS_MASK_SET(GPDRI,BIT(4))


//-----------------------------------------------------------------
// GPIO Port J Registers define
//-----------------------f------------------------------------------
#define PortJ_Init		0x00            // PORT Data

#define PortJ0_Ctrl		INPUT			//I    	76 	 NOVO#
#define PortJ1_Ctrl		OUTPUT			//O    	77 	 //ANGELAN012 EC_MUTE#
//#define PortJ2_Ctrl		INPUT			//O    	78 	 SUSWARN#(reserved)
#define PortJ2_Ctrl		OUTPUT			//O    	78 	 EC_TS_ON
#define PortJ3_Ctrl		INPUT			//I    	79 	 CPU_VR_READY
#define PortJ4_Ctrl		OUTPUT       	//O    	80 	 H_PROCHOT#_EC
#define PortJ5_Ctrl		INPUT			//O    	81 	 ENBKL
#define PortJ6_Ctrl		OUTPUT			//O    	128   AC_PRESENT_R
#define PortJ7_Ctrl		INPUT			//O    	2 	 PCIE_WAKE#(G_Skylake)

#define Read_EC_NOVO()		IS_MASK_CLEAR(GPDRJ,BIT(0))
//ANGELAN012 add s
#define EC_MUTE_ACT()		CLEAR_MASK(GPDRJ,BIT(1))
#define EC_MUTE_INACT()		SET_MASK(GPDRJ,BIT(1))
//ANGELAN012 add e
//MEILING001:S- remove EC_SD_PWR_EN pin.
/*#define EC_SD_PWR_EN_HI()		SET_MASK(GPDRJ,BIT(1))
#define EC_SD_PWR_EN_LOW() 		CLEAR_MASK(GPDRJ,BIT(1))
#define EC_SD_PWR_EN_OUTPUT		GPCRJ1 = OUTPUT		// S3,S5 SET input		
#define EC_SD_PWR_EN_INPUT		GPCRJ1 = INPUT		// S3,S5 SET input*/
//MEILING001:E-

#define EC_TS_ON_HI()       SET_MASK(GPDRJ,BIT(2)) // (first mask)
#define EC_TS_ON_LOW()      CLEAR_MASK(GPDRJ,BIT(2))
#define EC_TS_ON_INPUT	    GPCRJ2 = INPUT	   
#define EC_TS_ON_OUTPUT		GPCRJ2 = OUTPUT	 

//GPI4 change to GPJ3
#define Read_CPU_VR_READY()		IS_MASK_SET(GPDRJ,BIT(3))

//MEILING001:S-
/*#define EC_MainPwr_ON()  			CLR_MASK(GPDRJ,BIT(3))  // main_pwr_on is low active for Zx10, but Yx10 is high active.
#define EC_MainPwr_OFF()  			SET_MASK(GPDRJ,BIT(3))  
#define EC_MainPwr()					IS_MASK_CLEAR(GPDRJ,BIT(3))  
#define EC_MainPwr_OUTPUT			GPCRJ3 = OUTPUT	//ANGELAS044:add*/
//MEILING001:E-

#define H_PROCHOT_ON()			SET_MASK(GPDRJ,BIT(4))  //ANGELAS026:Modify PROCHOT# function. //ANGELAS027 0 to 1
#define H_PROCHOT_OFF()			CLEAR_MASK(GPDRJ,BIT(4))      //ANGELAS026:Modify PROCHOT# function. //ANGELAS027 1 to 0

#define Read_ENBKL_IN()			IS_MASK_SET(GPDRJ,BIT(5))

#define AC_PRESENT_LOW() 		CLEAR_MASK(GPDRJ,BIT(6))
#define AC_PRESENT_HI()			SET_MASK(GPDRJ,BIT(6))

#define PCIE_WAKE_HI()			SET_MASK(GPDRJ,BIT(7))
#define PCIE_WAKE_LOW() 		CLEAR_MASK(GPDRJ,BIT(7))
#define PCIE_WAKE_OUTPUT		GPCRJ7 = OUTPUT
#define PCIE_WAKE_INPUT			GPCRJ7 = INPUT

//-----------------------------------------------------------------------------
// DPGU
//-----------------------------------------------------------------------------
#define     EG_FB_CLAMP         0x10
#define     EG_GC6_EVENT        0x14
#define     EG_DGPU_PWROK       0x1C
#define     EG_DGPU_PWR_EN      0x24
#define     EG_DGPU_HOLD_RST    0x2C
#define     EG_DGPU_GC6_EN      0x34
#define     EG_NVDD_PWR_EN      0x48
#define     EG_KBL_DET          0x50

#define     EG_GPIO22           0x58
#define     EG_S_NVDD_PWR_EN    0x60
#define     EG_GPIO26           0x68
#define     EG_S_DGPU_RST       0x6C
#define     EG_S_DGPU_PWR_EN    0x74
#define     EG_S_DGPU_PWROK     0x7C
#define     EG_S_GC6_EVENT      0x84
#define     EG_S_GC6_EN         0x8C

#define DGPU_PWR_EN_HI()    EGPC_Write(EG_DGPU_PWR_EN,  0x03)
#define DGPU_PWR_EN_LOW()   EGPC_Write(EG_DGPU_PWR_EN,  0x02)

#define DGPU_HOLD_RST_HI()  EGPC_Write(EG_DGPU_HOLD_RST,0x03)
#define DGPU_HOLD_RST_LOW() EGPC_Write(EG_DGPU_HOLD_RST,0x02)

#define DGPU_GC6_EN_HI()    EGPC_Write(EG_DGPU_GC6_EN,  0x03)
#define DGPU_GC6_EN_LOW()   EGPC_Write(EG_DGPU_GC6_EN,  0x02)

#define NVDD_PWR_EN_HI()    EGPC_Write(EG_NVDD_PWR_EN,  0x03)
#define NVDD_PWR_EN_LOW()   EGPC_Write(EG_NVDD_PWR_EN,  0x02)

#define S_NVDD_PWR_EN_HI()  EGPC_Write(EG_S_NVDD_PWR_EN,0x03)
#define S_NVDD_PWR_EN_LOW() EGPC_Write(EG_S_NVDD_PWR_EN,0x02)

#define S_DGPU_RST_HI()     EGPC_Write(EG_S_DGPU_RST,   0x03)
#define S_DGPU_RST_LOW()    EGPC_Write(EG_S_DGPU_RST,   0x02)

#define S_DGPU_PWR_EN_HI()  EGPC_Write(EG_S_DGPU_PWR_EN,0x03)
#define S_DGPU_PWR_EN_LOW() EGPC_Write(EG_S_DGPU_PWR_EN,0x02)

#define DGPU_FB_CLAMP_HI()  EGPC_Write(EG_S_DGPU_PWR_EN,0x03)
#define DGPU_FB_CLAMP_LOW() EGPC_Write(EG_S_DGPU_PWR_EN,0x02)

#define IS_DGPU_PWROK()     EGPC_Read(EG_DGPU_PWROK)
#define IS_S_DGPU_PWROK()   EGPC_Read(EG_S_DGPU_PWROK)

//-----------------------------------------------------------------------------
// Input control
//-----------------------------------------------------------------------------
extern void Init_GPIO(void);
extern void Hook_SCION(void);
extern void Hook_SCIOFF(void);
extern void Hook_SMION(void);
extern void Hook_SMIOFF(void);
extern void Hook_A20ON(void);
extern void Hook_A20OFF(void);
extern void Hook_KBRSTON(void);
extern void Hook_KBRSTOFF(void);
extern void Hook_NUMLED_ON(void);
extern void Hook_NUMLED_OFF(void);
extern void Hook_CAPLED_ON(void);
extern void Hook_CAPLED_OFF(void);

typedef struct InitGPIOReg
{
    XBYTE 	*reg;
	BYTE	value;
} sInitGPIOReg;

#endif
