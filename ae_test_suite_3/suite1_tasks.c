#include "suite1_tasks.h"


// needs to be single digit to have proper length
static char * get_g_msg_from_index(int i)
{
    char * msg = mem_alloc(sizeof(G_MSG_LEN));
    sprintf(msg, "This  is  Message %d", i);

    return msg;
}

static void ** generate_five_expected_rcv()
{   
    void ** buf_arr = mem_alloc(sizeof(void*) * 5);
    int test_index = 1;
    SNDR_HDR * sndr_hdr = (SNDR_HDR *) g_student_tsk_cases[test_index].data; 

    buf_arr = mem_alloc(sizeof(U8*) * sndr_hdr->max_len);
    buf_arr[0] = create_message(sndr_hdr->sender_tid_arr[0], g_msg3, G_MSG_LEN); // allocates our message for us
    buf_arr[1] = create_message(sndr_hdr->sender_tid_arr[1], g_msg4, G_MSG_LEN); // allocates our message for us
    buf_arr[2] = create_message(sndr_hdr->sender_tid_arr[2], g_msg5, G_MSG_LEN); // allocates our message for us
    buf_arr[3] = create_message(sndr_hdr->sender_tid_arr[3], g_msg1, G_MSG_LEN); // allocates our message for us
    buf_arr[4] = create_message(sndr_hdr->sender_tid_arr[4], g_msg2, G_MSG_LEN); // allocates our message for us

    return buf_arr;
}


static int cmp_timeval(TIMEVAL * t1, TIMEVAL *t2){
    if (t1->sec > t2->sec){
        return 1;
    } else if (t1->sec < t2->sec){
        return -1;
    } else if (t1->usec > t2->usec){
        return 1;
    } else if (t1->usec < t2->usec){
        return -1;
    } else{
        return 0; // two tv are equal
    }
}

static void print_tv(TIMEVAL *tv)
{
    printf("TIMEVAL sec %d, usec %d\r\n", tv->sec, tv->usec);
}

static void subtract_tv(TIMEVAL * res, TIMEVAL * tv1, TIMEVAL * tv2)
{
    if (cmp_timeval(tv1, tv2) !=1) {
        TIMEVAL * tv3 = mem_alloc(sizeof(TIMEVAL));
        tv3->sec = tv1->sec;
        tv3->usec = tv1->usec;
        tv1 = tv2;
        
        TIMEVAL * to_delete = tv2;
        tv2 = tv3;
        
        mem_dealloc(to_delete);
    }
    
    unsigned long long sum = (tv1->sec *1000000 + tv1->usec) - (tv2->sec *1000000 + tv2->usec);

    res->usec = sum % 1000000;
    res->sec = sum / 1000000;
}

static void calc_period(TIMEVAL *tv, int size)
{   
    int res;
    res = RTX_TICK_SIZE * MIN_PERIOD * size;
    printf("res %d %d %d \r\n\r\n",res, (U32) res %1000000, (U32) res / 1000000);
    tv->usec = res % 1000000;
    tv->sec = res / 1000000;
}

static void print_tasks(task_t * arr, int len)
{
    for (int i =0; i < len; i++ ){
        printf("%d->", arr[i]);
    }
    printf("\r\n");
}

/*
 *===========================================================================
 *                             TEST TASKS 
 *===========================================================================
 */


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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    res3 = mbx_get(curr_tid);
    if (res3 == RTX_ERR && errno == ENOENT){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num ++;

    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
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
    int NUM_EXPECTED_TSKS = 4 +1 + NUM_DEFAULT_MBX_TSKS; //3 recvs, priv1, kcd and cdsip
    task_t tasks [MAX_TASKS];
    task_t tid, tid1, tid2, tid3;
    tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int) test_index, (int) case_num, (int) task_id);

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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int) test_index, (int) case_num, (int) task_id);

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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    
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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

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
    task_t tasks [MAX_TASKS];
	int res22;
    

    task_t curr_tid, rcv_tid1, rcv_tid2;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    U8  msg [] = "Congradulations YOu are pregnannt\r\n"; 
    RTX_MSG_HDR * buffer1 = create_message(curr_tid, msg, BUF_LEN); // generate message 

    // Case 3
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
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
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    res2 = send_msg_nb(MAX_TASKS+1, buffer1); // attempt to send msg to non-existent tid

    if (res2 == RTX_ERR && errno == EINVAL){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++;
    
    
    // Case 5 
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
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

    int resybby =0;
    
    // Case 6
    mbx_create(GET_BUFSIZE(G_MSG_LEN/2) *2);
    
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    char half_msg [] =    "HALF_MSG\r\r\n";
    char half_msg_p1 [] = "HALFMSG+1\r\n";
    char half_msg_m1 [] = "MSG/2-1\r\n";  
    
    RTX_MSG_HDR * buffer3 = create_message(curr_tid, half_msg, sizeof(half_msg)); // generate message 
    RTX_MSG_HDR * buffer4 = create_message(curr_tid, half_msg_p1, sizeof(half_msg_p1)); // generate message
    RTX_MSG_HDR * buffer5 = create_message(curr_tid, half_msg_m1, sizeof(half_msg_m1)); // generate message
    RTX_MSG_HDR * buffer6 = create_message(curr_tid, "aa", 2); // generate message
    
    resybby = mbx_get(curr_tid);

    res1 = send_msg_nb(curr_tid, buffer3);
    resybby = mbx_get(curr_tid);

    // attempt to send msg after msgbx is already full 
    res2 = send_msg_nb(curr_tid, buffer4);
    resybby = mbx_get(curr_tid);
    
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