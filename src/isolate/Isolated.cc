#include <curl/curl.h>
#include <err.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include <isolate/isolate.hh>

#define STACK_SIZE (1024 * 1024)  // Stack size for cloned child

namespace isolate
{
    static int childFunc(void *stuff)
    {
        char **argv = (char **)stuff;
        if (execvp(argv[0], argv) != 0)
            err(-1, "could not exeve");
        return 0;
    }

    void Isolated::run(const char *argv[]) const
    {
        char *stack;     // Start of stack buffer
        char *stackTop;  // End of stack buffer

        // Allocate memory to be used for the stack of the child
        stack = (char *)mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
        if (stack == MAP_FAILED)
            throw IsolatedException("could not mmap");

        stackTop = stack + STACK_SIZE;  // Assume stack grows downward

        int clone_flags = CLONE_NEWPID | SIGCHLD;

        // the result of this call is that our childFunc will be run in another
        // process returning it's pid
        pid_t pid =
            clone(childFunc, stackTop, clone_flags, argv);
        if (pid < 0)
        {
            munmap(stack, STACK_SIZE);
            throw IsolatedException("could not clone");
        }

        if (waitpid(pid, NULL, 0) == -1)
        {
            munmap(stack, STACK_SIZE);
            throw IsolatedException("could not waitpid");
        }
        munmap(stack, STACK_SIZE);
    }

    Isolated::Isolated(const std::string& folder)
        : folder_(folder)
    {
        if (chdir(folder_.c_str()) == -1)
            throw IsolatedException("could not chdir");
    }

    Isolated::Isolated(std::nullptr_t null)
        : folder_(".")
    {
        (void) null;
        throw IsolatedException("could not create temp directory");
    }

}
