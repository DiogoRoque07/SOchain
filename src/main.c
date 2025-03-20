#include <stdio.h>
#include "../inc/main.h"
#include "../inc/process.h"
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

/* Função que lê do stdin com o scanf apropriado para cada tipo de dados
 * e valida os argumentos da aplicação, incluindo o saldo inicial,
 * o número de carteiras, o número de servidores, o tamanho dos buffers
 * e o número máximo de transações. Guarda essa informação na estrutura info_container.
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
    destroy_shared_memory_structs(info, buffs);//tem de ser chamado antes do destroy_dynamic pq o dynamic deAlloca os structs info e buffs em si
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
        fprintf(stderr, "NUMBER OF ARGUMENTS INCORRECT\n");

        ret = -1;
    }
    // init_balance
    if (atoi(argv[1]) <= 0)
    {
        fprintf(stderr, "INITIAL BALANCE NEEDS TO BE POSITIVE\n");
        ret = -1;
    }

    // n_wallets
    if (atoi(argv[2]) <= 0)
    {
        fprintf(stderr, "NUMBER OF WALLETS NEEDS TO BE POSITIVE\n");
        ret = -1;
    }

    // n_servers
    if (atoi(argv[3]) <= 0)
    {
        fprintf(stderr, "NUMBER OF SERVERS NEEDS TO BE POSITIVE\n");
        ret = -1;
    }

    // buff_size
    // TODO: ver
    if (atoi(argv[4]) <= 0)
    {
        fprintf(stderr, "BUFFER SIZE NEEDS TO BE POSITIVE\n");
        ret = -1;
    }

    // max_txs
    if (atoi(argv[5]) <= 0)
    {
        fprintf(stderr, "MAX TRANSACTIONS NEEDS TO BE POSITIVE\n");
        ret = -1;
    }
    return ret;
}
/*
args usage: progam_name init_balance n_wallets n_servers buff_size max_txs
*/

void main_args(int argc, char *argv[], struct info_container *info)
{
    if (check_args(argc, argv) == -1)
    {
        printf("[Main] Parâmetros incorretos!");
        deallocate_dynamic_memory(info);
        exit(-1);
    }

    if (check_args(argc, argv) == 0)
    {
        printf("[Main] Parâmetros corretos!");
        info->init_balance = atof(argv[1]);
        info->n_wallets = atoi(argv[2]);
        info->n_servers = atoi(argv[3]);
        info->buffers_size = atoi(argv[4]);
        info->max_txs = atoi(argv[5]);
    }
}

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    (void)buffs;//colocado pq indica um erro ao compilar com as flags -Wall -Werror -Wextra
    info->wallets_pids = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    // info->wallets_stats = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));
    // buffs->buff_main_wallets = allocate_dynamic_memory(info->buffers_size);
}

// falta
void create_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    buffs->buff_main_wallets->ptrs = create_shared_memory(ID_SHM_MAIN_WALLETS_PTR, info->buffers_size *  sizeof(int));
    buffs->buff_main_wallets = create_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, info->buffers_size * sizeof(struct transaction));
   
    buffs->buff_wallets_servers->ptrs = create_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, sizeof(struct pointers));
    buffs->buff_wallets_servers = create_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, info->buffers_size * sizeof(struct transaction));
   
    buffs->buff_servers_main->ptrs = create_shared_memory(ID_SHM_SERVERS_MAIN_PTR, info->buffers_size *  sizeof(int));
    buffs->buff_servers_main = create_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, info->buffers_size * sizeof(struct transaction));
    
    info->wallets_stats = create_shared_memory(ID_SHM_WALLETS_STATS, sizeof(int) * info->n_wallets);
    info->servers_stats = create_shared_memory(ID_SHM_SERVERS_STATS, sizeof(int) * info->n_servers);
    
    info->balances = create_shared_memory(ID_SHM_BALANCES, sizeof(int) * info->n_wallets);
    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));

}

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    deallocate_dynamic_memory(info->wallets_pids);
    deallocate_dynamic_memory(info->servers_pids);

    deallocate_dynamic_memory(info);
    deallocate_dynamic_memory(buffs);

}

// falta
void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    // (void)info;
    // (void)buffs;
    /*
        buffs->buff_main_wallets->ptrs = create_shared_memory(ID_SHM_MAIN_WALLETS_PTR, info->buffers_size *  sizeof(int));
    buffs->buff_main_wallets = create_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, info->buffers_size * sizeof(struct transaction));
   
    buffs->buff_wallets_servers->ptrs = create_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, sizeof(struct pointers));
    buffs->buff_wallets_servers = create_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, info->buffers_size * sizeof(struct transaction));
   
    buffs->buff_servers_main->ptrs = create_shared_memory(ID_SHM_SERVERS_MAIN_PTR, info->buffers_size *  sizeof(int));
    buffs->buff_servers_main = create_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, info->buffers_size * sizeof(struct transaction));
    
    info->wallets_stats = create_shared_memory(ID_SHM_WALLETS_STATS, sizeof(int) * info->n_wallets);
    info->servers_stats = create_shared_memory(ID_SHM_SERVERS_STATS, sizeof(int) * info->n_servers);
    
    info->balances = create_shared_memory(ID_SHM_BALANCES, sizeof(int) * info->n_wallets);
    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));
    */

    destroy_shared_memory(ID_SHM_MAIN_WALLETS_PTR, buffs->buff_main_wallets->ptrs, info->buffers_size *  sizeof(int));
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, buffs->buff_main_wallets, info->buffers_size * sizeof(struct transaction));

    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, buffs->buff_wallets_servers->ptrs, sizeof(struct pointers));
    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, buffs->buff_wallets_servers, info->buffers_size * sizeof(struct transaction));

    destroy_shared_memory(ID_SHM_SERVERS_MAIN_PTR, buffs->buff_servers_main->ptrs, info->buffers_size *  sizeof(int));
    destroy_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, buffs->buff_servers_main,  info->buffers_size * sizeof(struct transaction));

    destroy_shared_memory(ID_SHM_WALLETS_STATS, info->wallets_stats, sizeof(int) * info->n_wallets );
    destroy_shared_memory(ID_SHM_SERVERS_STATS, info->servers_stats, sizeof(int) * info->n_servers);

    destroy_shared_memory(ID_SHM_BALANCES, info->balances, sizeof(int) * info->n_wallets);
    destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));



    // ver se é preciso por algo depois da info e buffs e o quÊ
    //  destroy_shared_memory(info->);
    //  destroy_shared_memory(buffs->);
}

void create_processes(struct info_container *info, struct buffers *buffs)
{
    for (int i = 0; i < info->n_wallets; i++)
        info->wallets_pids[i] = launch_wallet(i, info, buffs);

    for (int i = 0; i < info->n_servers; i++)
        info->servers_pids[i] = launch_server(i, info, buffs);
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

void user_interaction(struct info_container *info, struct buffers *buffs)
{
    int tx_counter = -1;
    while (1)
    {
        char *input = malloc(50);
        printf("[Main] Introduzir operação: ");
        scanf("%s", input);

        // dividir as informações do comando
        char *words = strtok(input, " ");
        if (!words)
            continue;
        char *op = words;

        switch (user_hash(op))
        {
        case 1: // "ball" command
            print_balance(info);
            break;
        case 2: // "trx" command
            create_transaction(&tx_counter, info, buffs);
            break;
        case 3: // "rcp" command
            receive_receipt(info, buffs);
            break;
        case 4: // "help" command
            printf("[Main] Operações disponíveis:");
            printf("[Main]  bal id - consultar o saldo da carteira identifcada por id");
            printf("[Main]  trx src_id dest_id amount - criar uma nova transação");
            printf("[Main]  rcp id - obter o comprovativo da transação de número id");
            printf("[Main]  help - imprime a informação sobre as operações disponíveis");
            printf("[Main]  stat - imprime as estatísticas atuais do SOchain");
            printf("[Main]  end - termina a execução do SOchain");
            break;
        case 5: // "stat" command
            print_stat(tx_counter, info);
            break;
        case 6: // "end" command
            end_execution(info, buffs);
            break;
        default:
            printf("[Main] Introduzir operação: [Main] Operação não reconhecida, insira 'help' para assistência.");
            break;
        }
    }
}

void write_final_statistics(struct info_container *info)
{
    printf("[Main] A encerrar a execução do SOchain! As estatísticas da execução são:");
    for (int i = 0; i < info->n_wallets; i++)
        printf("[Main] A carteira %d assinou %d transações e terminou com %.2f\n", i, info->wallets_stats[i], info->balances[i]);
    for (int i = 0; i < info->n_servers; i++)
        printf("[Main] Servidor %d assinou %d transações\n", i, info->servers_stats[i]);
}

void end_execution(struct info_container *info, struct buffers *buffs)
{
    *info->terminate = 1;
    (void)buffs;
    wait_processes(info);
    write_final_statistics(info);
}

void wait_processes(struct info_container *info)
{

    // ver se é para esperar para todos os processos ou so um dos seguintes servers
    for (int i = 0; i < info->n_servers; i++)
        wait_process(info->servers_pids[i]);

    for (int i = 0; i < info->n_wallets; i++)
        wait_process(info->wallets_pids[i]);
}

void print_balance(struct info_container *info)
{
    int wallet_id;
    scanf("%d", &wallet_id);
    if (wallet_id >= 0 && wallet_id < info->n_wallets)
        printf("[Main] O saldo da carteira %d é de %.2f SOT atualmente.\n", wallet_id, info->balances[wallet_id]);
    else
        printf("[Main] Não foi possível verificar o saldo da carteira %d\n", wallet_id);
}

// Falta
void create_transaction(int *tx_counter, struct info_container *info, struct buffers *buffs)
{
    // tx_counter++;
    // buffs->buff_main_wallets->buffer->src_id;
    // info->wallets_pids;
    (void)tx_counter;
    (void)info;
    (void)buffs;
}
// Falta
void receive_receipt(struct info_container *info, struct buffers *buffs)
{
    (void)info;
    (void)buffs;
}

void print_stat(int tx_counter, struct info_container *info)
{
    char tabulation[] = "        ";
    printf("- Configuração inicial:\n");
    printf("%sPropriedade     Valor\n", tabulation);
    printf("%sinit_balance    %.2f\n", tabulation, info->init_balance);
    printf("%sn_wallets       %d\n", tabulation, info->n_wallets);
    printf("%sn_servers       %d\n", tabulation, info->n_servers);
    printf("%sbuffer_size     %d\n", tabulation, info->buffers_size);
    printf("%smax_txs         %d\n", tabulation, info->max_txs);
    printf("- Variáveis atuais:\n");
    printf("%stx_count        %d\n", tabulation, tx_counter);
    printf("%sterminate       %d\n", tabulation, *info->terminate);
    printf("- Informações sobre as carteiras:\n");
    printf("%sCarteira        PID             Saldo           Transações Assinadas\n", tabulation);
    for (int i = 0; i < info->n_wallets; i++)
    {   // TODO fix
        // printf("%s%-15d%-15d%-15d%-15d\n", tabulation, i, info->wallets_pids[i], info->balances[i], info->wallets_stats[i]);
    }
    printf("- Informações sobre os servidores:\n");
    printf("%sServidor        PID             Transações Processadas\n", tabulation);
    for (int i = 0; i < info->n_servers; i++)
    {   // TODO fix
        // printf("%s%-15d%-15d%-15d\n", tabulation, i, info->servers_pids[i], info->servers_stats[i]);
    }
}

// TODO: adicionar cores ao [$(processo)]
void help()
{
    printf(BLUE "[Main] " RESET GREEN "Operações disponíveis:\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "bal id - consultar o saldo da carteira identificada por id.\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "trx src_id dest_id amount - criar uma nova transação.\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "rcp id - obter o comprovativo da transação de número id.\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "help - imprime a informação sobre as operações disponíveis.\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "stat - imprime as estatisticas atuais do sistema SOchain\n" RESET);
    printf(BLUE "[Main] " RESET GREEN "end - termina a execução do SOchain.\n" RESET);
}