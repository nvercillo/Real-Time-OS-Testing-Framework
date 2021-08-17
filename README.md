# Real-Time-OS-Testing-Framework


Devised testing framework for lowest level operating system. Writing over 4000 lines of modular C, tackled the task of
testing the nondeterministic nature of the real time EDF scheduler we implemented, in addition to testing the thread
creation and inter-thread communication and signaling




We can see an image below of how the scheduler is intended to work:
![Shceduler Image](https://i.stack.imgur.com/kk7kg.png)
Test Descriptions 

 

#Tests: #

#Test Suite 1: #

IN ORDER TO RUN THIS TEST,    SUITE1_TASKS.C   AND    STUDDENT_INC.C NEED TO BE MOVED INTO THE ACTIVE FOLDER IN ORDER TO AVOID MULTIPLICITY OF FN DEFS. 

 
## int start_test0 () ## 
Description:  Test the full functionality of mbx_get. 
Test successful case of mbx_get 

* Case 0: creates task with mailbox greater than 0 bytes, expects to receive mailbox for task 
 **Test error cases of mbx_get. In all the below cases RTX_ERROR is returned and ERRNO is set** 
* Case 1: attempts to receive mailbox for a task with no mailbox and expect correct error 
* Case 2: Creates mailbox for previously erroneous task and tests successful mailbox creation and message sending to self, then checks to see if mailbox size has changed 
* Case 3: attempts to get mailbox with invalid tid, expects correct error 
* Case 4: attempts to get mailbox for a task that exists beyond the maximum amount of tasks, expects correct error response 


## int start_test1 () ##

Description:  Test the full functionality of mbx_ls. 
Test successful case of mbx_ls 
* Case 0: creates a task which creates 3 additional tasks with mailboxes, thereby creating 4 tasks other than the default tasks, then uses mbx_ls with a count lower than the amount of running tasks with mailboxes, expects correct number of tasks from mbx_ls 

* Case 1: Building off of case 0, uses mbx_ls with a count greater than the amount of running tasks and expects correct response  

* Case2: Creates two tasks which register mailboxes, then removes mailboxes using mbx_rmv, uses mbx_ls to verify correct number of mailboxes 
**Test error cases of mbx_ls. In all the below cases RTX_ERROR is returned and ERRNO is set**

* Case 3: Attempts using mbx_ls for a null receiving buffer, expects correct error response 

 

## int start_test2 ()## 
Description:  Test the full functionality of send_msg_nb. 
Test successful case of send_msg_nb (same as test3 case 0 and 1) 
* Case 0: push message that fits to mbx with no error to non-blocked receiving task. 

* Case 1: push message that fits to mbx with no error to blocked receiving task. 
**Test error cases of send_msg_nb. In all the below cases RTX_ERROR is returned and ERRNO is set**

* Case 2: buf arg is NULL  

* Case 3: buffer is too small to hold the message  

* Case 4: calling tsk has no mailbox initialized  

* Case 5: no msg when function is called  

## int start_test3 () ## 

Description:  Test the full functionality of recv_msg_nb. 
Test successful case of  
* Case 0: push message that fits to mbx with no error to non-blocked receiving task. 

* Case 1: push message that fits to mbx with no error to blocked receiving task. 
**Test error cases of mbx_get. In all the below cases RTX_ERROR is returned and ERRNO is set** 
 
# Test Suite 2: # 

IN ORDER TO RUN THIS TEST,    SUITE2_TASKS.C   AND    STUDDENT_INC.C NEED TO BE MOVED INTO THE ACTIVE FOLDER IN ORDER TO AVOID MULTIPLICITY OF FN DEFS. 

## int start_test0 () ## 
Description:  Test the full functionality of blocking behaviour 
* Case 0: test that when mbx empty, correct seq of events occurs 

* Case 1: mbx_rmv() preempts properly w empty mbx and blocked tsks 

* Case 2: tsk_exit() preempts properly w empty mbx and blocked tsks 

* Case 3: test that when mbx full, correct seq of events occurs 

* Case 4: mbx_rmv() preempts properly w empty mbx and blocked tsks 

* Case 5: tsk_exit() preempts properly w empty mbx and blocked tsks 

 
# int start_test1 () # 
Description:  
* Case 0: test that when mbx full, premption occurs 

* Case 1: building off of last test check again if mbx_rmv() preempts properly w empty mbx and blocked tsks 

* Case 2: building off of last test check again if tsk_exit() preempts properly w empty mbx and blocked tsks 
 



# Test Suite 3 [SPECIAL INSTRUCTIONS REQUIRED] -  #
IN ORDER TO RUN THIS TEST,    SUITE3_TASKS.C   AND    STUDDENT_INC.C NEED TO BE MOVED INTO THE ACTIVE FOLDER IN ORDER TO AVOID MULTIPLICITY OF FN DEFS. 
## void start_test0(void):  ## 

Description: 

Test that preemption for RTT occurs for MED running NON-RTT tsk 

Test that preemption for RTT occurs for HIGH running NON-RTT tsk 

## void start_test1(void): ##

Description: Test that the EDF scheduler works correctly.  
* Stage 1: Create 3 different tasks, with a period of 30, 50 and 80, created in that order respectively. These tasks suspend 3, 1 and 1 times respectively. Measure the time that it takes for task_50 to become a RT task after task_30 is created. 

* Stage 2: Using blocking receive on the creating privileged task, wait for 5 tasks to terminate. 

* Stage 3: After the second time task_30 runs, create a task of period 20 that is suspended 3 times. Within task_20 wait until the time that 3 ran is greater than the time it took for task_50 to run after the first task_30 to ensure that the deadlines are preserved properly, then create a task with period 40 that suspends 1 time.  

* Stage 4: All 5 threads will send a message to a receiving mbx at each period of its execution, identified by its period. Additionally, before all 5 threads call tsk_exit, they send a message to the original privileged waiting on blocking recv.   

* Stage 6: After all the threads are known to have sent all their messages, yield to the other receiving mbx task, to receive all of the period identified message to asses that the order of execution follows the EDF principles.  
 

#Test Suite 4 [SPECIAL MANUAL TESTING SUITE] - Clock tests:   # 

NOTE: FOLLOW INSTRUCTIONS LISTED IN UART2 ON WHAT TO INPUT INTO UART1 TO PERFORM TEST 

## void task0 (void)   ## 

Description:  Test the full functionality of the wall clock task  

* Stage 1: User is prompted to type in %WR to observe clock appearing 

* Stage 2: User is prompted to type in %WR again to observe clock resetting 

* Stage 3: User is prompted to type in %WS 12:12:12 to observe wall clock setting to time 12:12:12 

* Stage 4: User is prompted to type in %WS 12:12:12 to observe wall clock resetting to time 12:12:12 

* Stage 4: User is prompted to type in %WS 123:123:123 to observe that nothing changes in the clock behaviour 

* Stage 5: User is prompted to type in %WT to observe wall clock disappear 

* Stage 6: User is prompted to type %WR to observe the wall clock appear at time 00:00:00 again 

* Stage 7: User is prompted to type in %WT to observe wall clock disappear 

* Stage 8: User is prompted to type %WS 13:13:13 to observe the wall clock appear at time 13:13:13 

* Stage 9: User is prompted to type %WT to observe wall clock disappear again 

