#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <bits/fcntl-linux.h>
#include <sys/stat.h>
const char *dir_separator = "------------------------------------------\n";
const char *default_dir_path = ".";
const int s_buf_size = 256;
const char *long_listing_format = "%s\t%s %u %u %s %ld \n";

void format_time(const time_t raw_time, char *buffer, size_t buf_size) {
    const struct tm *time_info = localtime(&raw_time);
    strftime(buffer, buf_size, "%Y-%m-%d %H:%M:%S", time_info);
}

void mode_to_string(mode_t mode, char *str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-');

    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-');

    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? ((mode & S_ISVTX) ? 't' : 'x') : ((mode & S_ISVTX) ? 'T' : '-');

    str[9] = '\0';  // Null-terminate the string
}

void next_entry(DIR *dirp) {
    const long int curr_pos = telldir(dirp);
    seekdir(dirp, curr_pos);
}

void get_full_sub_entry_name(char full_entry_name[768], const char *dir_name, char *d_name) {
    sprintf(full_entry_name + strlen(full_entry_name), "%s", dir_name);
    sprintf(full_entry_name + strlen(full_entry_name), "%s", "/");
    sprintf(full_entry_name + strlen(full_entry_name), "%s", d_name);
}

void print_long_listing_format(DIR *dirp, const char *dir_name, char *d_name) {
    char full_entry_name[s_buf_size*3] = {};
    get_full_sub_entry_name(full_entry_name, dir_name, d_name);

    const int fd = open(full_entry_name, O_RDONLY);
    if (fd < 0) {
        printf("\nError opening %s for fstat: ", full_entry_name);
        perror("Can't do long listing format on entry because");
        next_entry(dirp);
        return;
    }
    struct stat st;
    fstat(fd, &st);
    char mode[s_buf_size] = {};
    mode_to_string(st.st_mode, mode);

    char mod_time[s_buf_size] = {};
    format_time(st.st_mtim.tv_sec,mod_time, s_buf_size);

    printf(long_listing_format, d_name, mode,st.st_uid, st.st_gid, mod_time, st.st_size);
    close(fd);
}

void print_dir_contents(DIR *dirp, const char* dir_name,const bool list_mode, const bool show_hidden) {
    struct dirent *curr_entry;
    while ((curr_entry = readdir(dirp)) != NULL) {
        char *d_name = curr_entry->d_name;

        if (!show_hidden && d_name[0] == '.') {
            next_entry(dirp);
            continue;
        }

        if (list_mode) {
            print_long_listing_format(dirp, dir_name, d_name);
        } else {
            printf("%s\n", d_name);
        }
        next_entry(dirp);
    }
}

void parse_args_to_paths_and_flags(const int argc, char **argv, const char *dir_paths[], const char *flags[]) {
    if (argc > 1) {
        int dir_count = 0;
        int flags_count = 0;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-' || strlen(argv[i]) < 2) {
                dir_paths[dir_count] = argv[i];
                dir_count++;
            } else {
                flags[flags_count] = argv[i];
                flags_count++;
            }
        }
    }
}

void handle_dir_entry(const char *dir_name, bool list_mode, bool show_hidden) {
    if (dir_name != NULL) {
        DIR *dirp = opendir(dir_name);
        if (dirp == NULL) {
            printf("Error fetching dir entry for name: %s\n", dir_name);
            perror("In handle dir entry");
        } else {
            print_dir_contents(dirp, dir_name, list_mode, show_hidden);
            closedir(dirp);
        }
        printf("%s", dir_separator);
    }
}

void set_settings_from_flags(const char *flags[], bool *list_mode, bool *show_hidden) {
    for (size_t i = 0; flags[i] != NULL; i++) {
        if (strcmp(flags[i], "-l") == 0) {
            *list_mode = true;
        } else if (strcmp(flags[i], "-a") == 0) {
            *show_hidden = true;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *dir_paths[argc] = {};
    const char *flags[argc] = {};
    bool list_mode = false;
    bool show_hidden = false;
    dir_paths[0] = default_dir_path;
    parse_args_to_paths_and_flags(argc, argv, dir_paths, flags);

    set_settings_from_flags(flags, &list_mode, &show_hidden);

    for (int dir_path = 0; dir_path < argc; dir_path++) {
        const char *dir_name = dir_paths[dir_path];
        handle_dir_entry(dir_name, list_mode, show_hidden);
    }
    return 0;
}
