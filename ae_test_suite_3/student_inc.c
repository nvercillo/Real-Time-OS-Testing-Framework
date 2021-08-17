#include "student_inc.h"

char  g_msg_generic [] = "Generic message!!!!";
char  g_msg1 [] = "This is Message 1!!";
char  g_msg2 [] = "This is Message 2!!";
char  g_msg3 [] = "This is Message 3!!";
char  g_msg4 [] = "This is Message 4!!";
char  g_msg5 [] = "This is Message 5!!";
char  g_msg6 [] = "This is Message 6!!";
char  g_msg7 [] = "This is Message 7!!";

const char   PREFIX[]      = "G31-TS2";
const char   PREFIX_LOG[]  = "G31-TS2-LOG";
const char   PREFIX_LOG2[] = "_log_testing_task_is_runningG31-TS2-LOG2";

AE_STUDENT_CASE     g_ae_student_cases[MAX_NUM_TESTS];
task_t      g_student_priviledged_task;
TASK_INIT   g_student_init_tasks[NUM_INIT_TASKS];
AE_STUDENT_CASE_TSK g_student_tsk_cases[MAX_NUM_TESTS];
AE_STUDENT_XTEST    g_ae_student_xtest;

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_student_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

void _log_testing_task_is_running(int task_num, task_t tid, int case_num)
{
    printf("Running task: %d, task_id: %d, case: %d\r\n", task_num, tid, case_num);
}
unsigned int count_set_bits(int n)
{
    unsigned int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
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

RTX_MSG_HDR * create_message(task_t sender_tid, U8 * msg, U32 len_msg)
{
//     if (BUF_LEN < sizeof(RTX_MSG_HDR) + len_msg){
// #ifdef DEBUG_0
//     printf("Requested message size is larger then BUF_LEN\r\n");
// #endif /* DEBUG_0 */
//         return NULL;
//     }
    
    U8 * buf1 = mem_alloc(sizeof(RTX_MSG_HDR) + len_msg);
    mem_util_zero(buf1, sizeof(RTX_MSG_HDR) + len_msg);

    // create message buffer
    RTX_MSG_HDR * p_msg_hdr = (RTX_MSG_HDR*) buf1;
    p_msg_hdr->length = sizeof(RTX_MSG_HDR) + len_msg;
    p_msg_hdr->type = MY_MSG_TYPE;
    p_msg_hdr->sender_tid = sender_tid;

    buf1 += sizeof(RTX_MSG_HDR);

    mem_util_cpy(buf1, msg, len_msg); // copy the msg to the buffer
    
    return p_msg_hdr;
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
    printf("\r\nPASSED TEST %d.%d\r\n\r\n", (int)test_id, (int) index);
}

int MARK_TEST_FAILED(int test_id, int index){
    printf("\r\nFAILED TEST %d.%d\r\n\r\n", (int) test_id, (int) index);
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

void print_results(AE_STUDENT_CASE *p_ae_cases)
{
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


void print_tasks(task_t * arr, int len)
{
    for (int i =0; i<len; i++ ){
        printf("%d->", arr[i]);
    }
    printf("\r\n");
}

void print_arr(U32 * arr, int len)
{
    for (int i =0; i<len; i++ ){
        printf("%d->", arr[i]);
    }
    printf("\r\n");
}