#include "vector.h" 

  
Vector VectorNew(void)  
{  
    Vector v = (Vector)  
        malloc(sizeof(struct VectorSt));  
    assert(v != NULL);  
    v->size = 0;  
    v->maxSize = 8;  
    v->data = (conditionRunner *)malloc(sizeof(conditionRunner)* v->maxSize);  
    assert(v->data != NULL);  
    return v;  
}  
  
void VectorPushBack(Vector v, conditionRunner e)  
{  
    assert(v != NULL);  
    if (v->size >= v->maxSize) {  
  
        v->maxSize *= 2;  
        v->data = (conditionRunner *)realloc(v->data, v->maxSize * sizeof(conditionRunner));  
        assert(v->data != NULL);  
    }  
  
    v->data[v->size++] = e;  
}  
  
conditionRunner VectorPopBack(Vector v)  
{  
    assert(v != NULL && v->size > 0);  
    return v->data[--v->size];  
}  
  
conditionRunner VectorGet(Vector v, int index)  
{  
    assert(v != NULL && index >= 0 && index < v->size);  
    return v->data[index];  
}  
  
int VectorSize(Vector v)  
{  
    assert(v != NULL);  
    return v->size;  
}  
  
int VectorMaxSize(Vector v)  
{  
    assert(v != NULL);  
    return v->maxSize;  
}  
  
void VectorRm(Vector v, int index)  
{  
    assert(v != NULL || index <= v->size - 1);  
    int i;  
    if (index< v->size - 1){  
        for (i = index; i < v->size - 1; ++i)  
            v->data[i] = v->data[i + 1];  
        v->size--;  
    }  
    else{  
        v->size--;  
    }  
}  
  
void VectorDelete(Vector v)  
{  
    assert(v != NULL && v->data != NULL);  
    free(v->data);  
    free(v);  
}  

  

