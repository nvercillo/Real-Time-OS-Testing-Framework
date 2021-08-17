/**************************************************************************//**
 * @file        ae_student_util.c
 * @brief       Helper Functions for Test Suite 
 *
 * @version     V1
 * @authors     Stefan Vercillo 
 * @date        2021 MAY 
 *
 * @note        Implementation of commonly used utils functions
 *
 *****************************************************************************/

#ifdef IS_STUDENT_TEST

#include "ae_student_util.h"
#include "ae_util.h"
#include "uart_polling.h"
#include "printf.h"
#include "rtx_errno.h"
#include "ae_tasks_util.h"
#include "mem_util.h"

unsigned int count_set_bits(int n)
{
    unsigned int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

RTX_MSG_HDR * create_message(task_t sender_tid, U8 * msg, U32 len_msg)
{
//     if (BUF_LEN < sizeof(RTX_MSG_HDR) + len_msg){
// #ifdef DEBUG_0
//     printf("Requested message size is larger then BUF_LEN\n");
// #endif /* DEBUG_0 */
//         return NULL;
//     }
    
    U8 * buf1 = mem_alloc(sizeof(RTX_MSG_HDR) + len_msg);
    mem_util_zero(buf1, BUF_LEN);

    // create message buffer
    RTX_MSG_HDR * p_msg_hdr = (RTX_MSG_HDR*) buf1;
    p_msg_hdr->length = sizeof(RTX_MSG_HDR) + len_msg;
    p_msg_hdr->type = MY_MSG_TYPE;
    p_msg_hdr->sender_tid = sender_tid;

    buf1 += sizeof(RTX_MSG_HDR);

    mem_util_cpy(buf1, msg, len_msg); // copy the msg to the buffer
    
    return p_msg_hdr;
}

RTX_MSG_HDR * create_register_interrupt_message(task_t sender_tid, U8 * identifier)
{
    size_t msg_size = sizeof(RTX_MSG_HDR) + 1;
    U8 * buf1 = mem_alloc(msg_size);
    mem_util_zero(buf1, msg_size);

    // create message buffer
    RTX_MSG_HDR * p_msg_hdr = (RTX_MSG_HDR*) buf1;
    p_msg_hdr->length = msg_size;
    p_msg_hdr->type = KCD_REG;
    p_msg_hdr->sender_tid = sender_tid;

    buf1 += sizeof(RTX_MSG_HDR);

    mem_util_cpy(buf1, identifier, 1); // copy the msg to the buffer

    return p_msg_hdr;
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

/* 
 *      GENERIC TASKS  
*/
void silent_task(void)
{
    // pass
    tsk_exit();
}

// Generic reciever thread
void reciever_set_to_MEDIUM_task(void)
{
    int task_id = 1;
    int case_num = 0; 
    task_t current_tid;
    
    current_tid = tsk_gettid();
    _log_testing_task_is_running(task_id, current_tid, case_num); // log test information

    mbx_create(BUF_LEN);

    tsk_set_prio(current_tid, MEDIUM);    

    tsk_exit();
}

int process_sub_result(int test_id, int index, int result)
{
    if ( result == 1 ) {
        g_ae_student_cases[test_id].results |= BIT(index);
    }
    print_log(test_id, index, result, g_ae_student_xtest.msg);
    return 0;
}

int MARK_TEST_PASSED(int test_id, int index){
    process_sub_result(test_id, index, 1);
    printf("\nPASSED TEST %d.%d\n\n", (int)test_id, (int) index);
}

int MARK_TEST_FAILED(int test_id, int index){
    printf("\nFAILED TEST %d.%d\n\n", (int) test_id, (int) index);
}

void _log_testing_task_is_running(int task_num, task_t tid, int case_num)
{
    printf("Running task: %d, task_id: %d, case: %d\n", task_num, tid, case_num);
}

int print_log(int test_id, int index, int result, char *msg)
{
    printf("%s: Test[%d]-[%d]: %s.\r\n",  PREFIX_LOG, test_id, index, msg);
    printf("%s: Test[%d]-[%d]: result = %d.\r\n",  PREFIX_LOG, test_id, index, result);
    return 0;
}

void test_exit(void)
{
    printf("%s: FINISHED\r\n", PREFIX);
    print_summary();
}

void print_summary(void)
{
    print_results(g_ae_student_cases);
    printf("%s: END\r\n", PREFIX);
    ae_exit();
    
}

void print_results(AE_STUDENT_CASE *p_ae_cases){
    int total = 0;
    int total_pass  = 0;
    
    for ( int i = 0; i < g_ae_student_xtest.num_tests; i++) {
        int num_bits = p_ae_cases[i].num_bits;
        int pass = count_set_bits(p_ae_cases[i].results);
        total_pass += pass;
        total += num_bits;
        printf("%s: Test %d: %u/%u PASSED. Expected = 0x%X, result = 0x%X\r\n", \
            PREFIX_LOG, i, pass, num_bits, BIT(num_bits) - 1, p_ae_cases[i].results);
    }
    printf("%s: %u/%u test function(s) COMPLETED.\r\n",PREFIX, g_ae_student_xtest.num_tests_run, g_ae_student_xtest.num_tests);
    printf("%s: %d/%u tests PASSED.\r\n", PREFIX, total_pass, total);
    printf("%s: %d/%u tests FAILED.\r\n", PREFIX, total - total_pass, total); 
}

void ae_exit(void)
{
    while(1);
}

#endif