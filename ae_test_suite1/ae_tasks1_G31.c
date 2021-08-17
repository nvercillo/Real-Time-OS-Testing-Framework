/*
 *Tasks Test Suite 1:
 *Tests the error cases and preemption behaviour of tsk_get, tsk_set_prio and tsk_create functions
 */

/**************************************************************************//**
 * @file        ae_tasks1_g31.c
 * @brief       Lab2 public testing suite 1
 *
 * @version     V1.2021.06
 * @authors     Stefan Vercillo, Sathira Katugaha
 * @date        2021 Jun
 * *
 *****************************************************************************/

#include "uart_polling.h"
#include "printf.h"
#include "ae.h"
#include "ae_util.h"
#include "rtx_errno.h"
#include "ae_tasks_util.h"
#include "ae_tasks.h"
    
#define NUM_TESTS 10          // number of tests

const char   PREFIX[]      = "G31-TASKS-TS1";
const char   PREFIX_LOG[]  = "G31-TASKS-TS1-LOG";
const char   PREFIX_LOG2[] = "G31-TASKS-TS1-LOG2";

task_t priviledged_task;

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

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
    g_ae_xtest.test_id = 0;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests = NUM_TESTS;
    g_ae_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_tsk_cases[i].p_ae_case = &g_ae_cases[i];
        g_tsk_cases[i].p_ae_case->results  = 0x0;
        g_tsk_cases[i].p_ae_case->test_id  = i;
        g_tsk_cases[i].p_ae_case->num_bits = 0;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

/*---------------------------------------------------------------------------------------------------*/
/************ Methods to set the properties for test result records *****************/
void gen_req_generic(int test_id)
{
    g_tsk_cases[test_id].p_ae_case->num_bits = 1;
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

void gen_req_test2(int test_id)
{
    g_tsk_cases[test_id].p_ae_case->num_bits = 2;
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

void gen_req_test5(int test_id)
{
    g_tsk_cases[test_id].p_ae_case->num_bits = 3;
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

void gen_req_test6()
{
    int test_id = 6;
    g_tsk_cases[test_id].p_ae_case->num_bits = 1;
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 32; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 5;
    g_tsk_cases[test_id].pos = 0;

    update_ae_xtest(test_id);
}

void gen_req_test7()
{
    int test_id = 7;
    g_tsk_cases[test_id].p_ae_case->num_bits = 1;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 32; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 1;
    g_tsk_cases[test_id].pos = 0;
    g_tsk_cases[test_id].num_tasks =0;

    update_ae_xtest(test_id);
}

void gen_req_test8()
{
    int test_id = 8;
    g_tsk_cases[test_id].p_ae_case->num_bits = 1;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 32; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 3;
    g_tsk_cases[test_id].pos = 0;
    g_tsk_cases[test_id].num_tasks =0;

    update_ae_xtest(test_id);
}

void gen_req_test9()
{
    int test_id = 9;
    g_tsk_cases[test_id].p_ae_case->num_bits = 1;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 32; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 6;
    g_tsk_cases[test_id].pos = 0;
    g_tsk_cases[test_id].num_tasks =0;

    update_ae_xtest(test_id);
}

/*---------------------------------------------------------------------------------------------------*/

/*
 * TEST 0
 * --------------------
 * description: Tests error case of the tsk_set_prio function for invalid tasks.
 * Sets priority of a task that doesn’t exist to HIGH, which is a task simply
 * denoted by the literal character “r” and then expects correct error response.
 * The test will pass if the errno global variable is set to EINVAL and otherwise will fail. 
 */
void test0_start(){
    //set test properties and result handling variables
    int test_id = 0;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_generic(test_id);

    //set priority of non-existent task
    int res = tsk_set_prio('r', HIGH); // 'r' is a random unsigned char

    //check for correct error
    if (res == -1){
        if (errno == EINVAL)
            process_sub_result(test_id, *p_index, 1);
        else
            printf("%s Invalid errno: Got %d, expected %d\r\n", PREFIX, res, EFAULT);
    } else{
        printf("%s res %d\r\n ", PREFIX ,res);
    }
    
    *(p_index)++;
}

/*
 * TEST 1
 * --------------------
 * description: Checks error case of the tsk_set_prio function in which an unprivileged
 * task attempts to set the priority of a privileged task. Test creates an unprivileged
 * task which is programmed to change the priority of a privileged task. If the task is
 * unsuccessful the test will pass, otherwise the test will fail.
 */
void test1_start(){
    //set test properties and result handling variables
    int test_index =1;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_generic(test_index);

    //create task running test1_task1
    task_t tid1; 
    int ret_val = tsk_create(&tid1, &test1_task1, HIGH, 0x200);

    //test1_task1 is expected to change the priority of a privileged task
    //and record the result in the following array position
    int ret = g_tsk_cases[test_index].data;

    //test1_task1 should not be able to do this
    //check if correct error was set after task ran
    if (ret == -1){
        if (errno == ((int)EPERM))
            MARK_TEST_PASSED(test_index, *p_index);
        else
            printf("%s Invalid errno: Got %d, expected %d\r\n", PREFIX, errno, EPERM);
    } else{
        printf("res %d\r\n", ret);
    }
    
    *(p_index)++;
}

/*
 * TEST: ERRNOS for tsk_get
 * --------------------
 * description:Tests error cases of the tsk_get function. Tests error case for getting an 
 * invalid task by requesting a non-existent task denoted by the literal character “r”. 
 * The test will pass if the errno global variable is set to EINVAL and otherwise will fail. 
 * Tests error case for storing a task in a non-existent memory space by passing in a NULL 
 * parameter for the pointer location of the received task. The test will pass if the errno 
 * global variable is set to EFAULT and otherwise will fail.
 */
void test2_start(){
    //set test properties and result handling variables
    int test_index = 2;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_test2(test_index);
   
    task_t tid1; 
    RTX_TASK_INFO * buffer; 
    buffer = mem_alloc(sizeof(RTX_TASK_INFO));
    
    //create task with an invalid task entry function
    int res = tsk_get('r', buffer); // 'r' is a random unsigned char

    //task should not be created
    if (res == -1){
        if (errno == EINVAL)
            MARK_TEST_PASSED(test_index, *p_index);
        else
            printf("%s Invalid errno: Got %d, expected %d\r\n", PREFIX, res, EFAULT);
    } else{
        printf("%s res %d\r\n", PREFIX, res);
    }
    
    *(p_index)++;

    //create task with an invalid task memory location
    res = tsk_get(tid1, NULL);

    //task should not be created
    if (res == -1){
        if (errno == EFAULT)
            MARK_TEST_PASSED(test_index, *p_index);
        else
            printf("%s Invalid errno: Got %d, expected %d\r\n ", PREFIX, res, EFAULT);
    } else{
        printf("%s res %d\r\n ", PREFIX, res);
    }

    mem_dealloc(buffer);
    *(p_index)++;
}

/*
 * TEST 3
 * --------------------
 * description: Tests functionality of tsk_create function. Tests making a task and yielding 
 * it. Task should be created and yieleded successfully for test pass otherwise test will 
 * fail. Tests creating task of unreasonable memory size and expecting failure. If task 
 * creation fails test will pass otherwise test will fail.     
 */
void test3_start()
{
    //set test properties and result handling variables
    int test_index = 3;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_test2(test_index);
    
    task_t tid1; 
    task_t tid2;
    RTX_TASK_INFO * buffer;

    buffer = mem_alloc(sizeof(RTX_TASK_INFO));

    //create low priority task with no requested memory
    int ret_val = tsk_create(&tid1, &empty_task, LOW, 0x0);  
    tsk_get(tid1, buffer);
    
    //task should be created with minimum memory size
    if (ret_val == -1){
        printf("%s Failed to create task\r\n", PREFIX);
    } else{
        //yield task
        tsk_yield(); // execute tsk/remove tsk from ready queue
    }

    //check if task was loaded to memory of correct size
    if (buffer->u_stack_size == PROC_STACK_SIZE){
        MARK_TEST_PASSED(test_index, *p_index);
    }
    *(p_index)++;

    printf("%s U stack size: got %d, expected %d\r\n", PREFIX, buffer->u_stack_size, PROC_STACK_SIZE);

    //create high priority task of unreasonable memory size
    int val = 2147483640;
    ret_val = tsk_create(&tid2, &empty_task, HIGH, val);
    
    //task should not be created
    if (ret_val ==-1){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        tsk_yield(); // execute tsk/remove tsk from ready queue in failing case
    }
    *(p_index)++;

    mem_dealloc(buffer);
}

/*
 * TEST 4
 * --------------------
 * description: Tests error functionality of tsk_create function under invalid priority
 * requests. Tests creating task with an invalid priority number of “123” and expects the
 * correct errno recording of EINVAL. If incorrect error response test will fail otherwise
 * test will pass.
 */
void test4_start()
{
    //set test properties and result handling variables
    int test_index = 4;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_generic(test_index);

    //create task with invalid priority
    task_t tid1;
    int res = tsk_create(&tid1, &empty_task, 123, 0x0);  

    //task should not be created and correct errno should be set
    if (res == -1){
        if (errno == EINVAL)
            MARK_TEST_PASSED(test_index, *p_index);
        else
            printf("%s Invalid errno: Got %d, expected %d\r\n ", PREFIX, errno, EINVAL);
    } else{
        printf("%s res %d\r\n ", PREFIX, res);
    }

    *(p_index)++;
}

/*
 * TEST 5
 * --------------------
 * description: Tests functionality of tsk_create function under multiple uses. Tests 
 * making maximum number of tasks and creating all tasks without error. Maximum number
 * of tasks is the MAX_TASKS macro less two tasks for the currently running tasks running
 * tests in the test suite. Tests creating one test more than the maximum number of tasks.
 * This request should return an error. If all tasks were created except the task created 
 * after maximum tasks were created test will pass, otherwise test will fail. 
 */
void test5_start()
{   
    //set test properties and result handling variables
    int test_index = 5;
    gen_req_test5(test_index);
    U8  *p_index = &(g_ae_xtest.index);

    //create array to fit maximum number of tasks plus one less the currently running two tasks
    task_t task_arr [MAX_TASKS-1];
    task_t task_over_max;

    // create maximum amount of tasks and record results
    int failed_task_under_max = -1;
    for (int i=0; i<MAX_TASKS-2; i++){
        int ret_val = tsk_create(&task_arr[i], &silent_task, LOWEST, 0x200);

        printf("%s index:  %d RUN: %d  \r\n", PREFIX, i, ret_val);
        if (ret_val !=0){
            failed_task_under_max = i;
            break;
        }
    }

    //make sure all tasks were created
    if (failed_task_under_max == -1){
        MARK_TEST_PASSED(test_index, *p_index);
    }
    *(p_index)++;
  
    // create one extra task
    int ret_val = tsk_create(&task_over_max, &silent_task, LOWEST, 0x200);

    if (ret_val == -1){
        failed_task_under_max = MAX_TASKS;
    }

    //should not be able to create last task because too many tasks are running
    //check if correct index of task creation failed
    if (failed_task_under_max == -1) { 
        printf("%s Expected to create %d tasks failed on task %d (indexed by 0)\r\n",
        PREFIX, MAX_TASKS, failed_task_under_max);
    } else if (failed_task_under_max == MAX_TASKS){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        printf("%s Was able to create over the max tasks limit\r\n", PREFIX);
    }
    *(p_index)++;

    //change priority of all tasks to high such that they all run to completion
    for(int i =0; i<MAX_TASKS-2; i++){
        tsk_set_prio(task_arr[i], HIGH);
    }

    // create maximum number of tasks again and record results
    failed_task_under_max = -1;
    for (int i=0; i<MAX_TASKS-2; i++){
        int ret_val = tsk_create(&task_arr[i], &silent_task, LOWEST, 0x200);

        printf("%s index:  %d RUN: %d  \r\n", PREFIX, i, ret_val);
        if (ret_val !=0){
            failed_task_under_max = i;
            break;
        }
    }

    //all tasks should have been created because the previous tasks are expected to exit
    if (failed_task_under_max == -1){
        MARK_TEST_PASSED(test_index, *p_index);
    }
    *(p_index)++;

    for(int i =0; i<MAX_TASKS-2; i++){
        tsk_set_prio(task_arr[i], HIGH);
    }
}

/*
 * TEST 6 
 * --------------------
 * description: Tests nested preemption. Tests this by creating a task at a 
 * LOW priority, this task creates another task at a MEDIUM priority which 
 * finally creates a task at a HIGH priority. Afterwards tsk_yield is called 
 * and the test compares the task preemption order to an expected order. Test 
 * will pass if the tasks occurred to our expected order of occurrence, otherwise will fail.
 */
void test6_start()
{
    //set test properties and result handling variables
    int test_index = 6;
    U8  *p_index = &(g_ae_xtest.index);
    gen_req_test6();
    
    task_t tid1;
    printf("\r\n\r\n%s RUNNING TEST 6\r\n", PREFIX);
    
    //create test6_task1 at a LOW priority, which will create a test6_task2 at a MEDIUM
    //priority which will in turn create a test6_test3 at a HIGH priority in order to 
    //create an expected order of preemption once yielded
    int res = tsk_create(&tid1, &test6_task1, LOW, 0x200);
    tsk_yield();

    create_expected_results(test_index);     /* fill the expected results array for comparison */
    process_results_of_sequence(test_index, p_index);
    
    *(p_index)++;
}

/*
 * TEST 7
 * --------------------
 * description: Test a basic priority change which causes preemption. In this test, 
 * test7_task1 is created at LOWEST priority initially. Because our privileged task, 
 * priv1_task which runs all the testing code is at priority LOW, this task will not 
 * run initially. However, after setting the priority to HIGH, it now becomes the 
 * highest priority task in the system and is run. 
 */
void test7_start()
{
    //set test properties and result handling variables
    int test_index = 7;
    task_t tid1;
    U8  *p_index = &(g_ae_xtest.index);

    gen_req_test7();
    

    /* a task with lower priority than priv1_task is created, it will not run initially */
    int res = tsk_create(&tid1, &test7_task1, LOWEST, 0x200); 

    /* task priority is set to higher than priv1_task, triggers premption*/
    tsk_set_prio(tid1, HIGH); 
    
    create_expected_results(test_index);     /* fill the expected results array for comparison */ 
    process_results_of_sequence(test_index, p_index);
    
    *(p_index)++;
}


/*
 * TEST 8
 * --------------------
 * description: Tests the scenario where two tsks are created with the same priority.
 * As seen in our code, an initial task test8_task1 is created, which then subsequently
 * calls the creation of test8_task2 at the same priority. We then test the expected 
 * behavior which is for the first task to finish executing before the second task is executed.
 */
void test8_start()
{
    //set test properties and result handling variables
    int test_index = 8;
    task_t tid1;
    U8  *p_index = &(g_ae_xtest.index);

    gen_req_test8();
    
    /* triggers the creation of HIGH priority tasks */
    int res = tsk_create(&tid1, &test8_task1, HIGH, 0x200); 
    
    create_expected_results(test_index);     /* fill the expected results array for comparison */
    process_results_of_sequence(test_index, p_index);
    *(p_index)++;
}


/*
 * TEST 9
 * --------------------
 * description: Tests nested tasks with lower priority, asserting preemptive behaviror 
 * is as expected. An intiial task with a HIGH priority is run, such that it creates a 
 * task of MEDIUM priority, which then subsequently creates a task of LOW priority 
 * (priv1_task needs to be yielded for this last task to run as it is also a LOW 
 * priority task.) The expected behavior is that the HIGH priority task finishes completely
 *  before the MEDIUM task finishes completely before the LOW priority task is run
 */
void test9_start()
{
    //set test properties and result handling variables
    int test_index = 9;
    task_t tid1;
    U8  *p_index = &(g_ae_xtest.index);
    task_t current_priv_task = tsk_gettid();


    gen_req_test9();
    
    /* we need to know the task id of te privledged task to make sure we are returning properly to it */
    update_task_ids_array(test_index, priviledged_task);

    int res = tsk_create(&tid1, &test9_task1, HIGH, 0x200); 
    update_exec_seq(test_index, current_priv_task);

    /* we need to yield priv1 so that test9_task3 can be run  */  
    tsk_yield();
    
    create_expected_results(test_index);     /* fill the expected results array for comparison */
    process_results_of_sequence(test_index, p_index);
    *(p_index)++;
}

/*
 *===========================================================================
 *                             TASKS
 *===========================================================================
 */


void priv_task1(void)
{
    task_t tid = tsk_gettid();
    priviledged_task = tid;
    
    test0_start();
    test1_start();
    test2_start();
    test3_start();
    test4_start();
    test5_start();
    test6_start();
    test7_start();
    test8_start();
    test9_start();

    test_exit();
}

void silent_task(void)
{
    // pass
    tsk_exit();
}

void empty_task(void)
{
    printf("\r\n\r\n%s Empty task running!\r\n\r\n\r\n", PREFIX);
    for (int i =0; i<100000; i++){
        //pass 
    }
    tsk_exit();
}


/*TEST 1 BEGIN*/
void test1_task1(void)
{
    int test_id = 1;
    int task_num = 1;
    task_t tid = tsk_gettid();

    _log_testing_task_is_running(task_num, tid);
    printf("%s Attempting to set priority of a privledged task from non privledged task\r\n", PREFIX);
    
    printf("%s priviledged_task %d \r\n", PREFIX, priviledged_task);
    int res = tsk_set_prio(priviledged_task, HIGH);
    g_tsk_cases[test_id].data = res;

    tsk_exit();
}
/*TEST 1 END*/


/*TEST 6 BEGIN*/
void test6_task1(void)
{
    int test_id = 6;
    int task_num = 1;
    task_t tid = tsk_gettid();

    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    int ret_val = tsk_create(&tid, &test6_task2, MEDIUM, 0x200);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    tid = tsk_gettid();
    update_exec_seq(test_id, tid); 
    tsk_exit();
}

void test6_task2(void)
{
    int test_id = 6; 
    int task_num = 2;

    task_t tid = tsk_gettid();
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    int ret_val = tsk_create(&tid, &test6_task3, HIGH, 0x200);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    tid = tsk_gettid();
    update_exec_seq(test_id, tid);
    tsk_exit();
}


void test6_task3(void)
{
    int test_id = 6;
    int task_num = 3;
    task_t tid = tsk_gettid();

    base_update_task_template(test_id, task_num);
    
    tsk_exit();
}
/*TEST 6 END*/

/*TEST 7 BEGIN*/
void test7_task1(void)
{
    int test_id = 7; 
    task_t tid = tsk_gettid();
    printf("\r\n%s TASK 10 TID %d\r\n\r\n", PREFIX, tid); 

    update_exec_seq(test_id, tid);
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */

    tsk_exit();
}
/*TEST 7 END*/

/*TEST 8 BEGIN*/
void test8_task1(void)
{
    int test_id = 8; 
    int task_num = 1;
    task_t tid = tsk_gettid();
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    int ret_val = tsk_create(&tid, &test8_task2, HIGH, 0x200);
    tid = tsk_gettid();
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    tsk_exit();
}

void test8_task2(void)
{
    int test_id = 8;
    int task_num =2;

    base_update_task_template(test_id, task_num);
    tsk_exit();
}
/*TEST 8 END*/

/*TEST 9 BEGIN*/
void test9_task1(void)
{
    int test_id = 9; 
    int task_num = 1;
    task_t tid = tsk_gettid();
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    int ret_val = tsk_create(&tid, &test9_task2, MEDIUM, 0x200);
    tid = tsk_gettid();
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    tsk_exit();
}
void test9_task2(void)
{
    int test_id = 9; 
    int task_num = 2;
    task_t tid = tsk_gettid();
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    int ret_val = tsk_create(&tid, &test9_task3, LOW, 0x200);
    tid = tsk_gettid();
    update_exec_seq(test_id, tid);
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    tsk_exit();
}

void test9_task3(void)
{
    int test_id = 9;
    int task_num = 3;

    base_update_task_template(test_id, task_num);
    tsk_exit();
}
/*TEST 9 END*/



/* DESCIPRTION: generic teest that updates the execution seq
and task array while output 6 characters at delayed intervals */
void base_update_task_template(int test_id, int task_num)
{
    task_t tid = tsk_gettid();
    _log_testing_task_is_running(task_num, tid);  /* generic log function */
    update_task_ids_array(test_id, tid);   /* task_ids are used for sequential comparision */
    update_exec_seq(test_id, tid);
    printf("\r\n%s Output for Task 11: \r\n", PREFIX);
    for (int i =0; i<6000; i++){
        int mod = i%1000;   
        if (mod == 0){
            char c = (char) (i/1000 + 65);
            printf("%c %c %c %c \r\n", c, c, c, c);
        }
    }
    printf("\r\n");
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */