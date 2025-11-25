#include "types.h"
#include "fcntl.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "user.h"
#include "fs.h"
#include "file.h"

#define member_size(type, member) sizeof(((type *)0)->member)

int can_read(char *name) {
    int i, fd;
    char data;
    fd = open(name, O_RDONLY);
    if (fd == -1)
        return 0;
    i = read(fd, &data, 1);
    close(fd);
    if (i != 1)
        return 0;
    return 1;
}

int can_write(char *name) {
    int i, fd;
    fd = open(name, O_WRONLY);
    if (fd == -1)
        return 0;
    i = write(fd, "A", 1);
    close(fd);
    if (i != 1)
        return 0;
    return 1;
}

int
main(int argc, char *argv[])
{
    int fd;

    if (sizeof(struct dinode) != 64) {
        printf(2, "Test 0 failed, struct dinode does not preserves original "
                  "size of 64. Current size %d\n", sizeof(struct dinode));
        exit();
    }

    //Test 1
    if (sizeof(struct inode) != 144) {
        printf(2, "Test 1 failed, struct inode does not preserves original "
                  "size of 144. Current size %d\n", sizeof(struct dinode));
        exit();
    }

    // Test 2
    chmod("ls", S_IREAD);
    if (can_read("ls") == 0) {
        printf(2, "Test 2 failed, could not read ls\n");
        exit();
    }
    // Test 3
    chmod("ls", 0);
    if (can_read("ls") == 1) {
        printf(2, "Test 3 failed, was able to read ls\n");
        exit();
    }
    chmod("ls", S_IREAD | S_IWRITE); 

    // Test 4, root dir was not affected by permissions
    fd = open("newfile", O_CREATE);
    if (fd == -1) {
        printf(2, "Test 4 failed, could not create newfile\n");
        exit();
    }
    close(fd);
    // Test 5,  new files have all permissions by default
    if (can_write("newfile") == 0 || can_read("newfile") == 0) {
        printf(2, "Test 5 failed, could not write/read newfile\n");
        exit();
    }
    // Test 6, cannot re-write file without write permission
    chmod("newfile", S_IREAD);
    fd = open("newfile", O_CREATE);
    if (fd != -1) {
        printf(2, "Test 6 failed, was able to overwrite newfile\n");
        exit();
    }
    unlink("newfile"); //Delete new file so we can re run test

    printf(1, "Tests passed successfully\n");
    exit();
}
