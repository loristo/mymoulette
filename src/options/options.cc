#include <err.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include <options/options.hh>
#include <mymoulette.hh>

namespace options
{
    static void help(void)
    {
        std::cout
            << "MyMoulette, the students' nightmare, now highly secured\n"
            << "Usage: ./mymoulette [-v student_workdir] "
            << "<-I docker-img | rootfs-path> moulette_prog [ moulette_arg [...] ]\n"
            << "    rootfs-path is the path to the directory containing the new rootfs "
            << "(exclusive with -I option)\n"
            << "    docker-img is an image available on hub.docker.com (exclusive with "
            << "rootfs-path)\n"
            << "    moulette_prog will be the first program to be launched, must already "
            << "be in the environment\n"
            << "    student_workdir is the directory containing the code to grade\n";
    }

    static void help(int i)
    {
        help();
        exit(i);
    }

    static void help(int i, const char *msg)
    {
        warnx(msg);
        help();
        exit(i);
    }

    Options::Options(int argc, char *argv[])
    {
        int opt;
        while ((opt = getopt(argc, argv, "v:I:h")) != -1) {
            switch (opt) {
            case 'v':
                this->student_dir_ = optarg;
                break;
            case 'I':
                this->is_docker_img_ = true;
                this->path_.docker_img = optarg;
                break;
            case 'h':
                help();
                break;
            default: /* '?' */
                help(ERR_BADOPTION);
                break;
            }
        }

        if (!this->is_docker_img_)
        {
            if (optind == argc)
                help(ERR_BADOPTION, "missing rootfs_path and moulette_prog");
            this->path_.rootfs = argv[optind];
            optind++;
        }

        if (optind == argc)
            help(ERR_BADOPTION, "missing moulette_prog");

        this->moulette_prog_ = argv + optind;
    }

}
