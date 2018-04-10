#include "hashgen.h"
#include <fstream>
#include <cstring>
#include <gcrypt.h>
#include <sstream> 
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>

using namespace std;

string hashGenerator(string encryptContent, size_t txtLength){
    
    size_t index;
    char * hashBuffer = (char*)malloc(33);
    char * textBuffer = (char*)malloc(txtLength+1);
    char * fileContentLine = (char*)malloc(txtLength);
    string hashCode = " ";
    char hash[64];
    memset(hashBuffer, 0, 33);

    strncpy(textBuffer, encryptContent.c_str(), txtLength);

    gcry_md_hash_buffer(
        GCRY_MD_SHA256, // gcry_cipher_hd_t
        hashBuffer,    // void *
        textBuffer,    // const void *
        txtLength      // size_t
    );

    for (index = 0; index<32; index++){
       sprintf(&hash[index*2],"%02X", (unsigned char)hashBuffer[index]);
    }
    
    hashCode = hash;
    free(hashBuffer);
    free(textBuffer);
    free(fileContentLine);
    return hashCode;
}

string generatePasswordHash(string username, string password, string passSalt){
    
    string passwordHash;
    string hashContent;
    int i;
    
    for(i=0;i<=2;i++){                        // r=3;
        hashContent.append(password);
        hashContent.append(passSalt);
        
        hashContent = hashGenerator(hashContent,hashContent.size());
    }
    
    return hashContent;
}

string userSaltGenertator(void){
    
    string salt;
    
    // Disable stdout buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    unsigned int randval;
  
    FILE *f;

    f = fopen("/dev/urandom", "r");
    fread(&randval, sizeof(randval), 1, f);
    fclose(f);
    
    ostringstream convert;   // stream used for the conversion
    convert << randval;      
    salt = convert.str();
    
    fflush(stdout);
    usleep(1);
    
    return salt;
}