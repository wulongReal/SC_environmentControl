#include "envir.h"

Vector gDevCondState[ModMax] = {0,0,0,0,0,0,0,0,0,0,0,0};


void initCondState()
{
    int i = 0;

    for(i = 0; i < ModMax;i++)
    {
        gDevCondState[i] = VectorNew();
    }

    ///一两周保育冬季模式初始化
    conditionRunner a1 = {{{0,27},{0,1}},{30,0,0,0,0,0,0}};
    conditionRunner a2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner a3 = {{{28,29},{0,1}},{100,1,0,0,0,0,0}};
    VectorPushBack(gDevCondState[EarlyNurseryWinner],a1);  
    VectorPushBack(gDevCondState[EarlyNurseryWinner],a2);  
    VectorPushBack(gDevCondState[EarlyNurseryWinner],a3);  

    ///一两周保育春秋模式初始化
    conditionRunner b1 = {{{0,27},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner b2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner b3 = {{{28,38},{0,1}},{100,1,0,0,0,0,0}};
    VectorPushBack(gDevCondState[EarlyNurserySpring],b1);  
    VectorPushBack(gDevCondState[EarlyNurserySpring],b2);  
    VectorPushBack(gDevCondState[EarlyNurserySpring],b3);  


    ///一两周保育夏季模式初始化
    conditionRunner d1 = {{{0,27},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner d2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner d3 = {{{28,29},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner d4 = {{{29,30},{0,1}},{30,1,1,1,0,0,0}};
    conditionRunner d5 = {{{30,31},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner d6 = {{{31,32},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner d7 = {{{32,38},{0,1}},{100,1,1,1,1,1,1}};
    VectorPushBack(gDevCondState[EarlyNurserySummer],d1);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d2);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d3);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d4);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d5);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d6);  
    VectorPushBack(gDevCondState[EarlyNurserySummer],d7);  

     ///三四周保育冬季模式初始化
    conditionRunner e1 = {{{0,25},{0,1}},{50,0,0,0,0,0,0}};
    conditionRunner e2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner e3 = {{{26,38},{0,1}},{100,1,0,0,0,0,0}};
    VectorPushBack(gDevCondState[MidNurseryWinner],e1);  
    VectorPushBack(gDevCondState[MidNurseryWinner],e2);  
    VectorPushBack(gDevCondState[MidNurseryWinner],e3);  

    ///三四周保育春秋模式初始化
    conditionRunner f1 = {{{0,25},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner f2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner f3 = {{{26,27},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner f4 = {{{27,28},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner f5 = {{{28,38},{0,1}},{100,1,1,0,0,0,0}};
    VectorPushBack(gDevCondState[MidNurserySpring],f1);  
    VectorPushBack(gDevCondState[MidNurserySpring],f2);  
    VectorPushBack(gDevCondState[MidNurserySpring],f3);  
    VectorPushBack(gDevCondState[MidNurserySpring],f4);  
    VectorPushBack(gDevCondState[MidNurserySpring],f5); 


    ///三四周保育夏季模式初始化
    conditionRunner g1 = {{{0,25},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner g2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner g3 = {{{26,27},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner g4 = {{{27,28},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner g5 = {{{28,29},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner g6 = {{{29,30},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner g7 = {{{30,38},{0,1}},{100,1,1,1,1,1,1}};
    VectorPushBack(gDevCondState[MidNurserySummer],g1);  
    VectorPushBack(gDevCondState[MidNurserySummer],g2);  
    VectorPushBack(gDevCondState[MidNurserySummer],g3);  
    VectorPushBack(gDevCondState[MidNurserySummer],g4);  
    VectorPushBack(gDevCondState[MidNurserySummer],g5);  
    VectorPushBack(gDevCondState[MidNurserySummer],g6);  
    VectorPushBack(gDevCondState[MidNurserySummer],g7);  


    ///五六周保育冬季模式初始化
    conditionRunner h1 = {{{0,23},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner h2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner h3 = {{{24,38},{0,1}},{100,1,0,0,0,0,0}};
    VectorPushBack(gDevCondState[HigherNurseryWinner],h1);  
    VectorPushBack(gDevCondState[HigherNurseryWinner],h2);  
    VectorPushBack(gDevCondState[HigherNurseryWinner],h3);  

    ///五六周保育春秋模式初始化
    conditionRunner i1 = {{{0,23},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner i2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner i3 = {{{24,25},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner i4 = {{{25,26},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner i5 = {{{26,38},{0,1}},{100,1,1,0,0,0,0}};
    VectorPushBack(gDevCondState[HigherNurserySpring],i1);  
    VectorPushBack(gDevCondState[HigherNurserySpring],i2);  
    VectorPushBack(gDevCondState[HigherNurserySpring],i3);  
    VectorPushBack(gDevCondState[HigherNurserySpring],i4);  
    VectorPushBack(gDevCondState[HigherNurserySpring],i5); 


    ///五六周保育夏季模式初始化
    conditionRunner j1 = {{{0,23},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner j2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner j3 = {{{24,25},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner j4 = {{{25,26},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner j5 = {{{26,27},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner j6 = {{{27,28},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner j7 = {{{28,38},{0,1}},{100,1,1,1,1,1,1}};
    VectorPushBack(gDevCondState[HigherNurserySummer],j1);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j2);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j3);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j4);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j5);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j6);  
    VectorPushBack(gDevCondState[HigherNurserySummer],j7);  



     ///育肥冬季模式初始化
    conditionRunner k1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner k2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner k3 = {{{22,38},{0,1}},{100,1,0,0,0,0,0}};
    VectorPushBack(gDevCondState[FattenWinner],k1);  
    VectorPushBack(gDevCondState[FattenWinner],k2);  
    VectorPushBack(gDevCondState[FattenWinner],k3);  

    ///育肥春秋模式初始化
    conditionRunner m1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner m2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner m3 = {{{22,23},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner m4 = {{{23,24},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner m5 = {{{24,38},{0,1}},{100,1,1,0,0,0,0}};
    VectorPushBack(gDevCondState[FattenSpring],m1);  
    VectorPushBack(gDevCondState[FattenSpring],m2);  
    VectorPushBack(gDevCondState[FattenSpring],m3);  
    VectorPushBack(gDevCondState[FattenSpring],m4);  
    VectorPushBack(gDevCondState[FattenSpring],m5); 


    ///育肥夏季模式初始化
    conditionRunner n1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner n2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner n3 = {{{22,23},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner n4 = {{{23,24},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner n5 = {{{24,25},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner n6 = {{{25,26},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner n7 = {{{26,38},{0,1}},{100,1,1,1,1,1,1}};
    VectorPushBack(gDevCondState[FattenSummer],n1);  
    VectorPushBack(gDevCondState[FattenSummer],n2);  
    VectorPushBack(gDevCondState[FattenSummer],n3);  
    VectorPushBack(gDevCondState[FattenSummer],n4);  
    VectorPushBack(gDevCondState[FattenSummer],n5);  
    VectorPushBack(gDevCondState[FattenSummer],n6);  
    VectorPushBack(gDevCondState[FattenSummer],n7);  
}


/***************************************************************************************************************
返回:设备运行的比例0 - 100。非变频设备:0 关闭，100 打开；变频设备输出运行百分比
输入参数:
mode 环境模式(含季节模式)
dev  设备类型
evirParaN 环境参数个数
返回:
0 -100 为正常返回，大于100返回出错。

******************************************************************************************************************/

uchar getDevState(uchar mode,devSort dev,uchar evirPara[])
{
    if(mode >= ModMax || dev >= devMax)
    {
        return 101;
    }

    
    int size = VectorSize(gDevCondState[mode]);

    for(int i = 0;i < size;i++)
    {
        conditionRunner condition = VectorGet(gDevCondState[mode], i);

        int flag = 1;
        for(int j = 0;j < CONSTRAINTER_NUM;j++)
        {
            if(evirPara[j]< condition.constraint[j][0] || evirPara[j] >= condition.constraint[j][1])
            {

                flag = 0;
                break;
            }

        }

        if(flag)
        {
            return condition.state[dev];
        }
    }

    return 101;
  
}




