#ifndef STUDENT_INC_H
#define STUDENT_INC_H

#include "uart_polling.h"
#include "printf.h"
#include "rtx.h"
#include "rtx_errno.h"
#include "mem_util.h"
#include "common_ext.h"

#define G_MSG_LEN 20 // each msg needs to be this big 
#define     MAX_LEN_MSG         128     // test message in bytes including '\0'
#define     MAX_LEN_SEQ         32      // max test case execution sequence record length
#define     MAX_NUM_TESTS       10      // max number of tests
#define     NUM_INIT_TASKS      2       // number of tasks during initialization
#define     BUF_LEN             128         // receiver buffer length
#define     MY_MSG_TYPE         69          // some customized message type
#define     G_MSG_LEN           20 // each msg needs to be this big 
#define     GET_BUFSIZE(msg_size)  (sizeof(RTX_MSG_HDR) + msg_size)

typedef struct sndr_hdr
{
    task_t recv_tid;

    U32 max_len;  // max len of arr
    U32 len_arr;  // current len of arr (initalized to 0)
    U32 * prio_arr;
    U32 * blocked_arr;
    U32 data;
    task_t * sender_tid_arr;

    U32 *  curr_priority; // priority of curr task  
} SNDR_HDR;

typedef struct arr_n_cntr
{
    void * arr;
    int len;
    int max_len;
    U32 data;
} ARR_N_CNTR;

typedef struct task_storage
{
    void ** exp_buf_arr;
    U8 ** recv_bufs; // arr of U8 * 
    BOOL valid1;
} TASK_STORAGE;

typedef struct pair{
    U32 A;
    U32 B;
    U32 C;
} PAIR;


typedef struct ae_student_xtest
{
    U8    test_id;                  /**< test case ID */
    U8    index;                    /**< sub test index in a specific test case*/
    U8    num_tests;                /**< number of test cases/functions in a test suite*/
    U8    num_tests_run;            /**< number of completed test cases/functions */
    char  msg[MAX_LEN_MSG];         /**< testing message buffer */
} AE_STUDENT_XTEST;

/**
 * @brief   generic test case object
 */

typedef struct ae_student_case 
{
    U32         results;            /**< test results, each test has sub test for each bit */
    U8          test_id;            /**< test ID **/
    U8          num_bits;           /**< results are saved in bits[0:num-1] */
} AE_STUDENT_CASE;  

/**
 * @briekf  task management test case object
 */

typedef struct ae_student_case_tsk
{
    AE_STUDENT_CASE     *p_ae_case;                 /**< points to generic case object */    
    U8          len;                        /**< length of seq_exec array */
    U8          pos;                        /**< next free spot position in the seq array*/
    U8          pos_expt;                   /**< expected next free spot position in seq_expt when test finishes */
    task_t      seq[MAX_LEN_SEQ];           /**< actual task execution order   */
    task_t      seq_expt[MAX_LEN_SEQ];      /**< expected task exectuion order */
    task_t      task_ids[MAX_LEN_SEQ];      /**< unique task ids in order of arrival  */
    U8          num_tasks;                  /**< total number of test called by this point of test execution  */
    U32         data;                        /**< miscellaneous data used by test functions */
} AE_STUDENT_CASE_TSK;


/*
 *===========================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================
 */

extern char  g_msg_generic [];
extern char  g_msg1 [];
extern char  g_msg2 [];
extern char  g_msg3 [];
extern char  g_msg4 [];
extern char  g_msg5 [];
extern char  g_msg6 [];
extern char  g_msg7 [];

extern const char   PREFIX[];
extern const char   PREFIX_LOG[];
extern const char   PREFIX_LOG2[];

extern AE_STUDENT_CASE     g_ae_student_cases[MAX_NUM_TESTS];
extern task_t      g_student_priviledged_task;
extern TASK_INIT   g_student_init_tasks[NUM_INIT_TASKS];
extern AE_STUDENT_CASE_TSK g_student_tsk_cases[MAX_NUM_TESTS];
extern AE_STUDENT_XTEST    g_ae_student_xtest;


char * get_g_msg_from_index(int i);
void ** generate_five_expected_rcv();

void priv_task1(void);

void set_ae_tasks(TASK_INIT *tasks, int num);
void silent_task(void);
void reciever_set_to_MEDIUM_task(void);
int process_sub_result(int test_id, int index, int result);
int MARK_TEST_PASSED(int test_id, int index);
int MARK_TEST_FAILED(int test_id, int index);
int print_log(int test_id, int index, int result, char *msg);
void test_exit(void);
void print_summary(void);
void ae_exit(void);
void init_ae_tsk_test(void);
void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num);
void update_ae_xtest(int test_id);
void gen_req_generic(int test_id, int num_cases);
void _log_testing_task_is_running(int task_num, task_t tid, int case_num);
void print_results(AE_STUDENT_CASE *p_ae_castest_exites);
void print_tasks(task_t * arr, int len);
unsigned int count_set_bits(int n);
RTX_MSG_HDR * create_message(task_t sender_tid, U8 * msg, U32 len_msg);

void start_test0(void);
void start_test1(void);
void start_test2(void);
void start_test3(void);



#endif