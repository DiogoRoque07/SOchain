# Shared Memory and Dynamic Memory Usage

## When to Use Shared Memory
- **Question**: Should shared memory be used for the server and wallets (e.g., dynamic for the server and shared for the wallets)?
    - **Answer (omeir)**: Use shared memory only in `buffer_wallet_server`.
    - **Answer (prof)**: *View table below*

## Process Creation
- **Question**: In the `create_process` function, is it necessary to execute the created processes (e.g., using `execl`, `execve`, or another `exec` function), or is it enough to just create them?
    - **Answer (Bowser)**: Call the functions `launch_wallet` and `launch_server`.
    - **Answer (prof)**: *(No response provided)*

## Transaction Counting
- **Question**: In the professor's program, does a failed transaction count toward the total number of transactions? Is this expected?
    - *yes, a failed transaction should count*

## Missing Help Information
- **Observation**: The professor's program help does not include the `stat` command.

---

## Memory Usage Overview

### Data Structures and Memory Types

| **Data Structure**               | **Memory Type**   | **Size**                  | **Reason**                                                                 |
|-----------------------------------|-------------------|---------------------------|-----------------------------------------------------------------------------|
| `BUFF_RA_MAIN_WALLETS`            | dynamic memory     | `buff_size * sizeof(struct)` | Communication between Main and Wallets.                                    |
| `BUFF_CIRC_WALLETS_SERVERS`       | dynamic memory     | `buff_size * sizeof(struct)` | Communication between Wallets and Servers.                                 |
| `BUFF_RA_SERVERS_MAIN`            | dynamic memory     | `buff_size * sizeof(struct)` | Communication between Servers and Main.                                    |
| `BUFF_RA_MAIN_WALLETS transacation buffer`            | Shared Memory     | ` info->buffers_size * sizeof(struct transaction)` | Communication between Main and Wallets.                                    |
| `BUFF_CIRC_WALLETS_SERVERS transacation buffer`       | Shared Memory     | ` info->buffers_size * sizeof(struct transaction)` | Communication between Wallets and Servers.                                 |
| `BUFF_RA_SERVERS_MAIN transacation buffer`            | Shared Memory     | ` info->buffers_size * sizeof(struct transaction)` | Communication between Servers and Main.                                    |
| `BUFF_RA_MAIN_WALLETS transacation pointers`            | Shared Memory     | `sizeof(struct pointers)` | Communication between Main and Wallets.                                    |
| `BUFF_CIRC_WALLETS_SERVERS transacation pointers`       | Shared Memory     | `sizeof(struct pointers)` | Communication between Wallets and Servers.                                 |
| `BUFF_RA_SERVERS_MAIN transacation pointers`            | Shared Memory     | `sizeof(struct pointers)` | Communication between Servers and Main.                                    |
| `info->balances`                  | Shared Memory     | `sizeof(double) * N`      | Shared between Wallets and Servers to update wallet balances.              |
| `info->terminate`                 | Shared Memory     | `sizeof(int)`             | Shared between all processes to signal termination.                        |
| `info->wallets_stats`             | Shared Memory     | `sizeof(int) * N`         | Shared if Wallets update their transaction counts.                         |
| `info->servers_stats`             | Shared Memory     | `sizeof(int) * M`         | Shared if Servers update their transaction counts.                         |
| `info->wallets_pids`              | Dynamic Memory    | `sizeof(pid_t) * N`       | Managed only by the Main process.                                          |
| `info->servers_pids`              | Dynamic Memory    | `sizeof(pid_t) * M`       | Managed only by the Main process.                                          |
| Buffers' pointers (structs)       | Shared Memory     | `sizeof(pointer) * K`     | Required for managing circular buffers in shared memory.                   |

*Note*: `N` is the number of Wallets, `M` is the number of Servers, and `K` is the number of buffer pointers.

---
### Detalhes sobre o Uso de Memória

#### Memória Partilhada para Buffers

- "Esta transação criada é então escrita pela Main num buffer de comunicação de acesso aleatório (identificado por `BUFF_RA_MAIN_WALLETS`), onde ficará disponível para leitura pelas entidades que representam as carteiras (Wallets)."
- "De seguida, esta Wallet escreve a transação assinada por ela num buffer de comunicação do tipo circular (identificado por `BUFF_CIRC_WALLETS_SERVERS`), onde ficará disponível para leitura por qualquer entidade que representa os servidores (Servers)."
- "O servidor escreve a transação executada e assinada por ele para um buffer de comunicação de acesso aleatório (identificado por `BUFF_RA_SERVERS_MAIN`), onde ficará disponível para leitura pela Main mediante um pedido de obtenção do comprovativo (receipt) por parte do utilizador."

#### Memória Partilhada para Saldos

- "A Main acederá à estrutura de dados partilhada que conterá os saldos de cada carteira e retornará o valor que lá estiver atualmente."

#### Memória Partilhada para Terminação

- "Antes de tentar ler a transação do buffer, deve verificar se `info->terminate` tem valor 1. Em caso afirmativo, retorna imediatamente."

#### Memória Dinâmica para PIDs

- "O PID de cada processo Wallet e Server criado no sistema deve ser armazenado."
- "Deve ser reservada memória dinâmica para arrays como `*_pids` usando a função `allocate_dynamic_memory` do `memory.h`."

#### Memória Partilhada para Estatísticas

- "O número de transações assinadas por cada Wallet e o número de transações validadas e processadas por cada Server devem ser rastreados."

---

### Resumo

#### Memória Partilhada:

- Buffers (`BUFF_RA_MAIN_WALLETS`, `BUFF_CIRC_WALLETS_SERVERS`, `BUFF_RA_SERVERS_MAIN`).
- Saldos das carteiras (`info->balances`).
- Flag de terminação (`info->terminate`).
- Arrays de estatísticas (`info->wallets_stats`, `info->servers_stats` se atualizados por Wallets/Servers).

#### Memória Dinâmica:

- PIDs de Wallets e Servers (`info->wallets_pids`, `info->servers_pids`).
- Arrays de estatísticas (`info->wallets_stats`, `info->servers_stats` se apenas usados pela Main).


# Como limpar a memória dos buffers alocados no `main`?
A função `main_args` só recebe o parâmetro `info`. Como é que é suposto limpar a memória dos buffers alocados no `main` caso os argumentos nao foram dados corretamente?

# Uso de variaveis globais
Nos usamos uma variavel global para que os processos do servidor e wallet nao entrarem no user interaction
- Ja não usamos mas podemos usar?


# Criação de processos
Nos criamos os processos a partir do main (um fork por processos de server e wallet), é suposto arranjaremos uma forma mais eficiente?

# handle memory loss when quiting with CTRL-C or other signals
maybe use libx or simillar to capture the event

# se os buffers forem de memoria dinamica o valgrind quexia se, apesar de nao haver leaks ha muitos invalid writes e invalid reads, no entanto o programa funciona 

# OQ FAZER QUANDO O BUFFER DOS RECIBOS(BUFF_SERVERS_MAIN) FICAR CHEIO