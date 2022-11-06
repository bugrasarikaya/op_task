#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "schedulers.h"
#include "list.h"
#include "cpu.h"
struct node *head=NULL;
void add(char *name, int priority, int burst){ //Assigns a node's feature and then inserts it into a linked list.
    static int task_count=0;
    Task *temp_task = malloc(sizeof(Task));
    temp_task->name=name;
    temp_task->tid=++task_count;
    temp_task->priority=priority;
    temp_task->burst=burst;
    insert(&head,temp_task);
    temp_task=NULL;
    free (temp_task);
}
struct node *pickNextTask(struct node *head){ //Returns the node which has minimum burst time from task list.
    struct node *ptr=head;
    int min_burst_time=ptr->task->burst;
    int min_id=ptr->task->tid;
    while(ptr!=NULL){
        if(ptr!=head){
            if(min_burst_time>ptr->task->burst){ //Finds minimum burst time and task id of the task which has this burst time.
                min_burst_time=ptr->task->burst;
                min_id=ptr->task->tid;
            }
            else if(min_burst_time==ptr->task->burst)if(min_id>ptr->task->tid)min_id=ptr->task->tid; //If burst times are same, it implements the FCFS.
        }
        ptr=ptr->next;
    }
    if(ptr!=head)ptr=head;
    while(ptr->task->tid!=min_id)ptr=ptr->next; //Finds the node which has minimum burst time.
    return ptr;
}
void schedule(){ //Runs tasks from the list according to their burst times by using SJF scheduling algorithm.
    int time=0;
    int task_count=0;
    for(struct node *ptr=head;ptr!=NULL;ptr=ptr->next,task_count++); //Calculates task count;
    int **average_array=(int**)malloc(sizeof(int*)*task_count);
    for(int i=0;i<task_count;i++)average_array[i]=(int*)malloc(sizeof(int)*3); //3 is for response time, turnaround time, waiting time.
    struct node *temp_node;
    while(head){
        temp_node=pickNextTask(head);
        average_array[temp_node->task->tid-1][0]=time; //Response Time = First Time - Arrival Time
        run(temp_node->task,temp_node->task->burst);
        time+=temp_node->task->burst;
        average_array[temp_node->task->tid-1][1]=time; //Turnaround Time = Completion Time - Arrival Time
        average_array[temp_node->task->tid-1][2]=average_array[temp_node->task->tid-1][1]-temp_node->task->burst; //Waiting Time = Turnaround Time - Burst Time
        delete(&head,temp_node->task);
    }
    int total_response_time,total_turn_around_time,total_waiting_time;
    total_response_time=total_turn_around_time=total_waiting_time=0;
    for(int i=0;i<task_count;i++){ //Calculates total times.
        total_response_time+=average_array[i][0];
        total_turn_around_time+=average_array[i][1];
        total_waiting_time+=average_array[i][2];
    }
    printf("Average Response Time: %.3f units\n",(float)total_response_time/(float)task_count);
    printf("Average Turnaround Time: %.3f units\n",(float)total_turn_around_time/(float)task_count);
    printf("Average Waiting Time: %.3f units\n",(float)total_waiting_time/(float)task_count);
    for(int i=0;i<task_count;i++)free(average_array[i]);
    free(average_array);
}
