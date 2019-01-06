#include "sys.h"

#define dbh GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define dbl GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define busww PAout(5)           //位操作，速度快三分之一
#define buswr PAin(5)
void dy_bus_init(void);
void wb(u8 da);
u8 rb(void);
void write(u8 add,u8 da);
void read(void);

typedef struct power{
 float pjdl;
float wd;
float dy;
float dl;
}powercs;
extern powercs pcs;