#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TYPE_ADDR uint16_t  // type addresses
#define TYPE_PN   uint8_t   // type page number
#define TYPE_OFFS uint8_t   // type offset
#define TYPE_FN   uint8_t   // type frame number


#define NUM_OF_ADDR 1000
void read_addresses(char* fn, uint16_t* addr, int num);
void read_backing_store(char *fn, uint8_t* bs, uint8_t pn);
// void      write_out(char *fn, uint8_t* frame);

int main(int argc, char**argv){ 
    // maybe argc = 1 in linux!
    if(argc != 2){
        printf("please input a filename!\n");
        return 0;
    }

    uint16_t* addresses_logical = (uint16_t*)malloc(sizeof(uint16_t) * NUM_OF_ADDR);
    uint16_t* addresses_physics = (uint16_t*)malloc(sizeof(uint16_t) * NUM_OF_ADDR);
    read_addresses(argv[1], addresses_logical, NUM_OF_ADDR);
    for(int ii = 0; ii < NUM_OF_ADDR; ii++){
        // printf("%d: %hu ", ii+1, addresses_logical[ii]);
        uint8_t page_number = addresses_logical[ii] >> 8, offset = addresses_logical[ii] & 0xFF;
        // printf("page_nunber: %u, offset = %u\n", page_number, offset);

        uint8_t* bs = (uint8_t*)malloc(sizeof(uint8_t));
        read_backing_store("BACKING_STORE.bin", bs, page_number);
        int16_t backing_store = (int16_t)*bs;
        // printf("backing store %d\n", backing_store);
        addresses_physics[ii] = (backing_store << 8) | ((uint16_t)offset);
        // printf("addresses_physics %u\n", addresses_physics[ii]);
        printf("Virtual address: %u Physical address: %u\n", addresses_logical[ii], addresses_physics[ii]);

    }
    
    return 0;
}

/**
 * fn:  filename
 * num: num of addresses
 * */
void read_addresses(char* fn, uint16_t* addr, int num){
    FILE *fp = fopen(fn, "r");
    if(NULL == fp || num <= 0){
        printf("failed to open dos.txt\n"); 
        return;
    }
    for(int ii = 0; ii < num; ii++){
        fscanf(fp, "%hu", &addr[ii]);
    }
    fclose(fp);
}

/**
 * fn: filename
 * pn: page number
 * */
void read_backing_store(char* fn, uint8_t* bs, uint8_t pn){
    FILE *fp = fopen(fn, "rb");
    if(NULL == fp){
        printf("failed to open %s\n", fn); 
        return;
    }
    fseek(fp, pn * sizeof(uint8_t), SEEK_SET);
    // printf("pos = %ld\n", ftell(fp));
    fread(bs, sizeof(uint8_t), 1 , fp);
    fclose(fp);
}
