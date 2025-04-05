/**
 * @autores (grupo SO-003)
 * OMEIR HAROON (61810)
 * Isa David    (61871)
 * Diogo Roque  (61857)
 */



#include <stdio.h>
#include "../inc/main.h"
#include "../inc/process.h"
#include "../inc/wallet.h"
#include "../inc/server.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

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
#define LIGHT_BLUE "\033[38;5;117m"
#define LIGHT_GREEN "\033[92m"
#define LIGHT_YELLOW "\033[93m"
#define LIGHT_RED "\033[91m"
#define LIGHT_MAGENTA "\033[95m"
#define LIGHT_CYAN "\033[96m"
#define GRAY "\033[90m"

static int G_correct_args = 0;

/**
    * prints the reciept (or transaction information) in a readable way
 */
void mprintTX(struct transaction tx){
    printf(YELLOW "Transaction ID: " RESET "%d | " 
        GREEN "Source ID: " RESET "%d | " 
        CYAN "Destination ID: " RESET "%d | " 
        MAGENTA "Amount: " RESET "%.2f | " 
        BLUE "Wallet Signature: " RESET "%d | " 
        RED "Server Signature: " RESET "%d\n",
        tx.id, tx.src_id, tx.dest_id, tx.amount, tx.wallet_signature, tx.server_signature);
}


int main(int argc, char *argv[])
{
    struct info_container *info = allocate_dynamic_memory(sizeof(struct info_container));
    struct buffers *buffs = allocate_dynamic_memory(sizeof(struct buffers));

    main_args(argc, argv, info);
    create_dynamic_memory_structs(info, buffs);
    create_shared_memory_structs(info, buffs);
    create_processes(info, buffs);


    user_interaction(info, buffs);

    destroy_shared_memory_structs(info, buffs);
    destroy_dynamic_memory_structs(info, buffs);
    return 0;
}

/**
 * Check the validity of the arguments given
 */
int check_args(int argc, char *argv[])
{
    int ret = 0;
    if (argc != 6)
    {
        fprintf(stderr, RED "NUMBER OF ARGUMENTS INCORRECT" YELLOW "(example: ./SOchain 200 5 5 5 10)\n" RESET);
        G_correct_args = -1;
        return (-1);
    }
    // init_balance
    if (atoi(argv[1]) <= 0)
    {
        fprintf(stderr,RED "INITIAL BALANCE NEEDS TO BE POSITIVE\n" RESET);
        ret = -1;
    }

    // n_wallets
    if (atoi(argv[2]) <= 0)
    {
        fprintf(stderr,RED "NUMBER OF WALLETS NEEDS TO BE POSITIVE\n" RESET);
        ret = -1;
    }

    // n_servers
    if (atoi(argv[3]) <= 0)
    {
        fprintf(stderr,RED "NUMBER OF SERVERS NEEDS TO BE POSITIVE\n" RESET);
        ret = -1;
    }

    // buff_size
    if (atoi(argv[4]) <= 0)
    {
        fprintf(stderr,RED "BUFFER SIZE NEEDS TO BE POSITIVE\n" RESET);
        ret = -1;
    }

    // max_txs
    if (atoi(argv[5]) <= 0)
    {
        fprintf(stderr,RED "MAX TRANSACTIONS NEEDS TO BE POSITIVE\n" RESET);
        ret = -1;
    }
    G_correct_args = ret;

    return ret;
}


void main_args(int argc, char *argv[], struct info_container *info)
{
    if (check_args(argc, argv) == -1)
    {
        printf("[" BLUE"MAIN" RESET "]" RED " Parâmetros incorretos!\n" RESET);
        return;
    }

    if (check_args(argc, argv) == 0)
    {
        printf("[" BLUE"MAIN" RESET "]" GREEN " Parâmetros corretos!\n" RESET);
        info->init_balance = atof(argv[1]);
        info->n_wallets = atoi(argv[2]);
        info->n_servers = atoi(argv[3]);
        info->buffers_size = atoi(argv[4]);
        info->max_txs = atoi(argv[5]);
        
        printf("[" BLUE "MAIN" RESET "]" YELLOW " init_balance"RESET": %.2f, " YELLOW"n_wallets"RESET": %d, "YELLOW"n_servers"RESET": %d, "YELLOW"buffers_size"RESET": %d, "YELLOW"max_txs"RESET": %d\n", 
                info->init_balance, info->n_wallets, info->n_servers, info->buffers_size, info->max_txs);
    }
}

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    if(G_correct_args == -1){
        return;
    }
    info->wallets_pids = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));
    buffs->buff_main_wallets = allocate_dynamic_memory(info->buffers_size * sizeof(struct ra_buffer));
    buffs->buff_wallets_servers = allocate_dynamic_memory(info->buffers_size * sizeof(struct circ_buffer));
    buffs->buff_servers_main = allocate_dynamic_memory(info->buffers_size * sizeof(struct ra_buffer));

}

void create_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    if(G_correct_args == -1)
        return;
    buffs->buff_main_wallets->buffer = create_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_main_wallets->ptrs = create_shared_memory(ID_SHM_MAIN_WALLETS_PTR, info->buffers_size *  sizeof(int));
   
    buffs->buff_wallets_servers->buffer = create_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_wallets_servers->ptrs = create_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, sizeof(struct pointers));

    buffs->buff_servers_main->buffer = create_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, info->buffers_size * sizeof(struct transaction));
    buffs->buff_servers_main->ptrs = create_shared_memory(ID_SHM_SERVERS_MAIN_PTR, info->buffers_size *  sizeof(int));
    
    info->wallets_stats = create_shared_memory(ID_SHM_WALLETS_STATS, sizeof(int) * info->n_wallets);
    info->servers_stats = create_shared_memory(ID_SHM_SERVERS_STATS, sizeof(int) * info->n_servers);
    
    info->balances = create_shared_memory(ID_SHM_BALANCES, sizeof(int) * info->n_wallets);
    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));

    *info->terminate = 0;
}

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    deallocate_dynamic_memory(info->servers_pids);
    deallocate_dynamic_memory(info->wallets_pids);
    
    deallocate_dynamic_memory(info);
    
    deallocate_dynamic_memory(buffs->buff_main_wallets);
    deallocate_dynamic_memory(buffs->buff_wallets_servers);
    deallocate_dynamic_memory(buffs->buff_servers_main);
    
    deallocate_dynamic_memory(buffs);
}

void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    if(G_correct_args == -1)
        return;
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_PTR, buffs->buff_main_wallets->ptrs, info->buffers_size *  sizeof(int));
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, buffs->buff_main_wallets, info->buffers_size * sizeof(struct ra_buffer));

    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, buffs->buff_wallets_servers->ptrs, sizeof(struct pointers));
    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, buffs->buff_wallets_servers, info->buffers_size * sizeof(struct circ_buffer));

    destroy_shared_memory(ID_SHM_SERVERS_MAIN_PTR, buffs->buff_servers_main->ptrs, info->buffers_size *  sizeof(int));
    destroy_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, buffs->buff_servers_main,  info->buffers_size * sizeof(struct ra_buffer));

    destroy_shared_memory(ID_SHM_WALLETS_STATS, info->wallets_stats, sizeof(int) * info->n_wallets );
    destroy_shared_memory(ID_SHM_SERVERS_STATS, info->servers_stats, sizeof(int) * info->n_servers);

    destroy_shared_memory(ID_SHM_BALANCES, info->balances, sizeof(int) * info->n_wallets);
    destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));
}

void create_processes(struct info_container *info, struct buffers *buffs)
{
    if(G_correct_args == -1)
        return;
    for (int i = 0; i < info->n_wallets; i++)
    {
        info->wallets_pids[i] = launch_wallet(i, info, buffs);
    }

    for (int i = 0; i < info->n_wallets; i++)
        info->balances[i] = info->init_balance;

    for (int i = 0; i < info->n_servers; i++){
        info->servers_pids[i] = launch_server(i, info, buffs);
    }
}

/**
 * Returns a hash for the user interaction commands
 * bal -> 1
 * trx -> 2
 * rcp -> 3
 * help -> 4
 * stat -> 5
 * end -> 6
 * undefined command -> 7
 * undefined command not meant to
 */
int user_hash(char op[])
{

    if (strcmp(op, "bal") == 0)
        return 1;
    else if (strcmp(op, "trx") == 0)
        return 2;
    else if (strcmp(op, "rcp") == 0)
        return 3;
    else if (strcmp(op, "help") == 0)
        return 4;
    else if (strcmp(op, "stat") == 0)
        return 5;
    else if (strcmp(op, "end") == 0)
        return 6;
    else
        return 7;
}

char * toLowerCase(char * s){
    char* temp = s;
    int i = 0;
    while(temp[i] != '\n' && temp[i] != ' ' && temp[i] != '\0') {
        if (temp[i] >= 'A' && temp[i] <= 'Z') {
            s[i] = temp[i] + (32);
        } else {
            s[i] = temp[i];
        }
        i++;
    }
    return s;
}

void user_interaction(struct info_container *info, struct buffers *buffs)
{
    if(G_correct_args == -1 ||*info->terminate == 1)
        return;

    int *tx_counter = malloc(sizeof(int));
    *tx_counter = 0;
    while (1)
    {
        char *input = malloc(sizeof(char *));
        printf("[" BLUE "MAIN" RESET"] Introduzir operação: ");
        scanf( "%s", input);
        printf("\n");
        input = toLowerCase(input);
        char *words = strtok(input, " ");
        if (!words)
            continue;
        char *op = words;

        switch (user_hash(op))
        {
        case 1: // "bal" command
            print_balance(info);
            break;
        case 2: // "trx" command
            create_transaction(tx_counter, info, buffs);
            break;
        case 3: // "rcp" command
            receive_receipt(info, buffs);
            break;
        case 4: // "help" command
            help();
            break;
        case 5: // "stat" command
            print_stat(*tx_counter, info);
            break;
        case 6: // "end" command
        
            if(input)
                free(input);
            if(tx_counter)
                free(tx_counter);
            end_execution(info, buffs);

            return;
        default:
            printf("["RED "MAIN" RESET "] Operação:"RED " %s" RESET  " não reconhecida, insira '"YELLOW"help"RESET"' para assistência.\n", input);
            break;
        }

        if(input)
            free(input);
    }
    if(tx_counter)
        free(tx_counter);
}

void write_final_statistics(struct info_container *info)
{
    printf("["BLUE "MAIN" RESET "] A "RED"encerrar"RESET" a execução do SOchain! As estatísticas da execução são:\n");
    for (int i = 0; i < info->n_wallets; i++)
        printf("["BLUE "MAIN" RESET "] A carteira "YELLOW"%d"RESET" assinou "GREEN"%d"RESET" transações e terminou com "GREEN"%.2f"RESET" SOT!\n", i, info->wallets_stats[i], info->balances[i]);
    for (int i = 0; i < info->n_servers; i++)
        printf("["BLUE "MAIN" RESET "] Servidor "YELLOW"%d"RESET" assinou "GREEN"%d"RESET" transações\n", i, info->servers_stats[i]);
}

void end_execution(struct info_container *info, struct buffers *buffs)
{
    *info->terminate = 1;
    (void)buffs;//para evitar problemas de compilação com a flag WEXTRA
    wait_processes(info);
    write_final_statistics(info);
}

void wait_processes(struct info_container *info)
{
    for (int i = 0; i < info->n_wallets; i++)
        wait_process(info->wallets_pids[i]);
    for (int i = 0; i < info->n_servers; i++)
        wait_process(info->servers_pids[i]);

}

void print_balance(struct info_container *info)
{
    int wallet_id;
    scanf("%d", &wallet_id);
    if (wallet_id >= 0 && wallet_id < info->n_wallets)
        printf("["BLUE "MAIN" RESET "] O saldo da carteira "YELLOW"%d"RESET" é de "GREEN"%.2f"RESET" SOT atualmente.\n", wallet_id, info->balances[wallet_id]);
    else
        printf("["RED "MAIN" RESET "] Não foi possível verificar o saldo da carteira "YELLOW"%d"RESET"\n", wallet_id);
}

/**
 * Caso varias servidores processam a mesma transação estes não ocupam lugar no buffer
 */
void remove_extras_from_buffer(struct info_container *info, struct buffers *buffs){

    for(int i = 0; i < info->buffers_size; i++){
        if(buffs->buff_servers_main->ptrs[i] == 1){

            int id1 = buffs->buff_servers_main->buffer[i].id;
            for (int j = 0; j < info->buffers_size; j++)
            {
                if(i != j && buffs->buff_servers_main->ptrs[i] == 1){
                    if(id1 == buffs->buff_servers_main->buffer[j].id){
                        buffs->buff_servers_main->ptrs[j] = 0;
                    }
                }
            }
            
        }
    }
}

void create_transaction(int *tx_counter, struct info_container *info, struct buffers *buffs)
{
    struct transaction tx;
    (*tx_counter)++;
    
    tx.id = *tx_counter - 1;
    scanf("%d", &tx.src_id);
    scanf("%d", &tx.dest_id);
    scanf("%f", &tx.amount);
    if (*tx_counter > info->max_txs) {
        printf("["RED "MAIN" RESET "] Número máximo de transações (%d) atingido. Não é possível criar uma nova transação.\n", info->max_txs);
        return;
    }

    if (tx.src_id < 0 || tx.dest_id < 0 || !tx.amount) {
        printf("["RED "MAIN" RESET "] Argumentos insuficientes para criar uma transação. Use: "YELLOW"trx src_id dest_id amount\n"RESET);
        return;
    }
    printf("["BLUE "MAIN" RESET "] A transação %d foi criada para transferir %.2f SOT da carteira %d para a carteira %d!\n", tx.id + 1, tx.amount ,tx.src_id, tx.dest_id);
    write_main_wallets_buffer(buffs->buff_main_wallets, info->buffers_size, &tx);
    usleep(50000);
    if (tx.src_id < 0 || tx.src_id >= info->n_wallets || tx.dest_id < 0 || tx.dest_id >= info->n_wallets || info->balances[tx.src_id] < tx.amount)
    {
        tx.server_signature = -1;
        printf("["RED "MAIN" RESET "] A transação %d foi criada para transferir %.2f SOT da carteira %d para a carteira %d "RED"falhou por algum motivo!\n"RESET, tx.id + 1, tx.amount ,tx.src_id, tx.dest_id);
        return;
    }
    info->balances[tx.src_id] -= tx.amount;
    info->balances[tx.dest_id] += tx.amount;
    printf("["BLUE "MAIN" RESET "] A transação %d para transferir %.2f SOT da carteira %d para a carteira %d "GREEN"foi efetuada!\n"RESET, tx.id + 1, tx.amount ,tx.src_id, tx.dest_id);

    remove_extras_from_buffer(info, buffs);
}

void receive_receipt(struct info_container *info, struct buffers *buffs)
{
    int transac_id;
    scanf("%d", &transac_id);
    for (int i = 0; i < info->buffers_size; i++)
    {
        if(buffs->buff_servers_main->ptrs[i] == 1)
        {
            if(buffs->buff_servers_main->buffer[i].id == transac_id)
            {
                struct transaction *tx = &buffs->buff_servers_main->buffer[i];
                mprintTX(*tx);
                return;
            }
        }
    }
} 

void print_stat(int tx_counter, struct info_container *info)
{
    char tabulation[] = "        ";
    printf(LIGHT_MAGENTA"- Configuração inicial:\n");
    printf(ORANGE "%sPropriedade     Valor\n", tabulation);
    printf(YELLOW "%sinit_balance    "GREEN"%.2f\n", tabulation, info->init_balance);
    printf(YELLOW "%sn_wallets       "GREEN"%d\n", tabulation, info->n_wallets);
    printf(YELLOW"%sn_servers       "GREEN"%d\n", tabulation, info->n_servers);
    printf(YELLOW"%sbuffer_size     "GREEN"%d\n", tabulation, info->buffers_size);
    printf(YELLOW"%smax_txs         "GREEN"%d\n", tabulation, info->max_txs);
    printf(LIGHT_MAGENTA"- Variáveis atuais:\n");
    printf(YELLOW"%stx_count        "GREEN"%d\n", tabulation, tx_counter);
    printf(YELLOW"%sterminate       "GREEN"%d\n", tabulation, *info->terminate);
    printf(LIGHT_MAGENTA"- Informações sobre as carteiras:\n");
    printf(ORANGE"%sCarteira        PID             Saldo           Transações Assinadas\n" RESET, tabulation);
    for (int i = 0; i < info->n_wallets; i++)
    {   
        char* balances = malloc(20 * sizeof(char));
        sprintf(balances,"%.2f SOT", info->balances[i]);
        printf(GREEN"%s%-16d"LIGHT_BLUE"%-16d"RESET"%-16s"GREEN"%-12d\n", tabulation, i, info->wallets_pids[i], balances ,info->wallets_stats[i]);
        free(balances);
    }
    printf(LIGHT_MAGENTA"- Informações sobre os servidores:\n");
    printf(ORANGE"%sServidor        PID             Transações Processadas\n"RESET, tabulation);
    for (int i = 0; i < info->n_servers; i++)
    {   
        printf("%s"GREEN"%-16d"LIGHT_BLUE"%-16d"GREEN"%-16d\n"RESET   , tabulation, i, info->servers_pids[i], info->servers_stats[i]);
    }
}

void help()
{
    printf("["BLUE "MAIN" RESET "]" ORANGE"  Operações disponíveis:\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  bal id - consultar o saldo da carteira identifcada por id\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  trx src_id dest_id amount - criar uma nova transação\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  rcp id - obter o comprovativo da transação de número id\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  help - imprime a informação sobre as operações disponíveis\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  stat - imprime as estatísticas atuais do SOchain\n"RESET);
    printf("["BLUE "MAIN" RESET "]" ORANGE"  end - termina a execução do SOchain\n"RESET);

}