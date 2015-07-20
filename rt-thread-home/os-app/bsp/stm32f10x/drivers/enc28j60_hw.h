/*
 * Description:	enc28j60 hardware init 
 */
#define ENC28J60_HW_H_

#include <stm32f10x.h>


/*
 * SPI2
 * PB12 -- NSS, PB13 -- SCK, PB14 -- MISO, PB15 -- MOSI
 */

#define SPI2_RCC_PERIPH			RCC_APB1Periph_SPI2
#define SPI2_PORT 			GPIOB
#define SPI2_NSS_PORT 			GPIOB
#define RESET_PORT			GPIOB	
#define INT_PORT			GPIOB

#define SPI2_NSS			GPIO_Pin_12
#define SPI2_SCK			GPIO_Pin_13
#define SPI2_MISO			GPIO_Pin_14
#define SPI2_MOSI			GPIO_Pin_15
#define RESET_PIN			GPIO_Pin_11
#define INT_PIN				GPIO_Pin_10



/* spi mode */
#define ENC28J60_SPIX			SPI2
#define ENC28J60_SPI_RCC_PERIPH		SPI2_RCC_PERIPH
#define ENC28J60_SPI_PORT 		SPI2_PORT
#define ENC28J60_SPI_NSS_PORT		SPI2_NSS_PORT

#define ENC28J60_SPI_SCK		SPI2_SCK		
#define ENC28J60_SPI_MISO		SPI2_MISO	
#define ENC28J60_SPI_MOSI		SPI2_MOSI		 
#define ENC28J60_SPI_NSS		SPI2_NSS		


/* control gpio */
/* RESET PIN */
#define ENC28J60_RESET_PIN		RESET_PIN
#define ENC28J60_RESET_PORT		RESET_PORT

/* CS PIN */
#define ENC28J60_CS_PIN			SPI2_NSS
#define ENC28J60_CS_PORT		SPI2_NSS_PORT

/* INT PIN */
#define ENC28J60_INT_PIN		INT_PIN
#define ENC28J60_INT_PORT		INT_PORT
#define ENC28J60_INT_PORT_SOURCE	GPIO_PortSourceGPIOB
#define ENC28J60_INT_PIN_SOURCE		GPIO_PinSource11
#define ENC28J60_INT_EXT_LINE		EXTI_Line11
#define ENC28J60_INT_NUM 		EXTI15_10_IRQn
	 
#define ENC28J60_PREEMPTION_PRI		1
#define ENC28J60_SUB_PRI		1

#define ENC28J60_CS_HIGH     	GPIO_SetBits(ENC28J60_CS_PORT, ENC28J60_CS_PIN)
#define ENC28J60_CS_LOW      	GPIO_ResetBits(ENC28J60_CS_PORT, ENC28J60_CS_PIN)

#define is_spix_sr_flag_set(spix, flag)	(0 != ((spix)->SR & (flag)))
#define spi_i2s_send_byte(spix, data)	((spix)->DR = (unsigned char)data)
#define spi_i2s_recv_byte(spix)		((spix)->DR)


#if 1
/**
 * \addtogroup net
 *
 * @{
 */
/**
 * \addtogroup net_driver
 *
 * @{
 */
/**
 * \addtogroup net_driver_enc28j60
 *
 * @{
 */
/**
 * \file
 * Microchip ENC28J60 I/O header (license: GPLv2)
 *
 * \author Roland Riegel
 */

/* partitioning of the internal 8kB tx/rx buffers
 */
/*
 * DS80349C.errata #5: Use the lower segment of the buffer memory for the receive buffer,
 *			starting at address 0000h.
 */
#define RX_START 0x0000     /* do not change this! */
#define TX_END (0x2000 - 1) /* do not change this! */
#define TX_START 0x1a00
#define RX_END (TX_START - 1)

/* register addresses consist of three parts:
 * bits 4-0: real address
 * bits 6-5: bank number
 * bit    7: skip a dummy byte (for mac/phy access)
 */
#define MASK_ADDR 0x1f
#define MASK_BANK 0x60
#define MASK_DBRD 0x80
#define SHIFT_ADDR 0
#define SHIFT_BANK 5
#define SHIFT_DBRD 7

/* bank independent */
#define EIE             0x1b
#define EIR             0x1c
#define ESTAT           0x1d
#define ECON2           0x1e
#define ECON1           0x1f

/* bank 0 */
#define ERDPTL          (0x00 | 0x00)
#define ERDPTH          (0x01 | 0x00)
#define EWRPTL          (0x02 | 0x00)
#define EWRPTH          (0x03 | 0x00)
#define ETXSTL          (0x04 | 0x00)
#define ETXSTH          (0x05 | 0x00)
#define ETXNDL          (0x06 | 0x00)
#define ETXNDH          (0x07 | 0x00)
#define ERXSTL          (0x08 | 0x00)
#define ERXSTH          (0x09 | 0x00)
#define ERXNDL          (0x0a | 0x00)
#define ERXNDH          (0x0b | 0x00)
#define ERXRDPTL        (0x0c | 0x00)
#define ERXRDPTH        (0x0d | 0x00)
#define ERXWRPTL        (0x0e | 0x00)
#define ERXWRPTH        (0x0f | 0x00)
#define EDMASTL         (0x10 | 0x00)
#define EDMASTH         (0x11 | 0x00)
#define EDMANDL         (0x12 | 0x00)
#define EDMANDH         (0x13 | 0x00)
#define EDMADSTL        (0x14 | 0x00)
#define EDMADSTH        (0x15 | 0x00)
#define EDMACSL         (0x16 | 0x00)
#define EDMACSH         (0x17 | 0x00)

/* bank 1 */
#define EHT0            (0x00 | 0x20)
#define EHT1            (0x01 | 0x20)
#define EHT2            (0x02 | 0x20)
#define EHT3            (0x03 | 0x20)
#define EHT4            (0x04 | 0x20)
#define EHT5            (0x05 | 0x20)
#define EHT6            (0x06 | 0x20)
#define EHT7            (0x07 | 0x20)
#define EPMM0           (0x08 | 0x20)
#define EPMM1           (0x09 | 0x20)
#define EPMM2           (0x0a | 0x20)
#define EPMM3           (0x0b | 0x20)
#define EPMM4           (0x0c | 0x20)
#define EPMM5           (0x0d | 0x20)
#define EPMM6           (0x0e | 0x20)
#define EPMM7           (0x0f | 0x20)
#define EPMCSL          (0x10 | 0x20)
#define EPMCSH          (0x11 | 0x20)
#define EPMOL           (0x14 | 0x20)
#define EPMOH           (0x15 | 0x20)
#define EWOLIE          (0x16 | 0x20)
#define EWOLIR          (0x17 | 0x20)
#define ERXFCON         (0x18 | 0x20)
#define EPKTCNT         (0x19 | 0x20)

/* bank 2 */
#define MACON1          (0x00 | 0x40 | 0x80)
#define MACON2          (0x01 | 0x40 | 0x80)
#define MACON3          (0x02 | 0x40 | 0x80)
#define MACON4          (0x03 | 0x40 | 0x80)
#define MABBIPG         (0x04 | 0x40 | 0x80)
#define MAIPGL          (0x06 | 0x40 | 0x80)
#define MAIPGH          (0x07 | 0x40 | 0x80)
#define MACLCON1        (0x08 | 0x40 | 0x80)
#define MACLCON2        (0x09 | 0x40 | 0x80)
#define MAMXFLL         (0x0a | 0x40 | 0x80)
#define MAMXFLH         (0x0b | 0x40 | 0x80)
#define MAPHSUP         (0x0d | 0x40 | 0x80)
#define MICON           (0x11 | 0x40 | 0x80)
#define MICMD           (0x12 | 0x40 | 0x80)
#define MIREGADR        (0x14 | 0x40 | 0x80)
#define MIWRL           (0x16 | 0x40 | 0x80)
#define MIWRH           (0x17 | 0x40 | 0x80)
#define MIRDL           (0x18 | 0x40 | 0x80)
#define MIRDH           (0x19 | 0x40 | 0x80)

/* bank 3 */
#define MAADR1          (0x00 | 0x60 | 0x80)
#define MAADR0          (0x01 | 0x60 | 0x80)
#define MAADR3          (0x02 | 0x60 | 0x80)
#define MAADR2          (0x03 | 0x60 | 0x80)
#define MAADR5          (0x04 | 0x60 | 0x80)
#define MAADR4          (0x05 | 0x60 | 0x80)
#define EBSTSD          (0x06 | 0x60)
#define EBSTCON         (0x07 | 0x60)
#define EBSTCSL         (0x08 | 0x60)
#define EBSTCSH         (0x09 | 0x60)
#define MISTAT          (0x0a | 0x60 | 0x80)
#define EREVID          (0x12 | 0x60)
#define ECOCON          (0x15 | 0x60)
#define EFLOCON         (0x17 | 0x60)
#define EPAUSL          (0x18 | 0x60)
#define EPAUSH          (0x19 | 0x60)

/* phy */
#define PHCON1          0x00
#define PHSTAT1         0x01
#define PHID1           0x02
#define PHID2           0x03
#define PHCON2          0x10
#define PHSTAT2         0x11
#define PHIE            0x12
#define PHIR            0x13
#define PHLCON          0x14

/* EIE */
#define EIE_INTIE       7
#define EIE_PKTIE       6
#define EIE_DMAIE       5
#define EIE_LINKIE      4
#define EIE_TXIE        3
#define EIE_WOLIE       2
#define EIE_TXERIE      1
#define EIE_RXERIE      0

/* EIR */
#define EIR_PKTIF       6
#define EIR_DMAIF       5
#define EIR_LINKIF      4
#define EIR_TXIF        3
#define EIR_WOLIF       2
#define EIR_TXERIF      1
#define EIR_RXERIF      0

/* ESTAT */
#define ESTAT_INT       7
#define ESTAT_LATECOL   4
#define ESTAT_RXBUSY    2
#define ESTAT_TXABRT    1
#define ESTAT_CLKRDY    0

/* ECON2 */
#define ECON2_AUTOINC   7
#define ECON2_PKTDEC    6
#define ECON2_PWRSV     5
#define ECON2_VRPS      4

/* ECON1 */
#define ECON1_TXRST     7
#define ECON1_RXRST     6
#define ECON1_DMAST     5
#define ECON1_CSUMEN    4
#define ECON1_TXRTS     3
#define ECON1_RXEN      2
#define ECON1_BSEL1     1
#define ECON1_BSEL0     0

/* EWOLIE */
#define EWOLIE_UCWOLIE  7
#define EWOLIE_AWOLIE   6
#define EWOLIE_PMWOLIE  4
#define EWOLIE_MPWOLIE  3
#define EWOLIE_HTWOLIE  2
#define EWOLIE_MCWOLIE  1
#define EWOLIE_BCWOLIE  0

/* EWOLIR */
#define EWOLIR_UCWOLIF  7
#define EWOLIR_AWOLIF   6
#define EWOLIR_PMWOLIF  4
#define EWOLIR_MPWOLIF  3
#define EWOLIR_HTWOLIF  2
#define EWOLIR_MCWOLIF  1
#define EWOLIR_BCWOLIF  0

/* ERXFCON */
#define ERXFCON_UCEN    7
#define ERXFCON_ANDOR   6
#define ERXFCON_CRCEN   5
#define ERXFCON_PMEN    4
#define ERXFCON_MPEN    3
#define ERXFCON_HTEN    2
#define ERXFCON_MCEN    1
#define ERXFCON_BCEN    0

/* MACON1 */
#define MACON1_LOOPBK   4
#define MACON1_TXPAUS   3
#define MACON1_RXPAUS   2
#define MACON1_PASSALL  1
#define MACON1_MARXEN   0

/* MACON2 */
#define MACON2_MARST    7
#define MACON2_RNDRST   6
#define MACON2_MARXRST  3
#define MACON2_RFUNRST  2
#define MACON2_MATXRST  1
#define MACON2_TFUNRST  0

/* MACON3 */
#define MACON3_PADCFG2  7
#define MACON3_PADCFG1  6
#define MACON3_PADCFG0  5
#define MACON3_TXCRCEN  4
#define MACON3_PHDRLEN  3
#define MACON3_HFRMEN   2
#define MACON3_FRMLNEN  1
#define MACON3_FULDPX   0

/* MACON4 */
#define MACON4_DEFER    6
#define MACON4_BPEN     5
#define MACON4_NOBKOFF  4
#define MACON4_LONGPRE  1
#define MACON4_PUREPRE  0

/* MAPHSUP */
#define MAPHSUP_RSTINTFC 7
#define MAPHSUP_RSTRMII  3

/* MICON */
#define MICON_RSTMII    7

/* MICMD */
#define MICMD_MIISCAN   1
#define MICMD_MIIRD     0

/* EBSTCON */
#define EBSTCON_PSV2    7
#define EBSTCON_PSV1    6
#define EBSTCON_PSV0    5
#define EBSTCON_PSEL    4
#define EBSTCON_TMSEL1  3
#define EBSTCON_TMSEL0  2
#define EBSTCON_TME     1
#define EBSTCON_BISTST  0

/* MISTAT */
#define MISTAT_NVALID   2
#define MISTAT_SCAN     1
#define MISTAT_BUSY     0

/* ECOCON */
#define ECOCON_COCON2   2
#define ECOCON_COCON1   1
#define ECOCON_COCON0   0

/* EFLOCON */
#define EFLOCON_FULDPXS 2
#define EFLOCON_FCEN1   1
#define EFLOCON_FCEN0   0

/* PHCON1 */
#define PHCON1_PRST     15
#define PHCON1_PLOOPBK  14
#define PHCON1_PPWRSV   11
#define PHCON1_PDPXMD   8

/* PHSTAT1 */
#define PHSTAT1_PFDPX   12
#define PHSTAT1_PHDPX   11
#define PHSTAT1_LLSTAT  2
#define PHSTAT1_JBSTAT  1

/* PHCON2 */
#define PHCON2_FRCLNK   14
#define PHCON2_TXDIS    13
#define PHCON2_JABBER   10
#define PHCON2_HDLDIS   8

/* PHSTAT2 */
#define PHSTAT2_TXSTAT  13
#define PHSTAT2_RXSTAT  12
#define PHSTAT2_COLSTAT 11
#define PHSTAT2_LSTAT   10
#define PHSTAT2_DPXSTAT 9
#define PHSTAT2_PLRITY  4

/* PHIE */
#define PHIE_PLINKIE    4
#define PHIE_PGEIE      1

/* PHIR */
#define PHIR_LINKIF     4
#define PHIR_PGIF       2

/* PHLCON */
#define PHLCON_LACFG3   11
#define PHLCON_LACFG2   10
#define PHLCON_LACFG1   9
#define PHLCON_LACFG0   8
#define PHLCON_LBCFG3   7
#define PHLCON_LBCFG2   6
#define PHLCON_LBCFG1   5
#define PHLCON_LBCFG0   4
#define PHLCON_LFRQ1    3
#define PHLCON_LFRQ0    2
#define PHLCON_STRCH    1
#endif

extern void start_enc28j60(void);
extern unsigned int enc28j60_readwrite_byte(u8 data); 
extern unsigned int enc28j60_readbyte(void);
extern void spix_rec_data(SPI_TypeDef* SPIx, u8* buffer, u16 buffer_len, unsigned dummy);
extern void spix_send_data(SPI_TypeDef* SPIx, const u8* data, u16 data_len);




