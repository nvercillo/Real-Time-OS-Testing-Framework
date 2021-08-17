#include "suite3_tasks.h"

/* 
Below shows the order of execution of tsks 
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 
                      2       2       2         2     
  3           3           3               3       
  5                               5     
  8                                                 8
*/ 
void gen_expected_val_test1(U8 *** p_exp_arr)
{

    (*p_exp_arr)[0]  =  get_g_msg_from_index(30);
    (*p_exp_arr)[1]  =  get_g_msg_from_index(50);   
    (*p_exp_arr)[2]  =  get_g_msg_from_index(80);
    (*p_exp_arr)[3]  =  get_g_msg_from_index(30);
    (*p_exp_arr)[4]  =  get_g_msg_from_index(20);
    (*p_exp_arr)[5]  =  get_g_msg_from_index(40);
    (*p_exp_arr)[6]  =  get_g_msg_from_index(50);
    (*p_exp_arr)[7]  =  get_g_msg_from_index(20);
    (*p_exp_arr)[8]  =  get_g_msg_from_index(30);
    (*p_exp_arr)[9]  =  get_g_msg_from_index(20);
    (*p_exp_arr)[10] =  get_g_msg_from_index(40);
    (*p_exp_arr)[11] =  get_g_msg_from_index(80);
    (*p_exp_arr)[12] =  get_g_msg_from_index(30);
    (*p_exp_arr)[13] =  get_g_msg_from_index(20);

}


/*
 *===========================================================================
 *                             STATIC HELPER FUNCTIONS 
 *===========================================================================
 */

// needs to be double digit to have proper length
static char * get_g_msg_from_index(int i)
{
    char * msg = mem_alloc(sizeof(G_MSG_LEN));
    sprintf(msg, "This is Message %d!", i);

    return msg;
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

static void add_tv(TIMEVAL * res, TIMEVAL * tv1, TIMEVAL * tv2)
{    
    unsigned long long sum = (tv1->sec *1000000 + tv1->usec) + (tv2->sec *1000000 + tv2->usec);

    res->usec = sum % 1000000;
    res->sec = sum / 1000000;
}


static void calc_period(TIMEVAL *tv, int size)
{   
    int res;
    res = RTX_TICK_SIZE * MIN_PERIOD * size;
    // printf("res %d %d %d \r\n\r\n",res, (U32) res %1000000, (U32) res / 1000000);
    tv->usec = res % 1000000;
    tv->sec = res / 1000000;
}

/*
 *===========================================================================
 *                             TEST TASKS 
 *===========================================================================
 */

// TODO:  make get EINVAL case 
/* TEST 0 */ 
void test0_case0_task0(void)
{
    TIMEVAL * tv_get, * tv_set ;
    int set_res, get_res, set_get_cmp_res, susp_res; 
    int invalid_get_res, invalid_set_res, invalid_set_res1, susp_eperm;
    
    int task_id = 0;
    int test_index = 0; 
    int case_num = 0; 
    // task_t tasks [MAX_TASKS];
    task_t curr_tid;

    curr_tid = tsk_gettid();
    U8  *p_index = &(g_ae_student_xtest.index);
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    
		
    // Case 0: test that preemption for RTT occurs for MED tsk 
    printf("RUNNING TEST %d, CASE %d, TASK %d\r\n", (int)test_index, (int) case_num, (int) task_id);
    
    tv_get = mem_alloc(sizeof(TIMEVAL));
    tv_set = mem_alloc(sizeof(TIMEVAL));

    sndr_hdr->sender_tid_arr[sndr_hdr->len_arr] = curr_tid;
    sndr_hdr->len_arr ++;
    
    susp_res = rt_tsk_susp();
    susp_eperm = errno;

    // attempt to set not a multiple of TICK_SZ * MIN_PER
    tv_set->sec = 0;
    tv_set->usec = 123; 
    invalid_set_res1 = rt_tsk_set(tv_set);
    
    int einval_set = errno;

    calc_period(tv_set, 10); 

    set_res = rt_tsk_set(tv_set);

    // attempt to set already RT task
    invalid_set_res = rt_tsk_set(tv_set);
    int eperm_set = errno;

    // attempt to get no RTT 
    invalid_get_res = rt_tsk_get(g_student_priviledged_task, tv_get);
    int eninv_get = errno;
    
    get_res = rt_tsk_get(curr_tid, tv_get);

    set_get_cmp_res = cmp_timeval(tv_get, tv_set);
    print_tv(tv_get);
    print_tv(tv_set);
    
    if (
        susp_res == RTX_ERR
        && susp_eperm == EPERM 
        && invalid_set_res1 == RTX_ERR
        && einval_set == EINVAL
        && set_res == RTX_OK
        && invalid_set_res == RTX_ERR
        && eperm_set == EPERM
        && invalid_get_res == RTX_ERR
        && eninv_get == EINVAL    
        && set_get_cmp_res == 0
    ){
        MARK_TEST_PASSED(test_index, *p_index);  
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;

    mem_dealloc(tv_set);
    mem_dealloc(tv_get);

    tsk_exit();
}

void recieving_mbx(void)
{

    int test_index = 1;
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    U8  *p_index = &(g_ae_student_xtest.index);
    BOOL valid;

    task_t curr_tid = tsk_gettid();
    PAIR * pair;

    pair = (PAIR*)sndr_hdr->data;
    pair->A = curr_tid;
    
    // create space
    mbx_create(GET_BUFSIZE(G_MSG_LEN) * sndr_hdr->max_len); // Plus one is for coordination


    // allocate buffer space
    U8 ** buf_arr = mem_alloc(sizeof(U8*) * sndr_hdr->max_len);
    U8 ** exp_arr = mem_alloc(sizeof(U8*) * sndr_hdr->max_len);
    
    printf("Running recv\r\n");
    tsk_yield(); // return to parent task

    // wait until signaled by parent that its safe to proceed
    while (!pair->B){
        // artificial delay
        printf("waiting\r\n");
        for (int i =0; i< 1000000; i++){} // add delay so context switching isnt that expensive 

        tsk_yield();
    }

    printf("Remaining space left in recv mbx: %d \r\n", mbx_get(curr_tid));

    // assess order of execution
    for (int i =0; i<sndr_hdr->max_len; i++){ // recall fifo
        int res1;

        buf_arr[i] = mem_alloc(GET_BUFSIZE(G_MSG_LEN)); 

        mem_util_zero(buf_arr[i], GET_BUFSIZE(G_MSG_LEN));
 
        res1 = recv_msg_nb(buf_arr[i], GET_BUFSIZE(G_MSG_LEN)); // sndr should have run to completion
        
        if (res1 == RTX_ERR){ printf("RECV TEST FAILURE %d\r\n", errno); exit(1);}
    }
    
    printf("Returning to recv\r\n");
    gen_expected_val_test1((U8***)&exp_arr);

    printf("\r\nGOT: \r\n");
    
    for (int i =0; i<sndr_hdr->max_len; i++){
        printf("%s\r\n", buf_arr[i] +  sizeof(RTX_MSG_HDR));
    }
    printf("\r\n");

    printf("EXPECTED :\r\n");
    for (int i =0; i<sndr_hdr->max_len; i++){
        printf("%s\r\n", exp_arr[i]);
    }
    printf("\r\n");

    
    valid = TRUE;
    // compare expected array vs recieved array
    for (int i =0; i<sndr_hdr->max_len; i++){
        if (mem_util_cmp(exp_arr[i], buf_arr[i] +  sizeof(RTX_MSG_HDR), G_MSG_LEN) != 0){
            valid = FALSE;
            break;
        }
    }    

    if (valid){
        MARK_TEST_PASSED(test_index, *p_index);
    } else {
        MARK_TEST_FAILED(test_index, *p_index);
    }
    (*p_index)++;
    

    // memory deallocation
    for (int i =0; i<sndr_hdr->max_len; i++){ // recall fifo
        mem_dealloc(exp_arr[i]);
        mem_dealloc(buf_arr[i]);
    }
    mem_dealloc(exp_arr);
    mem_dealloc(buf_arr);

    tsk_exit();
}

void mk_task_tq_2(void)
{

    int period = 20;
    int num_susp = 3;
    int test_index = 1;
    int res;
    task_t curr_tid, child_tid;

    curr_tid = tsk_gettid();
    U8 ** send_buf_arr = mem_alloc(sizeof(U8*) *(num_susp +1));

    TIMEVAL * tv_set ;
    int set_res; 
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    PAIR * pair = (PAIR *) sndr_hdr->data;

    TIMEVAL ** tv_arr = (TIMEVAL**) pair->C;

    // allocate memory relevant for task
    tv_set = mem_alloc(sizeof(TIMEVAL));

    for (int i=0; i < num_susp + 1; i++){
        send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
    }

    
    calc_period(tv_set, period);

    TIMEVAL * to_beat = mem_alloc(sizeof(TIMEVAL));
    TIMEVAL * tv_res = mem_alloc(sizeof(TIMEVAL));
    subtract_tv(to_beat, tv_arr[1], tv_arr[0]);
    tv_res->sec = 0;
    tv_res->usec = 0; 

    while(cmp_timeval(tv_res, to_beat) == -1){
        tim_get(tv_arr[3]); // NOTICE this is before rt_tsk_set 
        
        subtract_tv(tv_res, tv_arr[3], tv_arr[2]);

        for(int i=0; i<10000; i++){} //artifical delay
    }
    
    mem_dealloc(to_beat);
    mem_dealloc(tv_res);

    set_res = rt_tsk_set(tv_set);

    // CREATE   
    tsk_create(&child_tid, &mk_task_tq_4, HIGH, 0x200);

    for (int i =0; i< num_susp; i++){
        // send to designated recieving thread sndr_hdr->data
        res = send_msg_nb((task_t) (pair->A), send_buf_arr[i]); 
        printf("Sending msg %d, res %d\r\n", period, res);
        rt_tsk_susp();
    }

    res = send_msg_nb((pair->A), send_buf_arr[num_susp]); 
    printf("Sending msg %d, res %d\r\n", period, res);
			 
    mem_dealloc(tv_set);

    // printf("Remaining space left in mbx: %d \r\n", mbx_get(sndr_hdr->recv_tid));
    U8 * send_buf = ((U8*)create_message(sndr_hdr->recv_tid, g_msg1, G_MSG_LEN)); 
    res = send_msg_nb(sndr_hdr->recv_tid, send_buf);
    // printf("Exiting period %d, res %d\r\n", period, res);
    
		
    tsk_exit();
}

void mk_task_tq_3(void)
{
    int period = 30;
    int res;
    int num_susp = 3;

    int test_index = 1; 

    task_t curr_tid, child_tid;

    curr_tid = tsk_gettid();
    
    U8 ** send_buf_arr = mem_alloc(sizeof(U8*) *( num_susp +1));
    
    TIMEVAL * tv_set;
    int set_res; 
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    PAIR * pair = (PAIR *) sndr_hdr->data;
    TIMEVAL ** tv_arr = (TIMEVAL **) pair->C; 

    // allocate memory relevant for task
    tv_set = mem_alloc(sizeof(TIMEVAL));

    for (int i=0; i < num_susp + 1; i++){
        send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
    }

    calc_period(tv_set, period);
    
    tim_get(tv_arr[0]); // NOTICE this is before rt_tsk_set 
    
    set_res = rt_tsk_set(tv_set);

    // send to designated recieving thread sndr_hdr->data
    res = send_msg_nb((task_t) (pair->A), send_buf_arr[0]); 
    printf("Sending msg %d, res %d \r\n", period, res);
    if (res == RTX_ERR){ printf("SEND TEST FAILURE"); exit(1);}
    rt_tsk_susp();

    // send to designated recieving thread sndr_hdr->data
    res = send_msg_nb((task_t) (pair->A), send_buf_arr[1]); 
    printf("Sending msg %d, res %d \r\n", period, res);
    if (res == RTX_ERR){ printf("SEND TEST FAILURE"); exit(1);}

    // CREATE mk_task_tq_2
    tim_get(tv_arr[2]); // NOTICE this is before rt_tsk_set 

    tsk_create(&child_tid, &mk_task_tq_2, HIGH, 0x200);
    rt_tsk_susp();
    

    // send to designated recieving thread sndr_hdr->data
    res = send_msg_nb((task_t) (pair->A), send_buf_arr[2]); 
    printf("Sending msg %d, res %d \r\n", period, res);
    if (res == RTX_ERR){ printf("SEND TEST FAILURE"); exit(1);}
    rt_tsk_susp();


    res = send_msg_nb((pair->A), send_buf_arr[3]);
    printf("Sending msg %d, res %d\r\n", period, res);
    
    mem_dealloc(tv_set);

    // printf("Remaining space left in mbx: %d \r\n", mbx_get(sndr_hdr->recv_tid));
    U8 * send_buf = ((U8*)create_message(sndr_hdr->recv_tid, g_msg1, G_MSG_LEN)); 
    res = send_msg_nb(sndr_hdr->recv_tid, send_buf);
    // printf("Exiting period %d, res %d\r\n", period, res);
    
    tsk_exit();
}


void mk_task_tq_4(void)
{
    int period = 40;
    int res;
    int num_susp = 1;
    int test_index = 1;
    task_t child_tid, curr_tid;

    curr_tid = tsk_gettid();
    U8 ** send_buf_arr = mem_alloc(sizeof(U8*) *(num_susp +1));
    
    TIMEVAL * tv_set;
    int set_res; 
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    PAIR * pair = (PAIR *) sndr_hdr->data;

    // allocate memory relevant for task
    tv_set = mem_alloc(sizeof(TIMEVAL));

    for (int i=0; i < num_susp + 1; i++){
        send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
    }

    calc_period(tv_set, period);
    set_res = rt_tsk_set(tv_set);

    for (int i =0; i< num_susp; i++){
        // send to designated recieving thread sndr_hdr->data
        res = send_msg_nb((task_t) (pair->A), send_buf_arr[i]); 
        printf("Sending msg %d, res %d\r\n", period, res);
        rt_tsk_susp();
    }

    res = send_msg_nb((pair->A), send_buf_arr[num_susp]); 
    printf("Sending msg %d, res %d\r\n", period, res);

    mem_dealloc(tv_set);

    // printf("Remaining space left in mbx: %d \r\n", mbx_get(sndr_hdr->recv_tid));
    U8 * send_buf = ((U8*)create_message(sndr_hdr->recv_tid, g_msg1, G_MSG_LEN)); 
    res = send_msg_nb(sndr_hdr->recv_tid, send_buf);
    // printf("Exiting period %d, res %d\r\n", period, res);
    

    tsk_exit();

}

void mk_task_tq_5(void)
{

    int period = 50;
    int res;
    int num_susp = 1;
    int test_index = 1;

    task_t curr_tid = tsk_gettid();
    U8 ** send_buf_arr = mem_alloc(sizeof(U8*) *(num_susp +1));
    
    TIMEVAL * tv_set ;
    int set_res; 
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;
    PAIR * pair = (PAIR *) sndr_hdr->data;
    TIMEVAL ** tv_arr = (TIMEVAL **) pair->C; 

    // allocate memory relevant for task
    tv_set = mem_alloc(sizeof(TIMEVAL));

    for (int i=0; i < num_susp + 1; i++){
        send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
    }

    calc_period(tv_set, period);
    set_res = rt_tsk_set(tv_set);
    
    // NOTICE this is before rt_tsk_set 
    tim_get(tv_arr[1]);
    

    for (int i =0; i< num_susp; i++){
        // send to designated recieving thread sndr_hdr->data
        res = send_msg_nb((task_t) (pair->A), send_buf_arr[i]); 
        printf("Sending msg %d, res %d\r\n", period, res);
        rt_tsk_susp();
    }

    res = send_msg_nb((pair->A), send_buf_arr[num_susp]); 
    printf("Sending msg %d, res %d\r\n", period, res);
			 
    mem_dealloc(tv_set);

    // printf("Remaining space left in mbx: %d \r\n", mbx_get(sndr_hdr->recv_tid));
    U8 * send_buf = ((U8*)create_message(sndr_hdr->recv_tid, g_msg1, G_MSG_LEN)); 
    res = send_msg_nb(sndr_hdr->recv_tid, send_buf);
    // printf("Exiting period %d, res %d\r\n", period, res);
    
    tsk_exit();
}

void mk_task_tq_8(void)
{
    int period = 80;
    int res;
    int num_susp = 1;
    int test_index = 1;

    task_t curr_tid = tsk_gettid();
    // printf("RUNNING 8 ,  tid  %d \r\n", curr_tid);
    
    U8 ** send_buf_arr = mem_alloc(sizeof(U8*) *(num_susp+ 1));
    
    TIMEVAL * tv_set ;
    int set_res; 
    SNDR_HDR * sndr_hdr = (SNDR_HDR*) g_student_tsk_cases[test_index].data;

    PAIR * pair = (PAIR *) sndr_hdr->data;

    // allocate memory relevant for task
    tv_set = mem_alloc(sizeof(TIMEVAL));
    
    for (int i=0; i < num_susp + 1; i++){
        send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
    }

    calc_period(tv_set, period);
    set_res = rt_tsk_set(tv_set);

    for (int i =0; i< num_susp; i++){
        // send to designated recieving thread sndr_hdr->data
        // send_buf_arr[i] = ((U8*) create_message(curr_tid, get_g_msg_from_index(period), G_MSG_LEN));
        res = send_msg_nb((task_t) (pair->A), send_buf_arr[i]); 
        printf("Sending msg %d, res %d\r\n", period, res);
        rt_tsk_susp();
    }
    
    // send_buf_arr[num_susp] = ((U8*) create_message(curr_tid, g_msg1, G_MSG_LEN));
    res = send_msg_nb(pair->A, send_buf_arr[num_susp]);
    printf("Sending msg %d, res %d\r\n", period, res);
    
    mem_dealloc(tv_set);

    // printf("Remaining space left in mbx: %d \r\n", mbx_get(sndr_hdr->recv_tid));
    U8 * send_buf = ((U8*)create_message(sndr_hdr->recv_tid, g_msg1, G_MSG_LEN)); 
    res = send_msg_nb(sndr_hdr->recv_tid, send_buf);
    // printf("Exiting period %d, res %d\r\n", period, res);

    tsk_exit();
}