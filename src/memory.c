#include "../inc/memory.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

void* allocate_dynamic_memory(int size)
{
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

//MELHORAR
void* create_shared_memory(char* name, int size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    
    unsigned int userID = getuid();
    char userID_str[10];
    snprintf(userID_str, sizeof(userID_str), "%u", userID);
    char temp_name[256];
    snprintf(temp_name, sizeof(temp_name), "%s%s", name, userID_str);
    name = strdup(temp_name);
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void deallocate_dynamic_memory(void* ptr)
{
    if (ptr)
        free(ptr);
}

void destroy_shared_memory(char* name, void* ptr, int size)
{
    munmap(ptr, size);//libertar shared mem
    shm_unlink(name);//desmapear(supostamente opcional mas boa pratica)
}

//Escreve uma transação no buffer de memória partilhada entre a Main e as carteiras.
void write_main_wallets_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx)
{
    int i;
    int found = 0;
    for(i = 0; i < buffer_size && found == 0; i++){
        if(!buffer->ptrs[i]){
            buffer->buffer[i] = *tx;
            buffer->ptrs[i] = 1;
            found = 1;
        }
    }
}

void write_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx)
{
    //check for full buff(circular): if the position next to in is out or (in->next == out)
    //use buffer size instead of one, (((in->next) % buffersize) == out)
    int next = (buffer->ptrs->in + 1 ) % buffer_size;

    if (next == buffer->ptrs->out)
        return;

    buffer->buffer[buffer->ptrs->in] = *tx;
    buffer->ptrs->in = next;
}

void write_servers_main_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx)
{
    int i;
    int found = 0;
    for(i = 0; i < buffer_size && found == 0; i++){
        if(!buffer->ptrs[i]){
            buffer->buffer[i] = *tx;
            buffer->ptrs[i] = 1;
            found = 1;
        }
    }
}

void read_main_wallets_buffer(struct ra_buffer* buffer, int wallet_id, int buffer_size, struct transaction* tx)
{
    int i;
    int found = 0; 

    for (i = 0; i < buffer_size && found == 0; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].src_id == wallet_id) {
            *tx = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            found = 1;
        }
    }
    if (!found) {
        tx->id = -1;
    }
}

void read_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx)
{
    int in_pos = buffer->ptrs->in;
    int out_pos = buffer->ptrs->out;
    int next = (buffer->ptrs->in + 1) % buffer_size;

    if(in_pos == out_pos){
        tx->id = -1;
        return;
    }
    *tx = buffer->buffer[out_pos];

    buffer->ptrs->out = next;
}

void read_servers_main_buffer(struct ra_buffer* buffer, int tx_id, int buffer_size, struct transaction* tx)
{
    int i;
    int found = 0; 

    for (i = 0; i < buffer_size && found == 0; i++)
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].id == tx_id) {
            *tx = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            found = 1;
        }
    if (!found) {
        tx->id = -1;
    }
}