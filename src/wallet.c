#include "../inc/wallet.h"
#include <unistd.h>
#include <stdio.h>

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

void wprintTX(struct transaction tx)
{
    printf(YELLOW "Transaction ID: " RESET "%d | " GREEN "Source ID: " RESET "%d | " CYAN "Destination ID: " RESET "%d | " MAGENTA "Amount: " RESET "%.2f | " BLUE "Wallet Signature: " RESET "%d | " RED "Server Signature: " RESET "%d\n",
           tx.id, tx.src_id, tx.dest_id, tx.amount, tx.wallet_signature, tx.server_signature);
}

int execute_wallet(int wallet_id, struct info_container *info, struct buffers *buffs)
{
    int num_transacs = 0;
    while (*info->terminate == 0)
    {
        struct transaction tx;
        wallet_receive_transaction(&tx, wallet_id, info, buffs);
        if (tx.id == -1)
            usleep(10000);
        else
        {
            wallet_process_transaction(&tx, wallet_id, info);
            wallet_send_transaction(&tx, info, buffs);
            num_transacs++;
        }
    }
    return num_transacs;
}

void wallet_receive_transaction(struct transaction *tx, int wallet_id, struct info_container *info, struct buffers *buffs)
{
    if (*info->terminate == 1)
        return;
    read_main_wallets_buffer(buffs->buff_main_wallets, wallet_id, info->buffers_size, tx);
}

void wallet_process_transaction(struct transaction *tx, int wallet_id, struct info_container *info)
{

    if (tx->src_id == wallet_id)
    {
        tx->wallet_signature = wallet_id;
        info->wallets_stats[wallet_id]++;
    }
}

void wallet_send_transaction(struct transaction *tx, struct info_container *info, struct buffers *buffs)
{
    if (*info->terminate == 1)
        return;
    write_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, tx);
}
