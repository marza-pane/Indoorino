
#define SIZE0 20
#define SIZE1 150

#include "AESLib.h"
#include "utils.h"

AESLib aesLib;

void setup() {
    
    utils::board::debug_init();

    Serial.begin(115200);
    
    char b0 [SIZE0];
    char b1 [SIZE1];
    char iv[N_BLOCK];
    char key[N_BLOCK];
    char *c0;
    char *c1;
    
    strcpy(b0, "ASDFGHJKLOIUYTREWXC");
    strcpy(b1, "ASDFGHJKLOIUYTREWXCASDFGHJKLOIUYTREWXCASDFGHJKLOIUYTREWXCASDFGHJKLOIUYTREWXCASDFGHJKLOIUYTREWXCASDFGHJKLOIUYTREWXC");
    
    info_os("extimated cipher size 0: [%d]", aesLib.get_cipher_length(SIZE0));
    info_os("extimated cipher64 size 0: [%d]", aesLib.get_cipher64_length(SIZE0));
    c0 = (char *)calloc(10*SIZE0,1);
    info_os("real cipher size 0: [%d]", aesLib.encrypt((uint8_t*)b0, SIZE0, c0, (uint8_t*)key, N_BLOCK, (uint8_t*)iv));


    info_os("extimated cipher size 1: [%d]", aesLib.get_cipher_length(SIZE1));
    info_os("extimated cipher64 size 1: [%d]", aesLib.get_cipher64_length(SIZE1));    
    c1 = (char *)calloc(10*SIZE1,1);
    info_os("real cipher size 1: [%d]", aesLib.encrypt((uint8_t*)b1, SIZE1, c1, (uint8_t*)key, N_BLOCK, (uint8_t*)iv));
    
}

void loop() {
  // put your main code here, to run repeatedly:

}
