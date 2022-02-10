#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid = ps();

    if(pid == getpid()) {
        printf("ps test: OK\n");
        exit(0);
    }
    else {
        printf("ps test: FAIL\n");
        exit(1);
    }
}
