/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#include "Memory.h"
#include "Sleep.h"

#define MEM_SIZE 64

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: TestWrite.ex key floatvaluetowrite\n");
        return -1;
    }

    int  key  = atoi(argv[1]);
    char *mem = (char *)SharedMemoryAlloc(key, MEM_SIZE);
    if(mem == NULL){
        printf("Failed to allocated shared memory with key: %d\n", key);
        return -1;
    }
    if(argc > 2){
        printf("Going to write on memory mapped on key: %d at %p\n", key, mem);
        //*((float *)mem) = atof(argv[2]);
        while(1)
        *((int32*)mem) = atoi(argv[2]);
    }
    SharedMemoryFree(mem);
}


