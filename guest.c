#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define FILE_SIZE (4096)

char *map;
int fd;
long page_size, map_size;

static int open_mmap(void)
{
    fd = open("/tmp/testFile", O_RDWR);

    if(fd < 0) {
        fprintf(stderr, "Error : can't open file\n");
        return -1;
    }

    /* ページサイズからマッピング時のサイズを計算 */
    page_size = getpagesize();
    map_size = (FILE_SIZE / page_size + 1) * page_size;

    /* メモリ上にマッピング。今回は文字列データとして扱えるようにする */
    map = (char*)mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mmap == MAP_FAILED) {
        fprintf(stderr, "Error : mmap failed\n");
        return -1;
    }

    memset(map, 0, FILE_SIZE);
    return 0;
}

int main(int argc, char *argv[])
{
    int err;
    char *dest;

    printf("beremetal guest test\n");
    /* ファイルの中身をはき出す */

    err = open_mmap();
    if (err)
        return 1;

    dest = map;
    while (1) {
        sleep(1);
        dest[2]++;
    }

    close(fd);
    munmap(map, map_size);

    return 0;
}
