#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NUM_OF_ADDR 1000
uint16_t *get_addresses(char* fn, int num);
uint8_t  get_backing_store(char *fn, uint8_t pn);

int main(int argc, char**argv){ 
    // maybe argc = 1 in linux!
    if(argc != 2){
        printf("please input a filename!\n");
        return 0;
    }

    uint16_t* addresses = get_addresses(argv[1], NUM_OF_ADDR);
    for(int ii = 0; ii < NUM_OF_ADDR; ii++){
        printf("%d: %hu ", ii+1, addresses[ii]);
        uint8_t page_number = addresses[ii] >> 8, offset = addresses[ii] & 0xFF;
        printf("page_nunber: %u, offset = %u\n", page_number, offset);
        uint8_t backing_store = get_backing_store("BACKING_STORE.bin", page_number);
    }
    
    return 0;
}

/**
 * fn:  filename
 * num: num of addresses
 * */
uint16_t *get_addresses(char* fn, int num){
    uint16_t *addr = (uint16_t*) malloc(sizeof(uint16_t)*num);
    FILE *fp = fopen(fn, "r");
    if(fp == NULL){
        printf("failed to open dos.txt\n"); 
        return NULL;
    }
    for(int ii = 0; ii < num; ii++){
        fscanf(fp, "%hu", &addr[ii]);
    }
    fclose(fp);
    return addr;
}

/**
 * fn: filename
 * pn: page number
 * */
uint8_t get_backing_store(char* fn, uint8_t pn){
    uint8_t *bs = (uint8_t*) malloc(sizeof(uint8_t));
    FILE *fp = fopen("BACKING_STORE.bin", "r");
    if(fp == NULL){
        printf("failed to open %s\n", fn); 
        return 0;
    }
    fseek(fp, pn * sizeof(uint8_t), SEEK_SET);
    fread(bs, sizeof(uint8_t), 1 , fp);
    fclose(fp);
    printf("bs = %u\n", *bs);
    return *bs;
}
