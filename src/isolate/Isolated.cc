#include <curl/curl.h>
#include <err.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include <isolate/isolate.hh>
#include <cgroups/cgroups.hh>

#define STACK_SIZE (1024 * 1024)  // Stack size for cloned child

namespace isolate
{

    static int childFunc(void *stuff)
    {
        struct exec_data *exec_data = (struct exec_data *)stuff;
        return exec_data->container->child(exec_data->argv);
    }

    int Isolated::child(char *argv[])
    {
        (void) argv;
        while (getuid() != 0)
            continue;
        try
        {
            this->pivot_root();
            this->set_hostname();
        }
        catch (IsolatedException& e)
        {
            warn(e.what());
            return -1;
        }
        catch (cgroups::CgroupException& e)
        {
            warn(e.what());
            return -1;
        }
        catch (std::exception& e)
        {
            warn(e.what());
            return -1;
        }
        execvp(argv[0], argv);
        return 0;
    }

    void Isolated::pivot_root()
    {
        if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == 1)
            throw IsolatedException("could not mount private");

        if (mount(folder_.c_str(), folder_.c_str(), NULL, MS_BIND, NULL) == -1)
            throw IsolatedException("could not mount bind");

        if (chdir(folder_.c_str()) == -1)
            throw IsolatedException("could not chdir");

        if (mkdir("oldroot", 0777) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir oldroot");

        if (mkdir("sys", 0744) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir sys");
        if (mount("none", "sys", "sysfs", 0, "") == -1)
            throw IsolatedException("could not mount sys");

        if (mkdir("proc", 0744) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir proc");
        if (mount("none", "proc", "proc", 0, "") == -1)
            throw IsolatedException("could not mount proc");

        if (mkdir("tmp", 0744) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir tmp");
        if (mount("none", "tmp", "tmpfs", 0, "size=16M,uid=0,mode=700") == -1)
            throw IsolatedException("could not mount tmp");

        if (mkdir("dev", 0744) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir dev");
        if (mount("dev", "/dev", NULL, MS_BIND, NULL) == -1)
            throw IsolatedException("could not mount bind dev");

        if (mkdir("home", 0744) == -1 && errno != EEXIST)
            throw IsolatedException("could not mkdir home");

        if (syscall(SYS_pivot_root, ".", "oldroot") == -1)
            throw IsolatedException("could not pivot_root");

        if (umount2("oldroot", MNT_DETACH) == -1)
            throw IsolatedException("could not umount old root");

        if (rmdir("oldroot") == -1)
            throw IsolatedException("could not rmdir old root");
    }

    void Isolated::set_hostname()
    {
        auto randchar = []() -> char
        {
            constexpr char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        std::string res;
        srand(time(NULL));
        for (unsigned i = 0; i < 12; ++i)
            res += randchar();
        sethostname(res.c_str(), 12);

    }

    void Isolated::run(char *argv[])
    {
        char *stack;
        char *stackTop;

        struct exec_data exec_data(argv, this);

        stack = (char *)mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
        if (stack == MAP_FAILED)
            throw IsolatedException("could not mmap");

        stackTop = stack + STACK_SIZE;


        int clone_flags =
              CLONE_NEWNS
            | CLONE_NEWPID
            | CLONE_NEWUTS
            | CLONE_NEWIPC
            | CLONE_NEWUSER
            | CLONE_NEWNET
            | SIGCHLD;

        pid_t pid =
            clone(childFunc, stackTop, clone_flags, &exec_data);
        if (pid < 0)
        {
            munmap(stack, STACK_SIZE);
            throw IsolatedException("could not clone");
        }

        {
            uid_t uid = getuid();
            std::stringstream ss;
            ss << "/proc/" << pid << "/uid_map";
            std::ofstream map(ss.str());
            map << "0 " << uid << " 1";
            map.close();
            if (!map)
                throw IsolatedException("could not write uid_map");
        }

        {
            std::stringstream ss;
            ss << "/proc/" << pid << "/setgroups";
            std::ofstream map(ss.str());
            map << "deny";
            map.close();
            if (!map)
                throw IsolatedException("could not write setgroups");
        }

        {
            gid_t gid = getgid();
            std::stringstream ss;
            ss << "/proc/" << pid << "/gid_map";
            std::ofstream map(ss.str());
            map << "0 " << gid << " 1";
            map.close();
            if (!map)
                throw IsolatedException("could not write gid_map");
        }

        std::cout << "pid: " << pid << '\n';
        if (waitpid(pid, NULL, 0) == -1)
        {
            munmap(stack, STACK_SIZE);
            throw IsolatedException("could not waitpid");
        }
        munmap(stack, STACK_SIZE);
    }

    Isolated::Isolated(const std::string& folder)
        : folder_(folder)
    {}

    exec_data::exec_data(char **argv, Isolated *container)
        : container(container),
          argv(argv)
    {}

    Isolated::Isolated(std::nullptr_t null)
        : folder_(".")
    {
        (void) null;
        throw IsolatedException("could not create temp directory");
    }

    Isolated::~Isolated()
    {
    }
}
