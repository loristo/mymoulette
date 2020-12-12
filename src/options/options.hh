#pragma once

namespace options
{
    class Options
    {
    public:
        Options(int argc, char *argv[]);
        ~Options() = default;

    private:
        bool is_docker_img = false;
        char *student_dir = NULL;
        union
        {
            char *docker_img;
            char *rootfs;
        } path;
        char **moulette_prog;
    };
}
