/*******************************************************************************
  National DP83848 PHY definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_dp83848.h

  Summary:
    National DP83848 PHY definitions

  Description:
    This file provides the National DP83848 PHY definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END

#ifndef _NAT_DP83848C_H_

#define _NAT_DP83848C_H_

typedef enum
{
    /*
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,
    PHY_REG_BMSTAT      = 1,
    // extended registers: 2-15
    PHY_REG_PHYID1      = 2,
    PHY_REG_PHYID2      = 3,
    PHY_REG_ANAD        = 4,
    PHY_REG_ANLPAD      = 5,
    PHY_REG_ANLPADNP    = 5,
    PHY_REG_ANEXP       = 6,
    PHY_REG_ANNPTR      = 7,
    PHY_REG_ANLPRNP     = 8,
    */
    // specific vendor registers: 16-31
    PHY_REG_STAT            = 0x10,
    PHY_REG_MII_INT_CTRL    = 0x11,
    PHY_REG_MII_INT_STAT    = 0x12,
    PHY_REG_FALSE_CS_COUNT  = 0x14,
    PHY_REG_RXERR_COUNT     = 0x15,
    PHY_REG_PCS_CONFIG      = 0x16,
    PHY_REG_RMII_BYPASS     = 0x17,
    PHY_REG_LED_CTRL        = 0x18,
    PHY_REG_PHY_CTRL        = 0x19,
    PHY_REG_10BT_CTRL       = 0x1a,
    PHY_REG_TEST_CTRL       = 0x1b,
    PHY_REG_ENERGY_CTRL     = 0x1d,

    //
    //PHY_REGISTERS     = 32    // total number of registers
}ePHY_VENDOR_REG;
// updated version of ePHY_REG


// vendor registers
//
typedef union {
  struct {
    unsigned ELAST_BUF    :2;
    unsigned RX_UNF_STS   :1;
    unsigned RX_OVF_STS   :1;
    unsigned RMII_REV1_0  :1;
    unsigned RMII_MODE    :1;
    unsigned              :10;
  };
  struct {
    unsigned short w      :16;
  };
} __RMIIBYPASSbits_t;   // reg 0x17: PHY_REG_RMII_BYPASS
#define _RMIIBYPASS_RMII_MODE_MASK    0x0020
#define _RMIIBYPASS_RMII_REV1_0_MASK  0x0010
#define _RMIIBYPASS_RX_OVF_STS_MASK   0x0008
#define _RMIIBYPASS_RX_UNF_STS_MASK   0x0004
#define _RMIIBYPASS_ELAST_BUF_MASK    0x0003




typedef union {
  struct {
    unsigned PHYADDR      :5;
    unsigned LED_CFG      :2;
    unsigned BP_STRETCH   :1;
    unsigned BIST_START   :1;
    unsigned BIST_STATUS  :1;
    unsigned PSR_15       :1;
    unsigned BIST_FE      :1;
    unsigned PAUSE_TX     :1;
    unsigned PAUSE_RX     :1;
    unsigned FORCE_MDIX   :1;
    unsigned MDIX_EN      :1;
  };
  struct {
    unsigned short w      :16;
  };
} __PHYCTRLbits_t;  // reg 0x19: PHY_REG_PHY_CTRL
#define _PHYCTRL_PHYADDR_MASK      0x001f
#define _PHYCTRL_LED_CFG_MASK      0x0060
#define _PHYCTRL_BP_STRETCH_MASK   0x0080
#define _PHYCTRL_BIST_START_MASK   0x0100
#define _PHYCTRL_BIST_STATUS_MASK  0x0200
#define _PHYCTRL_PSR_15_MASK       0x0400
#define _PHYCTRL_BIST_FE_MASK      0x0800
#define _PHYCTRL_PAUSE_TX_MASK     0x1000
#define _PHYCTRL_PAUSE_RX_MASK     0x2000
#define _PHYCTRL_FORCE_MDIX_MASK   0x4000
#define _PHYCTRL_MDIX_EN_MASK      0x8000






#endif  // _NAT_DP83848C_H_

