/**
 * @autores (grupo SO-003)
 * OMEIR HAROON (61810)
 * Isa David    (61871)
 * Diogo Roque  (61857)
 */

#include "../inc/memory.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define PINK "\033[35m"
#define ORANGE "\033[38;5;214m"



void printTX(struct transaction tx){
    printf(YELLOW "Transaction ID: " RESET "%d | " 
           GREEN "Source ID: " RESET "%d | " 
           CYAN "Destination ID: " RESET "%d | " 
           MAGENTA "Amount: " RESET "%.2f | " 
           BLUE "Wallet Signature: " RESET "%d | " 
           RED "Server Signature: " RESET "%d\n",
           tx.id, tx.src_id, tx.dest_id, tx.amount, tx.wallet_signature, tx.server_signature);
}

void *allocate_dynamic_memory(int size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

// MELHORAR!!

void *create_shared_memory(char *name, int size)
{
    char temp_name[256];
    snprintf(temp_name, sizeof(temp_name), "%s_%u", name, getuid());
    int fd = shm_open(temp_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1){
        printf("shm_open failed for %s", temp_name);
        return NULL;
    }
    if (ftruncate(fd, size) == -1)
    {
        printf(RED"ftruncate failed"RESET);
        return NULL;
    }
    void* ptr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf(RED"shm-mmap"RESET);
        return NULL;
    }
    memset(ptr, 0, size);
    return ptr;
}

void deallocate_dynamic_memory(void *ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = NULL;
    }
}

void destroy_shared_memory(char *name, void *ptr, int size)
{
    if(!ptr)
        return;
    char temp_name[256];
    snprintf(temp_name, sizeof(temp_name), "%s_%u", name, getuid());
    munmap(ptr, size);
    shm_unlink(temp_name);

}

void write_main_wallets_buffer(struct ra_buffer *buffer, int buffer_size, struct transaction *tx)
{

    int i;
    int found = 0;
    for (i = 0; i < buffer_size && found == 0; i++)
    {
        if (!buffer->ptrs[i])
        {
            buffer->buffer[i] = *tx;
            buffer->ptrs[i] = 1;
            found = 1;
        }
    }
}

void write_wallets_servers_buffer(struct circ_buffer *buffer, int buffer_size, struct transaction *tx)
{

    int next = (buffer->ptrs->in + 1) % buffer_size;
    int out_pos = buffer->ptrs->out;

    if (next == buffer->ptrs->out)
        return;

    buffer->buffer[out_pos] = *tx;
    buffer->ptrs->in = next;
}

void write_servers_main_buffer(struct ra_buffer *buffer, int buffer_size, struct transaction *tx)
{
    int i;
    int found = 0;
    for (i = 0; i < buffer_size && found == 0; i++)
    {
        if (buffer->ptrs[i] == 0)
        {
            buffer->buffer[i] = *tx;
            buffer->ptrs[i] = 1;
            found = 1;
        }
    }
}

void read_main_wallets_buffer(struct ra_buffer *buffer, int wallet_id, int buffer_size, struct transaction *tx)
{
    int i;
    int found = 0;

    for (i = 0; i < buffer_size && found == 0; i++)
    {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].src_id == wallet_id)
        {
            *tx = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            found = 1;
        }
    }
    if (!found)
    {
        tx->id = -1;
        
    }
}

void read_wallets_servers_buffer(struct circ_buffer *buffer, int buffer_size, struct transaction *tx)
{
    int in_pos = buffer->ptrs->in;
    int out_pos = buffer->ptrs->out;
    int next = (buffer->ptrs->out + 1) % buffer_size;

    if (in_pos == out_pos)
    {
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

void read_servers_main_buffer(struct ra_buffer *buffer, int tx_id, int buffer_size, struct transaction *tx)
{
    int i;
    int found = 0;

    for (i = 0; i < buffer_size && found == 0; i++){
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].id == tx_id)
        {
            *tx = buffer->buffer[i];
            found = 1;
        }
    }
    if (!found)
    {
        tx->id = -1;
    }
}