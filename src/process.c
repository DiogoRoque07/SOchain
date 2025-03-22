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
        execute_wallet(wallet_id, info, buffs);
        destroy_dynamic_memory_structs(info, buffs);
        exit(0);
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
        execute_server(server_id, info, buffs);
        destroy_dynamic_memory_structs(info, buffs);
        exit(0);
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
    if (waitpid(process_id, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        printf("Process %d did not terminate normally.\n", process_id);
        return -1;
    }
}