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
#ifndef AE_STUDENT_UTIL_H_
#define AE_STUDENT_UTIL_H_

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
    
#define     MAX_NUM_TESTS       10      // max number of tests
#define     NUM_INIT_TASKS      2       // number of tasks during initialization
#define     BUF_LEN             128         // receiver buffer length
#define     MY_MSG_TYPE         69          // some customized message type
#define     G_MSG_LEN           20 // each msg needs to be this big 
#define     GET_BUFSIZE(msg_size)  (sizeof(RTX_MSG_HDR) + msg_size)


/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */

#include "ae.h"
#include "ae_inc.h"
/*
 *===========================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================
 */

// // The following globals are defined in the Test Suite .c file
// extern const char   PREFIX[];       // test output file prefix
// extern const char   PREFIX_LOG[];   // test output file log prefix
// extern AE_XTEST     g_ae_xtest;     // test data, re-use by each test case
// extern AE_CASE      g_ae_cases[];   // points to the array of testing cases
// extern AE_CASE_TSK  g_tsk_cases[];  // array of testing cases

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

extern AE_STUDENT_CASE     g_ae_student_cases[];
extern AE_STUDENT_CASE_TSK g_student_tsk_cases[];
extern AE_STUDENT_XTEST    g_ae_student_xtest;

extern const char   PREFIX[];
extern const char   PREFIX_LOG[];

int  MARK_TEST_PASSED(int test_id, int index);
int  MARK_TEST_FAILED(int test_id, int index);

// setup all tests 
void init_ae_tsk_test(void); // fn def needed for set_ae_tasks

// update the ae_test struct to current test
void update_ae_xtest(int test_id);

// create allocated message and header
RTX_MSG_HDR * create_message(task_t sender_tid, U8 * msg, U32 len_msg);

// create allocated message and header for registering keyboard interrupt
RTX_MSG_HDR * create_register_interrupt_message(task_t sender_tid, U8 * identifier);

// genereate req struct used by tests
void gen_req_generic(int test_id, int num_cases);

// set init for inital NUM_INIT_TASKS tasks
void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num);

// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num);


/* 
 *      GENERIC TASKS  
 */
// empty function
void silent_task(void);

// create mailbox and 
void initial_task(void);

// Generic reciever thread
void reciever_set_to_MEDIUM_task(void);

// task which runs all tests
void priv_task1(void); // fn def needed for set_ae_tasks

// update ae xtest 
void update_ae_xtest(int test_id);

void _log_testing_task_is_running(int task_num, task_t tid, int case_num);

int  print_log(int test_id, int index, int result, char *msg); 

void test_exit(void);

void print_summary(void);

void print_results(AE_STUDENT_CASE *p_ae_cases);

void ae_exit(void);  
#endif // !AE_STUDENT_UTIL_H_
#endif //Student test check