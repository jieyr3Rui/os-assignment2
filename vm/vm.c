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

FILE *file_addresses, *file_backing_store;

uint16_t  addresses_logical[NUM_OF_ADDR] = {0};
uint8_t   addresses_physics[NUM_OF_FRAME][SIZE_OF_FRAME] = {0};
uint8_t   page_table_page[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   page_table_frame[SIZE_OF_PAGE_TABLE] = {0};
uint8_t   tlb_page[SIZE_OF_TLB] = {0};
uint8_t   tlb_frame[SIZE_OF_TLB] = {0};

int32_t   tlb_hit = 0;
int32_t   page_fault = 0;
int32_t   page_table_curr_max_index = 0;

void read_addresses_logical(void);
void read_backing_store(void);
void get_page(uint16_t address_logigcal);

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
        printf("%u\n", addresses_logical[ii]);
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
    printf("pn: %u off: %u\n", page_number, offset);
    uint8_t frame_number = 0;
    uint8_t flag = 0;

    // check the tlb, tlb hit
    if(flag == 0){
        for(int ii = 0; ii < SIZE_OF_TLB; ii++){
            if(tlb_page[ii] == page_number){
                frame_number = tlb_frame[ii];
                tlb_hit++;
                flag = 1;
            }
        }
    }

    // check the page table
    if(flag == 0){
        for(int ii = 0; ii < page_table_curr_max_index; ii++){
            if(page_table_page[ii] == page_number){
                frame_number = page_table_frame[ii];
                flag = 1;
            }
        }
    }

    // page fault
    if(flag == 0){
        read_backing_store();
        page_fault++;
        flag = 1;
    }


}


/**
 * fn: filename
 * pn: page number
 * */
void read_backing_store(void){
    // signed char     buffer[256];
    // FILE *fp = fopen(fn, "r");
    // if(NULL == fp){
    //     printf("failed to open %s\n", fn); 
    //     return;
    // }
    // fseek(fp, (int)pn * 256, SEEK_SET);
    // // printf("pos = %ld\n", ftell(fp));
    // fread(bs, sizeof(uint8_t), 256 , fp);
    // fclose(fp);
}
