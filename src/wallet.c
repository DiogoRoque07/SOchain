#include "../inc/wallet.h"
#include <unistd.h>

int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs){
    int num_transacs = 0;
    while(*info->terminate == 0){
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

/* Função que lê uma transação do buffer de memória partilhada entre a main e as carteiras apenas 
 * caso o src_id da transação seja o seu próprio id. Antes de tentar ler a transação, deve verificar 
 * se info->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
 */
void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs){
    if(*info->terminate == 1)
        return;
    read_main_wallets_buffer(buffs->buff_main_wallets, wallet_id, info->buffers_size, tx);
}

//VER ESTA SE ESTÁ BEM

void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info){
        // static int count_signed_transactions = 0; //isso conta para nada visto que nao e retornado
        if (tx->src_id == wallet_id) {
            // (void)info;
            tx->wallet_signature = generate_signature(tx, wallet_id, info);//de onde vem o generate signature?
            // count_signed_transactions++;
        }
}

//VER ESTA SE ESTÁ BEM

void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    if(*info->terminate == 1){
        return;
    }
    write_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, tx);
}
