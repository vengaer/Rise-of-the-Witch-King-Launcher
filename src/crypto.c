#include "crypto.h"
#include <openssl/md5.h>
#include <stdio.h>

#define CHUNK_SIZE 1024

bool md5sum(char const* restrict filename, char* restrict checksum) {
    int i, num_bytes;
    FILE* fp = fopen(filename, "rb");

    if(!fp) {
        errdispf("%s could not be opened for hashing\n", filename);
        return false;
    }

    char data[CHUNK_SIZE];
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX context;
    MD5_Init(&context);

    while((num_bytes = fread(data, 1, CHUNK_SIZE, fp)))
        MD5_Update(&context, data, num_bytes);

    MD5_Final(hash, &context);
    fclose(fp);

    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&checksum[i*2], "%02x", (unsigned int)hash[i]);

    return true;
}

