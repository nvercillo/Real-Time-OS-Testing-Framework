/**
 * @brief       Group 31 Lab1 Memory Test Suite 1
 * @details     Tests for basic allocations and deallocations of varying block sizes
 * 
 *              The first function in this file is a test suite
 *              function, it is solely responisble for executing
 *              all of the tests within the suite.
 * 
 *              Each test is stored in a function definition and
 *              is described in a comment above the definition
 */

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae.h"
#include "ae_util.h"
#include "rtx_errno.h"
#include "ae_tasks_util.h"

//smallest possible memory allocation size
#define ATOMIC_BLOCK_SIZE WORD_SIZE

//expected return value of a successful mem_dealloc function call
#define DEALLOC_SUCCESS_CODE 0
    
#define NUM_TESTS 3          // number of tests
 
const char   PREFIX[]      = "G31-MEM-TS1";
const char   PREFIX_LOG[]  = "G31-MEM-TS1-LOG";
const char   PREFIX_LOG2[] = "G31-MEM-TS1-LOG2";

task_t priviledged_task;

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

void gen_req_num_bits(int test_id, int num_bits);

U32 test_mem(void);
void record_test_result(U32 test_num, U32 result);

void _test_alloc_block(U32 test_num, U32 block_size);
void _test_alloc_one_byte_greater_than_block(U32 test_num, U32 block_size);
void _test_alloc_one_byte_less_than_block(U32 test_num, U32 block_size);

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
    //tracks size of block being allocated in test
    U32 block_size = RAM1_SIZE;
    U32 num_sub_tests = 0;
    while( block_size >= ATOMIC_BLOCK_SIZE){
      block_size /= 2;
      num_sub_tests += 1;
    }
    gen_req_num_bits(0, num_sub_tests);
    block_size = RAM1_SIZE;
    while( block_size >= ATOMIC_BLOCK_SIZE){
      //test allocating blocks of all possible sizes in memory
      _test_alloc_block(0, block_size);

      block_size /= 2;
    }

    //for testing allocating block sizes less than a given size, do not end with
    //smallest possible block size
    block_size = RAM1_SIZE;
    gen_req_num_bits(1, num_sub_tests);
    while( block_size >= ATOMIC_BLOCK_SIZE){
      //test allocating blocks one byte less than each possible block size
      _test_alloc_one_byte_less_than_block(1, block_size);

      block_size /= 2;
    }

    //for testing allocating block sizes greater than a given size, start with
    //highest block size lower than the entire RAM module size
    block_size = RAM1_SIZE/2;
    num_sub_tests = 0;
    while( block_size >= ATOMIC_BLOCK_SIZE){
      block_size /= 2;
      num_sub_tests += 1;
    }
    gen_req_num_bits(2, num_sub_tests);
    block_size = RAM1_SIZE/2;
    while( block_size >= ATOMIC_BLOCK_SIZE){
      //test allocating blocks one byte greater than each possible block size
      _test_alloc_one_byte_greater_than_block(2, block_size);

      block_size /= 2;
    }

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
 * TEST: Alloc/Dealloc The Size of a Given Block
 * --------------------
 * description:     allocate and then deallocate a block which is 
 *                  of a given size 
 * 
 *                  Successful allocations are checked by noting if a memory address
 *                  within the bounds of RAM are present
 * 
 *                  If test is run for all block sizes we can deduce that newly freed memory can be reused
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_alloc_block(U32 test_num, U32 block_size){
    U32 passed = 1;
    //alloc requested block size
    void * res = mem_alloc(block_size);

    //NULL return indicats unsuccessful allocation
    if (res == NULL || ((U32)res) < RAM1_START || ((U32)res) >= RAM1_END){
      passed = 0;
    }
		
    //dealloc block and check for correct response code
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Alloc/Dealloc One Byte Greater Than Size of a Block
 * --------------------
 * description:     allocate and then deallocate a block which is 
 *                  one byte greater than a given block size. 
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 * 
 *                  Successful allocations are checked by noting if a memory address
 *                  within the bounds of RAM are present
 * 
 *                  If test is run for all block sizes we can deduce that newly freed memory can be reused
 * 
 *                  DO NOT set block_size to size of entire RAM module.
 *                  Test wil fail when allocating for memory greater than
 *                  what is available
 */
void _test_alloc_one_byte_greater_than_block(U32 test_num, U32 block_size){
    U32 passed = 1;
    //alloc requested block size plus an extra byte
    void * res = mem_alloc(block_size + 0x1);

    //NULL return indicats unsuccessful allocation
    if (res == NULL || ((U32)res) < RAM1_START || ((U32)res) >= RAM1_END){
      passed = 0;
    }
		
    //dealloc block and check for correct response code
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Alloc/Dealloc One Byte Less Than Size of a Block
 * --------------------
 * description:     allocate and then deallocate a block which is 
 *                  one byte less than a given block size. 
 * 
 *                  Successful allocations are checked by noting if a memory address
 *                  within the bounds of RAM are present
 * 
 *                  If test is run for all block sizes we can deduce that newly freed memory can be reused
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_alloc_one_byte_less_than_block(U32 test_num, U32 block_size){
    U32 passed = 1;
    //alloc requested block size less one byte
    void * res = mem_alloc(block_size - 0x1);

    //NULL return indicats unsuccessful allocation
    if (res == NULL || ((U32)res) < RAM1_START || ((U32)res) >= RAM1_END){
      passed = 0;
    }
		
    //dealloc block and check for correct response code
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}
