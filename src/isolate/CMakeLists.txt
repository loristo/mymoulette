#MAKE
set(isolate_src
    Isolated.cc
    IsolatedDocker.cc
    IsolatedException.cc
    IsolatedRootfs.cc
    Curl.cc)

add_library(isolate ${isolate_src})

target_link_libraries(isolate
        PRIVATE nlohmann_json
        PRIVATE curl
        )
