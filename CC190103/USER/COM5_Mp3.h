#ifndef COM_MP3_H_1110_
#define COM_MP3_H_1110_
typedef enum
{
	MP3_PLAY = 0x01,
	MP3_STOP = 0x02,
  MP3_SETVOL = 0x31,
	MP3_SELMUSIC = 0x41,
	MP3_SELDOCMUSIC = 0x42,
	MPC_SELDOC_SETMUS = 0x44,//选择文件名及歌曲名
}MP3_CMD;
typedef enum
{
  Folder_NO1 = 1,
	Folder_NO2 = 2,
}MP3_Folder;
typedef struct
{
	MP3_CMD Type;
	MP3_Folder Folder_Num;
	unsigned short para;
}MP3_SET;


#endif

