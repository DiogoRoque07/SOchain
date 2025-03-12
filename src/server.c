#include "../inc/server.h"
#include <unistd.h>

int execute_server(int server_id, struct info_container *info, struct buffers *buffs)
{
    int trx_remaining = info->max_txs;
    while (info->terminate == 0 && trx_remaining > 0)
    {

        server_receive_transaction(buffs->buff_wallets_servers->buffer, info, buffs);
        if(buffs->buff_wallets_servers->buffer->id == -1)
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

void server_receive_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    if (*info->terminate == 1)
        return;
    read_wallets_servers_buffer(buffs->buff_wallets_servers, info->buffers_size, tx);
}

// Função que processa uma transação tx, verificando a validade dos identificadores das carteiras de origem e destino,
// dos fundos da carteira de origem e a assinatura da carteira de origem. Atualiza os saldos das carteiras envolvidas,
// adiciona a assinatura do servidor e incrementa o contador de transações processadas pelo servidor.
void server_process_transaction(struct transaction* tx, int server_id, struct info_container* info){
    if(tx->src_id < 0 || tx->src_id > info->n_wallets || tx->dest_id < 0 || tx->dest_id > info->n_wallets
        || info->balances[tx->src_id] >= tx->amount
           /*verificar tbm a assinatura da carteira de origem (src wallet)*/ )
        return;
    
    info->balances[tx->src_id] -= tx->amount;
    info->balances[tx->dest_id] += tx->amount;
// adicionar assinatura do servido, acho que é isto:
    tx->server_signature = server_id;
    wallet_process_transaction(tx, tx->src_id, info);//ns se isto tem de estar aqui
    info->servers_stats[server_id]++;

}

// Função que escreve uma transação correta processada no buffer de memória partilhada entre os servidores e a main.
// Caso o servidor não tenha assinado a transação, essa não será escrita pois significa que a transação era inválida.
// Se não houver espaço no buffer, a transação não é enviada e o recibo perde-se.
void server_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs){
    if (!tx->server_signature || tx->server_signature == -1)
        return;
    write_servers_main_buffer(buffs->buff_servers_main, info->buffers_size, tx);
}