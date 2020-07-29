#ifndef ENVIR_H  
#define ENVIR_H  

#include "vector.h"

typedef enum devSort
{
    VariFreqFan1 = 0,
    VariFreqFan2,
    VariFreqFan3,
    Fan1,
    Fan2,
    Fan3,
    Fan3,
    Fan5,
    Fan6,
    CurTain1,
    CurTain2,
    devMax,


}devSort;


typedef enum modClassify
{
    EarlyNurseryWinner = 0,
    EarlyNurserySpring,
    EarlyNurserySummer,
    MidNurseryWinner,
    MidNurserySpring,
    MidNurserySummer,
    HigherNurseryWinner,
    HigherNurserySpring,
    HigherNurserySummer,
    FattenWinner,
    FattenSpring,
    FattenSummer,
    ModMax,
}


#endif

