#include "../inc/wallet.h"


int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs){
    int num_transacs = 0;
    while(info->terminate == 0){
        struct transaction transac;

        read_main_wallets_buffer(buffs->buff_main_wallets, wallet_id, info->buffers_size, &transac);
        
        if (transac.id == wallet_id){
            wallet_process_transaction(&transac, wallet_id, info);
            wallet_send_transaction(&transac, info, buffs);
            num_transacs++;
        }
    }

    return num_transacs;
}

void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs){

}

void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info){

}

void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    
}