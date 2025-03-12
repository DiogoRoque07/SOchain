#include "../inc/server.h"

int execute_server(int server_id, struct info_container *info, struct buffers *buffs)
{
    int trx_remaining = info->max_txs;
    while (info->terminate == 0 && trx_remaining > 0)
    {

        server_receive_transaction(buffs->buff_wallets_servers, info, buffs);
        if (buffs->buff_wallets_servers == -1)
            usleep(100000);
        if (server_id == buffs->buff_wallets_servers->buffer->id)
        {
            server_process_transaction(buffs->buff_wallets_servers->buffer, server_id, info);
            server_send_transaction(buffs->buff_servers_main->buffer, info, buffs);
            trx_remaining--;
        }
    }
    return trx_remaining;
}

void server_receive_transaction(struct transaction *tx, struct info_container *info, struct buffers *buffs)
{
    read_wallets_servers_buffer(buffs->buff_wallets_servers->buffer, info->buffers_size, tx);
}

void server_process_transaction(struct transaction *tx, int server_id, struct info_container *info)
{
}

void server_send_transaction(struct transaction *tx, struct info_container *info, struct buffers *buffs)
{
}