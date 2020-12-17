#MAKE
set(isolate_src
    Isolated.cc
    IsolatedDocker.cc
    IsolatedException.cc
    IsolatedRootfs.cc)

add_library(isolate ${isolate_src})
