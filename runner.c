#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/inotify.h>

#define FILE_SIZE (4096)

char *map;
int map_fd;
long page_size, map_size;



static int open_mmap(void)
{
    map_fd = open("/tmp/testFile", O_RDWR);

    if(map_fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    page_size = getpagesize();
    map_size = (FILE_SIZE / page_size + 1) * page_size;

    map = (char*)mmap(NULL, map_size, PROT_READ | PROT_WRITE,
                      MAP_SHARED, map_fd, 0);

    if (mmap == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    memset(map, 0, FILE_SIZE);
    return 0;
}

void dump(void *p, int size)
{
    char* addr = p;
    char* end = addr + size;
    bool all_zero;
    int i;

    printf("--- dump\n");
    while (addr < end) {

        all_zero = true;
        for (i = 0; (i < 16) && (addr + i < end); i++)
            if (addr[i] != 0)
                all_zero = false;
        if (!all_zero) {
            printf("0x%04x: ", (unsigned int)(void*) addr);
            for (i = 0; (i < 16) && (addr + i < end); i++)
                printf("%02x ", addr[i]);
            printf("\n");
        }
        addr += 16;
    }
    printf("---\n");
}

int notify_fd;
int watch_fd;

void init_notify(void)
{
    notify_fd = inotify_init();
    if (notify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    watch_fd = inotify_add_watch(notify_fd, "/tmp/testFile", IN_MODIFY);
    //watch_fd = inotify_add_watch(fd, "/tmp/testFile", IN_ALL_EVENTS);
    if (watch_fd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
}

void close_notify(void)
{
    inotify_rm_watch( notify_fd, watch_fd );
    close( notify_fd );
}

int main(int argc, char *argv[])
{
    int err;
    static char buffer[1];

    printf("beremetal runner\n");

    err = open_mmap();
    if (err)
        return 1;

    init_notify();


    while (1) {
        read(notify_fd, buffer, sizeof(buffer));
        dump(map, FILE_SIZE);
        exit(0);
    }

    close_notify();

    close(map_fd);
    munmap(map, map_size);

    return 0;
}
