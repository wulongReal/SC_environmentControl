////////////cvector.h////////////////////////////////  
#ifndef VECTOR_H  
#define VECTOR_H  
  
#include <stdlib.h>  
#include <assert.h>  
#include "stdint.h"




#define CONSTRAINTER_NUM 2//限制条件
typedef struct conditionRunner
{
    uint8_t constraint[CONSTRAINTER_NUM][2];

    uint8_t state[7];

}conditionRunner;

typedef struct VectorSt {  
    uint8_t size;  
    uint8_t maxSize;  
    conditionRunner *data;  
} *Vector;  
  
  
  
Vector VectorNew(void);  
void VectorPushBack(Vector v, conditionRunner e);  
conditionRunner VectorPopBack(Vector v);  
conditionRunner VectorGet(Vector v, int index);  
int VectorSize(Vector v);  
int VectorMaxSize(Vector v);  
void VectorRm(Vector v, int index);  
void VectorDelete(Vector v);  
  
#endif  

