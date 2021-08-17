/**
 * @brief       Group 31 Lab1 Memory Test Suite 2
 * @details     Tests for behaviours when deallocating memory using the buddy system
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
#define NUM_TESTS 6
//smallest possible memory allocation size
#define ATOMIC_BLOCK_SIZE WORD_SIZE
//maximum number of blocks that can be allocated if allocating for atomic block size
#define MAX_NO_ATOMIC_BLOCKS RAM1_SIZE / ATOMIC_BLOCK_SIZE

//expected return value of a successful mem_dealloc function call
#define DEALLOC_SUCCESS_CODE 0
 
const char   PREFIX[]      = "G31-MEM-TS2";
const char   PREFIX_LOG[]  = "G31-MEM-TS2-LOG";
const char   PREFIX_LOG2[] = "G31-MEM-TS2-LOG2";

task_t priviledged_task;

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

void gen_req_num_bits(int test_id, int num_bits);

U32 test_mem(void);
void record_test_result(U32 test_num, U32 result);

void _test_alloc_entire_block(U32 test_num);
void _test_alloc_entire_block_2(U32 test_num);
void _test_alloc_buddies(U32 test_num);
void _test_coalescing_free_regions(U32 test_num);
void _test_external_fragmentation(U32 test_num);
void _test_internal_fragmentation(U32 test_num);

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
    _test_alloc_entire_block(0);
    gen_req_num_bits(1, 1);
    _test_alloc_entire_block_2(1);
    gen_req_num_bits(2, 1);
    _test_alloc_buddies(2);
    gen_req_num_bits(3, 1);
    _test_coalescing_free_regions(3);
    gen_req_num_bits(4, 1);
    _test_external_fragmentation(4);
    gen_req_num_bits(5, 1);
    _test_internal_fragmentation(5);

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
 * TEST: Alloc/Dealloc entire RAM block using one allocation
 * --------------------
 * description:     allocate and then deallocate 32 kilobytes
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_alloc_entire_block(U32 test_num){
    U32 passed = 1;

    //allocate block
    void * res = mem_alloc(RAM1_SIZE);

    //check if successfully allocated
    if (res == NULL){
      passed = 0;
    }
		
    //deallocate block and check for successful deallocation
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Allocates atomic size blocks of memory to fill entire RAM
 * --------------------
 * description:     Allocates the smallest possible block size repeatedly until RAM is full
 *                  then deallocates all blocks
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_alloc_entire_block_2(U32 test_num){
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
 * TEST: Alloc/Dealloc two equal size blocks of the smallest possible allocation size
 * --------------------
 * description:     allocate and then deallocate two blocks of 32 bytes
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_alloc_buddies(U32 test_num){
    U32 passed = 1;

    //allocate blocks
    void * res = mem_alloc(ATOMIC_BLOCK_SIZE);
    void * res2 = mem_alloc(ATOMIC_BLOCK_SIZE);

    //check if error wasn't returned
    if (res == NULL || res2 == NULL){
      passed = 0;
    }
		
    //deallocate blocks
		if(mem_dealloc(res) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}
    
		if(mem_dealloc(res2) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Checks the ability to allocate memory of larger sizes after smaller blocks are freed
 * --------------------
 * description:     Allocates memory for a block the size of half the RAM module
 *                  and two blocks a quarter of the size of the RAM module.
 *                  Tries to deallocate the quarter blocks and then allocate another half.
 *                  Tries to deallocate both half blocks and then allocate the entire module.
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_coalescing_free_regions(U32 test_num){
    U32 passed = 1;

    //allocate two quarter blocks and one half block
    void * res_unit_block_1 = mem_alloc(RAM1_SIZE/4);
    void * res_unit_block_2 = mem_alloc(RAM1_SIZE/4);
    void * res_2_unit_blocks = mem_alloc(RAM1_SIZE/2);

    //check if all allocations were successful
    if (
      res_unit_block_1  == NULL ||
      res_unit_block_2  == NULL ||
      res_2_unit_blocks  == NULL
    ){
      passed = 0;
    }
		
    //dealloc the quarter blocks and check for successful response
		if(mem_dealloc(res_unit_block_1) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    if(mem_dealloc(res_unit_block_2) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }
		
    //allocate another half block
		void * res_2_unit_blocks_2 = mem_alloc(RAM1_SIZE/2);
		
    //check for successful allocation
    if (res_2_unit_blocks_2 == NULL){
      passed = 0;
    }
		
    //dealloc the half blocks and check for successful response
		if(mem_dealloc(res_2_unit_blocks) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}
    
		if(mem_dealloc(res_2_unit_blocks_2) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}
		
    //allocate entire RAM module
		void * res_all_memory = mem_alloc(RAM1_SIZE);
		
		//check for successful allocation
    if (res_all_memory == NULL){
      passed = 0;
    }
		
    //dealloc RAM module
		if(mem_dealloc(res_all_memory) != DEALLOC_SUCCESS_CODE){
			passed = 0;
		}

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Allocates and deallocates quarters of the RAM module in different permutations
 * --------------------
 * description:     Allocates 4 quarters of the RAM module.
 *                  Chooses and deallocates 2 of the quarters.
 *                  Checks how many free blocks of memory there are
 *                  Observes if fragmentation has occured
 *                  Repeats for every permutation of 2 blocks to deallocate
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_external_fragmentation(U32 test_num){
    U32 passed = 1;

    //allocate 4 quarters of RAM module
    void* res1 = mem_alloc(RAM1_SIZE/4);
    void* res2 = mem_alloc(RAM1_SIZE/4);
    void* res3 = mem_alloc(RAM1_SIZE/4);
    void* res4 = mem_alloc(RAM1_SIZE/4);

    //check amount of free blocks of memory
    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    U32 free_blocks = mem_dump();

    //check for successful allocations
    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    //dealloc 2 of the quarters
    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res2) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    //check amount of free memory blocks
    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    //if blocks have not coalesced into one bigger block memory is fragmented
    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 1 and 2\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 1 and 2\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    //dealloc rest of the memory
    if(mem_dealloc(res3) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    //repeat above logic 5 more times for every permutation of freeing 2 of the quarter blocks and checking for fragmentation
    res1 = mem_alloc(RAM1_SIZE/4);
    res2 = mem_alloc(RAM1_SIZE/4);
    res3 = mem_alloc(RAM1_SIZE/4);
    res4 = mem_alloc(RAM1_SIZE/4);

    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    free_blocks = mem_dump();

    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res3) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 1 and 3\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 1 and 3\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    if(mem_dealloc(res2) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    res1 = mem_alloc(RAM1_SIZE/4);
    res2 = mem_alloc(RAM1_SIZE/4);
    res3 = mem_alloc(RAM1_SIZE/4);
    res4 = mem_alloc(RAM1_SIZE/4);

    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    free_blocks = mem_dump();

    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 1 and 4\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 1 and 4\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    if(mem_dealloc(res2) != DEALLOC_SUCCESS_CODE || mem_dealloc(res3) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    res1 = mem_alloc(RAM1_SIZE/4);
    res2 = mem_alloc(RAM1_SIZE/4);
    res3 = mem_alloc(RAM1_SIZE/4);
    res4 = mem_alloc(RAM1_SIZE/4);

    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    free_blocks = mem_dump();

    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    if(mem_dealloc(res2) != DEALLOC_SUCCESS_CODE || mem_dealloc(res3) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 2 and 3\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 2 and 3\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    res1 = mem_alloc(RAM1_SIZE/4);
    res2 = mem_alloc(RAM1_SIZE/4);
    res3 = mem_alloc(RAM1_SIZE/4);
    res4 = mem_alloc(RAM1_SIZE/4);

    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    free_blocks = mem_dump();

    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    if(mem_dealloc(res2) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 2 and 4\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 2 and 4\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res3) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    res1 = mem_alloc(RAM1_SIZE/4);
    res2 = mem_alloc(RAM1_SIZE/4);
    res3 = mem_alloc(RAM1_SIZE/4);
    res4 = mem_alloc(RAM1_SIZE/4);

    printf("%s Checking amount of free blocks after allocating entire RAM module\r\n", PREFIX);
    free_blocks = mem_dump();

    if(res1 == NULL || res2 == NULL || res3 == NULL || res4 == NULL){
      passed = 0;
    }

    if(mem_dealloc(res3) != DEALLOC_SUCCESS_CODE || mem_dealloc(res4) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    printf("%s Checking amount of free blocks after deallocating 2 of 4 blocks\r\n", PREFIX);
    free_blocks = mem_dump();

    if(free_blocks == 2){
      printf("%s Memory did not coalesc after deallocating blocks 3 and 4\r\n", PREFIX);
      printf("%s Memory was externally fragmented\r\n", PREFIX);
    } else {
      printf("%s Memory coalesced after deallocating blocks 3 and 4\r\n", PREFIX);
      printf("%s Memory was not externally fragmented\r\n", PREFIX);
    }

    if(mem_dealloc(res1) != DEALLOC_SUCCESS_CODE || mem_dealloc(res2) != DEALLOC_SUCCESS_CODE){
      passed = 0;
    }

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}

/*
 * TEST: Checks logic of the buddy system by allocating for less memory than RAM module is capable of in such a way that no more memory can be allocated
 * --------------------
 * description:     Allocates memory for 1 and more than 1 halves of the RAM module.
 *                  Asserts that no more memory can be allocated as both 1 half and more
 *                  than one half of memory will take up 2 half sized blocks and therefore allocate
 *                  the entire RAM module.
 *                   
 *                  res is intended to be the first byte of a new 
 *                  memory block.
 */
void _test_internal_fragmentation(U32 test_num){
    U32 passed = 1;
    //allocate a half of the RAM module
    void* half_of_memory = mem_alloc(RAM1_SIZE/2);
    //allocate another half less 8 bytes
    void* less_than_half_of_memory = mem_alloc(RAM1_SIZE/2 - 0x8);
    //check the amount of free blocks of memory (should be 0)
    mem_dump();
    //try and allocate the last 8 bytes of the RAM
    void* remaining_8_bytes_of_memory = mem_alloc(0x8);

    //assert that the first two allocations were successful, however, not the last one
    if(
      half_of_memory == NULL ||
      less_than_half_of_memory == NULL ||
      remaining_8_bytes_of_memory != NULL
    ){
      passed = 0;
    }

    /* this should be run at the bottom of every test */
    record_test_result(test_num, passed);
}