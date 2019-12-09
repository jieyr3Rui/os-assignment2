#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TYPE_ADDR uint16_t  // type addresses
#define TYPE_PN   uint8_t   // type page number
#define TYPE_OFFS uint8_t   // type offset
#define TYPE_FN   uint8_t   // type frame number


#define SIZE_OF_PHYSICS    128
#define SIZE_OF_FRAME      256
#define SIZE_OF_TLB        16
#define SIZE_OF_PAGE_TABLE 256

typedef struct node{
    uint8_t page;
    uint8_t frame;
    struct node* prior;
    struct node* next;
}table_t;

typedef enum ag{ fifo = 1, lru = 2, }ag_e;
typedef enum ta{ tlb = 0, page_table = 1,}ta_e;

FILE *file_addresses, *file_backing_store;

uint16_t  addresses_logical = 0;
uint8_t   addresses_physics[SIZE_OF_PHYSICS][SIZE_OF_FRAME] = {0};
uint8_t   page_table_page[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   page_table_frame[SIZE_OF_PAGE_TABLE] = {0};

table_t*  tlb_head = NULL;
table_t*  tlb_last = NULL;
table_t*  page_table_head = NULL;
table_t*  page_table_last = NULL;

uint32_t   tlb_hit = 0;
uint32_t   page_fault = 0;
uint32_t   tlb_node_number = 0;
uint32_t   page_table_node_number = 0;
uint32_t   addresses_physics_frame_used = 0;

void read_backing_store(uint8_t page, uint8_t frame);
void get_page(uint16_t address_logigcal);

void tlb_replace(uint8_t page, uint8_t frame);
void page_table_replace(uint8_t page, uint8_t *frame);

uint8_t table_list_search(uint8_t page, table_t** head, table_t** last, ag_e ag, uint8_t* frame_out);

int main(int argc, char**argv){ 
    // maybe argc = 1 in linux!
    // printf("arcg = %d\n", argc);
    if(argc != 3){
        printf("usage: ./vm [backing_store] [addresses.txt]\n");
        return 0;
    }

    file_addresses = fopen(argv[2], "r");
    if(NULL == file_addresses){
        printf("failed to open address.txt\n"); 
    }

    file_backing_store = fopen(argv[1], "rb");
    if(NULL == file_backing_store){
        printf("failed to open BACKING_STORE.bin\n"); 
    }

    while(fscanf(file_addresses, "%hu", &addresses_logical) != EOF){
        get_page(addresses_logical);
    }
    printf("tlb_hit = %d\npage_fault = %d\nphysics used = %d\n",tlb_hit, page_fault, addresses_physics_frame_used);

    fclose(file_addresses);
    fclose(file_backing_store);
    return 0;
}


void get_page(uint16_t address_logical){
    uint8_t page_number = ((address_logical & 0xFFFF) >> 8);
    uint8_t offset = (address_logical & 0xFF);
    uint8_t frame_number = 0;
    uint8_t flag_tlb = 0, flag_page_table = 0;


    
    // printf("test0\n");
    flag_tlb = table_list_search(page_number, &tlb_head, &tlb_last, fifo, &frame_number);
    
        // check tlb
    printf("test1\n");
    // check the page table
    flag_page_table = table_list_search(page_number, &page_table_head, &page_table_last, fifo, &frame_number);
    
    // page fault
    printf("test2\n");
    if(flag_page_table){ page_table_replace(page_number, &frame_number); page_fault++;}
    
    // printf("test3\n");
    if(flag_tlb) tlb_replace(page_number, frame_number);
    
    else tlb_hit++;
    //printf("test4\n");
    

    int8_t value = addresses_physics[frame_number][offset];
    
    printf("Virtual address: %d Physical address: %d Value: %d\n", address_logical, (frame_number << 8) | offset, value);
    printf("physic used = %d\n", addresses_physics_frame_used);
}

void tlb_replace(uint8_t page, uint8_t frame){
    table_t* new_node = (table_t*)malloc(sizeof(table_t));
    new_node->frame = frame;
    new_node->page = page;
    new_node->next = NULL;
    new_node->prior = NULL;
    // page is not in tlb
    if(tlb_node_number == 0){
        tlb_head = new_node;
        tlb_last = new_node;
    }
    else{
        if(tlb_node_number == SIZE_OF_TLB){
            table_t* p = tlb_last;
            tlb_last = tlb_last->prior;
            tlb_last->next = NULL;
            free(p);
        }
        // insert node to head
        tlb_head->prior = new_node;
        new_node->next = tlb_head;
        tlb_head = new_node;
    }
    // increase node number
    if(tlb_node_number < SIZE_OF_TLB) tlb_node_number++;
}

void page_table_replace(uint8_t page, uint8_t *frame){
    table_t* new_node = (table_t*)malloc(sizeof(table_t));
    new_node->page = page;
    new_node->next = NULL;
    new_node->prior = NULL;

    // page is not in tlb
    if(page_table_node_number == SIZE_OF_PAGE_TABLE ||
       addresses_physics_frame_used == SIZE_OF_PHYSICS){
        table_t* p = page_table_last;
        page_table_last = page_table_last->prior;
        page_table_last->next = NULL;
        // p->frame
        new_node->frame = p->frame;
        free(p);
        page_table_head->prior = new_node;
        new_node->next = page_table_head;
        page_table_head = new_node;
    }
    else if(page_table_node_number == 0){
        new_node->frame = addresses_physics_frame_used;
        page_table_head = new_node;
        page_table_last = new_node;
        addresses_physics_frame_used++;
        page_table_node_number++;
    }
    else{
        new_node->frame = addresses_physics_frame_used;
        page_table_head->prior = new_node;
        new_node->next = page_table_head;
        page_table_head = new_node;
        addresses_physics_frame_used++;
        page_table_node_number++;
    }
    read_backing_store(new_node->page, new_node->frame);
    *frame = new_node->frame;
}

uint8_t table_list_search(uint8_t page, table_t** p_head, table_t** p_last, ag_e ag, uint8_t* frame_out){
    table_t* head = *p_head; table_t* last = *p_last;
    
    table_t* p = head;
    int ii = 0;
    while(p != NULL){
        if(p->page == page){
            *frame_out = p->frame;

            if((ag == lru) && (p != head)){
                if(p == last){ last = p->prior;}
                else {p->next->prior = p->prior;}
                p->prior->next = p->next;
                p->prior = NULL;
                p->next = head;
                head->prior = p;
                head = p;
            }
            *p_head = head; *p_last = last;
            return 0;
        }
        p = p->next;
    }
    *p_head = head; *p_last = last;
    return 1;
}

/**
 * pn: page number
 * */
void read_backing_store(uint8_t page, uint8_t frame){
    int8_t  buffer[SIZE_OF_FRAME];
    fseek(file_backing_store, ((int)page) * SIZE_OF_FRAME, SEEK_SET);
    fread(buffer, sizeof(int8_t), SIZE_OF_FRAME, file_backing_store);
    for(int ii = 0; ii < SIZE_OF_FRAME; ii++){
        addresses_physics[frame][ii] = buffer[ii];
    }
}