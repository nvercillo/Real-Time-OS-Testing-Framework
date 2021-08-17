#ifndef SUITE3_TASKS_H
#define SUITE3_TASKS_H
#include "student_inc.h"



// TEST SUITE  2 
#define     NUM_TESTS       2       // number of tests
#define num_blocked_tasks   5 

void test0_case0_task0(void);
void test0_case0_task1(void);
void test0_case1_task0(void);
void test0_case1_task1(void);
void test1_case0_task0(void);
void test1_case0_task1(void);
void test1_case1_task0(void);
void test1_case1_task1(void);
void test1_case2_task0(void);
void test1_case2_task1(void);
void test2_case4_task0(void);
void test2_case5_task0(void);
void test3_case0_task0(void);
void test3_case0_task1(void);

void mk_task_tq_2(void);
void mk_task_tq_3(void);
void mk_task_tq_4(void);
void mk_task_tq_5(void);
void mk_task_tq_8(void);

void recieving_mbx(void);

void gen_expected_val_test1(U8 *** exp_arr);
#endif