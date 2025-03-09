#include "../inc/memory.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void* allocate_dynamic_memory(int size){
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void* create_shared_memory(char* name, int size){
    
    // Allows processes to read and write in the shared memory
    int protection = PROT_READ | PROT_WRITE;
    // Allows for shared memory but only to child processes of this one (no third-party access)
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    
    unsigned int userID = getuid();
    name = strcat(name, userID);
    return mmap(NULL, size, protection, visibility, -1,0);
    // int memfd = shm_open(name, O_CREAT | O_RDWR, 0666);
    // shmget(); //  https://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html
}

void deallocate_dynamic_memory(void* ptr){
    if (ptr)
        free(ptr);
}

void destroy_shared_memory(char* name, void* ptr, int size){
    munmap(ptr, size);//libertar shared mem
    shm_unlink(name);//desmapear(supostamente opcional mas boa pratica)
}

void write_main_wallets_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx){

}

void write_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx){

}

void write_servers_main_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx){

}

void read_main_wallets_buffer(struct ra_buffer* buffer, int wallet_id, int buffer_size, struct transaction* tx){

}

void read_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx){

}

void read_servers_main_buffer(struct ra_buffer* buffer, int tx_id, int buffer_size, struct transaction* tx){
    
}