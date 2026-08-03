/*******************************************************************************
                      ��Ȩ���� (C), 2022-,NCUROBOT
 *******************************************************************************
  �� �� ��   : buzzer.c
  �� �� ��   :
  ��    ��   : ����
  ��������   :
  ����޸�   :
  ��������   :
  �����б�   :
*******************************************************************************/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "buzzer.h"

/* �ڲ��궨�� ----------------------------------------------------------------*/

/* �ڲ��Զ����������͵ı��� --------------------------------------------------*/

/* �ڲ����� ------------------------------------------------------------------*/

/* �ڲ�����ԭ������ ----------------------------------------------------------*/

/* �������岿�� --------------------------------------------------------------*/
/**
  * @brief				��������LED����
  * @param[out]		��ͬƵ�ʼ���Ӧ����
  * @param[in]		note������long����
  * @retval				
*/
void Note(int note,float Long)
{
    if (note == 0) {
        // 休止符：停止 PWM 输出，静音
        HAL_TIM_PWM_Stop(&buzzer_htim, TIM_CHANNEL_1);
        HAL_Delay(Long * 200);
        return;
    }

    __HAL_TIM_DISABLE(&buzzer_htim);
    buzzer_htim.Instance->ARR = (21000 / note - 1) * 1u;
    buzzer_htim.Instance->CCR1 = (10500 / note - 1) * 1u;
    buzzer_htim.Instance->EGR = TIM_EGR_UG;  // 生成更新事件，立即加载影子寄存器
    __HAL_TIM_ENABLE(&buzzer_htim);
    HAL_TIM_PWM_Start(&buzzer_htim, TIM_CHANNEL_1);  // PH6

    HAL_Delay(Long * 200);
}
/********** ���㡷-GALA**********/
void gala_you(void) 
{
	float t=0.2;

	Note( note_5B , 1 );    //
	Note( note_G , 1 );
	Note( note_5B , 2 );
	Note( note_G , 1 );
	Note( note_5B ,  2);
	Note( note_G , 1 );
	Note( note_5D ,  2);
	Note( note_G , 1 );
	Note( note_5C , 1 );
	Note( note_5C , 1 );
	Note( note_G , 1 );
	Note( note_5B , 1 );
	Note( note_5C , 1 );
	
	Note( note_5B , 1 );
	Note( note_G , 1 );
	Note( note_5B , 2 );
	Note( note_G , 1 );
	Note( note_5B ,  2);
	Note( note_G , 1 );
	Note( note_5D ,  2);
	Note( note_G , 1 );
	Note( note_5C , 1 );
	Note( note_5C , 1 );
	Note( note_G , 1 );
	Note( note_5B , 1 );
	Note( note_5C , 1 );
	
	Note( note_5B , 1 );
	Note( note_G , 1 );
	Note( note_5B , 2 );
	Note( note_G , 1 );
	Note( note_5B ,  2);
	Note( note_G , 1 );
	Note( note_5D ,  2);
	Note( note_G , 1 );
	Note( note_5C , 1 );
	Note( note_5C , 1 );
	Note( note_G , 1 );
	Note( note_5B , 1 );
	Note( note_5C , 1 );

  Note( note_5B , 1 );
	Note( note_G , 1 );
	Note( note_5B , 2 );
	Note( note_G , 1 );
	Note( note_5B , 2 );
	Note( note_G , 1 );
	Note( note_5D ,  2);
	Note( note_G , 1 );
	Note( note_5C , 1 );
	Note( note_5C , 1 );
	Note( note_G , 1 );
	Note( note_D , 2 );
	
	Note( note_E , 6 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	
	Note( note_5C , 4 );
	Note( note_5B , 4 );
	Note( note_E , 4 );
	Note( note_D , 2 );
	
	Note( note_E , 6 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 8 );
	
	Note( note_5B , 2 );
	Note( note_5D , 4 );
	Note( note_E , 10 );
	
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5B , 4 );
	
	Note( note_5C , 4 );
	Note( note_5B , 4 );
	Note( note_E , 4 );
	Note( note_D , 2 );
	
	Note( note_E , 6 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5B , 4 );
	
	Note( note_5C , 4 );
	Note( note_5D , 10 );

	Note( 0 , 4 );      //��һֱ׷Ѱ����
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	
	Note( 0 , 2 );      //�����ԶҲ����
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_C , 6 );
	 
	Note( 0 , 2 );      //ȴ�ܱ����ž���
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
//	Note( note_D , 2 );
	Note( note_D , 2 );
//	Note( note_C , 1 );
	Note( note_C , 2 );  
	Note( note_D , 2 );  

	Note( 0 , 2 );      
	Note( note_E , 2 );  //��һֱ��������
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	
	Note( 0 , 2 );      //�����������һ���
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 4 );
	Note( note_C , 6 );
	
	Note( 0 , 2 );      //���һ�Ц���ҿ���
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_B , 2 );
	Note( note_A , 1 );
	Note( note_3G , 5 );
	
	Note( 0 , 1 );      //��������������
	Note( note_3G , 1 );
	Note( 0 , t );
	Note( note_3G , 1 );
	Note( 0 , t );
	Note( note_3G , 1 );
	Note( note_G , 4 );
	Note( note_E , 3 );
	Note( note_D , 1 );
	Note( note_C , 2 );
	Note( 0 , t );
	Note( note_C , 4 );  
	
	Note( 0 , 1 );
	Note( note_C , 2 );      //���յ�ϸ��
	Note( 0 , 0.05 );
	Note( note_C , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_A , 6 );
	
	Note( 0 , 2 );          //Ĺ���ĳ���
	Note( note_A , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_C , 2 );
	Note( note_D , 6 );
	Note( 0 , 2 ); 
	
	Note( note_E , 4 );      //�Ҵ�������������
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 );
	Note( note_5C , 6 );
	
	Note( note_C , 2 );     //Ϊ������������޵�����
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_5A , 4 );
	Note( note_G , 1 );
	Note( note_5A , 3 );
	Note( note_G , 1 );
	Note( 0 , 0.1 );    
	Note( note_G , 3 );
	Note( 0 , 0.1 );
	Note( note_G , 4 );
	Note( note_D , 8 );
	
	Note( note_E , 4 );     //����������һ��Ц��
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 );
	Note( note_5C , 6 );

	Note( note_C , 2 );     //Ҳ���Һ�ɵ���Ҳ�����
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5B , 4 );
	Note( note_5A , 6 );
	Note( 0 , t );
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( note_5A , 3 );
	Note( note_5C , 4 );
	Note( note_5D , 6 );
	
	Note( 0 , 2 );
	Note( note_G , 2 );   //��Ը��ѽ
	Note( note_5C , 2 );
	Note( note_5B , 1 );
	Note( note_5C , 12 );
		
	Note( 0 , 4 );
		
  Note( 0 , 2 );      //���Ƕ�׷Ѱ����
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	
	Note( 0 , 2 );      //�������㵱��Ψһ
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_C , 6 );
	
//	Note( 0 , 4 );
	
	Note( 0 , 2 );      //���ȴ����Ϊ��
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
//	Note( note_D , 2 );
	Note( note_D , 2 );
//	Note( note_C , 1 );
	Note( note_C , 2 );  
	Note( note_D , 2 );
	
	Note( 0 , 2 );      //���Ƕ���������
	Note( note_E , 2 );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	
	Note( 0 , 2 );      //����������������
	Note( note_E , 2 );
	Note( 0 , t );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 4 );
	Note( note_C , 6 );
	
//	Note( 0 , 4 );
	
	Note( 0 , 2 );      //һ��ӵ��һ������
	Note( note_E , 2 );
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_E , 3 );
	Note( note_D , 2 );
	Note( note_B , 2 );
	Note( note_A , 1 );
	Note( note_3G , 5 );
	
	Note( 0 , 1 );      //�����㲻Ϊ˭�غ�
	Note( note_3G , 1 );
	Note( 0 , t );
	Note( note_3G , 1 );
	Note( 0 , t );
	Note( note_3G , 1 );
	Note( note_G , 4 );
	Note( note_E , 3 );
	Note( note_D , 1 );
	Note( note_C , 2 );
	Note( 0 , t );
	Note( note_C , 4 );
	
	Note( 0 , 1 );
	Note( note_C , 2 );      //����ŵ����
	Note( 0 , t );
	Note( note_C , 2 );
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_A , 6 );
	
	Note( 0 , 2 );          //������ͣ��
	Note( note_A , 2 );
	Note( note_E , 2 );
	Note( note_D , 2 );
	Note( note_C , 2 );
	Note( note_D , 10 );
	
	Note( 0 , 4 ); 
	Note( note_E , 4 );      //��֪��ֻ�в��ϳ���
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 );
	Note( note_5C , 6 );
	
	Note( note_C , 2 );     //���ܹ�����������Ĳ���
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_5A , 5 );
	Note( note_G , 1 );
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( 0 , t );
	Note( note_G , 3 );
	Note( 0 , 0.1 );
	Note( note_D , 2 );
	Note( 0 , 0.1 );
	Note( note_D , 8 );
	
	Note( note_E , 4 );     //�����Ǻ���������
	Note( note_F , 4 );
	Note( note_G , 4 );
	Note( 0 , 0.5 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5D , 4 );
	Note( note_5C , 6 );
	
	Note( note_C , 2 );    //��˿��׷�
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 4 );
	Note( note_5A , 5 );

	Note( note_G , 1 );     //ֻ������ش�
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( note_5A , 3 );
	Note( note_5C , 4 );
	Note( note_5D , 6 );
	
	Note( 0 , 2 );
	Note( note_D , 2 );   //��Ը��ѽ
	Note( note_E , 2 );
	Note( note_D , 1 );
	Note( note_C , 12 );
	
	//������
	
	Note( 0 , 4 );
	Note( note_E , 4 );      //�Ҵ�������������
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 ); 
	Note( note_5C , 6 );
	
	Note( note_C , 2 );     //Ϊ������������޵�����
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_5A , 5 );
	Note( note_G , 1 );
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( 0 , 0.1 );
	Note( note_G , 3 );
	Note( 0 , 0.1 );
	Note( note_G , 4 );
	Note( note_D , 8 );
	
	Note( note_E , 4 );     //����������һ��Ц��
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 );
	Note( note_5C , 6 );

	Note( note_C , 2 );     //Ҳ���Һ�ɵ���Ҳ�����
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 4 );
	Note( note_5A , 6 );
	Note( 0 , t );
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( note_5A , 3 );
	Note( note_5C , 4 );
	Note( note_5D , 6 );
	
  Note( note_E , 4 );      //��֪��ֻ�в��ϳ���
	Note( note_F , 4 );
	Note( note_G , 6 );
	Note( note_E , 2 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5B , 4 );
	Note( note_5C , 6 );
	
	Note( note_C , 2 );     //���ܹ�����������Ĳ���
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_G , 4 );
	Note( note_5A , 5 );
	Note( note_G , 1 );
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( 0 , 0.1 );
	Note( note_G , 3 );
	Note( 0 , 0.1 );
	Note( note_G , 4 );
//	Note( note_D , 2 );
//	Note( 0 , 0.1 );
	Note( note_D , 8 );
	
	Note( note_E , 4 );     //�����Ǻ���������
	Note( note_F , 4 );
	Note( note_G , 4 );
	Note( 0 , 1 );
	Note( note_G , 2 );
	Note( note_E , 1 );
	Note( note_G , 3 );
	Note( note_5D , 4 );
	Note( note_5C , 6 );
	
	Note( note_C , 2 );    //��˿��׷�
	Note( note_D , 2 );
	Note( note_E , 2 );
	Note( note_5C , 4 );
	Note( note_5A , 5 );

	Note( note_G , 1 );     //ֻ������ش�
	Note( note_5A , 2 );
	Note( note_G , 1 );
	Note( note_5A , 3 );
	Note( note_5C , 4 );
	Note( note_5D , 6 );
	
	Note( 0 , 2 );
	Note( note_G , 2 );   //��Ը��ѽ
	Note( note_5C , 2 );
	Note( note_5B , 1 );
	Note( note_5C , 12 );
	
	//β
	Note( 0 , 4 );
	Note( note_E , 4 );
	Note( note_D , 4 );   
	Note( note_C , 4 );
	Note( note_G , 4 );
	Note( note_C , 4 );
	Note( note_D , 4 );
	Note( note_E , 4 );   
	Note( note_F , 4 );
	Note( note_G , 4 );
	Note( note_F , 4 );
	
	Note( note_E , 4 );
	Note( note_D , 4 );   
	Note( note_C , 4 );
	Note( note_D , 4 );
	Note( note_E , 4 );
	Note( note_F , 4 );
	Note( note_E , 4 );   
	Note( note_D , 4 );
	Note( note_C , 4 );
	Note( note_G , 4 );
	
	Note( note_E , 4 );
	Note( note_D , 4 );   
	Note( note_E , 4 );
	Note( note_F , 4 );
	Note( note_G , 4 );
	Note( note_F , 4 );
	Note( note_E , 4 );   
	Note( note_D , 4 );
	
	Note( note_E , 4 );   
	Note( note_D , 4 );
	Note( note_E , 4 );
	Note( note_F , 4 );
	
	Note( note_E , 4 );
	Note( note_D , 4 );   
	Note( note_C , 4 );
	Note( note_G , 4 );
	Note( note_C , 4 );
	Note( note_D , 4 );
	Note( note_E , 4 );   
	Note( note_F , 4 );
	Note( note_G , 4 );
	Note( note_F , 4 );
	Note( note_E , 4 );
	Note( note_D , 4 ); 
	
}
	

