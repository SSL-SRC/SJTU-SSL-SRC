/********************************************************************************
********************************************************************************
* Ȩ��:    �����Ͻ������˹ɷ����޹�˾
* �ļ���:    main.c          
* ��������:    
           ������
* �汾      ����       ʱ��          ״̬
* V1.0      chensx     2015.8.6   �����ļ� 
*****************************************************************************
*****************************************************************************/ 
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "typedef.h"
#include "motor.h"
#include "timer.h"
#include "init.h"
#include "action.h"
#include "robot.h"



/*******************************************************************************
* Function Name   : int main(void)
* Description	  : ������
*					
* Input		      : None 
* Output		  : None
* Return		  : None.
*******************************************************************************/
int main(void)
{
	do_init();
	do_run();
}


/*******************************************************************************
* Function Name   : assert_failed
* Description	  :
*					
* Input		      : None 
* Output		  : None
* Return		  : None.
*******************************************************************************/
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
	*/

  /* Infinite loop */
  while (1)
  {
  }
}
#endif





