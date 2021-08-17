

/**************************************************************************//**
 * @file        ae_tasks.h
 * @brief       tasks header file
 *
 * @version     V1.2021.05
 * @date        2021 MAY
 *
 *****************************************************************************/

#ifndef AE_TASKS_H_
#define AE_TASKS_H_

#include "rtx.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
#ifdef SIM_TARGET       // using the simulator is slow
#define DELAY 100000
#else
#define DELAY 10000000
#endif // SIM_TARGET

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */

extern void priv_task1      (void);
void task1                  (void);
void task2                  (void);
void task3                  (void);
void silent_task            (void);
void empty_task             (void);
void test1_task1            (void);
void test6_task1            (void);
void test6_task2            (void);
void test6_task3            (void);
void test7_task1            (void);
void test8_task1            (void);
void test8_task2            (void);
void test9_task1            (void);
void test9_task2            (void);
void test9_task3            (void);

int test0_200_start            (int test_id);
void test1_200_start            (int test_id, int test_id_data);

void gen_req0               (int test_id);
void test0_start            (void);
void test1_start            (void);
void test2_start            (void);
void test3_start            (void);
void test4_start            (void);
void test5_start            (void);
void test6_start            (void);
void test7_start            (void);
void test8_start            (void);
void test9_start            (void);

void init_ae_tsk_test       (void);
int  update_exec_seq        (int test_id, task_t tid);

#endif // !AE_TASKS_H_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */