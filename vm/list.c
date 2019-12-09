/**
  ******************************************************************************
  * @file    double_circle_link.c
  * @author  jieyr3 17308074
  * @version V1.0.0
  * @date    12-March-2019
  * @brief   the first project for data structure and algorithm, please compile
  * @brief   and run this file,
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"

/**
  * @brief  type of insert enumeration
  */
typedef enum insert_type{
    pre = 0, /*!< insert the node before pointer */
    suf = 1, /*!< insert the node behind pointer */
}insert_type_e;

/**
  * @brief  node structure definition
  */
typedef struct node {
    struct node* prior;  /*!< prior node pointer */
    int          data ;  /*!< data in the node   */
    struct node* next ;  /*!< next node pointer  */
}jd_t;


/** @defgroup work_mode define
  * @{
  */
#define INSERT  0  /*!< insert data into the list */
#define DELETE  1  /*!< delete node in the list   */
#define SHOW    2  /*!< show the list once        */
#define EXIT    3  /*!< exit the while loop       */
/**
  @}
  */

/**
  * @brief  create a double circle list
  * @param  array  : an array that store data of the new list
  * @param  length : length of the new list
  * @retval none
  */
jd_t* jd_create_list(int array [], int length){
    printf("\ncreating list...\n");
    if (length <= 0){
        printf("\nthe length is 0, create failed.\n");
        return NULL;
    }

    jd_t* pointer_header = (jd_t*)malloc(sizeof(jd_t));
    pointer_header->data  = array[0];

    if (length == 1){
        pointer_header->prior = pointer_header;
        pointer_header->next  = pointer_header;
        return pointer_header;
    }

    jd_t* pointer_last = pointer_header;
    jd_t* node_temp;
    for (int ii = 1; ii < length; ii++){
        node_temp = (jd_t*)malloc(sizeof(jd_t));
        node_temp->data = array[ii];
        node_temp->prior = pointer_last;
        pointer_last->next = node_temp;
        pointer_last = node_temp;
    }
    node_temp->next = pointer_header;
    pointer_header->prior = node_temp;
    printf("\nsuccessfully create list!\n");
    return pointer_header;
}

/**
  * @brief  delete and free the whole list
  * @param  pointer_header : the header pointer of the list
  * @retval none
  */
void jd_delete_list(jd_t* pointer_header){
    printf("\ndeleting list...\n\n");
    if(pointer_header == NULL){
        printf("\nthis list is empty\n");
        return;
    }
    int length = 1, node_count = 0;
    jd_t* node_temp = pointer_header;
    while(node_temp->next != pointer_header){
        length++;
        node_temp = node_temp->next;
    }
    while(length--){
        jd_t* node_temp_next = node_temp->next;
        free(node_temp);
        node_temp = node_temp_next;
        printf("deleting the node_%d\n",node_count++);
        if(node_temp == NULL)break;
    }
    printf("\nsuccessfully delete list!\n");
}

/**
  * @brief  show the whole list
  * @param  pointer_header : the header pointer of the list
  * @retval none
  */
void jd_show_list(jd_t* pointer_header){
    printf("\nshowing list ... \n\n");
    if(pointer_header == NULL){
        printf("this is a empty list.\n");
        return;
    }
    jd_t* node_temp = pointer_header;
    int node_cout = 0;
    do{
        printf("node_count = %d, data = %d\n",node_cout++,node_temp->data);
        node_temp = node_temp->next;
    }while(node_temp != pointer_header);
    printf("\nshow end.\n");
}

/**
  * @brief  insert a node in the list
  * @param  pointer     : the neighboring node in the list
  * @param  data_insert : data to be inserted into the list
  * @param  ins         : type of insert (front or back)
  * @retval pointer of the data node
  */
jd_t* jd_insert_node(jd_t *pointer, int data_insert, insert_type_e ins){
    if(pointer == NULL){
        printf("\npointer is null, insert failed! :-(\n");
        return pointer;
    }

    jd_t* node_insert = (jd_t*)malloc(sizeof(jd_t));
    node_insert->data = data_insert;
    jd_t* pointer_prior;
    jd_t* pointer_next ;
    if     (ins == pre){
        pointer_prior = pointer->prior;
        pointer_next  = pointer;
    }
    else if(ins == suf){
        pointer_prior = pointer;
        pointer_next  = pointer->next;
    }
    printf("\ninserting [%d] between [%d] and [%d]...\n",
           node_insert->data, pointer_prior->data, pointer_next->data);

    /* insert operation */
    node_insert->prior  = pointer_prior; node_insert->next   = pointer_next;
    pointer_prior->next = node_insert  ; pointer_next->prior = node_insert ;
    printf("\nsuccessfully insert\n");

    return node_insert;
}

/**
  * @brief  delete a node in the list
  * @param  pointer_delete : the node that will be deleted
  * @retval none
  */
void jd_delete_node(jd_t *pointer_delete){
    /* delete node operation */
    jd_t* pointer_prior = pointer_delete->prior;
    jd_t* pointer_next  = pointer_delete->next;

    pointer_delete->prior->next = pointer_next;
    pointer_delete->next->prior = pointer_prior;
    free(pointer_delete);
}

/**
  * @brief  given an integer, find the position for it in the list and insert it
  * @brief  the list is incremental
  * @param  pointer_header : header pointer of the list
  * @param  data           : integer
  * @retval header pointer of new list
  */
jd_t* jd_sequentially_insert_data(jd_t* pointer_header, int data){
    printf("\ninserting [%d] into the list (incrementally)...\n",data);

    if(pointer_header == NULL){
        printf("\nnull\n");
        /* initialize the list using data */
        pointer_header = (jd_t*)malloc(sizeof(jd_t));
        pointer_header->data  = data;
        pointer_header->next  = pointer_header;
        pointer_header->prior = pointer_header;
        printf("\nempty list, insert it into the first node\n");
        jd_show_list(pointer_header);
        return pointer_header;
    }

    if(pointer_header->data > data){
        printf("\ndata is min in the list, insert it before the header...\n");
        pointer_header = jd_insert_node(pointer_header, data, pre);
    }
    else{
        jd_t* node_temp = pointer_header;
        do{
            printf("\nif data [%d] is suitable between [%d] and [%d]? ",
                   data, node_temp->data, node_temp->next->data);
            if((data > node_temp->data            &&
                data <= node_temp->next->data)    ||
                node_temp->next == pointer_header ){
                printf("  yes\n");
                jd_insert_node(node_temp, data, suf);
                break;
            }
            printf("  no\n");
            node_temp = node_temp->next;
        }while(node_temp != pointer_header);
    }
    jd_show_list(pointer_header);
    return pointer_header;
}

/**
  * @brief  given an integer, find the position of it in the list and delete it
  * @brief  the list is incremental
  * @param  pointer_header : header pointer of the list
  * @param  data           : integer
  * @retval header pointer of new list
  */
jd_t* jd_sequentially_delete_data(jd_t* pointer_header, int data){
    printf("\ndeleting [%d] of the list (incremental)...\n",data);

    if(pointer_header == NULL){
        printf("\nempty list, delete failed\n");
        return pointer_header;
    }

    if(pointer_header->data == data){

        /* there is not only one node in the list */
        if(pointer_header->next != pointer_header){

            pointer_header = pointer_header->next;
            jd_delete_node(pointer_header);
            jd_show_list(pointer_header);
            return pointer_header;
        }

        /* there is only one node in the list, after deleting it will be empty */
        else{
            free(pointer_header);
            jd_t* pointer_null = NULL;
            printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("\nwarning : the list is empty now!\n");
            printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            return pointer_null;
        }
    }

    jd_t* node_temp = pointer_header->next;

    while(node_temp != pointer_header){
        if(node_temp->data == data){
            jd_delete_node(node_temp);
            printf("\nsuccessfully delete node!\n");
            jd_show_list(pointer_header);
            return pointer_header;
        }
        node_temp = node_temp->next;
    }

    printf("\nsorry, there is no element in the list is equal to the data given\n");
    printf("\nthe list is showed as below, please try again :-)\n");
    jd_show_list(pointer_header);
    return pointer_header;
}

/**
  * @brief  main function of this project
  * @retval none
  */
int main(){
    int length = 0;
    printf("Please input the length of this double circle list: ");
    scanf("%d",&length);
    while(length <= 0){
        printf("please enter an int > 0, try again :-)!\n");
        printf("Please input the length of this double circle list: ");
        scanf("%d",&length);
    }

    /* get the array for our list */
    int* array = (int*)malloc(length * sizeof(int));
    for(int ii = 0; ii < length; ii++){
        array[ii] = ii * ii;
    }

    /* create and show the list first */
    jd_t* pointer_header = jd_create_list(array, length);
    jd_show_list(pointer_header);
    int work_mode = 0;
    int data = 0;
    while(1){
        printf("\n*************************************************************\n");
        printf("welcome to a new work loop!\n");
        printf("please input a command to tell me what to do:\n");
        printf("(0):insert / (1):delete / (2):show / (3)exit\n");
        scanf("%d",&work_mode);

        /* switch command id */
        switch(work_mode){

        case INSERT:
            printf("\nplease enter the data to insert : ");
            scanf("%d",&data);
            pointer_header = jd_sequentially_insert_data(pointer_header, data);
            break;

        case DELETE:
            printf("\nplease enter the data to delete : ");
            scanf("%d",&data);
            pointer_header = jd_sequentially_delete_data(pointer_header,data);
            break;

        case SHOW:
            jd_show_list(pointer_header);
            break;

        case EXIT:
            break;

        default:
            printf("\ninvalid input, try again :-) \n");
            break;
        }
        if(work_mode == EXIT) break;
    }

    jd_delete_list(pointer_header);
    if(array != NULL) free(array);
    printf("\nend~\nwelcome next!\n");
    return 0;
}
