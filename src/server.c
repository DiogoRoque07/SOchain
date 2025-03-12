#include "../inc/server.h"
#include <unistd.h>

int execute_server(int server_id, struct info_container* info, struct buffers* buffs){
    int num_transacs = 0;
    while (info->terminate == 0){

        server_receive_transaction(buffs->buff_wallets_servers->buffer, info, buffs);
        if(buffs->buff_wallets_servers->buffer->id == -1)
            usleep(100000);
        if(server_id == buffs->buff_wallets_servers->buffer->id){
            server_process_transaction(buffs->buff_wallets_servers->buffer, server_id, info);
            server_send_transaction(buffs->buff_servers_main->buffer, info, buffs);
            num_transacs++;
        }
    }
    return num_transacs;
}

void server_receive_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    (void)tx;
    read_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, buffs->buff_wallets_servers->buffer);
}

void server_process_transaction(struct transaction* tx, int server_id, struct info_container* info){
    (void)tx;
    (void)server_id;
    (void) info;
}

void server_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    (void)tx;
    (void)buffs;
    (void) info;
}