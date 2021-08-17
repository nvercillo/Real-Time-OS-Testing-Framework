#include "suite2_tasks.h"
/*
 *===========================================================================
 *                             HELPER FUNCTIONS 
 *===========================================================================
 */

static void send_five_msgs(
    SNDR_HDR ** pp_sndr_hdr, 
    U8 ** send_buf_arr, 
    task_t curr_tid,
    int test_index, 
    BOOL case0
){

    *pp_sndr_hdr = mem_alloc(sizeof(SNDR_HDR));
    SNDR_HDR* sndr_hdr = *pp_sndr_hdr;
    sndr_hdr->max_len = 5;
    sndr_hdr->len_arr = 0;
    sndr_hdr->recv_tid = curr_tid; // referes to calling function 
    sndr_hdr->prio_arr = mem_alloc(sizeof(U32) * sndr_hdr->max_len);
    sndr_hdr->sender_tid_arr = mem_alloc(sizeof(task_t) * sndr_hdr->max_len);
    sndr_hdr->curr_priority = mem_alloc(sizeof(int));
    mem_util_zero(sndr_hdr->prio_arr, sizeof(U32) * sndr_hdr->max_len);


    g_student_tsk_cases[test_index].data = (U32) sndr_hdr;

    *(sndr_hdr->curr_priority) = HIGH;
    if (case0){
        tsk_create(&sndr_hdr->sender_tid_arr[0], &test1_case0_task1, HIGH, 0x200); // msg3
        tsk_create(&sndr_hdr->sender_tid_arr[1], &test1_case0_task1, HIGH, 0x200); // msg4 

        tsk_create(&sndr_hdr->sender_tid_arr[2], &test1_case0_task1, MEDIUM, 0x200); // msg1 won't run until yielded 
        tsk_create(&sndr_hdr->sender_tid_arr[3], &test1_case0_task1, MEDIUM, 0x200); // msg2 won't run until yielded 

        tsk_create(&sndr_hdr->sender_tid_arr[4], &test1_case0_task1, HIGH, 0x200); // msg7
    } else {
        sndr_hdr->blocked_arr = mem_alloc(sizeof(U32) * 5);

        for (int i=0; i<5; i ++){  
            sndr_hdr->blocked_arr[i] = -1;
        }
        sndr_hdr->data = 0;
        tsk_create(&sndr_hdr->sender_tid_arr[0], &test1_case1_task1, HIGH, 0x200); // msg3
        tsk_create(&sndr_hdr->sender_tid_arr[1], &test1_case1_task1, HIGH, 0x200); // msg4 

        tsk_create(&sndr_hdr->sender_tid_arr[2], &test1_case1_task1, MEDIUM, 0x200); // msg1 won't run until yielded 
        tsk_yield();
        tsk_create(&sndr_hdr->sender_tid_arr[3], &test1_case1_task1, MEDIUM, 0x200); // msg2 won't run until yielded 
        tsk_yield();
        tsk_create(&sndr_hdr->sender_tid_arr[4], &test1_case1_task1, HIGH, 0x200); // msg7   
    }
}

static char * get_g_msg_from_index(int i)
{
    char * msg;
    switch (i) {
        case 0:
            msg = g_msg1;
            break;
        case 1:
            msg = g_msg2;
            break;
        case 2:
            msg = g_msg3;
            break;
        case 3:
            msg = g_msg4;
            break;
        case 4:
            msg = g_msg5;
            break;
        case 5:
            msg = g_msg6;
            break;
        case 6:
            msg = g_msg7;
            break;
        default:
            printf("\r\n\r\nFAILED got index %d %d\r\n\r\n", i, tsk_gettid());
            break;
    }

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


U32*  generate_expected_test1_case3()
{
    U32 * exp_arr = mem_alloc(sizeof(U32) * 5);
    exp_arr[0] = 1;
    exp_arr[1] = 2;
    exp_arr[2] = 3;
    exp_arr[3] = 4;
    exp_arr[4] = 5;
    return exp_arr;
}

/*
 *===========================================================================
 *                             TEST TASKS 
 *===========================================================================
 */

/* TEST 0 */ 
void test0_case0_task0(void) // RUN AT HIGH PRIORITY
{
//  *      Case 0: test that when mbx empty, premption occurs 
//  *      Case 1: test that when mbx empty, preemption doesnt occur
    int res1, res2, res3;
    int task_id = 0;
    int test_index = 0; 
    int case_num = 0; 
    task_t curr_tid, sndr_tid;
    SNDR_HDR * sndr_hdr; 
    U8 * recv_buf; 
    U8 * send_buf_arr[5];
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    mbx_create(GET_BUFSIZE(G_MSG_LEN) * 5); // mbx can receive 5 msgs

    // Case 0
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    g_student_tsk_cases[test_index].data = curr_tid;
    
    tsk_create(&sndr_tid, &test0_case0_task1, HIGH, 0x200); // doesnt run yet 

    recv_buf = mem_alloc(GET_BUFSIZE(G_MSG_LEN)); // blocks, sndr runs to completion 
    mem_util_zero(recv_buf, GET_BUFSIZE(G_MSG_LEN));

    res1 = recv_msg(recv_buf, GET_BUFSIZE(G_MSG_LEN)); // sndr should have run to completion
    
    int sub_res = g_student_tsk_cases[test_index].data;
    if (res1 == RTX_OK && sub_res == 1){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num ++;

    // Case 1
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    g_student_tsk_cases[test_index].data = curr_tid;
    
    // sender is blocked until this receiving thread is blocked
    tsk_create(&sndr_tid, &test0_case1_task1, MEDIUM, 0x200); // medium does not run by default

    mem_util_zero(recv_buf, GET_BUFSIZE(G_MSG_LEN)); // wash buffer from last case 

    // sndr should run send and be preepted by this tasks
    res1 = recv_msg(recv_buf, GET_BUFSIZE(G_MSG_LEN)); 
    res2 = g_student_tsk_cases[test_index].data;

    // finish executing sndr_tid
    tsk_set_prio(sndr_tid, HIGH);
    tsk_yield();

    res3 = g_student_tsk_cases[test_index].data; // should be set to
    if (res1 == RTX_OK &&  res2 == 1 && res3 == 0){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    case_num++;

    mem_dealloc(recv_buf);
    tsk_yield(); // yeild so rcving thread can terminates
    tsk_exit();
}


void test0_case0_task1(void)
{
    int res1, res2, res3;
    int task_id = 1;
    int test_index = 0; 
    int case_num = 0; 
    RTX_MSG_HDR * buffer1;

    task_t curr_tid, rcv_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    rcv_tid = g_student_tsk_cases[test_index].data;
    
    buffer1 = create_message(rcv_tid, g_msg1, G_MSG_LEN); // generate message 
    send_msg_nb(rcv_tid, buffer1); // send msg sucessfully

    g_student_tsk_cases[test_index].data = 1; // indicate that send occured 
    
    tsk_exit();
}


void test0_case1_task1(void)
{
    int res1, res2, res3;
    int task_id = 1;
    int test_index = 0; 
    int case_num = 1; 
    RTX_MSG_HDR * buffer1;

    task_t curr_tid, rcv_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    rcv_tid = g_student_tsk_cases[test_index].data;
    
    g_student_tsk_cases[test_index].data = 1; // indicate that send occured 
    buffer1 = create_message(rcv_tid, g_msg1, G_MSG_LEN); // generate message
    send_msg_nb(rcv_tid, buffer1); // send msg sucessfully

    g_student_tsk_cases[test_index].data = 0; // indicate second entrance into block  
    
    tsk_exit();
}




/* TEST 1 */ 
void test1_case0_task0(void) // RUN AT MEDIUM PRIORITY
{
//  *      Case 0: test that when mbx full, premption occurs 
    int res1, res2, res3, current_mbx_size;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 0; 
    task_t curr_tid;
    task_t curr_tid1;
    SNDR_HDR * sndr_hdr; 


    U8 * recv_bufs[num_blocked_tasks];
    U8 * send_buf_arr[num_blocked_tasks];
    void ** exp_buf_arr;
    BOOL valid1 = TRUE;
    BOOL valid2 = TRUE;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    mbx_create(GET_BUFSIZE(G_MSG_LEN) * 5); // mbx can receive 5 msgs + 5 headers 
    
    // Case 0 
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    // fill up mbx first 
    for (int i =0; i<5; i++){ // 5 is size of mbx
        RTX_MSG_HDR * buf = create_message(curr_tid, g_msg_generic, G_MSG_LEN); // send to ourselves
        res1 = send_msg(curr_tid, buf); // self is recieving
        if (res1 != RTX_OK){
            printf("FAILED send_msg_nb %d\r\n", res1);
            exit();
        }
    }    // Mbx is now full 

    current_mbx_size = mbx_get(curr_tid);  // should be 5 * GET_BUFSIZE(G_MSG_LEN)
    printf("\r\nCurrent space in  msgs %d \r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));    

    send_five_msgs(&sndr_hdr, send_buf_arr, curr_tid, test_index, TRUE); // send  5 msgs that will be blocked 
    // 5 msgs in mbx, 5 pending messages: 3 high  2 low priority 

    // unblock 5 blocked sending threads by emptying the mbx 
    for (int i =0; i<num_blocked_tasks; i++){   // recieve first 5 generic messages 
    current_mbx_size = mbx_get(curr_tid);  
    printf("Current space in msgs %d\r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));
        void * buf = mem_alloc(GET_BUFSIZE(G_MSG_LEN));
        res2 = recv_msg(buf, GET_BUFSIZE(G_MSG_LEN));   
        printf("recv_buf %s,  %d   %d\r\n", (U8*) buf + sizeof(RTX_MSG_HDR), res2 , errno);
        mem_dealloc(buf);
        if (res2 != RTX_OK){
            printf("FAILED recv_msg %d\r\n", res1);
            exit();
        }
    }

    
    current_mbx_size = mbx_get(curr_tid);  // should be 3 * GET_BUFSIZE(G_MSG_LEN)
    printf("Current space in msgs %d\r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));

    // execute two medium tsks
    *(sndr_hdr->curr_priority) = MEDIUM;
    tsk_yield();    // should push current task back to MED queue
    tsk_yield();    // push 1st MED task to execute 
    tsk_yield();    // push 2nd MED task to execute
    
    current_mbx_size = mbx_get(curr_tid);  // should be 0
    printf("Current space in msgs %d\r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));
    
    // empty the mailbox
    for (int i =0; i<num_blocked_tasks; i++){
        // allocate recv buffers 
        recv_bufs[i] = mem_alloc(GET_BUFSIZE(G_MSG_LEN));
        res2 = recv_msg(recv_bufs[i], GET_BUFSIZE(G_MSG_LEN));
				
        printf("NEW recv %s\r\n", recv_bufs[i] + sizeof(RTX_MSG_HDR));
        if (res2 != RTX_OK){
            printf("FAILED recv_msg %d\r\n", res1);
            exit();
        }
    }

    // generate expected values 
    exp_buf_arr = generate_five_expected_rcv(); // allocates buffer in function

    // check order of msgs
    for (int i =0; i<sndr_hdr->len_arr; i++){
        printf("ORDER %d GOT %s EXPECTED %s\r\n", i+1, recv_bufs[i]+sizeof(RTX_MSG_HDR), (U8*) exp_buf_arr[i]+sizeof(RTX_MSG_HDR));
				
        RTX_MSG_HDR * rmh, * rb;
        rmh = exp_buf_arr[i];
        rb = (RTX_MSG_HDR*) recv_bufs[i];
        printf("ORDER %d GOT SENDER %d EXPECTED %d\r\n", i+1, rb->sender_tid, rmh->sender_tid);

        if (mem_util_cmp((void *) exp_buf_arr[i], (void*) recv_bufs[i], GET_BUFSIZE(G_MSG_LEN)) != 0){
            valid2 = FALSE;
            printf("FAILED did not recive expected value\r\n");
            exit();
        }
    }
    
    // make sure expected == actual
    if (valid1 && valid2){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    // deallocate structures
    for (int i =0; i<sndr_hdr->len_arr; i++){
        mem_dealloc(exp_buf_arr[i]);
        mem_dealloc(recv_bufs[i]);
    }

    mem_dealloc(sndr_hdr->curr_priority);
    mem_dealloc(sndr_hdr->sender_tid_arr);
    mem_dealloc(sndr_hdr->prio_arr);
    mem_dealloc(sndr_hdr);
    mem_dealloc(exp_buf_arr);
    tsk_exit();
}


void test1_case0_task1(void)
{
    int res1, res2, res3;
    int task_id = 1;
    int test_index = 1;
    int case_num = 0;
    RTX_MSG_HDR * buffer1;
    SNDR_HDR * sndr_hdr;
    U8 * send_buf;

    task_t curr_tid, rcv_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    // printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    sndr_hdr = (SNDR_HDR *) g_student_tsk_cases[test_index].data; 
    
    // add to the message buffer for comparision latter 
    int msg_ind =0;
    if (*(sndr_hdr->curr_priority) == HIGH){
        // there is is two medium task, it will be msg1
        msg_ind = sndr_hdr->len_arr+2; // high priority tasks start from msg3 
    } else if (*(sndr_hdr->curr_priority) == MEDIUM){
        U32 num_tasks = 0;
        // check if a medium task already exists, 
        for (int i =0; i<sndr_hdr->max_len; i++){
            if (sndr_hdr->prio_arr[i] == MEDIUM){
                num_tasks ++;
                break;
            }
        }
        msg_ind = num_tasks;
    }

    sndr_hdr->sender_tid_arr[sndr_hdr->len_arr] = curr_tid;
    sndr_hdr->prio_arr[sndr_hdr->len_arr] = *(sndr_hdr->curr_priority);
    sndr_hdr->len_arr ++;

    // create the required message
    printf("Sending message %d\r\n", curr_tid);
    char * msg = get_g_msg_from_index(msg_ind);
    send_buf = ((U8*)create_message(curr_tid, msg, G_MSG_LEN)); 
    res1 = send_msg(sndr_hdr->recv_tid, send_buf);
    printf("Returning to message thread %d\r\n", curr_tid);
    tsk_exit();
}


void test1_case1_task0(void) // RUN AT MEDIUM PRIORITY
{
//  *      Case 1: mbx_rmv() preempts properly w empty mbx and blocked tsks 
    int res1, res2, res3, current_mbx_size;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 1; 
    task_t curr_tid;
    SNDR_HDR * sndr_hdr; 

    U8 * recv_bufs[num_blocked_tasks];
    U8 * send_buf_arr[num_blocked_tasks];
    void ** exp_buf_arr = mem_alloc(sizeof(void*));
    BOOL valid1 = TRUE;
    BOOL valid2 = FALSE;


    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    mbx_create(GET_BUFSIZE(G_MSG_LEN) * 5); // mbx can receive 5 msgs + 5 headers 
    
    // Case 0 
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    // fill up mbx first 
    for (int i =0; i<5; i++){ // 5 is size of mbx
        RTX_MSG_HDR * buf = create_message(curr_tid, g_msg_generic, G_MSG_LEN); // send to ourselves
        res1 = send_msg(curr_tid, buf); // self is recieving
        if (res1 != RTX_OK){
            printf("FAILED send_msg_nb %d\r\n", res1);
            exit();
        }
    }    // Mbx is now full 

    current_mbx_size = mbx_get(curr_tid);  // should be 5 * GET_BUFSIZE(G_MSG_LEN)
    printf("\r\nCurrent space in  msgs %d \r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));    

    send_five_msgs(&sndr_hdr, send_buf_arr, curr_tid, test_index, FALSE); // send  5 msgs that will be blocked 
    
    *(sndr_hdr->curr_priority) = MEDIUM;
    // execute two medium tsks
    tsk_yield();
    tsk_yield();
    tsk_yield();
		
    // unblock all threads
    mbx_rmv();
		
    int exp_arr[5];
    
    exp_arr[0] = sndr_hdr->sender_tid_arr[0];
    exp_arr[1] = sndr_hdr->sender_tid_arr[1];
    exp_arr[2] = sndr_hdr->sender_tid_arr[4];
    exp_arr[3] = sndr_hdr->sender_tid_arr[2];
    exp_arr[4] = sndr_hdr->sender_tid_arr[3];

    // check order of msgs
    for (int i =0; i<sndr_hdr->len_arr; i++){
        if (sndr_hdr->blocked_arr[i] != exp_arr[i]){
            valid2 = TRUE;
        } 
    }

    tsk_yield();

    exp_arr[0] = sndr_hdr->sender_tid_arr[0];
    exp_arr[1] = sndr_hdr->sender_tid_arr[1];
    exp_arr[2] = sndr_hdr->sender_tid_arr[4];
    exp_arr[3] = sndr_hdr->sender_tid_arr[2];
    exp_arr[4] = sndr_hdr->sender_tid_arr[3];
    
    
    // check order of msgs
    for (int i =0; i<sndr_hdr->len_arr; i++){
        printf("ind %d GOT %d  EXP %d \r\n", i, sndr_hdr->blocked_arr[i], exp_arr[i]);
        if (sndr_hdr->blocked_arr[i] != exp_arr[i]){
            valid1 = FALSE;
        } 
    }
    
    if (valid1 && valid2){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    // deallocate structures    
    mem_dealloc(sndr_hdr->blocked_arr);
    mem_dealloc(sndr_hdr->prio_arr);
    mem_dealloc(sndr_hdr->sender_tid_arr);
    mem_dealloc(sndr_hdr->curr_priority);
    mem_dealloc(sndr_hdr);
    mem_dealloc(exp_buf_arr);

    tsk_yield(); // yeild so rcving thread can terminates
    tsk_exit();
}


void test1_case1_task1(void)
{
    int res1, res2, res3;
    int task_id = 1;
    int test_index = 1;
    int case_num = 0;
    RTX_MSG_HDR * buffer1;
    SNDR_HDR * sndr_hdr;
    U8 * send_buf;

    task_t curr_tid, rcv_tid;
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    sndr_hdr = (SNDR_HDR *) g_student_tsk_cases[test_index].data; 
    
    // add to the message buffer for comparision latter 
    int msg_ind =0;
    if (*(sndr_hdr->curr_priority) == HIGH){
        // there is is two medium task, it will be msg1
        printf("HIGH\r\n");
        msg_ind = sndr_hdr->len_arr+2; // high priority tasks start from msg3 
    } else if (*(sndr_hdr->curr_priority) == MEDIUM){
        printf("MEDIUM\r\n");
        U32 num_tasks = 0;
        // check if a medium task already exists, 
        for (int i =0; i<sndr_hdr->max_len; i++){
            if (sndr_hdr->prio_arr[i] == MEDIUM){
                num_tasks ++;
                break;
            }
        }
        msg_ind = num_tasks;
    }

    sndr_hdr->prio_arr[sndr_hdr->len_arr] = *(sndr_hdr->curr_priority);
    sndr_hdr->len_arr ++;
    sndr_hdr->blocked_arr[msg_ind] = -1;    // indicate task blocked
    sndr_hdr->sender_tid_arr[sndr_hdr->len_arr] = curr_tid;

    // create the required message
    printf("Sending message %d\r\n", curr_tid);
    char * msg = get_g_msg_from_index(msg_ind);
    send_buf = ((U8*)create_message(curr_tid, msg, G_MSG_LEN)); 
    res1 = send_msg(sndr_hdr->recv_tid, send_buf);
    printf("Returning to message thread %d\r\n", curr_tid);
    mem_dealloc(send_buf);  // we dont care about the message
    printf("putting %d here %d\r\n",curr_tid, sndr_hdr->len_arr-1);
    sndr_hdr->blocked_arr[sndr_hdr->data] = curr_tid;    // indicate task blocked

    sndr_hdr->data +=1;
    tsk_exit();
}

// CASE 2
void test1_case2_task0(void) // RUN AT MEDIUM 
{
//  *      Case 2: tsk_exit() preempts properly w empty mbx and blocked tsks
    int res1, res2, res3, current_mbx_size;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 2; 
    task_t curr_tid, task1_tid;
    SNDR_HDR * sndr_hdr; 
    BOOL valid1 = TRUE;
    BOOL valid2 = TRUE;
    
    U8 * recv_bufs[num_blocked_tasks];
    U8 * send_buf_arr[num_blocked_tasks];
    void ** exp_buf_arr;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    mbx_create(GET_BUFSIZE(G_MSG_LEN) * 5); // mbx can receive 5 msgs + 5 headers 
    
    // Case 0 
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    // fill up mbx first 
    for (int i =0; i<5; i++){ // 5 is size of mbx
        RTX_MSG_HDR * buf = create_message(curr_tid, g_msg_generic, G_MSG_LEN); // send to ourselves
        res1 = send_msg(curr_tid, buf); // self is recieving
        if (res1 != RTX_OK){
            printf("FAILED send_msg_nb %d\r\n", res1);
            exit();
        }
    }    // Mbx is now full 

    current_mbx_size = mbx_get(curr_tid);  // should be 5 * GET_BUFSIZE(G_MSG_LEN)
    printf("\r\nCurrent space in  msgs %d \r\n", current_mbx_size / GET_BUFSIZE(G_MSG_LEN));    

    send_five_msgs(&sndr_hdr, send_buf_arr, curr_tid, test_index, FALSE); // send  5 msgs that will be blocked 

    g_student_tsk_cases[test_index].data = (U32) sndr_hdr;
    
    // execute two medium tsks
    tsk_yield();
    tsk_yield();
    tsk_yield();
    
	
    // task that tests remaining blocked functions were unblocked 
    //  and that they execute in the correct order 
    tsk_create(&task1_tid, &test1_case2_task1, MEDIUM, 0x200);

    // unblock all threads
    tsk_exit();
}


void test1_case2_task1(void)
{
    int res1, res2, res3, current_mbx_size;
    int task_id = 0;
    int test_index = 1; 
    int case_num = 2; 
    task_t curr_tid, task1;
    SNDR_HDR * sndr_hdr; 
    BOOL valid1 = TRUE;
    BOOL valid2 = FALSE;
	
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
        

    int exp_arr[5];

    exp_arr[0] = sndr_hdr->sender_tid_arr[0];
    exp_arr[1] = sndr_hdr->sender_tid_arr[1];
    exp_arr[2] = sndr_hdr->sender_tid_arr[4];
    exp_arr[3] = sndr_hdr->sender_tid_arr[2];
    exp_arr[4] = sndr_hdr->sender_tid_arr[3];

    sndr_hdr = (SNDR_HDR *) g_student_tsk_cases[test_index].data;

    // check order of msgs
    for (int i =0; i<sndr_hdr->len_arr; i++){
        if (sndr_hdr->blocked_arr[i] != exp_arr[i]){
            valid2 = TRUE;
        }
    }

    tsk_yield();
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    exp_arr[0] = sndr_hdr->sender_tid_arr[0];
    exp_arr[1] = sndr_hdr->sender_tid_arr[1];
    exp_arr[2] = sndr_hdr->sender_tid_arr[4];
    exp_arr[3] = sndr_hdr->sender_tid_arr[2];
    exp_arr[4] = sndr_hdr->sender_tid_arr[3];
    
    // check order of msgs
    for (int i =0; i<sndr_hdr->len_arr; i++){
        printf("ind %d GOT %d  EXP %d\r\n", i, sndr_hdr->blocked_arr[i], exp_arr[i]);
        if (sndr_hdr->blocked_arr[i] != exp_arr[i]){
            valid1 = FALSE;
        }
    }
    
    if (valid1 && valid2){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    // deallocate structures    
    mem_dealloc(sndr_hdr->blocked_arr);
    mem_dealloc(sndr_hdr->prio_arr);
    mem_dealloc(sndr_hdr->sender_tid_arr);
    mem_dealloc(sndr_hdr->curr_priority);

    tsk_yield(); // yeild so rcving thread can terminates
    tsk_exit();
}



void test1_case3_task0(void)
{
    int test_index = 1; 
    int case_num = 3; 
    int task_id = 0;

    task_t curr_tid, tid1;
    ARR_N_CNTR * arr_n_cntr; 
	
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    arr_n_cntr = (ARR_N_CNTR*) g_student_tsk_cases[test_index].data;

    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);

    arr_n_cntr->arr = (task_t *) arr_n_cntr->arr;
    
    for (int i=0; i<arr_n_cntr->max_len; i++){
        ((task_t*) arr_n_cntr->arr)[i] = 0;
    }

    arr_n_cntr->len = 0;
    
    ((task_t*) arr_n_cntr->arr)[arr_n_cntr->len] = 1;
    arr_n_cntr->len ++;

    // printf("1->\r\n");
    tsk_create(&tid1, &test1_case3_task1, arr_n_cntr->data, 0x200);
    tsk_yield();

    // printf("3->\r\n");
    ((task_t*) arr_n_cntr->arr)[arr_n_cntr->len] = 3;
    arr_n_cntr->len ++;

    tsk_yield();
    
    // printf("5->\r\n");
    ((task_t*) arr_n_cntr->arr)[arr_n_cntr->len] = 5;

    arr_n_cntr->len ++;


    U32 * exp_arr = generate_expected_test1_case3();

    BOOL valid = TRUE;
    for (int i =0; i<arr_n_cntr->max_len; i++){
        if (exp_arr[i] != ((task_t*) arr_n_cntr->arr)[i]){
            valid = FALSE;
            break;
        }
    }


    printf("GOT      :");
    for (int i=0 ; i<5; i++){
        U32 d = (U32) (((task_t*) arr_n_cntr->arr)[i]);
        printf("%d->", d);
    }
    printf("\r\n");

    printf("EXPECTED :");
    print_arr(exp_arr, arr_n_cntr->max_len);
    printf("\r\n");

    if (valid){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    
    mem_dealloc(exp_arr);
    tsk_yield();

    tsk_exit();
}

void test1_case3_task1(void)
{
    int test_index = 1; 
    int case_num = 3; 
    int task_id = 1;

    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    task_t curr_tid, task1;
    ARR_N_CNTR * arr_n_cntr; 
	
    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);

    arr_n_cntr = (ARR_N_CNTR*) g_student_tsk_cases[test_index].data;
    
    ((task_t*) arr_n_cntr->arr)[arr_n_cntr->len] = 2;

    arr_n_cntr->len++;

    // printf("2->\r\n");
    tsk_yield();

    // printf("4->\r\n");
    
    ((task_t*) arr_n_cntr->arr)[arr_n_cntr->len] = 4;

    arr_n_cntr->len++;

    tsk_exit();
}
        