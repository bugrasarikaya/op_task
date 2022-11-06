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
void swap_pointers(struct node **ptr_1, struct node **ptr_2) { //Swaps two pointers.
	struct node *temp = *ptr_1;
	*ptr_1 = *ptr_2;
	*ptr_2 = temp;
}
void swap_nodes(struct node **head, struct node **node_1, struct node **node_2) { //Swaps two nodes without changing nodes' data.
    if((*node_1)!=(*head)&&(*node_2)->next!=NULL){ //Swaps two nodes which are both not first and last node.
        struct node *prev_node_1=*head;
        while(prev_node_1->next!=(*node_1))prev_node_1=prev_node_1->next;
        prev_node_1->next=(*node_2);
        (*node_1)->next=(*node_2)->next;
        (*node_2)->next=(*node_1);
    }
    else if((*node_1)==(*head)&&(*node_2)->next!=NULL){ //Swaps two nodes which one is first node.
        //struct node *prev_node_1=*head;
        //while(prev_node_1->next!=(*node_1))prev_node_1=prev_node_1->next;
        //prev_node_1->next = (*node_2);
        (*node_1)->next=(*node_2)->next;
        (*node_2)->next=(*node_1);
    }
    else if((*node_1)!=(*head)&&(*node_2)->next==NULL){ //Swaps two nodes which one is last node.
        struct node *prev_node_1=*head;
        while(prev_node_1->next!=(*node_1))prev_node_1=prev_node_1->next;
        prev_node_1->next = (*node_2);
        (*node_1)->next=(*node_2)->next;
        (*node_2)->next=(*node_1);
    }
    else if((*node_1)==(*head)&&(*node_2)->next==NULL){ //Swaps two nodes which one is first node and other one is last node (The situation of that linked list has only 2 nodes.).
        //struct node *prev_node_1=*head;
        //while(prev_node_1->next!=(*node_1))prev_node_1=prev_node_1->next;
        //prev_node_1->next = (*node_2);
        (*node_1)->next=(*node_2)->next;
        (*node_2)->next=(*node_1);
    }
	if(*head==*node_1)*head=(*node_2); //Protects the head pointer's target before the swapping.
	swap_pointers(node_1,node_2);
}
void sort_list(struct node **head){ //Sorts task queue according to priorities by Bubble Sort.
    struct node *ptr;
    struct node *ptr_next;
    int task_count=0;
    for(ptr=*head;ptr!=NULL;ptr=ptr->next,task_count++); //Calculates task count;
    int limit=task_count;
    for(int i=0;i<task_count-1;i++){
        ptr=*head;
        for(int j=0;j<limit-1;ptr=ptr->next,j++){
            if(ptr->task->priority>ptr->next->task->priority){
            ptr_next=ptr->next;
            swap_nodes(head,&ptr,&ptr_next);}
            else if(ptr->task->priority==ptr->next->task->priority&&ptr->task->tid<ptr->next->task->tid){
            ptr_next=ptr->next;
            swap_nodes(head,&ptr,&ptr_next);}
        }
        limit--;
    }
}
struct node *pickNextTask(struct node *head){ //Returns the current node according to Round-Robin.
    struct node *ptr=head;
    static struct node* future_ptr=NULL; //Future node is the next task of the current task.
    if(future_ptr==head){ //Situation of that future node is head.
    	if(ptr->next==NULL)future_ptr=ptr; //Situation of that there is only one node in the list.
        else if(ptr->next->next==NULL){ //Situation of that there are two nodes in the list.
        future_ptr=ptr;
        ptr=future_ptr->next;
        }
        else if(ptr->next->next!=NULL){ //Situation of that there are more than two nodes in the list
            while(ptr->next!=NULL)ptr=ptr->next;
            future_ptr=ptr;
            ptr=head;
        }
    }else if(ptr->next->next==NULL&&future_ptr==ptr->next){ //Situation of that there are two nodes in the list and future node is first second node.
        future_ptr=ptr;
        ptr=future_ptr->next;
    }else if(ptr->next->next!=NULL&&future_ptr==NULL){ //Situation of that there are more than two nodes in the list and future node is not assigned to a node yet.
        while(ptr->next->next!=NULL)ptr=ptr->next;
        future_ptr=ptr;
        ptr=future_ptr->next;
    }else if(ptr->next->next!=NULL&&future_ptr==ptr->next){ //Situation of that there are more than two nodes in the list and future node is first second node.
        future_ptr=ptr;
        ptr=future_ptr->next;
    }else if(ptr->next->next!=NULL&&future_ptr!=NULL&&future_ptr!=head){ //Situation of that there are more than two nodes in the list and future node is not head.
        while(ptr->next!=future_ptr)ptr=ptr->next;
        future_ptr=ptr;
        ptr=ptr->next;
    }
    return ptr;
}
void schedule(){ //Runs tasks from the list by using Priority with Round-Robin.
    int time=0;
    int task_count=0;
    for(struct node *ptr=head;ptr!=NULL;ptr=ptr->next,task_count++); //Calculates task count;
    int **average_array=(int**)malloc(sizeof(int*)*task_count);
    for(int i=0;i<task_count;i++)average_array[i]=(int*)malloc(sizeof(int)*3);
    int *first_time_array=(int*)malloc(sizeof(int)*task_count);
    for(int i=0;i<task_count;i++)first_time_array[i]=0; //Initalization of first_time_array.
    int *burst_array=(int*)malloc(sizeof(int)*task_count);
    for(int i=0;i<task_count;i++)burst_array[i]=0; //Initalization of burst_array.
    int quantum=10;
    int burst_time;
    sort_list(&head);
    struct node *temp_node;
    while(head){
        temp_node=pickNextTask(head);
        burst_time=(temp_node->task->burst>=quantum)?quantum:temp_node->task->burst; //Calculates burst time according to quantum.
        if(!first_time_array[temp_node->task->tid-1]){ //When the task gets the CPU for the first time.
        average_array[temp_node->task->tid-1][0]=time; //Response Time = First Time - Arrival Time
        first_time_array[temp_node->task->tid-1]=1;
        }
        run(temp_node->task,burst_time);
        time+=burst_time;
        burst_array[temp_node->task->tid-1]+=burst_time;
        average_array[temp_node->task->tid-1][1]=time; //Turnaround Time = Completion Time - Arrival Time
        average_array[temp_node->task->tid-1][2]=average_array[temp_node->task->tid-1][1]-burst_array[temp_node->task->tid-1]; //Waiting Time = Turnaround Time - Burst Time
        temp_node->task->burst-=burst_time; //Calculates remaining burst time.
        if(!temp_node->task->burst)delete(&head,temp_node->task); //Deletes the node if remaining burst time is 0.
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
    free(first_time_array);
    free(burst_array);
}
