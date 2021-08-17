/**************************************************************************//**
 * @file        ae_task1_G31.c
 * @brief       Lab4 testing suite 3
 *
 * @version     V1.2021.06
 * @authors     Stefan Vercillo
 * @date        2021 Jun
 * *
 *****************************************************************************/
#include "suite3_tasks.h"

// test 2 
#define     NUM_TESTS       2       // number of tests

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
 * @brief Test EDF scheduler 
 *      Case 0: test that preemption for RTT occurs for MED tsk 
 *      Case 1: test that preemption for RTT occurs for HIGH tsk 
 */
void start_test0(void)
{
    /* Test Setup*/
    int res;
    task_t curr_tid, tid1, tid2;
    int test_index = 0;
    gen_req_generic(test_index, 2); // 2 case per test suite

    U8  *p_index = &(g_ae_student_xtest.index);
    curr_tid = tsk_gettid();
    
    SNDR_HDR* sndr_hdr = mem_alloc(sizeof(SNDR_HDR));

    sndr_hdr->len_arr = 0;
    sndr_hdr->max_len = 10;
    sndr_hdr->sender_tid_arr = mem_alloc(sizeof(task_t) * sndr_hdr->max_len);
    
    g_student_tsk_cases[test_index].data = (U32) sndr_hdr;

    tsk_create(&tid2, &test0_case0_task0, MEDIUM, 0x200);
    tsk_create(&tid2, &test0_case0_task0, HIGH, 0x200);

    mem_dealloc(sndr_hdr->sender_tid_arr);
    mem_dealloc(sndr_hdr);
}

/**
 * @brief Test EDF scheduler 
 *      Case 0: test that order of execution is followed
 */
void start_test1(void)
{
    /* NOTE:  
    There are 2 two mbxs used. One is used to assess that all of
    of the sending threads have terminate, and one is a 
    seperate receiving mbx */

    /* Test Setup*/

    mbx_rmv(); // remove previously created mbx in priv1 
    int res, recv_res1, recv_res2, recv_res3, recv_res4;
    task_t curr_tid, mbx_recv_tid, tid1, tid2, tid3, tid4;
    
    int test_index = 1;

    U32 num_different_threads = 5;
    U8 ** end_sig_buf_arr; // 5 different threads running 
    end_sig_buf_arr = mem_alloc(sizeof(U8*) * num_different_threads);
    
    gen_req_generic(test_index, 1); 

    
    curr_tid = tsk_gettid();
    
    U8  *p_index = &(g_ae_student_xtest.index);

    SNDR_HDR* sndr_hdr = mem_alloc(sizeof(SNDR_HDR));
    sndr_hdr->max_len = 14; // indicates size of mailbox
    sndr_hdr->recv_tid = curr_tid;

    /* NOTE: 
    in pair A-> recieving mbx task id
    in pair B-> singal_ready  */ 

    PAIR * pair = mem_alloc(sizeof(PAIR));
    pair->B = FALSE;
    
    TIMEVAL ** tv_arr = mem_alloc(sizeof(TIMEVAL*) * 4);


    tv_arr[0] = mem_alloc(sizeof(TIMEVAL));
    tv_arr[1] = mem_alloc(sizeof(TIMEVAL));
    tv_arr[2] = mem_alloc(sizeof(TIMEVAL));
    tv_arr[3] = mem_alloc(sizeof(TIMEVAL));

    pair->C = (U32) tv_arr;
    
    sndr_hdr->data = (U32) pair;


    g_student_tsk_cases[test_index].data = (U32) sndr_hdr;

    // create mbx for sending tasks 
    // 4 threads for each of the four diff period threads
    mbx_create(GET_BUFSIZE(G_MSG_LEN) * num_different_threads);
    

    tsk_create(&mbx_recv_tid, &recieving_mbx, LOW, 0x1000);
    
    tsk_yield(); // initalize recieving_mbx

    printf("REMAINING space left in mbx: %d\r\n", mbx_get(pair->A));
    

    tsk_create(&tid2, &mk_task_tq_3, HIGH, 0x200);
    tsk_create(&tid3, &mk_task_tq_5, HIGH, 0x200);
    tsk_create(&tid4, &mk_task_tq_8, HIGH, 0x200);


    for (int i =0; i<num_different_threads; i++){
        end_sig_buf_arr[i] = mem_alloc(GET_BUFSIZE(G_MSG_LEN));
    }

    printf("WAITING FOR RECV MSGS\r\n");

    // wait for all num_different_threads period RT tsks to terminate
    for (int i =0; i<num_different_threads; i++){
        printf("BLOCKING RECIEVE\r\n");
        recv_msg(end_sig_buf_arr[i], GET_BUFSIZE(G_MSG_LEN));
    }

    printf("ALL MESSGES RECIEVED \r\n");
    // signal revcieving_mbx to resume and assess order
    pair->B = TRUE;


    // assess order of execution 
    tsk_yield(); // resume recieving_mbx 

    
    // deallocation
    for (int i =0; i<num_different_threads; i++){
        mem_dealloc(end_sig_buf_arr[i]);
    }
    
    mem_dealloc(tv_arr[0]);
    mem_dealloc(tv_arr[1]);
    mem_dealloc(tv_arr[2]);
    mem_dealloc(tv_arr[3]);
    mem_dealloc(tv_arr);
    mem_dealloc(pair);
    mem_dealloc(end_sig_buf_arr);
    mem_dealloc((void*) sndr_hdr->data);
    mem_dealloc(sndr_hdr);
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
