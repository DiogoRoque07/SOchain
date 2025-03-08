#include "../inc/main.h"

/*
args usage: init_balance n_wallets n_servers buff_size max_txs
*/

int main(int argc, char *argv[])
{
    // init data structures
    struct info_container *info = allocate_dynamic_memory(sizeof(struct info_container));
    struct buffers *buffs = allocate_dynamic_memory(sizeof(struct buffers));

    // execute main code
    main_args(argc, argv, info);
    create_dynamic_memory_structs(info, buffs);
    create_shared_memory_structs(info, buffs);
    create_processes(info, buffs);
    user_interaction(info, buffs);

    // release memory before terminating
    destroy_shared_memory_structs(info, buffs);
    destroy_dynamic_memory_structs(info, buffs);
    return 0;
}

double balance(int id)
{
    return -1;
}

int transaction(int src_id, int dest_id, double amount)
{
    int id = 0;
    return id;
}

// dar print aos comandos disponiveis
void help()
{
}

// dar print ao estado das variaveis do info_container
void status()
{
}

// terminar a execução do sistema SOchain
int end()
{
    return 0;
}