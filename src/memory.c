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

//MELHORAR!!
void* create_shared_memory(char* name, int size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    
    // char temp_name[256];
    // snprintf(temp_name, sizeof(temp_name), "%s_%u", name, getuid());
    // name = strdup(temp_name);
    printf("shared memory created with following name: %s\n", name);
    void * temp = mmap(NULL, size, protection, visibility, -1, 0);
    if (temp == MAP_FAILED) {
        printf("mmap failed for: %s\n", name);
        free(name);
        return NULL;
    }
    memset(temp, 0, size);
    return temp;
}

void deallocate_dynamic_memory(void* ptr)
{
    if (ptr)
        free(ptr);
}

void destroy_shared_memory(char* name, void* ptr, int size)
{
    if(ptr)
        munmap(ptr, size);//deallocate shared mem
    shm_unlink(name);
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
    int in_pos = buffer->ptrs->in;
    int out_pos = buffer->ptrs->out;

    if (next == buffer->ptrs->out)
        return;
    
    
        
        
    buffer->buffer[out_pos] = *tx;
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
    int next = (buffer->ptrs->out + 1) % buffer_size;

    if(in_pos == out_pos){
        tx->id = -1;
        return;
    }
    
    tx->id = buffer->buffer[out_pos].id;
    tx->src_id = buffer->buffer[out_pos].src_id;
    tx->dest_id = buffer->buffer[out_pos].dest_id;
    tx->amount = buffer->buffer[out_pos].amount;
    tx->wallet_signature = buffer->buffer[out_pos].wallet_signature;
    tx->server_signature = buffer->buffer[out_pos].server_signature;
    
    buffer->buffer[out_pos].id = -1;
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