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
    struct node* prioir;
    struct node* next;
}tlb_t;

FILE *file_addresses, *file_backing_store;

uint16_t  addresses_logical[NUM_OF_ADDR] = {0};
uint8_t   addresses_physics[NUM_OF_FRAME][SIZE_OF_FRAME] = {0};
uint8_t   page_table_page[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   page_table_frame[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   tlb_page[SIZE_OF_TLB] = {0};
uint8_t   tlb_frame[SIZE_OF_TLB] = {0};

int32_t   tlb_hit = 0;
int32_t   page_fault = 0;
int32_t   tlb_max_index = 0;
int32_t   page_table_max_index = 0;
int32_t   addresses_physics_frame_max_index = 0;

void read_addresses_logical(void);
void read_backing_store(uint8_t page_num);
void get_page(uint16_t address_logigcal);
void tlb_replace_fifo(uint8_t page_num, uint8_t frame_num);
void tlb_replace_lru(uint8_t page_num, int8_t frame_num);

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

    read_addresses_logical();
    for(int ii = 0; ii < NUM_OF_ADDR; ii++){
        get_page(addresses_logical[ii]);
    }

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
        for(int ii = 0; ii < tlb_max_index; ii++){
            if(tlb_page[ii] == page_number){
                frame_number = tlb_frame[ii];
                tlb_hit++;
                flag = 1;
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
    int8_t value = addresses_physics[frame_number][offset];
    printf("Virtual address: %d Physical address: %d Value: %d\n", address_logical, (frame_number << 8) | offset, value);
}

void tlb_replace_fifo(uint8_t page_num, uint8_t frame_num){
    int ii = 0;
    for(ii = 0; ii < tlb_max_index; ii++){
        if(tlb_page[ii] == page_num){
            break;
        }
    }
    if(ii == tlb_max_index){
        if(tlb_max_index < SIZE_OF_TLB){
            
        }
        else{
            
        }
        
    }

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

