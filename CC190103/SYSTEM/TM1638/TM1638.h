/*=======================================
06      02      0E      0A
16      12      1E      1A
07      03      0F      0B
17      13      1F      1B
/=======================================*/
//===TM1638���Ŷ���
#define TM1638_STB_PORT (GPIOA)
#define TM1638_STB_PORT_PER (RCC_AHB1Periph_GPIOA)
#define TM1638_STB_PIN  (GPIO_Pin_14)    //λ�����ʽ
#define TM1638_STB_PIN_NUM  (14)         //λ�����ʽ

#define TM1638_CLK_PORT (GPIOC)
#define TM1638_CLK_PORT_PER (RCC_AHB1Periph_GPIOC)
#define TM1638_CLK_PIN  (GPIO_Pin_6)
#define TM1638_CLK_PIN_NUM  (6)

#define TM1638_DIO_PORT (GPIOC)
#define TM1638_DIO_PORT_PER (RCC_AHB1Periph_GPIOC)
#define TM1638_DIO_PIN  (GPIO_Pin_7)
#define TM1638_DIO_PIN_NUM  (7)





#define TM_STB(x) PAout(TM1638_STB_PIN_NUM)=(x)
#define TM_CLK(x) PCout(TM1638_CLK_PIN_NUM)=(x)
#define TM_DIO(x) PCout(TM1638_DIO_PIN_NUM)=(x)
#define TM_READ   PCin(TM1638_DIO_PIN_NUM)



//#define TM_STB(x) GPIO_WriteBit(TM1638_STB_PORT,TM1638_STB_PIN_NUM,(x))
//#define TM_CLK(x) GPIO_WriteBit(TM1638_CLK_PORT,TM1638_CLK_PIN_NUM,(x))
//#define TM_DIO(x) GPIO_WriteBit(TM1638_DIO_PORT,TM1638_DIO_PIN_NUM,(x))
//#define TM_READ GPIO_ReadInputDataBit(TM1638_DIO_PORT,TM1638_DIO_PIN)


extern u8 DIS_BUF[];
extern u8 KEY;
extern u16 dis_data[];

void Init_TM1638(void);         //��ʼ������
void TM1638_PLAY(void);         //��ʾ����ֵ����������2ms
void TM1638_DIS(u8 da);         //���ȵ��ڡ�0���ر� 1-8������״̬��
void TM1638task (void* pa);