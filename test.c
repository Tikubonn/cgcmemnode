
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memnode.h"
#include "cgcmemnode.h"

#define TEST(form)\
if (form){\
printf("success: %s at %s:%d\n", #form, __FILE__, __LINE__);\
}\
else {\
printf("error: %s at %s:%d\n", #form, __FILE__, __LINE__); abort();\
}

static void test_cgcmemnode_gc (){
  // allocate and decrease. 
  {
    MAKE_LOCAL_CGCMEMNODE(cmemnode, 128, CGCMEMNODE_DEFAULT_RESOLUTION, NULL);
    int *value1 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value3 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1 != NULL);
    TEST(value2 != NULL);
    TEST(value3 != NULL);
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    TEST(*value1 == 1);
    TEST(*value2 == 2);
    TEST(*value3 == 3);
    TEST(cgcmemnode_decrease(value1, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value2, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value3, sizeof(int), cmemnode) == 0);
    int *value4 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value5 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value6 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value4 == value1);
    TEST(value5 == value2);
    TEST(value6 == value3);
  }
  // allocate and decrease. 
  {
    MAKE_LOCAL_CGCMEMNODE(cmemnode, sizeof(int) * 3, CGCMEMNODE_DEFAULT_RESOLUTION, NULL);
    int *value1 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value3 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value4 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1 != NULL);
    TEST(value2 != NULL);
    TEST(value3 != NULL);
    TEST(value4 == NULL);
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    TEST(*value1 == 1);
    TEST(*value2 == 2);
    TEST(*value3 == 3);
    TEST(cgcmemnode_decrease(value1, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value2, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value3, sizeof(int), cmemnode) == 0);
    int *value1a = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2a = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value3a = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value4a = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1a == value1);
    TEST(value2a == value2);
    TEST(value3a == value3);
    TEST(value4a == NULL);
  }
  // allocate, increase and decrease.
  {
    MAKE_LOCAL_CGCMEMNODE(cmemnode, sizeof(int) * 3, sizeof(int), NULL);
    int *value1 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value3 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value4 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1 != NULL);
    TEST(value2 != NULL);
    TEST(value3 != NULL);
    TEST(value4 == NULL);
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    TEST(*value1 == 1);
    TEST(*value2 == 2);
    TEST(*value3 == 3);
    TEST(cgcmemnode_increase(value2, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value1, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value2, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value3, sizeof(int), cmemnode) == 0);
    int *value1a = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2a = value2;
    int *value3a = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value4a = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1a == value1);
    TEST(value2a == value2);
    TEST(value3a == value3);
    TEST(value4a == NULL);
    *value1a = 1;
    *value2a = 2;
    *value3a = 3;
    TEST(*value1a == 1);
    TEST(*value2a == 2);
    TEST(*value3a == 3);
  }
}

static void test_make_cgcmemnode (){
  {
    cgcmemnode *cmemnode = make_cgcmemnode(128, CGCMEMNODE_DEFAULT_RESOLUTION, NULL);
    TEST(cmemnode != NULL);
    int *value1 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value2 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value3 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value1 != NULL);
    TEST(value2 != NULL);
    TEST(value3 != NULL);
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    TEST(*value1 == 1);
    TEST(*value2 == 2);
    TEST(*value3 == 3);
    TEST(cgcmemnode_decrease(value1, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value2, sizeof(int), cmemnode) == 0);
    TEST(cgcmemnode_decrease(value3, sizeof(int), cmemnode) == 0);
    int *value4 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value5 = cgcmemnode_allocate(sizeof(int), cmemnode);
    int *value6 = cgcmemnode_allocate(sizeof(int), cmemnode);
    TEST(value4 == value1);
    TEST(value5 == value2);
    TEST(value6 == value3);
    free_cgcmemnode(cmemnode);
  }
}

static void test_make_cgcmemnode2 (){
  {
    cgcmemnode *mnode = make_cgcmemnode(1024, 40, NULL);
    TEST(mnode != NULL);
    void *value1 = cgcmemnode_allocate(40, mnode);
    TEST(value1 != NULL);
    TEST(cgcmemnode_increase(value1, 40, mnode) == 0);
    TEST(cgcmemnode_decrease(value1, 40, mnode) == 0);
    void *value2 = cgcmemnode_allocate(40, mnode);
    TEST(value2 - value1 == 40);
    TEST(value2 != NULL);
    TEST(cgcmemnode_increase(value2, 40, mnode) == 0);
    TEST(cgcmemnode_decrease(value2, 40, mnode) == 0);
    void *value3 = cgcmemnode_allocate(40, mnode);
    TEST(value3 != NULL);
    TEST(value3 - value2 == 40);
    TEST(cgcmemnode_increase(value3, 40, mnode) == 0);
    TEST(cgcmemnode_decrease(value3, 40, mnode) == 0);
    free_cgcmemnode(mnode);
  }
}

int main (){
  test_cgcmemnode_gc();
  test_make_cgcmemnode();
  test_make_cgcmemnode2();
  return 0;
}
