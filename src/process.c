#include "../inc/process.h"
#include "../inc/wallet.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int launch_wallet(int wallet_id, struct info_container* info, struct buffers* buffs){
    int pid = fork();

    if (pid > 0) {
        return pid;
    } 
    else if (pid == 0){
        execute_wallet(wallet_id, info, buffs);
    }
    else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

int launch_server(int server_id, struct info_container* info, struct buffers* buffs){
    int pid = fork();
    
    if (pid > 0) {
        return pid;
    } 
    else if (pid == 0){
        execute_server(server_id, info, buffs);
    }
    else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

}

int wait_process(int process_id){
    int status;
    waitpid(process_id, &status, 0);
}