////////////cvector.h////////////////////////////////  
#ifndef VECTOR_H  
#define VECTOR_H  
  
#include <stdlib.h>  
#include <assert.h>  


#define CONSTRAINTER_NUM 2
typedef struct conditionRunner
{
    uchar constraint[CONSTRAINTER_NUM][2];

    uchar state[devMax];

}conditionRunner;

typedef struct VectorSt {  
    int size;  
    int maxSize;  
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

