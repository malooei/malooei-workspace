#include <rtdef.h>
#include <board.h>
#include <rtthread.h>
#include <finsh.h>  
#include <led.h>
#include <fsmc_sram_hw.h>
#include <fsmc_sram_api.h>


/*
 * description: sram use fsmc bus 
 *
 *
 */  
#if 0
/*******************************************************************************
* Function Name  : FSMC_SRAM_WriteBuffer
* Description    : Writes a Half-word buffer to the FSMC SRAM memory. 
* Input          : - pBuffer : pointer to buffer. 
*                  - WriteAddr : SRAM memory internal address from which the data 
*                    will be written.
*                  - NumHalfwordToWrite : number of half-words to write. 
*                    
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_SRAM_WriteBuffer(u16* pBuffer, u32 WriteAddr, u32 NumHalfwordToWrite)
{
  for(; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(u16 *) (Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer++;
    
    /* Increment the address*/  
    WriteAddr += 2;
  }   
}

/*******************************************************************************
* Function Name  : FSMC_SRAM_ReadBuffer
* Description    : Reads a block of data from the FSMC SRAM memory.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the SRAM memory.
*                  - ReadAddr : SRAM memory internal address to read from.
*                  - NumHalfwordToRead : number of half-words to read.
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_SRAM_ReadBuffer(u16* pBuffer, u32 ReadAddr, u32 NumHalfwordToRead)
{
  for(; NumHalfwordToRead != 0; NumHalfwordToRead--) /* while there is data to read */
  {
    /* Read a half-word from the memory */
    *pBuffer++ = *(vu16*) (Bank1_SRAM3_ADDR + ReadAddr);

    /* Increment the address*/  
    ReadAddr += 2;
  }  
}
#endif


/*
 * Description: Writes a Half-word buffer to the FSMC SRAM memory. 
 */
void fsmc_sram_write(u16* pbuffer, u32 offsetaddr, u32 halfwordtowrite)
{
	/* while there is data to write */
	for(; halfwordtowrite != 0; halfwordtowrite--){
		/* Transfer data to the memory */
		*(u16 *) (Bank1_SRAM3_ADDR + offsetaddr) = *pbuffer++;

		/* Increment the address*/  
		offsetaddr += 2;
	}   
}
  

/*
 * Description: Reads a block of data from the FSMC SRAM memory.
 */
void fsmc_sram_read(u16* pbuffer, u32 offsetaddr, u32 halfwordtoread)
{
	/* while there is data to read */
	for(; halfwordtoread != 0; halfwordtoread--){
		/* Read a half-word from the memory */
		*pbuffer++ = *(vu16*) (Bank1_SRAM3_ADDR + offsetaddr);

		/* Increment the address*/  
		offsetaddr += 2;
	}  
}
   
  
#if FSMC_SRAM_API_DEBUG    
void cmd_sram_api(int cmd, int data, int data1)
{
	u32 test_data;
       	u16 test_butter[128];/* 这段加上会导致系统崩溃 */
       	u8 test_butter1[128] = {"malooei test fsmc sram IS61LV25616 ....\n"};
       	u8 test_butter2[128] = {"IS61LV25616 sram fsmc test malooei ....\n"};
      
          
       	switch (cmd) {  
       		case 0:
       		     	rt_hw_led_on(2);
       			break;
       
       		case 1:
       	        	rt_hw_led_off(2);
       			break;
       
       		case 2:
       	        	rt_hw_led_on(3);
       			break;
       
       		case 3:  
       	        	rt_hw_led_off(3);
       			break;
       
       		case 4: 
			fsmc_sram_write(test_butter1, 0, 30);
			rt_kprintf("write data to IS61LV25616 by fsmc :\n");		
			rt_kprintf("%s ",test_butter1);						
			rt_kprintf("\n");


			fsmc_sram_read(test_butter, 0, 30);
			rt_kprintf("read data from IS61LV25616 by fsmc :\n");		
			rt_kprintf("%s ",test_butter);						
			rt_kprintf("\n");
    
			break;
	    
		case 5:    
			fsmc_sram_write(test_butter2, 0, 30);
			rt_kprintf("write data to IS61LV25616 by fsmc :\n");		
			rt_kprintf("%s ",test_butter2);						
			rt_kprintf("\n");

  
			fsmc_sram_read(test_butter, 0, 30);
			rt_kprintf("read data from IS61LV25616 by fsmc :\n");		
			rt_kprintf("%s ",test_butter);						
			rt_kprintf("\n");
			
			break;

		case 6:
			break;


		default:
			break;
	}
	  
	return;
}
FINSH_FUNCTION_EXPORT(cmd_sram_api, fsmc_sram_debug);
#endif  



  
