/**
 * @autores (grupo SO-003)
 * OMEIR HAROON (61810)
 * Isa David    (61871)
 * Diogo Roque  (61857)
 */

#include "../inc/server.h"
#include "../inc/wallet.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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

void sprintTX(struct transaction tx)
{
    printf(YELLOW "Transaction ID: " RESET "%d | " GREEN "Source ID: " RESET "%d | " CYAN "Destination ID: " RESET "%d | " MAGENTA "Amount: " RESET "%.2f | " BLUE "Wallet Signature: " RESET "%d | " RED "Server Signature: " RESET "%d\n",
           tx.id, tx.src_id, tx.dest_id, tx.amount, tx.wallet_signature, tx.server_signature);
}

int execute_server(int server_id, struct info_container *info, struct buffers *buffs)
{
    int trxs = 0;
    while (*info->terminate == 0)
    {
        struct transaction tx;
        memset(&tx, 0, sizeof(tx));
        server_receive_transaction(&tx, info, buffs);
        if (tx.id == -1)
        {
            usleep(10000);
        }
        if (tx.id >= 0)
        {
            server_process_transaction(&tx, server_id, info);
            server_send_transaction(&tx, info, buffs);
            trxs++;
        }
    }
    return trxs;
}

void server_receive_transaction(struct transaction *tx, struct info_container *info, struct buffers *buffs)
{
    if (*info->terminate == 1)
    {
        return;
    }

    read_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, tx);
}

void server_process_transaction(struct transaction *tx, int server_id, struct info_container *info)
{
    info->servers_stats[server_id]++;
    if (tx->src_id < 0 || tx->src_id >= info->n_wallets || tx->dest_id < 0 || tx->dest_id >= info->n_wallets || info->balances[tx->src_id] < tx->amount)
        tx->server_signature = -1;
    else
        tx->server_signature = server_id;
}

void server_send_transaction(struct transaction *tx, struct info_container *info, struct buffers *buffs)
{
    if (tx->server_signature == -1)
        return;

    write_servers_main_buffer(buffs->buff_servers_main, info->buffers_size, tx);
}