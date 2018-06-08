#include <ioCC2530.h>
#define LED_R P0_6            //定义P0.6口为LED_R控制端
#define LED_G P0_5            //定义P0.5口为LED_G控制端
#define LED_B P0_4            //定义P0.4口为LED_B控制端
#define SENSOR_INPUT P0_7     //定义P0.7口为传感器输入端
#define RGB_COLOR_COUNT 7     //RGB彩虹颜色的个数


typedef unsigned char uchar;
typedef unsigned int  uint;

struct RGB {
  uchar R;
  uchar G;
  uchar B;
};

struct RGB RGBLIST[RGB_COLOR_COUNT]; // RGB颜色数组

void InitT1(uchar ucR, uchar ucG, uchar ucB)
{
    EA=0;
    PERCFG = 0x00;  //位置1 定时器1,3,4，uart0,1外设端口都设置为位置1，
    P0SEL |= 0x70;  //P04-6用作外设接口
    P0DIR |= 0x70;      //output 
    P2DIR = 0x80;       //P0优先作为TIMER1()1st priority: Timer 1 channels 0-1
    PERCFG |= 0x03;     //优先级关键
    
    T1CCTL0 |= 0x04;    //设定timer1通道0输出比较模式 ，用以触发中断

    //清除中断标志
    IRCON &= ~0x02;     //定时器1中断标志。当定时器1中断发生时设为1并且当CPU向量指向中断服务例程时清除。
    T1STAT &= ~0x1f;    //第0~4位计数溢出标志 ~0x1f;
    
    T1CTL = 0x04;       //定时器1停止运行  0x04=8分频 Prd=64us Freq=15.63KHz
                        //0x00=1分频 Prd=8us Freq=125KHz  0x08=32分频  Prd=256us Freq=3.91KHz
    T1CNTL = 0x00; 
    T1CNTH = 0x00;
    T1CC0L = 0xff;      //时间周期 T1CC0L=  255;为8us  125kHZ  定时器主频32M
    T1CC0H = 0x00;
   
    //p0_4  蓝色   ucR=255 关闭RGB 
    if(ucB == 0)        
    {
        T1CCTL2 = 0x0c;
        ucB = 255;      //使其最长时间触发中断
    }
    else if(ucB == 255)
        T1CCTL2 = 0x04;    
    else
        T1CCTL2 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除   
    T1CC2L = ucB;       //占空比 80/200=0.4为 40%
    T1CC2H = 0x00;  
    
    //p0_5  绿色
    if(ucG == 0)        
    {
        T1CCTL3 = 0x0c;
        ucG = 255;      //使其最长时间触发中断
    }
    else if(ucG == 255)
        T1CCTL3 = 0x04;
    else
        T1CCTL3 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除         
    T1CC3L = ucG;       //占空比 80/200=0.4为 40%
    T1CC3H = 0x00;  
 
    //p0_6  红色
    if(ucR == 0)        
    {
        T1CCTL4 = 0x0c;
        ucR = 255;      //使其最长时间触发中断
    }
    else if(ucR == 255)
        T1CCTL4 = 0x04;
    else
        T1CCTL4 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除    
    T1CC4L = ucR;       //占空比 51/255为 20%
    T1CC4H = 0x00;  

    T1CTL |= 0x02;      // 模，从0x0000到T1CC0反复计数  分频器划分值。产生主动的时钟边缘用来更新计数器，00：不分频
    
    T1IE = 1;           //使能Timer1中断           
    EA = 1;             //开启总中断  
}


void SetT1(uchar ucR, uchar ucG, uchar ucB)
{   
    //p0_4  蓝色  //ucR=255 关闭RGB
    if(ucB == 0)        
    {
        T1CCTL2 = 0x0c;
        ucB = 255;      //使其最长时间触发中断
    }
    else if(ucB == 255)
        T1CCTL2 = 0x04;    
    else
        T1CCTL2 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除   
    T1CC2L = ucB;       //占空比 80/200=0.4为 40%
    T1CC2H = 0x00;  
    
    //p0_5  绿色
    if(ucG == 0)        
    {
        T1CCTL3 = 0x0c;
        ucG = 255;      //使其最长时间触发中断
    }
    else if(ucG == 255)
        T1CCTL3 = 0x04;
    else
        T1CCTL3 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除         
    T1CC3L = ucG;       //占空比 80/200=0.4为 40%
    T1CC3H = 0x00;  
 
    //p0_6  红色
    if(ucR == 0)        
    {
        T1CCTL4 = 0x0c;
        ucR = 255;      //使其最长时间触发中断
    }
    else if(ucR == 255)
        T1CCTL4 = 0x04;
    else
        T1CCTL4 = 0x34; //T1CCTL1.CMP[2:0]=110 当等于T1CC0时设置, 当等于T1CC1时清除    
    T1CC4L = ucR;       //占空比 51/255为 20%
    T1CC4H = 0x00;  
}


//延时函数 
void DelayMS(uint msec)
{
  uint i,j;
  for (i=0; i<msec; i++)
    for (j=0; j<535; j++);
}


//RGB彩虹颜色设置
void SetRGB() 
{
  //赤
  RGBLIST[0].R = 255;
  RGBLIST[0].G = 0;
  RGBLIST[0].B = 0; 
  
  //橙 
  RGBLIST[1].R = 255;
  RGBLIST[1].G = 128;
  RGBLIST[1].B = 0; 
  
  //黄
  RGBLIST[2].R = 255;
  RGBLIST[2].G = 255;
  RGBLIST[2].B = 0;
  
  //绿  
  RGBLIST[3].R = 0;
  RGBLIST[3].G = 255;
  RGBLIST[3].B = 0;
  
  //青
  RGBLIST[4].R = 0;
  RGBLIST[4].G = 127;
  RGBLIST[4].B = 255; 
  
  //蓝  
  RGBLIST[5].R = 0;
  RGBLIST[5].G = 0;
  RGBLIST[5].B = 255;
  
  //紫 
  RGBLIST[6].R = 139;
  RGBLIST[6].G = 0;
  RGBLIST[6].B = 255;
}


//IO初始化
void InitKey()
{
    P0IEN |= 0x02;    // P0.1 设置为中断方式 1：中断使能
    PICTL |= 0x01;    //下降沿触发   
    IEN1 |= 0x20;    //允许P0口中断; 
    P0IFG &= 0x00;    //初始化中断标志位
    EA = 1;          //打开总中断
}

//点亮彩带
void LightRGB()
{
  uchar i;
  for (i=0; i<RGB_COLOR_COUNT; i++)
  {
    SetT1(RGBLIST[i].R, RGBLIST[i].G, RGBLIST[i].B);
    DelayMS(1000);
  }
}


//程序入口函数
void main(void)
{
  SetRGB();
  InitT1(0, 0, 0);
  P0DIR &= ~0x80;             //P0.7定义为输入口
  
  while(1)                    //死循环
  {
    if(SENSOR_INPUT == 1)    //当光敏电阻处于黑暗中时P0.7高电平
    {
      LightRGB();            //点亮彩带 
    }  
    else
    {
      SetT1(0,0,0);          //检测到光线时P0.7为低电平
      DelayMS(1000);
    }
  }
}
