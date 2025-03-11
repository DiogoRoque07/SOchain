#include <stdio.h>
#include "../inc/main.h"
#include "../inc/process.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <string.h>


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


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
    info->wallets_pids = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    info->wallets_stats = allocate_dynamic_memory(info->n_wallets * sizeof(int));
    buffs->buff_main_wallets = allocate_dynamic_memory(info->buffers_size);
}

//falta
void create_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    (void)info;
    (void)buffs;
}

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    deallocate_dynamic_memory(info->wallets_pids);
    deallocate_dynamic_memory(info->wallets_stats);
    deallocate_dynamic_memory(buffs->buff_main_wallets);
    deallocate_dynamic_memory(info);
    deallocate_dynamic_memory(buffs);
}

//falta
void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    (void)info;
    (void)buffs;
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

void user_interaction(struct info_container *info, struct buffers *buffs)
{

    while(1){
        char *input;
        printf("[Main] Introduzir operação: ");
        scanf("%s",input);

        // dividir as informações do comando 
        char words[] = strtok(input," ");
        char op[] = words[0];
        
        int id1 = atoi(words[1]);

        switch (user_hash(op))
        {
        case 1:
            if(sizeof(words));
            print_balance(atoi(words[1]));
            break;
        
        
        default:
            printf("[Main] Introduzir operação: [Main] Operação não reconhecida, insira 'help' para assistência.");
            break;
        }
    }


    (void)info;
    (void)buffs;

}

//incompleto
int user_hash(char op[]){

    if(strcmp(op,"bal") == 1){
        return 1;
    } else if(strcmp(op,"trx")){

    }


}

void write_final_statistics(struct info_container *info)
{
    for (int i = 0; i < info->n_wallets; i++) {
        printf("Carteira %d assinou %d transações\n", i, info->wallets_stats[i]);
    }
    for (int i = 0; i < info->n_servers; i++) {
        printf("Servidor %d processou %d transações\n", i, info->servers_stats[i]);
    }
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
    printf("Indica o ID da carteira: ");
    scanf("%d", &wallet_id);
    if (wallet_id >= 0 && wallet_id < info->n_wallets)
        printf("Saldo atual da careira %d: %d\n", wallet_id, info->wallets_stats[wallet_id]);
    else
        printf("ID invalido\n");
}

//Falta
void create_transaction(int *tx_counter, struct info_container *info, struct buffers *buffs)
{
    (void)tx_counter;
    (void)info;
    (void)buffs;
}
// ctrl k; ctr c
// ctrl k; ctr u
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
    printf("%sPropriedade     Valor\n",tabulation);
    printf("%sinit_balance    %.2f",tabulation,info->init_balance);
    printf("%sn_wallets       %d",tabulation,info->n_wallets);
    printf("%sn_servers       %d",tabulation,info->n_servers);
    printf("%sbuffer_size     %d",tabulation,info->buffers_size);
    printf("%smax_txs         %d",tabulation,info->max_txs);
    printf("- Variáveis atuais:\n");
    printf("%stx_count        %.2f",tabulation,info->init_balance);
    printf("%sterminate       %.2f",tabulation,tx_counter);
    printf("- Informações sobre as carteiras:\n");
    printf("%sCarteira        PID             Saldo           Transações Assinadas\n");
    //informações sobre as carteiras
    for(int i = 0; i < info->n_wallets; i++){
        printf("%sid:%d        Saldo%.2f SOT        %d",tabulation,i,info->wallets_pids);
    }
    printf("- Informações sobre os servidores:\n");
    printf("%sServidor        PID             Transações Processadas\n");
    for (int i = 0; i < info->n_servers; i++)
    {
        printf("%sID:%dPID:%dTransações:%d",i,info->servers_pids[i],info->servers_stats[i]);
    }
    
    

    
    
    
    
    
    
    
    
    
    
    // ISSO É DO COPILOT SÓ PARA FICAR BONITINHO IG
    printf("Configurações iniciais do sistema:\n");
    printf("Saldo inicial: %.2f\n", info->init_balance);
    printf("Número de carteiras: %d\n", info->n_wallets);
    printf("Número de servidores: %d\n", info->n_servers);
    printf("Tamanho dos buffers: %d\n", info->buffers_size);
    printf("Número máximo de transações: %d\n", info->max_txs);
    printf("Variável terminate: %d\n", *info->terminate);
    printf("Contador de transações criadas: %d\n", tx_counter);

    printf("PIDs dos processos das carteiras:\n");
    for (int i = 0; i < info->n_wallets; i++) {
        printf("Carteira %d: PID %d\n", i, info->wallets_pids[i]);
    }

    printf("PIDs dos processos dos servidores:\n");
    for (int i = 0; i < info->n_servers; i++) {
        printf("Servidor %d: PID %d\n", i, info->servers_pids[i]);
    }

    printf("Número de transações assinadas por cada carteira:\n");
    for (int i = 0; i < info->n_wallets; i++) {
        printf("Carteira %d: %d transações\n", i, info->wallets_stats[i]);
    }

    printf("Número de transações processadas por cada servidor:\n");
    for (int i = 0; i < info->n_servers; i++) {
        printf("Servidor %d: %d transações\n", i, info->servers_stats[i]);
    }
}

void help()
{
    printf(GREEN "Comandos disponíveis:\n" RESET);
    printf(YELLOW "1. bal id" RESET " – Obtém o saldo atual da carteira cujo identificador é id.\n");
    printf(YELLOW "2. trx src_id dest_id amount" RESET " – Solicita a criação de uma transação em que a carteira de origem envia uma quantidade de SOT tokens para a carteira de destino.\n");
    printf(YELLOW "3. rcp id" RESET " – Obtém o comprovativo da execução de uma transação específica, identificada por id.\n");
    printf(YELLOW "4. stat" RESET " – Apresenta o estado atual das variáveis do info_container.\n");
    printf(YELLOW "5. help" RESET " – Mostra as informações de ajuda sobre as operações disponíveis.\n");
    printf(YELLOW "6. end" RESET " – Termina a execução do sistema SOchain.\n");
}