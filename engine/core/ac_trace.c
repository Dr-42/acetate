#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ac_trace.h"

static char prg_name[1024];

void ac_print_trace(size_t offset) { ac_fprint_trace(stdout, offset); }

int ac_fprint_trace(FILE *fp, size_t offset) {
    char buffer[1024];
    int len = ac_sprint_trace(buffer, offset);
    if (len < 0) {
        return -1;
    }
    fprintf(fp, "%s\n", buffer);
    return len;
}

#define _GNU_SOURCE
#include <execinfo.h>
#include <sys/wait.h>
#include <unistd.h>
#define __USE_GNU
#include <dlfcn.h>

#define MAX_STACK_FRAMES 64
void *buffer[MAX_STACK_FRAMES];

int ac_get_intermediate_trace(void **stack, size_t size) {
    return backtrace(stack, size);
}

int ac_sprint_intermediate_trace(void **stack, char *buffer, size_t offset,
                                 size_t size) {
    size_t prg_len = readlink("/proc/self/exe", prg_name, 1024);
    prg_name[prg_len] = '\0';
    buffer[0] = '\0';
    char tmp[1024];
    for (size_t i = offset + 1; i < size; i++) {
        // Execute addr2line and get prettified names
        char addr2line_cmd[512];
        void *addr = stack[i] - 1;
        Dl_info info;
        if (dladdr(addr, &info) != 0) {
            void *offset = (void *)((char *)addr - (char *)info.dli_fbase);
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", info.dli_fname,
                    offset);
        } else {
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", prg_name, addr);
        }
        char line[512];
        FILE *addr2line = popen(addr2line_cmd, "r");
        fgets(line, 512, addr2line);
        pclose(addr2line);
        if (line[0] == '?') {
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", prg_name, addr);
            FILE *addr2line = popen(addr2line_cmd, "r");
            fgets(line, 512, addr2line);
            pclose(addr2line);
        }
        // Remove pwd from path
        char *pwd = getenv("PWD");
        char *pwd_pos = strstr(line, pwd);
        if (pwd_pos != NULL) {
            // Preserve text before pwd
            char *line_pos = line;
            char *pwd_pos = strstr(line, pwd);
            while (line_pos != pwd_pos) {
                strncat(buffer, line_pos, 1);
                line_pos++;
            }
            // Skip pwd
            line_pos += strlen(pwd);
            // Skip slash
            line_pos++;
            // Print rest of line
            sprintf(tmp, "%s", line_pos);
        } else {
            sprintf(tmp, "%s", line);
        }
        strcat(buffer, tmp);
        // Stop at main
        if (strstr(line, "main at") != NULL) {
            break;
        }
    }
    buffer[strlen(buffer) - 1] = '\0';
    return strlen(buffer);
}

int ac_sprint_trace(char *buff, size_t offset) {
    size_t prg_len = readlink("/proc/self/exe", prg_name, 1024);
    prg_name[prg_len] = '\0';
    buff[0] = '\0';
    int size = backtrace(buffer, MAX_STACK_FRAMES);
    char tmp[1024];
    for (int i = offset; i < size; i++) {
        // Execute addr2line and get prettified names
        char addr2line_cmd[512];
        void *addr = buffer[i] - 1;
        Dl_info info;
        if (dladdr(addr, &info) != 0) {
            void *offset = (void *)((char *)addr - (char *)info.dli_fbase);
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", info.dli_fname,
                    offset);
        } else {
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", prg_name, addr);
        }
        char line[512];
        FILE *addr2line = popen(addr2line_cmd, "r");
        fgets(line, 512, addr2line);
        pclose(addr2line);
        if (line[0] == '?') {
            sprintf(addr2line_cmd, "addr2line -p -f -e %s %p", prg_name, addr);
            FILE *addr2line = popen(addr2line_cmd, "r");
            fgets(line, 512, addr2line);
            pclose(addr2line);
        }
        // Remove pwd from path
        char *pwd = getenv("PWD");
        char *pwd_pos = strstr(line, pwd);
        if (pwd_pos != NULL) {
            // Preserve text before pwd
            char *line_pos = line;
            char *pwd_pos = strstr(line, pwd);
            while (line_pos != pwd_pos) {
                strncat(buff, line_pos, 1);
                line_pos++;
            }
            // Skip pwd
            line_pos += strlen(pwd);
            // Skip slash
            line_pos++;
            // Print rest of line
            sprintf(tmp, "%s", line_pos);
        } else {
            sprintf(tmp, "%s", line);
        }
        strcat(buff, tmp);
        // Stop at main
        if (strstr(line, "main at") != NULL) {
            break;
        }
    }
    buff[strlen(buff) - 1] = '\0';
    return strlen(buff);
}
