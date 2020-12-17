#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)  // Stack size for cloned child

// Struct used to pass arguments
struct clone_args {
        char **argv;
};

// childFunc is the func that will be executed as the result of clone
static int
childFunc(void *stuff)
{
        struct clone_args *args = (struct clone_args *)stuff;
        if (execvp(args->argv[0], args->argv) != 0) {
                perror("execvp");
                exit(-1);
        }
        // we should never reach here!
        exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
        char *stack;     // Start of stack buffer
        char *stackTop;  // End of stack buffer

        if (argc < 2) {
               fprintf(stderr, "Usage: %s <child-process> [child-arg [...]]\n", argv[0]);
               exit(EXIT_SUCCESS);
        }

        // Allocate memory to be used for the stack of the child
        stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
        if (stack == MAP_FAILED) {
                perror("mmap");
                exit(EXIT_FAILURE);
        }

        stackTop = stack + STACK_SIZE;  // Assume stack grows downward

        // Here begins the magic
        struct clone_args args;
        args.argv = &argv[1];

        int clone_flags = CLONE_NEWPID | SIGCHLD;

        // the result of this call is that our childFunc will be run in another
        // process returning it's pid
        pid_t pid =
            clone(childFunc, stackTop, clone_flags, &args);
        if (pid < 0) {
                perror("clone");
                exit(EXIT_FAILURE);
        }
        // lets wait on our child process here before we, the parent, exits
        if (waitpid(pid, NULL, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);

}
