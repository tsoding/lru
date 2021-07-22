#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"

#define CFLAGS "-Wall", "-Wextra", "-std=c11", "-pedantic", "-ggdb"

const char *cc(void)
{
    const char *res = getenv("CC");
    return res ? res : "cc";
}

int main(int argc, char **argv)
{
    GO_REBUILD_URSELF(argc, argv);
    CMD(cc(), CFLAGS, "-o", "lru", "src/main.c");

    if (argc > 1) {
        if (strcmp(argv[1], "run") == 0) {
            CHAIN(CHAIN_CMD("time", "-p", "./lru", "./shakespeare.txt"),
                  OUT("out.txt"));
            CMD("diff", "-u", "out.txt", "./shakespeare.expected.txt");
        } else if (strcmp(argv[1], "gdb") == 0) {
            CMD("gdb", "./lru");
        } else {
            PANIC("Unknown subcommand %s", argv[1]);
        }
    }

    return 0;
}
