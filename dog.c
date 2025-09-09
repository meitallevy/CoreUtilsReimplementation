#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/param.h>

static long get_file_size(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size during fstat");
        return -1;
    }
    return st.st_size;
}

static void write_fd_content_to_stdout(int fd, ssize_t file_buffer_size) {
    ssize_t readed = 0;
    unsigned char file_buffer[file_buffer_size] = {};

    while ((readed = read(fd, file_buffer, sizeof(file_buffer))) > 0) {
        ssize_t written = write(STDOUT_FILENO, file_buffer, readed);
        printf("read %ld and written %ld\n", readed, written);
    }
}

int main(int argc, char *argv[]) {
    int fd = 0;
    ssize_t file_buffer_size = 4096;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                perror("Unable to open file");
                return -1;
            }
            file_buffer_size = MIN(get_file_size(fd), file_buffer_size);
            write_fd_content_to_stdout(fd, file_buffer_size);
        }
    } else {
        write_fd_content_to_stdout(fd, file_buffer_size);
    }
    close(fd);
    return 0;
}
