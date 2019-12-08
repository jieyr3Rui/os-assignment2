#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TYPE_ADDR uint16_t  // type addresses
#define TYPE_PN   uint8_t   // type page number
#define TYPE_OFFS uint8_t   // type offset
#define TYPE_FN   uint8_t   // type frame number


#define NUM_OF_ADDR        1000
#define NUM_OF_FRAME       256
#define SIZE_OF_FRAME      256
#define SIZE_OF_TLB        16
#define SIZE_OF_PAGE_TABLE 256

typedef struct node{
    uint8_t page;
    uint8_t frame;
    struct node* prior;
    struct node* next;
}tlb_t;

FILE *file_addresses, *file_backing_store, *file_output;

uint16_t  addresses_logical[NUM_OF_ADDR] = {0};
uint8_t   addresses_physics[NUM_OF_FRAME][SIZE_OF_FRAME] = {0};
uint8_t   page_table_page[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   page_table_frame[SIZE_OF_PAGE_TABLE] = {0};
tlb_t* tlb_head = NULL;
tlb_t* tlb_last = NULL;

int32_t   tlb_hit = 0;
int32_t   page_fault = 0;
int32_t   tlb_node_number = 0;
int32_t   page_table_max_index = 0;
int32_t   addresses_physics_frame_max_index = 0;

void read_addresses_logical(void);
void read_backing_store(uint8_t page_num);
void get_page(uint16_t address_logigcal);
void tlb_replace_fifo(uint8_t page, uint8_t frame);
void tlb_replace_lru(uint8_t page, int8_t frame);

int main(int argc, char**argv){ 
    // maybe argc = 1 in linux!
    if(argc != 2){
        printf("please input a filename!\n");
        return 0;
    }

    file_addresses = fopen(argv[1], "r");
    if(NULL == file_addresses){
        printf("failed to open address.txt\n"); 
    }

    file_backing_store = fopen("BACKING_STORE.bin", "rb");
    if(NULL == file_backing_store){
        printf("failed to open BACKING_STORE.bin\n"); 
    }

    file_output = fopen("out.txt", "w");
    if(NULL == file_output){
        printf("failed to open out.txt\n"); 
    }

    read_addresses_logical();
    for(int ii = 0; ii < NUM_OF_ADDR; ii++){
        get_page(addresses_logical[ii]);
    }
    printf("tlb_hit = %d\npage_fault = %d\n",tlb_hit, page_fault);

    fclose(file_addresses);
    fclose(file_backing_store);
    return 0;
}

/**
 * function
 * */
void read_addresses_logical(void){
    for(int ii = 0; ii < NUM_OF_ADDR; ii++){
        fscanf(file_addresses, "%hu", &addresses_logical[ii]);
    }
}


void get_page(uint16_t address_logical){
    uint8_t page_number = ((address_logical & 0xFFFF) >> 8);
    uint8_t offset = (address_logical & 0xFF);
    uint8_t frame_number = 0;
    uint8_t flag = 0;

    // check the tlb, tlb hit
    if(flag == 0){
        tlb_t* p = tlb_head;
        while(p != NULL){
            if(p->page == page_number){
                frame_number = p->frame;
                tlb_hit++;
                flag = 1;
                break;
            }
            else{
                p = p->next;
            }
        }
    }

    // check the page table
    if(flag == 0){
        for(int ii = 0; ii < page_table_max_index; ii++){
            if(page_table_page[ii] == page_number){
                frame_number = page_table_frame[ii];
                flag = 1;
            }
        }
    }

    // page fault
    if(flag == 0){
        read_backing_store(page_number);
        page_fault++;
        frame_number = addresses_physics_frame_max_index - 1;
        flag = 1;
    }
    // tlb_replace_fifo(page_number, frame_number);
    tlb_replace_lru(page_number, frame_number);
    int8_t value = addresses_physics[frame_number][offset];
    fprintf(file_output, "Virtual address: %d Physical address: %d Value: %d\n", address_logical, (frame_number << 8) | offset, value);
}

void tlb_replace_fifo(uint8_t page, uint8_t frame){
    tlb_t* p = tlb_head;
    while(p != NULL){
        // page is in tlb
        if(p->page == page){
            return;
        }
        p = p->next;
    }
    // page is not in tlb
    tlb_t* new_node = (tlb_t* )malloc(sizeof(tlb_t));
    new_node->page = page;
    new_node->frame = frame;
    new_node->next = NULL;
    new_node->prior = NULL;
    if(tlb_node_number == 0){
        tlb_head = new_node;
        tlb_last = new_node;
        
    }
    else{
        // insert node to head
        tlb_head->prior = new_node;
        new_node->next = tlb_head;
        tlb_head = new_node;
        // free the last one
        if(tlb_node_number == SIZE_OF_TLB){
            tlb_t* p = tlb_last;
            tlb_last = tlb_last->prior;
            tlb_last->next = NULL;
            free(p);
        }
    }
    // increase node number
    if(tlb_node_number < SIZE_OF_TLB) tlb_node_number++;
}

void tlb_replace_lru(uint8_t page, int8_t frame){
    tlb_t* p = tlb_head;
    // while (p != NULL){
    //     if(p->prior == NULL)printf("N");
    //     else printf("F");
    //     printf("%d", p->page);
    //     if(p->next == NULL)printf("N ");
    //     else printf("F ");
    //     p = p->next;
    // }
    // printf("\n");
    // p = tlb_head;
    while(p != NULL){
        // page is in tlb
        if(p->page == page){
            if(p != tlb_head){
                if(p == tlb_last){ tlb_last = p->prior;}
                else {p->next->prior = p->prior;}
                p->prior->next = p->next;
                p->prior = NULL;
                p->next = tlb_head;
                tlb_head->prior = p;
                tlb_head = p;
            }
            return;
        }
        p = p->next;
    }
    // page is not in tlb
    tlb_t* new_node = (tlb_t* )malloc(sizeof(tlb_t));
    new_node->page = page;
    new_node->frame = frame;
    new_node->next = NULL;
    new_node->prior = NULL;
    
    if(tlb_node_number == 0){
        tlb_head = new_node;
        tlb_last = new_node;
    }
    else{
        // insert node to head
        tlb_head->prior = new_node;
        new_node->next = tlb_head;
        tlb_head = new_node;
        // free the last one
        if(tlb_node_number == SIZE_OF_TLB){
            tlb_t* p = tlb_last;
            tlb_last = tlb_last->prior;

            tlb_last->next = NULL;
            free(p);
            
        }
    }
    // increase node number
    
    if(tlb_node_number < SIZE_OF_TLB) tlb_node_number++;
}

/**
 * pn: page number
 * */
void read_backing_store(uint8_t page_num){
    int8_t  buffer[SIZE_OF_FRAME];
    fseek(file_backing_store, ((int)page_num) * SIZE_OF_FRAME, SEEK_SET);
    fread(buffer, sizeof(int8_t), SIZE_OF_FRAME, file_backing_store);
    for(int ii = 0; ii < SIZE_OF_FRAME; ii++){
        addresses_physics[addresses_physics_frame_max_index][ii] = buffer[ii];
    }
    page_table_page[page_table_max_index] = page_num;
    page_table_frame[page_table_max_index] = addresses_physics_frame_max_index;

    page_table_max_index++;
    addresses_physics_frame_max_index++;
}

