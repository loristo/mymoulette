#pragma once

namespace options
{
    class Options
    {
    public:
        Options(int argc, char *argv[]);
        ~Options() = default;

    private:
        bool is_docker_img_ = false;
        char *student_dir_ = NULL;
        union
        {
            char *docker_img;
            char *rootfs;
        } path_;
        char **moulette_prog_;
    };
}
