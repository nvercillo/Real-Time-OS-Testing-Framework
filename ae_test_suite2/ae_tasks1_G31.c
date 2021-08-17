/**************************************************************************//**
 * @file        ae_task1_G31.c
 * @brief       Lab3 testing suite 1 
 *
 * @version     V1.2021.06
 * @authors     Stefan Vercillo
 * @date        2021 Jun
 * *
 *****************************************************************************/
#define IS_STUDENT_TEST

#include "ae_student_util.h"
#include "uart_polling.h"
#include "printf.h"
#include "rtx.h"
#include "rtx_errno.h"
#include "ae_tasks_util.h"
#include "mem_util.h"
#include "ae_util.h"
/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
    
#define     NUM_TESTS               4       // number of tests
#define     NUM_DEFAULT_MBX_TSKS    2   // number of default tsks w mbx
/*
 *===========================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================
 */

char  g_msg1 [] = "This is Message 1!\n";
char  g_msg2 [] = "This is Message 2!\n";
char  g_msg3 [] = "This is Message 3!\n";
char  g_msg4 [] = "This is Message 4!\n";
char  g_msg5 [] = "This is Message 5!\n";
char  g_msg6 [] = "This is Message 6!\n";
char  g_msg7 [] = "This is Message 7!\n";

void test0_case1_task0(void);
void test0_case4_task0(void);
void test1_case0_task0(void);
void test1_case2_task0(void);
void test1_case2_task1(void);
void test2_case4_task0(void);
void test2_case5_task0(void);
void test2_case5_task2(void);
void test3_case0_task0(void);
void test3_case0_task1(void);
void test3_case3_task0(void);
void priv_task1(void);

void start_test0(void);
void start_test1(void);
void start_test2(void);
void start_test3(void);

const char   PREFIX[]      = "G31-TS1";
const char   PREFIX_LOG[]  = "G31-TS1-LOG";
const char   PREFIX_LOG2[] = "G31-TS1-LOG2";

AE_STUDENT_CASE     g_ae_student_cases[MAX_NUM_TESTS];
AE_STUDENT_CASE_TSK g_student_tsk_cases[MAX_NUM_TESTS];
AE_STUDENT_XTEST    g_ae_student_xtest;

task_t      g_student_priviledged_task;
TASK_INIT   g_student_init_tasks[NUM_INIT_TASKS];

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

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_student_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

void update_ae_xtest(int test_id)
{
    g_ae_student_xtest.test_id = test_id;
    g_ae_student_xtest.index = 0;
    g_ae_student_xtest.num_tests_run++;
}

void gen_req_generic(int test_id, int num_cases)
{
    g_student_tsk_cases[test_id].p_ae_case->num_bits = num_cases;
    g_student_tsk_cases[test_id].p_ae_case->results = 0;
    g_student_tsk_cases[test_id].p_ae_case->test_id = test_id;
    update_ae_xtest(test_id);
}

/* TEST 0 */ 
void test0_case1_task0(void)
{
    int res1, res2, res3;
    int task_id = 0;
    int test_index = 0; 
    int case_num = 1; 
		task_t tasks [MAX_TASKS];

	int res22;
    res22= mbx_ls(tasks, MAX_TASKS);

    task_t curr_tid, rcv_tid;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
		
    // Case 1: tid assigned to mailbox with no mailbox created already
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    res3 = mbx_get(curr_tid);
    if (res3 == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num ++;

    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    res1 = mbx_create(MIN_MSG_SIZE); // create smallest mbx possible
    res2 = mbx_get(curr_tid);
    RTX_MSG_HDR * buffer = create_message(curr_tid, NULL, 0); // generate message 
    res1 = send_msg_nb(curr_tid, buffer); // send generic message 

    res2 = mbx_get(curr_tid);

    if (res1 == RTX_OK && res2 == 0){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    
    mem_dealloc(buffer);
    tsk_yield(); // yeild so rcving thread can terminates
    res2 = mbx_ls(tasks, MAX_TASKS);

    tsk_exit();
}

/* TEST 1 */ 
void test1_case0_task0(void)
{
    int res1, res2, res3;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 0; 
    int NUM_EXPECTED_TSKS = 3 +1 + NUM_DEFAULT_MBX_TSKS; //3 recvs, priv1, kcd and cdsip
    task_t tasks [MAX_TASKS];
    task_t tid, tid1, tid2, tid3;
    tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    
    tsk_create(&tid1, &reciever_set_to_MEDIUM_task, HIGH, 0x200);
    tsk_create(&tid2, &reciever_set_to_MEDIUM_task, HIGH, 0x200);
    tsk_create(&tid3, &reciever_set_to_MEDIUM_task, HIGH, 0x200);
    
    res1 = mbx_ls(tasks, 4);
    res3 = mbx_ls(tasks, MAX_TASKS);
    res2 = mbx_ls(tasks, NUM_EXPECTED_TSKS + 1); // one over currently present

    if (res1 == 4){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    if (res2 == NUM_EXPECTED_TSKS){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    // terminate the created tsks
    tsk_yield();
    tsk_yield();
    tsk_yield();
    
    tsk_exit();
}

void test1_case2_task0(void) 
{
    int res1, res2, res3;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 2; 
    task_t tasks [MAX_TASKS];

    task_t curr_tid, rcv_tid;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int) test_index, (int) case_num, (int) task_id);

    tsk_create(&rcv_tid, &test1_case2_task1, HIGH, 0x200);
    tsk_yield();

    // check num tasks before mbx_rmv
    res1 = mbx_ls(tasks, MAX_TASKS);
    tsk_set_prio(rcv_tid, HIGH);
    tsk_yield();

    // check num tasks after mbx_rmv
    res2 = mbx_ls(tasks, MAX_TASKS);
    tsk_set_prio(rcv_tid, HIGH); // terminate recv
    tsk_yield();

    if (res1 - res2 == 1) {
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }       
    (*p_index) ++; 
    tsk_exit();

}

void test1_case2_task1(void)
{   
    int test_index = 1;
    int task_id = 1;
    int case_num = 0; 
    task_t current_tid;
    
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int) test_index, (int) case_num, (int) task_id);

    current_tid = tsk_gettid();

    mbx_create(BUF_LEN);
    tsk_set_prio(current_tid, MEDIUM);    
    mbx_rmv();

    tsk_set_prio(current_tid, MEDIUM);    

    tsk_exit();
}


/* TEST 2 */ 
void test2_case4_task0(void)
{
    // Case 4: calling tsk has no mailbox initialized 
    int res1;
    int task_id = 0;
    int test_index = 2; 
    int case_num = 1; 

    task_t curr_tid;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);

    
    U8 * buf = mem_alloc(BUF_LEN);
    res1 = recv_msg_nb(buf, BUF_LEN);

    if (res1 == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    mem_dealloc(buf);
    tsk_exit();
}

void test2_case5_task0(void)
{
    //  Case 5: no msg when function is called
    int res1, res2;
    int task_id = 0;
    int test_index = 2; 
    int case_num = 1; 

    task_t curr_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    
    res1 = mbx_create(BUF_LEN);
    
    U8 * buf = mem_alloc(BUF_LEN);
    res2 = recv_msg_nb(buf, BUF_LEN);

    if (res1 == RTX_OK && res2 == RTX_ERR && errno == ENOMSG){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    mem_dealloc(buf);
    tsk_exit();
}

/* TEST 3 */ 
void test3_case0_task0(void)
{
//  *      Case 0: send to recv succesfully  
//  *      Case 1: send to self non-blocking
    int res1, res2;
    int task_id = 0;
    int test_index = 3; 
    int case_num = 0;

    U8 * recv_buf; 
    void * sent_buf1, *sent_buf2;
    task_t curr_tid, sndr_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    
    res1 = mbx_create(GET_BUFSIZE(G_MSG_LEN));

    // Case 0
    g_student_tsk_cases[test_index].data = curr_tid;
    tsk_create(&sndr_tid, &test3_case0_task1, HIGH, 0x200);
    recv_buf = mem_alloc(GET_BUFSIZE(G_MSG_LEN));
    mem_util_zero(recv_buf, GET_BUFSIZE(G_MSG_LEN));

    res2 = recv_msg_nb(recv_buf, GET_BUFSIZE(G_MSG_LEN));

    sent_buf1 = ((void*) g_student_tsk_cases[test_index].data);

    if (
        res1 == RTX_OK 
        && res2 == RTX_OK
        && mem_util_cmp(sent_buf1, recv_buf, GET_BUFSIZE(G_MSG_LEN)) == 0
    ){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    mem_dealloc(sent_buf1); // deallocate the message used in this test case 
    mem_util_zero(recv_buf, GET_BUFSIZE(G_MSG_LEN)); // clear buffer for nxt test

    // Case 1
    sent_buf2 = (void *) create_message(curr_tid, g_msg1, G_MSG_LEN); // generate message 
    res1 = send_msg_nb(curr_tid, sent_buf2); // send msg sucessfully

    res2 = recv_msg_nb(recv_buf, GET_BUFSIZE(G_MSG_LEN));

    if (
        res1 == RTX_OK 
        && res2 == RTX_OK
        && mem_util_cmp(sent_buf2, recv_buf, GET_BUFSIZE(G_MSG_LEN)) ==0 
    ){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    
    mem_dealloc(sent_buf2); // dealloc buffer sent into mbx
    mem_dealloc(recv_buf);
    tsk_exit();
}

void test3_case0_task1(void)
{
    int task_id = 0;
    int test_index = 3; 
    int case_num = 0; 
    RTX_MSG_HDR * buffer1;

    task_t curr_tid, rcv_tid;
    curr_tid = tsk_gettid();
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);

    rcv_tid = g_student_tsk_cases[test_index].data;
    
    buffer1 = create_message(curr_tid, g_msg1, G_MSG_LEN); // generate message 
    send_msg_nb(rcv_tid, buffer1); // send msg sucessfully

    // cast p_RTX to U32 so we can pass it as data
    g_student_tsk_cases[test_index].data = (U32) buffer1; 
    
    tsk_set_prio(curr_tid, MEDIUM);    

    tsk_exit();
}

void test3_case3_task0(void)
{
//  *      Case 3: ENOENT The receiver identified by the receiver_tid exists but does not have a mailbox.
//  *      Case 4: EINVAL The receiver identified by the receiver_tid does not exist. 

//  *      Case 5: EINVAL The receiver identified by the receiver_tid does not exist. 

//  *      Case 5: EMSGSIZE The length field in the buf exceeds the mailbox capacity. 
//  *      Case 6: ENOSPC The receiver_tid’s mailbox does not have enough free space for the message  
    
    int res1, res2, res3, res4;
    int task_id = 0;
    int test_index = 3; 
    int case_num = 3; 

    task_t curr_tid, rcv_tid1, rcv_tid2;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    U8  msg [] = "Congradulations YOu are pregnannt\n"; 
    RTX_MSG_HDR * buffer1 = create_message(curr_tid, msg, BUF_LEN); // generate message 

    // Case 3
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    res1 = send_msg_nb(curr_tid, buffer1); // attempt to send msg to self w/o mbx delcared

    if (res1 == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++;
    
    mbx_create(GET_BUFSIZE(G_MSG_LEN) +  G_MSG_LEN);
    // Case 4
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    
    res2 = send_msg_nb(MAX_TASKS+1, buffer1); // attempt to send msg to non-existent tid

    if (res2 == RTX_ERR && errno == EINVAL){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++;
    
    
    // Case 5 
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    RTX_MSG_HDR * buffer2 = create_message(curr_tid, g_msg1, G_MSG_LEN *2 + 1); 
    res2 = send_msg_nb(curr_tid, buffer2); // send msg 1 byte longer than mbx

    if (res2 == RTX_ERR && errno == EMSGSIZE){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++; 
    mbx_rmv();

    
    // Case 6
    // Note that there is BUF_LEN sized reciever
    mbx_create(GET_BUFSIZE(G_MSG_LEN/2) *2 );
    
    printf("RUNNING TEST %d, CASE %d, TASK %d\n", (int)test_index, (int) case_num, (int) task_id);
    
    char half_msg_p1 [] = "HALFMSG+1\n";
    char half_msg_m1 [] = "MSG/2-1\n";

    RTX_MSG_HDR * buffer3 = create_message(curr_tid, g_msg1, sizeof(g_msg1)/2); // generate message 
    
    res1 = send_msg_nb(rcv_tid2, buffer3);

    RTX_MSG_HDR * buffer4 = create_message(curr_tid, half_msg_p1, sizeof(half_msg_p1)); // generate message
    RTX_MSG_HDR * buffer5 = create_message(curr_tid, half_msg_m1, sizeof(half_msg_m1)); // generate message
    RTX_MSG_HDR * buffer6 = create_message(curr_tid, "aa", 2); // generate message
    
    // attempt to send msg after msgbx is already full 
    res2 = send_msg_nb(curr_tid, buffer4);
    
    int errno2 = errno;

    res3 = send_msg_nb(curr_tid, buffer5);
    res4 = send_msg_nb(curr_tid, buffer6);

    if (
        res1 == RTX_OK 
        && res2 == RTX_ERR 
        && errno2 == ENOSPC
        && res3 == RTX_OK
        && res4 == RTX_ERR
        && errno == ENOSPC
    ){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++;
    
    // kill rcving task
    tsk_yield();

    mem_dealloc(buffer1);
    mem_dealloc(buffer2);
    mem_dealloc(buffer3);
    mem_dealloc(buffer4);
    mem_dealloc(buffer5);
    mem_dealloc(buffer6);
    tsk_exit();
}


/* first created tsk  */
void priv_task1(void)
{
    
    g_student_priviledged_task = tsk_gettid();
    
    // create a mailbox for current task
    mbx_create(BUF_LEN);
    
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
    if (res == BUF_LEN){
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
