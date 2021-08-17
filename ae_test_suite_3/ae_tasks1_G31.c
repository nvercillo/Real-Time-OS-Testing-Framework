/**************************************************************************//**
 * @file        ae_task1_G31.c
 * @brief       Lab3 testing suite 1 
 *
 * @version     V1.2021.06
 * @authors     Stefan Vercillo
 * @date        2021 Jun
 * *
 *****************************************************************************/
#include "suite1_tasks.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */	
    
#define     NUM_TESTS               4       // number of tests
#define     NUM_DEFAULT_MBX_TSKS    2   // number of default tsks w mbx
#define     num_blocked_tasks   5 
/*
 *===========================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================
 */

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
    start_test2();
    start_test3();

    test_exit();
}


/*
 *===========================================================================
 *                             TEST DEFINITIONS 
 *===========================================================================
 */
/**
 * @brief Test the full functionality of mbx_get
 * 
 *  Test successful case of mbx_get 
 *       Case 0: used for mailbox that is expected to have > 0 bytes of memory remaining in mbx  
 *       Case 1: used for mailbox that is expected to have exactly 0 bytes of memory remaining in mbx
 *   Test error cases of mbx_get. In all the below cases RTX_ERROR is returned and ERRNO is set  
 *       Case 2: tid assigned to mailbox with no mailbox created already
 *       Case 3: tid <  0 
 *       Case 4: tid > MAXTAKS 
 */
void start_test0(void)
{
    /* Test Setup*/
    int res;
    task_t current_task, tid1;
    int test_index = 0;
    gen_req_generic(test_index, 5); // 5 case per test suite

    U8  *p_index = &(g_ae_student_xtest.index);

    current_task = tsk_gettid();

    /* CASE 0 */
    res = mbx_get(current_task);
    if (res == GET_BUFSIZE(G_MSG_LEN)){
        MARK_TEST_PASSED(test_index, *p_index);
    } else{
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
	
    /* CASE 1 && 2 */
    tsk_create(&tid1, &test0_case1_task0, MEDIUM, 0x200);
    
    /* CASE 3 */
    // invalid tid 
    res = mbx_get(-123);
    if (res == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);  
    } 
    (*p_index)++;

    /* CASE 4 */
    // should fail as there are only MAX_TASKS (indexed from 0)
    res = mbx_get(MAX_TASKS);
    if (res == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);  
    }
    (*p_index)++;
}

/**
 * @brief Test the full functionality of mbx_ls
 *  Test successful case of mbx_ls 
 *      Case 0: run function when there are >= count tasks in system 
 *      Case 1: run function when there are < count tasks in system   
 *      Case 2: assure that mbx_rmv works properly
 *  Test error cases of mbx_ls. In all the below cases RTX_ERROR is returned and ERRNO is set  
 *      Case 3: buf is NULL  
 */
void start_test1()
{
    /* Test Setup*/
    int res;
    task_t tid1;
    int test_index = 1;
    task_t tasks [MAX_TASKS];

    gen_req_generic(test_index, 4);

    U8  *p_index = &(g_ae_student_xtest.index);

    /* CASE 0 && CASE 1 */ 
    tsk_create(&tid1, &test1_case0_task0, MEDIUM, 0x200);

    /* CASE 2 */ 
    tsk_create(&tid1, &test1_case2_task0, HIGH, 0x200);

    /* CASE 3 */
    res = mbx_ls(NULL, MAX_TASKS);
    if (res == RTX_ERR && errno == EFAULT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else{
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
}


/**
 * @brief Test the error behaviour of recv_msg_nb. 
 *      Case 0: read recieved msg successfully   
 *      Case 1: read msg sent to self non-blocking successfully 
 *  Test error cases of rcv_msg_nb. In all the below cases RTX_ERROR is returned and ERRNO is set  
 *      Case 2: buf arg is NULL 
 *      Case 3: buffer is too small to hold the message 
 *      Case 4: calling tsk has no mailbox initialized 
 *      Case 5: no msg when function is called
 */
void start_test2()
{
    /* Test Setup*/
    int res1;
    task_t tid1;
    int test_index = 2;

    gen_req_generic(test_index, 4);

    U8  *p_index = &(g_ae_student_xtest.index);

    /* CASE 0 */ 
    /* SEE TEST 3 CASE 0 */
    
    /* CASE 1*/ 
    /* SEE TEST 3 CASE 1 */

    /* CASE 2*/ 
    res1 = recv_msg_nb(NULL, BUF_LEN);

    if (res1 == RTX_ERR && errno == EFAULT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else{
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    /* CASE 3*/ 
    char * small_buf = mem_alloc(MIN_MSG_SIZE -1);
    res1 = recv_msg_nb(small_buf, MIN_MSG_SIZE - 1);

    if (res1 == RTX_ERR && errno == ENOSPC){
        MARK_TEST_PASSED(test_index, *p_index);
    } else{
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    /* CASE 4*/ 
    tsk_create(&tid1, &test2_case4_task0, MEDIUM, 0x200);

    /* CASE 5*/ 
    tsk_create(&tid1, &test2_case5_task0, MEDIUM, 0x200);

    mem_dealloc(small_buf);    
}

/**
 * @brief Test the full functionality of send_msg_nb. 
 *  Test successful case of send_msg_nb 
 *      Case 0: send to recv succesfully  
 *      Case 1: send to self non-blocking
 *  Test error cases of send_msg_nb. In all the below cases RTX_ERROR is returned and ERRNO is set
 *      Case 2: EFAULT The buf argument is a null pointer.
 *      Case 3: ENOENT The receiver identified by the receiver_tid exists but does not have a mailbox.
 *      Case 4: EINVAL The receiver identified by the receiver_tid does not exist. 
 *      Case 5: EMSGSIZE The length field in the buf exceeds the mailbox capacity. 
 *      Case 6: ENOSPC The receiver_tid’s mailbox does not have enough free space for the message  
 */
void start_test3()
{
    /* Test Setup*/
    int res;
    task_t curr_tid, tid1;
    int test_index = 3;
    gen_req_generic(test_index, 7);


    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);


    /* CASE 0 && 1*/
    tsk_create(&tid1, &test3_case0_task0, MEDIUM, 0x200);
    
    /* CASE 2*/ 
    // attempt to send with NULL buffer
    res = send_msg_nb(curr_tid, NULL);

    if (res == RTX_ERR && errno == EFAULT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    /* CASE 3 && 4 && 5 && 6*/ 
    tsk_create(&tid1, &test3_case3_task0, MEDIUM, 0x200);
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
