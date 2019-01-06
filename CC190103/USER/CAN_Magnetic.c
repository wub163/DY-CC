#include "CAN_Magnetic.h"
#include "common.h"
void MagIO_Check(void)
{
	int temp = 0;
	int num = 0;




	if(gInnerData->fx==1)
	{
	gHalData->forward&=0xf8;
	}

	if(gInnerData->fx==-1)
	{
	 gHalData->forward&=0x1f;
	}


	//???8?IO???????????????
	gInnerData->MagFIOX = 0;
	for(temp = 0; temp < 8; temp++)
	{
	    if(1 == ((gHalData->forward>>temp)&1))
		{
		    num++;
		}
	    gInnerData->MagFIOX	+=(s16)(((gHalData->forward>>temp)&1) * (temp - 3.5) * gInnerData->MagDist);
	}
	//?????????IO???????,??????????
        gHalData->num=num;
	if(0 != num)
	{
	    //?????,????????????
		gInnerData->MagFIOX = gInnerData->MagFIOX / num;
	    gInnerData->MagFBefore = gInnerData->MagFIOX;	
		gInnerData->OnMag = 1;

	}
	else
	{
	    //???????,???????????,???????
	    if(gInnerData->MagFBefore > 0)
		{
	    	gInnerData->MagFIOX = 40;
		}
		else
		{
		gInnerData->MagFIOX = -40;
		} 
		gInnerData->OnMag = 0;
          }
	return;
}

