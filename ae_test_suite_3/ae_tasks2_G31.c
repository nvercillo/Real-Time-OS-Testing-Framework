/**************************************************************************//**
 * @file        ae_task1_G31.c
 * @brief       Lab4 testing suite 2 
 *
 * @version     V1.2021.06
 * @authors     Stefan Vercillo
 * @date        2021 Jun
 * *
 *****************************************************************************/

#include "suite2_tasks.h"



void init_ae_tsk_test(void)
{
    g_ae_student_xtest.test_id = 0;
    g_ae_student_xtest.index = 0;
    g_ae_student_xtest.num_tests = NUM_TESTS;
    g_ae_student_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_student_tsk_cases[i].p_ae_case = &g_ae_student_cases[i];
        g_student_tsk_cases[i].p_ae_case->results  = 0x0;
        g_student_tsk_cases[i].p_ae_case->test_id  = i;
        g_student_tsk_cases[i].p_ae_case->num_bits = 0;
        g_student_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
    }
    tasks[0].prio = LOW;
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].prio = HIGH;
    tasks[1].priv  = 0;
    tasks[1].ptask = &silent_task;
    
    init_ae_tsk_test();
}

/* first created tsk  */
void priv_task1(void)
{
    mbx_t  mbx_id;
    task_t tid;
    
    g_student_priviledged_task = tsk_gettid();
    tid = tsk_gettid();
    
    // create a mailbox for current task
    mbx_id = mbx_create(GET_BUFSIZE(G_MSG_LEN));
    
    start_test0();
    start_test1();

    test_exit();
}

/*
 *===========================================================================
 *                             TEST DEFINITIONS 
 *===========================================================================
 */

/**
 * @brief Test the full functionality of blocking behaviour
 *      Case 0: test that when mbx empty, correct seq of events occurs
 *      Case 1: mbx_rmv() preempts properly w empty mbx and blocked tsks
 *      Case 2: tsk_exit() preempts properly w empty mbx and blocked tsks
 *      Case 3: test that when mbx full, correct seq of events occurs
 *      Case 4: mbx_rmv() preempts properly w empty mbx and blocked tsks 
 *      Case 5: tsk_exit() preempts properly w empty mbx and blocked tsks
 */
void start_test0(void)
{
    /* Test Setup*/
    int res;
    task_t current_task, tid1, tid2;
    int test_index = 0;
    gen_req_generic(test_index, 2);
    U8  *p_index = &(g_ae_student_xtest.index);

    /* CASE 0 && 1 */
    tsk_create(&tid1, &test0_case0_task0, HIGH, 0x200);
}

void start_test1(void)
{
    /* Test Setup*/
    int res;
    task_t curr_tid, tid1, tid2, tid3, tid4;
    int test_index = 1;
    gen_req_generic(test_index, 5);
    U8  *p_index = &(g_ae_student_xtest.index);
    curr_tid = tsk_gettid();

    // /* CASE 0 */
    tsk_create(&tid1, &test1_case0_task0, MEDIUM, 0x200);

    /* CASE 1 */
    tsk_create(&tid2, &test1_case1_task0, MEDIUM, 0x200);

    /* CASE 2 */
    tsk_create(&tid3, &test1_case2_task0, MEDIUM, 0x200);

    /* CASE 3 && 4 && 5*/
    ARR_N_CNTR * arr_n_cntr; 
    
    arr_n_cntr = mem_alloc(sizeof(ARR_N_CNTR));
    arr_n_cntr->len = 0;
    arr_n_cntr->max_len = 5;

    /*  NOTE: 
        U32 indicates the order of execution, 
        shoud be 1->2->3->4->5.
    */
    arr_n_cntr->arr = mem_alloc(sizeof(U32) * arr_n_cntr->max_len);

    g_student_tsk_cases[test_index].data = (U32) arr_n_cntr;
    
    
    // set try yields at a MED prior
    arr_n_cntr->data = MEDIUM; 
    tsk_create(&tid4, &test1_case3_task0, arr_n_cntr->data, 0x200);
    tsk_yield();


    // set try yields at a HIGH prior
    arr_n_cntr->data = HIGH; 
    tsk_create(&tid4, &test1_case3_task0, arr_n_cntr->data, 0x200);

    mem_dealloc(arr_n_cntr->arr);
    mem_dealloc(arr_n_cntr);
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
