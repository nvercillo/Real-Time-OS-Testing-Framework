/**
 * @brief       Group 31 Lab1 Memory Test Suite 3
 * @details     Tests for error cases in mem_alloc and mem_dealloc functions
 * 
 *              The first function in this file is a test suite
 *              function, it is solely responisble for executing
 *              all of the tests within the suite.
 * 
 *              Each test is stores in a function definition and
 *              is described in a comment above the definition
 */

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae.h"
#include "ae_util.h"
#include "rtx_errno.h"
#include "ae_tasks_util.h"

//number of tests in suite
#define NUM_TESTS 4
//smallest possible memory allocation size
#define ATOMIC_BLOCK_SIZE WORD_SIZE
//maximum number of blocks that can be allocated if allocating for atomic block size
#define MAX_NO_ATOMIC_BLOCKS RAM1_SIZE / ATOMIC_BLOCK_SIZE

//expected return value of a successful mem_dealloc function call
#define DEALLOC_SUCCESS_CODE 0
 
const char   PREFIX[]      = "G31-MEM-TS3";
const char   PREFIX_LOG[]  = "G31-MEM-TS3-LOG";
const char   PREFIX_LOG2[] = "G31-MEM-TS3-LOG2";

task_t priviledged_task;

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

U32 test_mem(void);
void record_test_result(U32 test_num, U32 result);

void _test_over_allocating_fullsize(U32 test_num);
void _test_over_allocating_atomicsize(U32 test_num);
void _test_dealloc_out_of_range_block(U32 test_num);
void _test_dealloc_out_of_range_block_2(U32 test_num);


// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH + i;
        tasks[i].priv = 1;
    }
    tasks[0].prio = LOW;
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].prio = HIGH;
    tasks[1].priv  = 0;
    tasks[1].ptask = &silent_task;
    
    priviledged_task = tasks[0].tid;
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
    printf("%s: START\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

void gen_req_num_bits(int test_id, int num_bits)
{       
    //bits[0:3] pos check, bits[4:9] for exec order check
    g_tsk_cases[test_id].p_ae_case->num_bits = num_bits;
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 0;       // N/A for this test
    g_tsk_cases[test_id].pos_expt = 0;  // N/A for this test
    
    update_ae_xtest(test_id);
}

void silent_task(void)
{
    // pass
    tsk_exit();
}

void priv_task1(void)
{
    test_mem();
    test_exit();
}

/* test suite function used to call subsequent tests */
U32 test_mem(void) {
    //run tests
    gen_req_num_bits(0, 1);
    _test_over_allocating_fullsize(0);
    gen_req_num_bits(1, 1);
	  _test_over_allocating_atomicsize(1);
    gen_req_num_bits(2, 1);
    _test_dealloc_out_of_range_block(2);
    gen_req_num_bits(3, 1);
    _test_dealloc_out_of_range_block_2(3);

    return 0;
}

/*
 * Prints out test result for user and records result on corresponding
 * bit of result variable
 */
void record_test_result(U32 test_num, U32 result){
  process_sub_result(test_num, g_ae_xtest.index, result);
  g_ae_xtest.index++;
}

/*
 * TEST: Allocates more than the size of the RAM module and checks for the correct error response
 * --------------------
 * description:     Allocates for entire RAM module twice.
 *                  expects a NULL pointer from second allocation and ENOMEM error code
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_over_allocating_fullsize(U32 test_num){
    U32 passed = 1;

    //allocate entire ram size twice
    void * res = mem_alloc(RAM1_SIZE);
    void * res2 = mem_alloc(RAM1_SIZE);

    //assert first allocation was successful, second was unsuccessful and correct error code was set
    if (res == NULL || res2 != NULL || errno != ENOMEM){
      passed = 0;
    }
		
    //dealloc intentionally allocated memory
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Allocates atomic size blocks of memory to fill entire RAM and then tries to allocate an extra block
 * --------------------
 * description:     Allocates the smallest possible block size repeatedly until RAM is full
 *                  tries to allocate one more block
 *                  expected last allocation to be a NULL pointer and a ENOMEM error code
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_over_allocating_atomicsize(U32 test_num){
    U32 passed = 1;
		void* res;
	
    //fill memory with atomic size block allocations
		for (U32 i = 0; i < MAX_NO_ATOMIC_BLOCKS; i ++) {
			res = mem_alloc(ATOMIC_BLOCK_SIZE);
      //check for successful allocation
			if (res == NULL){
				passed = 0;
			}
		}

    //allocate an extra block
		void* res2 = mem_alloc(ATOMIC_BLOCK_SIZE);
		
    //assert that block was not allocated and correct error code was set
		if(res2 != NULL || errno != ENOMEM){
			passed = 0;
		}

    //set pointer to last possible allocation address of the unit block size
    res = (void*)(RAM1_START + (MAX_NO_ATOMIC_BLOCKS - 1)*ATOMIC_BLOCK_SIZE);
		
    //deallocate all blocks of unit size in RAM
		for (U32 i = 0; i < MAX_NO_ATOMIC_BLOCKS; i ++) {
			if(mem_dealloc((void *)((U32)res - i*ATOMIC_BLOCK_SIZE)) != DEALLOC_SUCCESS_CODE){
				passed = 0;
			}
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Attempts to dealloc a block of memory not within RAM bounds
 * --------------------
 * description:     Attempts to dealloc memory before the beginning of RAM
 *                  Checks for correct response and error code of EFAULT
 *                   
 *                  res is intended to be the response of the deallocation.
 */
void _test_dealloc_out_of_range_block(U32 test_num){
    U32 passed = 1;
    // dealloc non existent memory
    U32 res = mem_dealloc((void *)(RAM1_START - 0x1));
    
    //assert that error response was returned and correct error code was set
    if (res != -1 || errno != EFAULT){
			passed = 0;
    }
		
    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Attempts to dealloc a block of memory not within RAM bounds
 * --------------------
 * description:     Attempts to dealloc memory after the end of RAM
 *                  Checks for correct response and error code of EFAULT
 *                   
 *                  res is intended to be the response of the deallocation.
 */
void _test_dealloc_out_of_range_block_2(U32 test_num){
    U32 passed = 1;
    // dealloc non existent memory 
    U32 res = mem_dealloc((void *)(RAM1_END + 0x1));

    //assert that error response was returned and correct error code was set
    if (res != -1 || errno != EFAULT){
			passed = 0;
    }
		
    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}