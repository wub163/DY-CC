//#include "netconf.h"
#include "stm32f4xx.h"


#define powerruntemp    -15        //���޵���
#define powerchtemp     0          //��缫�޵���
#define powervol    23000          //���޵�ѹ
#define miniturn    5



//=====================
//mp3��ʾ����
#define mute                    0       //����


//240-250Ϊ��ʾ��
#define mp3_powerlow            240       //��ѹ������ʾ
#define mp3_powertemplow        241       //����¶ȵ���ʾ��
#define mp3_stop                242       //��ͣ����
#define mp3_stop_lock           243       //��ͣ������
#define initmp3vol              10        //ϵͳĬ������
#define initmp3num              1         //ϵͳ�������֡�



#define speedinc       4
#define speeddec        8