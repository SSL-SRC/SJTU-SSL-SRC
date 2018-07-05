/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    main.c          
* 功能描述:    
           主程序
* 版本      作者       时间          状态
* V1.0      chensx     2015.8.6   创建文件 
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
* Description	  : 主函数
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





