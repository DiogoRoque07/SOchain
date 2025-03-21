#include "../inc/process.h"
#include "../inc/wallet.h"
#include "../inc/server.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int launch_wallet(int wallet_id, struct info_container *info, struct buffers *buffs)
{

    int pid = fork();

    if (pid > 0)
    {
        return pid;
    }
    else if (pid == 0)
    {   
        return execute_wallet(wallet_id, info, buffs);
    }
    else
    {
        printf("\n\n \033[31m LAUNCH WALLET FALHOU    \033[0m \n\n");
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

int launch_server(int server_id, struct info_container *info, struct buffers *buffs)
{
    int pid = fork();

    if (pid > 0)
    {
        return pid;
    }
    else if (pid == 0)
    {
        return execute_server(server_id, info, buffs);
    }
    else
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

int wait_process(int process_id)
{
    int status;
    return waitpid(process_id, &status, 0);
}