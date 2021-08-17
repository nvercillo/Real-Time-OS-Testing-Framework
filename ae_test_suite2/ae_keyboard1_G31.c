/**************************************************************************//**
 * @file        ae_keyboard1_G31.c
 * @brief       Lab3 testing suite 3 
 *
 * @version     V1.2021.06
 * @authors     Sathira Katugaha
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
    
#define     NUM_TESTS       1

void test0_case0_task0(void);
void test0_case1_task0(void);
void test0_case1_task1(void);
void test0_case2_task0(void);
void test0_case3_task0(void);
void test0_case3_task1(void);
void test0_case4_task0(void);
void test0_case4_task1(void);
void test0_case4_task2(void);
void test1_case0_task0(void);
void test1_case1_task0(void);
void test1_case1_task1(void);
void test1_case1_task2(void);
void test1_case1_task3(void);
void test2_case0_task0(void);
void test2_case1_task0(void);

void start_test0(void);
void start_test1(void);
void start_test2(void);

const char   PREFIX[]      = "G31-TS3";
const char   PREFIX_LOG[]  = "G31-TS3-LOG";
const char   PREFIX_LOG2[] = "G31-TS3-LOG2";

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
        g_student_tsk_cases[i].pos = 0;
    }
    printf("\r\n\r\n****************************************************************\r\n");
    printf("\r\n%s: START KEYBOARD TESTS\r\n", PREFIX);
    printf("%s: PLEASE FOLLOW THE INSTRUCTIONS AS THEY APPEAR ON THE SCREEN\r\n", PREFIX);
    printf("\r\n***************************************************************\r\n\r\n");
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

void test0_case0_task0(void)
{
    task_t curr_tid;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%LT into UART1, expect one process running\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    while ( 1 );

    tsk_exit();
}

void test0_case1_task0(void)
{
    task_t tid1, tid2, tid3;

    tsk_create(&tid1, &test0_case1_task1, LOW, 0x200);
    tsk_create(&tid2, &test0_case1_task1, LOW, 0x200);
    tsk_create(&tid3, &test0_case1_task1, LOW, 0x200);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%LT into UART1, expect multiple processes running\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    while ( 1 );

    tsk_exit();
}

void test0_case1_task1(void)
{
    tsk_yield();
    while ( 1 );
    tsk_exit();
}


void test0_case2_task0(void)
{
    mbx_create(BUF_LEN);

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%LM into UART1, expect one process running with a mailbox\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    while ( 1 );

    tsk_exit();
}

void test0_case3_task0(void)
{
    task_t curr_tid, tid1, tid2, tid3;

    curr_tid = tsk_gettid();
    
    mbx_create(BUF_LEN);
    tsk_create(&tid1, &test0_case3_task1, MEDIUM, 0x200);
    tsk_create(&tid2, &test0_case3_task1, MEDIUM, 0x200);
    tsk_create(&tid3, &test0_case3_task1, MEDIUM, 0x200);
    tsk_yield();
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%LM into UART1, expect 4 processes running each having a mailbox\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    while ( 1 );

    tsk_exit();
}

void test0_case3_task1(void)
{
    mbx_create(BUF_LEN);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntest0_case3_task1 has run\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    tsk_yield();
    while ( 1 );
    tsk_exit();
}

void test0_case4_task0(void)
{
    task_t tid1, tid2, tid3;

    tsk_create(&tid1, &test0_case4_task1, MEDIUM, 0x200);
    tsk_create(&tid2, &test0_case4_task2, MEDIUM, 0x200);
    tsk_create(&tid3, &test0_case4_task1, MEDIUM, 0x200);
    tsk_yield();
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%LM into UART1, expect 4 processes running with every other having a mailbox\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    while ( 1 );

    tsk_exit();
}

void test0_case4_task1(void)
{
    mbx_create(BUF_LEN);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntest0_case4_task1 has run\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    tsk_yield();
    while ( 1 );
    tsk_exit();
}

void test0_case4_task2(void)
{
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntest0_case4_task2 has run\r\n");
    printf("\r\n******************************************************\r\n\r\n");
    tsk_yield();
    while ( 1 );
    tsk_exit();
}

void test1_case0_task0(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();

    mbx_create(BUF_LEN);

    U8  identifier [] = "a";

    RTX_MSG_HDR * msg = create_register_interrupt_message(curr_tid, identifier);
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%%s into the console followed by a message\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nyour message was: %s\r\n", msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

void test1_case1_task0(void)
{
    task_t curr_tid, tid1, tid2, tid3;

    curr_tid = tsk_gettid();

    tsk_create(&tid1, &test1_case1_task1, MEDIUM, 0x200);
    tsk_create(&tid2, &test1_case1_task2, MEDIUM, 0x200);
    tsk_create(&tid3, &test1_case1_task3, MEDIUM, 0x200);

    tsk_yield();
    tsk_yield();
    tsk_yield();
    tsk_exit();
}

void test1_case1_task1(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();

    mbx_create(BUF_LEN);

    U8  identifier [] = "b";

    RTX_MSG_HDR * msg = create_register_interrupt_message(curr_tid, identifier);
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%%s into the console followed by a message\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nexpected identifier was: %%%s", identifier);
    printf("\r\nmessage was: %s\r\n", msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

void test1_case1_task2(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();

    mbx_create(BUF_LEN);

    U8  identifier [] = "c";

    RTX_MSG_HDR * msg = create_register_interrupt_message(curr_tid, identifier);
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%%s into the console followed by a message\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nexpected identifier was: %%%s", identifier);
    printf("\r\nmessage was: %s\r\n", msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

void test1_case1_task3(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();

    mbx_create(BUF_LEN);

    U8  identifier [] = "d";

    RTX_MSG_HDR * msg = create_register_interrupt_message(curr_tid, identifier);
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\ntype %%%s into the console followed by a message\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nexpected identifier was: %%%s", identifier);
    printf("\r\nmessage was: %s\r\n", msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

/* TEST 0 */ 
void test2_case0_task0(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();

    mbx_create(BUF_LEN);

    U8  identifier [] = "L";

    RTX_MSG_HDR * msg = create_register_interrupt_message(curr_tid, identifier);
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nAttempted to register '%%%s' identifier\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nRecieved the following message for '%%%s' identifier: %s\r\n", identifier, msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

void test2_case1_task0(void)
{
    task_t curr_tid;
    int res1, res2;

    curr_tid = tsk_gettid();
    mbx_create(BUF_LEN);

    U8  identifier [] = "TOO_LONG_IDENTIFIER";
    size_t identifier_len = 19;
    size_t msg_size = sizeof(RTX_MSG_HDR) + identifier_len;
    U8 * buf1 = mem_alloc(msg_size);
    mem_util_zero(buf1, msg_size);

    RTX_MSG_HDR * p_msg_hdr = (RTX_MSG_HDR*) buf1;
    p_msg_hdr->length = msg_size;
    p_msg_hdr->type = KCD_REG;
    p_msg_hdr->sender_tid = curr_tid;

    buf1 += sizeof(RTX_MSG_HDR);

    mem_util_cpy(buf1, identifier, identifier_len);

    RTX_MSG_HDR * msg = p_msg_hdr;
    res1 = send_msg(TID_KCD, msg);
    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nAttempted to register '%s' identifier\r\n", identifier);
    printf("\r\n******************************************************\r\n\r\n");

    U8* recv_buf = mem_alloc(BUF_LEN);
    res2 = recv_msg(recv_buf, BUF_LEN);
    
    RTX_MSG_HDR* recv_msg_hdr = (RTX_MSG_HDR *) recv_buf;
    
    size_t rtx_hdr_size = sizeof(RTX_MSG_HDR);
    size_t header_len = ((size_t) recv_msg_hdr->length);
    size_t msg_len = header_len - rtx_hdr_size + 1;
    
    U8* msg_str = mem_alloc(msg_len);
    mem_util_zero(msg_str, msg_len);
    recv_buf += rtx_hdr_size;
    mem_util_cpy(msg_str, recv_buf, msg_len - 1);
    recv_buf -= rtx_hdr_size;

    printf("\r\n\r\n******************************************************\r\n");
    printf("\r\nRecieved the following message for '%s' identifier: %s\r\n", identifier, msg_str);
    printf("\r\n******************************************************\r\n\r\n");

    mem_dealloc(msg_str);
    mem_dealloc(recv_buf);
    mem_dealloc(msg);
    tsk_yield();
    tsk_exit();
}

void priv_task1(void)
{   
    start_test0();
    MARK_TEST_PASSED(0, 0);
    test_exit();
}

void start_test0(void)
{
    task_t tid1;

    tsk_create(&tid1, &test0_case0_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test0_case1_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test0_case2_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test0_case3_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test0_case4_task0, MEDIUM, 0x200);
}

void start_test1(void)
{
    task_t tid1;

    tsk_create(&tid1, &test1_case0_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test1_case1_task0, MEDIUM, 0x200);
}

void start_test2(void)
{
    task_t tid1;

    tsk_create(&tid1, &test2_case0_task0, MEDIUM, 0x200);
    //tsk_create(&tid1, &test2_case1_task0, MEDIUM, 0x200);
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
