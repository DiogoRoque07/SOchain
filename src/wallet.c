#include "../inc/wallet.h"
#include <unistd.h>

int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs){
    int num_transacs = 0;
    while(*info->terminate == 0){
        // struct transaction transac;

        wallet_receive_transaction(buffs->buff_main_wallets->buffer, wallet_id, info, buffs);
        if(buffs->buff_main_wallets->buffer->id == -1)
            usleep(100000);
        if (buffs->buff_main_wallets->buffer->id == wallet_id){
            wallet_process_transaction(buffs->buff_main_wallets->buffer, wallet_id, info);
            wallet_send_transaction(buffs->buff_main_wallets->buffer, info, buffs);
            num_transacs++;
        }
    }
    return num_transacs;
}

void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs){
    if(*info->terminate == 1)
        return;
    read_main_wallets_buffer(buffs->buff_main_wallets, wallet_id, info->buffers_size, tx);
    

}

// Função que assina uma transação comprovando que a carteira de origem src_id da transação corresponde
// ao wallet_id. Atualiza o campo wallet_signature da transação e incrementa o contador de transações
// assinadas pela carteira.
void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info){
    
}

void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    (void)tx;
    (void)buffs;
    (void) info;
}