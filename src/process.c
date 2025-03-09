#include "../inc/process.h"
#include <sys/wait.h>

int launch_wallet(int wallet_id, struct info_container* info, struct buffers* buffs){

}

int launch_server(int server_id, struct info_container* info, struct buffers* buffs){

}

int wait_process(int process_id){
    int status;
    waitpid(process_id, &status, 0);
}