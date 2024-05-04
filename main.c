#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h> // para getpid()

#define FIBER_STACK (1024 * 64)

// Estrutura para passar dados da thread para o processo pai
struct ThreadData {
    int thread_id;
    char message[100];
};

// A função que a thread executará
int threadFunction(void* argument)
{
    struct ThreadData* data = (struct ThreadData*)argument;

    printf("Child thread with ID %d says: %s\n", data->thread_id, data->message);

    return 0;
}

int main()
{
    void* stack;
    pid_t pid;
    int status;

    // Allocate the stack
    stack = malloc(FIBER_STACK);
    if (stack == 0)
    {
        perror("malloc: could not allocate stack");
        exit(1);
    }

    printf("Creating child thread\n");

    // Estrutura para passar dados da thread para o processo pai
    struct ThreadData thread_data;
    thread_data.thread_id = getpid();
    snprintf(thread_data.message, sizeof(thread_data.message), "Hello from child thread with ID %d!", thread_data.thread_id);

    // Chama o clone para criar a thread
    pid = clone(&threadFunction, (char*)stack + FIBER_STACK,
                SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &thread_data);
    if (pid == -1)
    {
        perror("clone");
        exit(2);
    }

    // Espera pela thread filho terminar
    pid = waitpid(pid, &status, 0);
    if (pid == -1)
    {
        perror("waitpid");
        exit(3);
    }

    // Verifica o status da thread filho
    if (WIFEXITED(status))
    {
        printf("Child thread returned with status %d\n", WEXITSTATUS(status));
    }
    else
    {
        printf("Child thread terminated abnormally\n");
    }

    // Libera o stack
    free(stack);
    printf("Child thread returned and stack freed.\n");

    return 0;
}
# Lab03
