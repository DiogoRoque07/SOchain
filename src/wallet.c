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


void wprintTX(struct transaction tx){
    printf(YELLOW "Transaction ID: " RESET "%d | " 
        GREEN "Source ID: " RESET "%d | " 
        CYAN "Destination ID: " RESET "%d | " 
        MAGENTA "Amount: " RESET "%.2f | " 
        BLUE "Wallet Signature: " RESET "%d | " 
        RED "Server Signature: " RESET "%d\n",
        tx.id, tx.src_id, tx.dest_id, tx.amount, tx.wallet_signature, tx.server_signature);
}

int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs)
{
    int num_transacs = 0;
    while(*info->terminate == 0){
        // write(1, "\nprocess loop\n", 15);
        struct transaction tx;
        wallet_receive_transaction(&tx, wallet_id, info, buffs);
        if(tx.id == -1)
        {
            // write(1, "\ngoing to sleep\n", 17);
            usleep(10000);
        }
        else {
            wallet_process_transaction(&tx, wallet_id, info);
            wallet_send_transaction(&tx, info, buffs);
            num_transacs++;
        }
    }   
    // printf("\nexiting wallet\n");
    // write(1, "\nexiting wallet\n", 17);
    return num_transacs;
}

void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs)
{
    if(*info->terminate == 1)
        return;
    read_main_wallets_buffer(buffs->buff_main_wallets, wallet_id, info->buffers_size, tx);
}

//VER ESTA SE ESTÁ BEM

void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info)
{
        // static int count_signed_transactions = 0; //isso conta para nada visto que nao e retornado
        // (void)info;//colocado para as flags -Wall -Werror -Wextra   
        if (tx->src_id == wallet_id) {
            // (void)info;
            tx->wallet_signature = wallet_id;
            // count_signed_transactions++;
            info->wallets_stats[wallet_id]++;
            // printf("wallet processed transaction:\n");
            // wprintTX(*tx);
            
        }
}

//VER ESTA SE ESTÁ BEM

void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs)
{
    if(*info->terminate == 1){
        return;
    }
    write_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, tx);
}
