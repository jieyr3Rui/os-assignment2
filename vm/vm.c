#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NUM_OF_ADDR 1000
uint16_t *get_addresses(const char* filename, int num);

int main(){ 
    const char* filename = "addresses.txt";
    uint16_t* addresses = get_addresses(filename, NUM_OF_ADDR);
    // for(int ii = 0; ii < NUM_OF_ADDR; ii++){
    //     printf("%d: %hd\n", ii+1, addresses[ii]);
    // }

    return 0;
}

/**
 * 
 * 
 **/
uint16_t *get_addresses(const char* fn, int num){
    uint16_t *addr = (uint16_t*) malloc(sizeof(uint16_t)*num);
    FILE *fp = fopen(fn, "r");
    if(NULL == fp){
        printf("failed to open dos.txt\n"); 
        return NULL;
    }
    for(int ii = 0; ii < num; ii++){
        fscanf(fp, "%hu", &addr[ii]);
    }
    fclose(fp);
    return addr;
}
