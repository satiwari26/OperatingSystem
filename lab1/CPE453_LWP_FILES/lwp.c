#include "lwp.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>


ptr_int_t * basePTR;
ptr_int_t * stackPTR;

//temp stack pointer to save the states
ptr_int_t * temp_st_ptr;

ptr_int_t * stackInit;

lwp_context lwp_ptable[LWP_PROC_LIMIT];
int lwp_procs = 0;  //initially set this to 0
int lwp_running = 0;    //initially set to 0

//global pointer that would point to the scheduler function
schedfun scheduler = NULL;

/**
 * @brief
 * round robin schedule algorithm
*/
int roundRobin(void){
    lwp_running = lwp_running == (lwp_procs - 1) ? 0 : lwp_running + 1;

    return lwp_running;
}

int new_lwp(lwpfun func, void * argument, size_t stackSize){

    lwp_procs++;    //increament this
    if(lwp_procs == 1){ //if lwps currently is 1, current running lwp = 0
        lwp_running = 0;
    }
    else{   //else increament this
        lwp_running++;
    }

    lwp_context contextInfo;
    contextInfo.pid = lwp_procs;
    contextInfo.stacksize = stackSize;
    
    //set the base ptr to null initially
    basePTR = NULL;

    //allocating stack for the thread
    stackInit = (ptr_int_t*)malloc(stackSize * sizeof(ptr_int_t));

    contextInfo.stack = (ptr_int_t *)stackInit; //set the stack ptr to current ptr

    stackPTR = stackInit + ((stackSize * sizeof(ptr_int_t)) - 1);   //move the stack pointer to the top of the stack and push content in reverse order

    //pusing the args in the stack
    *stackPTR = (ptr_int_t)argument;

    //move the pointer down
    stackPTR--;

    //push some fake lwp_exit address
    *stackPTR = 0xDEADBEEF;

    //move the pointer down
    stackPTR--;

    //push the function address in the stack
    *stackPTR = (ptr_int_t)func;

    //move the pointer down
    stackPTR--;

    //push the bogus base ptr
    *stackPTR = (ptr_int_t)basePTR;

    // setting base ptr to current stack ptr
    basePTR = stackPTR;

    //leaving the 7 register space in the stack
    stackPTR -= 7;

    //push the current stack pointer value on the stack
    *stackPTR = (ptr_int_t) basePTR;

    //set the current stack ptr in context ptr
    contextInfo.sp = (ptr_int_t *)stackPTR;

    //add the context to lwp_table
    lwp_ptable[lwp_running] = contextInfo;

    return contextInfo.pid;
}

/**
 * @brief
 * start the thread
*/
void lwp_start(){
    lwp_running = 0;    //initially start with the 1st thread
    SAVE_STATE();   //save the real context of the main thread
    GetSP(temp_st_ptr); //save the real stack pointer for later access
    SetSP(lwp_ptable[lwp_running].sp);  //scheduling lwp process to run and switching to its stack
    RESTORE_STATE();    //pops all general non-floating registers from stack to execute the program
}

/**
 * @brief
 * pause the lwp thread and brings the main thread back
*/
void lwp_stop(){
    SAVE_STATE();   //save the lwp thread state
    GetSP(lwp_ptable[lwp_running].sp);  //save the lwp thread stack
    SetSP(temp_st_ptr); //set the main thread back with its stack
    RESTORE_STATE();    //starts popping all the general non-floating register from stack to execute the program
}

/**
 * @brief
 * returns the pid of the current running thread
*/
int lwp_getpid(){
    return lwp_running;
}

/**
 * @brief
 * performs the yielding based on the schedule algorithm
*/
void lwp_yield(){
    SAVE_STATE();   //save the state of the current running thread before context switching
    GetSP(lwp_ptable[lwp_running].sp);     //save the current running stack ptr in our current running thread
    
    scheduler = scheduler == NULL ? roundRobin : scheduler;
    lwp_running = scheduler();

    SetSP(lwp_ptable[lwp_running].sp);  //set stack pointer and set its state
    RESTORE_STATE();    //start poping off the stack and start executing the next state
}

/**
 * @brief
 * sets the scheduler for the function
*/
void lwp_set_scheduler(schedfun sched){
    scheduler = sched;  //sets the scheduler function 
}

/**
 * @brief
 * terminates the thread execution
*/
void lwp_exit(){
    SAVE_STATE();   //save the real context of the main thread
    GetSP(lwp_ptable[lwp_running].sp); //save the real stack pointer for later access

    if(lwp_running == (lwp_procs - 1) && lwp_procs > 1){ //if last element remove it from the table directly
        free(lwp_ptable[lwp_running].stack);    //deallocate the memory
        lwp_procs--;    //reduce the number of procs
        lwp_running = scheduler();
        SetSP(lwp_ptable[lwp_running].sp);  //schedule different thread to run
        RESTORE_STATE();
    }
    else if(lwp_procs > 1 && lwp_running != (lwp_procs - 1)){   //not the last element to remove(i.e first or any middle element)
        free(lwp_ptable[lwp_running].stack);    //deallocate the memory
        int i;
        for(i = lwp_running + 1; i < lwp_procs; i++){
            lwp_ptable[i].pid = (lwp_ptable[i].pid) - 1;  //reduce the pid number as we are also reducing the index
            lwp_ptable[i - 1] = lwp_ptable[i];  //set the curr element to prev element
        }
        lwp_procs--;    //reduce the number of procs

        if(scheduler == roundRobin){    //if round robin running should be one previous since we updated the lwp_table
                                        // with next element in the list
            lwp_running = scheduler();
            if(lwp_running == 0){
                lwp_running = lwp_procs - 1;
            }
            else{
                lwp_running = lwp_running - 1;
            }
        }
        else{
            lwp_running = scheduler();
        }
        
        SetSP(lwp_ptable[lwp_running].sp);  //schedule different thread to run
        RESTORE_STATE();
    }
    else if(lwp_procs == 1){    //if only 1 thread is left
        free(lwp_ptable[lwp_running].stack);    //deallocate the memory
        lwp_procs--;    //reduce the number of procs
        SetSP(temp_st_ptr);  //set the stack pointer for the original thread
        RESTORE_STATE();
    }
}

