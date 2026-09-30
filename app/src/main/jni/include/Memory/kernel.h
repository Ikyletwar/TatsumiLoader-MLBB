#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <sys/utsname.h>
#include <regex.h>

class KernelDriver {
private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    int fd = -1;
    pid_t pid;

    void animated_print(const char* str, int delay_ms) {
        while (*str) {
            printf("%c", *str++);
            fflush(stdout);
            usleep(delay_ms * 1000);
        }
    }

    typedef struct _COPY_MEMORY {
        pid_t pid;
        uintptr_t addr;
        void* buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _MODULE_BASE {
        pid_t pid;
        char* name;
        uintptr_t base;
    } MODULE_BASE, *PMODULE_BASE;

    enum OPERATIONS {
        OP_INIT_KEY = 0x800,
        OP_READ_MEM = 0x801,
        OP_WRITE_MEM = 0x802,
        OP_MODULE_BASE = 0x803,
    };

    int symbol_file(const char *filename) {
        int length = strlen(filename);
        for (int i = 0; i < length; i++) {
            if (isupper(filename[i])) has_upper = 1;
            else if (islower(filename[i])) has_lower = 1;
            else if (ispunct(filename[i])) has_symbol = 1;
            else if (isdigit(filename[i])) has_digit = 1;
        }
        return has_upper && has_lower && !has_symbol && !has_digit;
    }

    char *execCom(const char *shell) {
        FILE *fp = popen(shell, "r");
        if (fp == NULL) return NULL;
        char buffer[256];
        char *result = (char *)malloc(1000);
        result[0] = '\0';
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            strcat(result, buffer);
        }
        pclose(fp);
        return result;
    }

    void createDriverNode(char *path, int major_number, int minor_number) {
        std::string command = "mknod " + std::string(path) + " c " + std::to_string(major_number) + " " + std::to_string(minor_number);
        system(command.c_str());
    }

    void removeDeviceNode(char* path) {
        unlink(path);
    }

    const char* get_dev() {
        const char* command = "for dir in /proc/*/; do cmdline_file=\"cmdline\"; comm_file=\"comm\"; proclj=\"$dir$cmdline_file\"; proclj2=\"$dir$comm_file\"; if [[ -f \"$proclj\" && -f \"$proclj2\" ]]; then cmdline=$(head -n 1 \"$proclj\"); comm=$(head -n 1 \"$proclj2\"); if echo \"$cmdline\" | grep -qE '^/data/[a-z]{6}$'; then sbwj=$(echo \"$comm\"); open_file=\"\"; for file in \"$dir\"/fd/*; do link=$(readlink \"$file\"); if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\"; break; fi; done; if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\"); sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //'); echo \"/dev/$sbwj\"; rm -Rf \"/dev/$sbwj\"; mknod \"/dev/$sbwj\" c \"$sbid\" 0; break; fi; fi; fi; done";
        FILE* file = popen(command, "r");
        if (file == NULL) return NULL;
        char result[512];
        if (fgets(result, sizeof(result), file) == NULL) {
            pclose(file);
            return NULL;
        }
        pclose(file);
        int len = strlen(result);
        if (len > 0 && result[len - 1] == '\n') result[len - 1] = '\0';
        return strdup(result);
    }

    char *gtqwq() {
        const char *dev_path = "/dev";
        DIR *dir = opendir(dev_path);
        if (dir == NULL) return NULL;
        const char *files[] = { "wanbai", "CheckMe", "Ckanri", "lanran", "video188" };
        struct dirent *entry;
        char *gtfile_path = NULL;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
            gtfile_path = (char *)malloc(path_length);
            snprintf(gtfile_path, path_length, "%s/%s", dev_path, entry->d_name);
            for (int i = 0; i < 5; i++) {
                if (strcmp(entry->d_name, files[i]) == 0) {
                    closedir(dir);
                    return gtfile_path;
                }
            }
            struct stat file_info;
            if (stat(gtfile_path, &file_info) < 0) {
                free(gtfile_path);
                gtfile_path = NULL;
                continue;
            }
            if (strstr(entry->d_name, "gpiochip") != NULL) {
                free(gtfile_path);
                gtfile_path = NULL;
                continue;
            }
            if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
                && strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
                if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0 || strcmp(entry->d_name, "stderr") == 0) {
                    free(gtfile_path);
                    gtfile_path = NULL;
                    continue;
                }
                size_t file_name_length = strlen(entry->d_name);
                time_t current_time;
                time(&current_time);
                int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
                if (file_year <= 1980) {
                    free(gtfile_path);
                    gtfile_path = NULL;
                    continue;
                }
                time_t atime = file_info.st_atime;
                time_t ctime = file_info.st_ctime;
                if ((atime == ctime)) {
                    if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0 && file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
                        closedir(dir);
                        return gtfile_path;
                    }
                }
            }
            free(gtfile_path);
            gtfile_path = NULL;
        }
        closedir(dir);
        return NULL;
    }

    char *driver_path() {
        struct dirent *de;
        DIR *dr = opendir("/proc");
        char *device_path = NULL;
        if (dr == NULL) return NULL;
        while ((de = readdir(dr)) != NULL) {
            if (strlen(de->d_name) != 8 || strcmp(de->d_name, "zoneinfo") == 0 || strcmp(de->d_name, "softirqs") == 0 || strcmp(de->d_name, "kallsyms") == 0 || strcmp(de->d_name, "consoles") == 0 || strcmp(de->d_name, "vmstat") == 0 || strcmp(de->d_name, "uptime") == 0 || strcmp(de->d_name, "NVTSPI") == 0 || strcmp(de->d_name, "aputag") == 0 || strcmp(de->d_name, "asound") == 0 || strcmp(de->d_name, "clkdbg") == 0 || strcmp(de->d_name, "crypto") == 0 || strcmp(de->d_name, "mounts") == 0 || strcmp(de->d_name, "pidmap") == 0 || strcmp(de->d_name, "bootprof") == 0) continue;
            int is_valid = 1;
            for (int i = 0; i < 8; i++) {
                if (!isalnum(de->d_name[i])) {
                    is_valid = 0;
                    break;
                }
            }
            if (is_valid) {
                device_path = (char*)malloc(11 + strlen(de->d_name));
                sprintf(device_path, "/proc/%s", de->d_name);
                struct stat sb;
                if (stat(device_path, &sb) == 0 && S_ISREG(sb.st_mode)) break;
                else {
                    free(device_path);
                    device_path = NULL;
                }
            }
        }
        closedir(dr);
        return device_path;
    }

    char *find_driver_path() {
        const char *dev_path = "/dev";
        DIR *dir = opendir(dev_path);
        if (dir == NULL) return NULL;
        const char *files[] = {"wanbai","CheckMe","Ckanri","lanran","video188"};
        struct dirent *entry;
        char *file_path = NULL;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "facking") == 0 || strcmp(entry->d_name, "vndbinder") == 0 || strcmp(entry->d_name, "YBBBYN") == 0) continue;
            size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
            file_path = (char *)malloc(path_length);
            snprintf(file_path, path_length, "%s/%s", dev_path, entry->d_name);
            for (int i = 0; i < 5; i++) {
                if (strcmp(entry->d_name, files[i]) == 0) {
                    closedir(dir);
                    return file_path;
                }
            }
            struct stat file_info;
            if (stat(file_path, &file_info) < 0) {
                free(file_path);
                file_path = NULL;
                continue;
            }
            if (strstr(entry->d_name, "gpiochip") != NULL) {
                free(file_path);
                file_path = NULL;
                continue;
            }
            if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
                && strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
                if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0 || strcmp(entry->d_name, "stderr") == 0) {
                    free(file_path);
                    file_path = NULL;
                    continue;
                }
                size_t file_name_length = strlen(entry->d_name);
                int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
                if (file_year <= 1980) {
                    free(file_path);
                    file_path = NULL;
                    continue;
                }
                time_t atime = file_info.st_atime;
                time_t ctime = file_info.st_ctime;
                if ((atime == ctime)) {
                    if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0 && file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
                        closedir(dir);
                        return file_path;
                    }
                }
            }
            free(file_path);
            file_path = NULL;
        }
        closedir(dir);
        return NULL;
    }

public:
    KernelDriver() {
        const char *green  = "\033[92m";
        const char *cyan   = "\033[96m";
        const char *yellow = "\033[93m";
        const char *red    = "\033[91m";
        const char *white  = "\033[97m";
        const char *reset  = "\033[0m";

        system("clear");
        printf("%s", cyan);
        animated_print("  ____  _                               _ \n", 1);
        animated_print(" / ___|| |_ __ _ _ __ ___ ___   ___   | |\n", 1);
        animated_print(" \\___ \\| __/ _` | '__/ __/ _ \\ / _ \\  | |\n", 1);
        animated_print("  ___) | || (_| | | | (_| (_) | (_) | |_|\n", 1);
        animated_print(" |____/ \\__\\__,_|_|  \\___\\___/ \\___/  (_)\n", 1);
        printf("        %s[ C R E A T E D  B Y  S T A R C O O L ]%s\n\n", yellow, cyan);

        animated_print("[+] System Status: READY\n", 10);
        animated_print("[+] Kernel Version: UNLOCKED\n\n", 10);

        printf("%s┌──────────────────────────────────────────────┐\n", green);
        printf("│ %s[1] %sQX Driver v1 (High Compatibility)       %s│\n", yellow, white, green);
        printf("│ %s[2] %sGT Driver (Special)                     %s│\n", yellow, white, green);
        printf("│ %s[3] %srt-dev %s[RECOMMENDED]                  %s│\n", yellow, cyan, red, green);
        printf("│ %s[4] %srt-proc (Alternate)                    %s│\n", yellow, white, green);
        printf("│ %s[5] %sQX Driver v2 (New Engine)               %s│\n", yellow, white, green);
        printf("└──────────────────────────────────────────────┘\n");

        int choice = 0;
        printf("%s\n [#] Select Driver > ", yellow);
        if (scanf("%d", &choice) != 1) {
            printf("%s[!] Invalid Input! Exiting...\n", red);
            exit(0);
        }
        printf("%s", reset);

        if (choice == 1) {
            char* dev_path_str = (char*)get_dev();
            if (dev_path_str) {
                fd = open(dev_path_str, O_RDWR);
                if (fd > 0) {
                    printf("Driver file: %s\n", dev_path_str);
                    unlink(dev_path_str);
                }
                free(dev_path_str);
            }
        } else if (choice == 2) {
            char *device_name = gtqwq();
            if (device_name) {
                fd = open(device_name, O_RDWR);
                free(device_name);
            }
        } else if (choice == 3) {
            char *device_name = find_driver_path();
            if (device_name) {
                fd = open(device_name, O_RDWR);
                free(device_name);
            }
        } else if (choice == 4) {
            char *device_name = driver_path();
            if (device_name) {
                fd = open(device_name, O_RDWR);
                free(device_name);
            }
        } else if (choice == 5) {
            char *output = execCom("ls -l /proc/*/exe 2>/dev/null | grep -E \"/data/[a-z]{6} \\(deleted\\)\"");
            if (output != NULL) {
                char filePath[256] = {0};
                char pid_str[56] = {0};
                char *procStart = strstr(output, "/proc/");
                if (procStart) {
                    char *pidStart = procStart + 6;
                    char *pidEnd = strchr(pidStart, '/');
                    if (pidEnd) {
                        strncpy(pid_str, pidStart, pidEnd - pidStart);
                        char *arrowStart = strstr(output, "->");
                        if (arrowStart) {
                            char *start = arrowStart + 3;
                            char *end = strchr(start, '(');
                            if (end) {
                                strncpy(filePath, start, end - start - 1);
                                char *replacePtr = strstr(filePath, "data");
                                if (replacePtr != NULL) {
                                    memmove(replacePtr + 3, replacePtr + 4, strlen(replacePtr + 4) + 1);
                                    memcpy(replacePtr, "dev", 3);
                                }
                                char cmd[256];
                                sprintf(cmd, "ls -al -L /proc/%s/fd/3", pid_str);
                                char *fdInfo = execCom(cmd);
                                if (fdInfo) {
                                    int major_number, minor_number;
                                    if (sscanf(fdInfo, "%*s %*d %*s %*s %d, %d", &major_number, &minor_number) == 2) {
                                        createDriverNode(filePath, major_number, minor_number);
                                        sleep(1);
                                        fd = open(filePath, O_RDWR);
                                        if (fd > 0) removeDeviceNode(filePath);
                                    }
                                    free(fdInfo);
                                }
                            }
                        }
                    }
                }
                free(output);
            }
        }

        if (fd <= 0) {
            printf("[-] Kernel driver not detected\n");
            exit(0);
        }
    }

    ~KernelDriver() {
        if (fd > 0) close(fd);
    }

    void initialize(pid_t target_pid) {
        this->pid = target_pid;
    }

    bool init_key(char* key) {
        if (ioctl(fd, OP_INIT_KEY, key) != 0) return false;
        return true;
    }

    bool read_memory(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;
        if (ioctl(fd, OP_READ_MEM, &cm) != 0) return false;
        return true;
    }

    bool write_memory(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;
        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) return false;
        return true;
    }

    template <typename T>
    T read_memory(uintptr_t addr) {
        T res{};
        if (this->read_memory(addr, &res, sizeof(T))) return res;
        return {};
    }

    template <typename T>
    bool write_memory(uintptr_t addr, T value) {
        return this->write_memory(addr, &value, sizeof(T));
    }

    uintptr_t get_module_base(char* name) {
        MODULE_BASE mb;
        mb.pid = this->pid;
        mb.name = name;
        if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) return 0;
        return mb.base;
    }
};

static KernelDriver *g_driver = nullptr;
