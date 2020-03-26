#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

regex_t regex;

char *info_fmt[] = {"%*d%*c%s%s%*s%*s%*s%*s%*s%*s%d%*s%*s%*s%*s%*s%*s%*s",
                    "%*d%*c%s%s%*s%*s%*s%*s%*s%*s%d%*s%*s%*s"};

char *prototype[4] = {"tcp", "tcp6", "udp", "udp6"};

typedef struct SOCKET_INFO {
    char local_addr[64];
    char rem_addr[64];
    int inode_id;
    pid_t pid;
    char p_name[64];
    char cmdline[1024];
    struct SOCKET_INFO *next;
} socket_info;

typedef struct NETSTATUS {
    socket_info *list[4];
} net_table;

socket_info *insert_socket_list(socket_info *head, socket_info *new);
void free_list(socket_info *head);
void free_net_table(net_table *tab);
void dump_list(socket_info *head, char *proto, char *filter, int reg_fail);
void dump_net_table(net_table *tab, int tcp_flag, int udp_flag, char *filter);
DIR *opendir_or_fail(char *path);
FILE *fopen_or_fail(char *path, char *mode);
int hex_to_bin(char ch);
int hex2bin(char *src, size_t count);
void convert_ipv4(char *addr, size_t len);
void convert_ipv6(char *addr, size_t len);
void load_net_table(net_table *tab);
socket_info *find_target_socket_info(socket_info *head, int inode_target);
socket_info *find_target(net_table *tab, int inode_target);
void netstate(net_table *tab);

int main(int argc, char **argv)
{
    int c;
    char *filter = NULL;
    int tcp_flag = 1, udp_flag = 1;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {{"tcp", no_argument, 0, 't'},
                                               {"udp", no_argument, 0, 'u'},
                                               {NULL, 0, 0, 0}};

        c = getopt_long(argc, argv, "tu", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 't':
            tcp_flag = 1;
            udp_flag = 0;
            break;
        case 'u':
            tcp_flag = 0;
            udp_flag = 1;
            break;
            break;
        case '?':
            printf("unknown option\n");
            tcp_flag = 0;
            udp_flag = 0;
            break;
        default:
            tcp_flag = 1;
            udp_flag = 1;
        }
    }
    if (optind < argc) {
        int size = 0;
        for (int i = optind; i < argc; i++)
            size += (strlen(argv[i]) + 1);
        filter = malloc(size * sizeof(char));
        for (int i = optind; i < argc - 1; i++) {
            filter = strcat(filter, argv[i]);
            filter = strcat(filter, " ");
        }
        filter = strcat(filter, argv[argc - 1]);
    }

    net_table *tab = malloc(sizeof(net_table));
    load_net_table(tab);
    netstate(tab);
    dump_net_table(tab, tcp_flag, udp_flag, filter);
    free_net_table(tab);
    return 0;
}

socket_info *insert_socket_list(socket_info *head, socket_info *new)
{
    if (!head)
        return new;
    socket_info *iter = head;
    for (; iter->next; iter = iter->next)
        ;
    iter->next = new;
    return head;
}

void free_list(socket_info *head)
{
    if (head->next)
        free_list(head->next);
    free(head);
}

void free_net_table(net_table *tab)
{
    for (int i = 0; i < 4; i++)
        free_list(tab->list[i]);
}

void dump_list(socket_info *head, char *proto, char *filter, int reg_fail)
{
    do {
        if (!head)
            return;
    } while (0);

    if (!reg_fail) {
        if (regexec(&regex, head->cmdline, 0, NULL, 0) == 0 ||
            regexec(&regex, head->p_name, 0, NULL, 0) == 0)
            printf("%5s\t%-22s\t%-22s\t%-d/%-s%-40s\n", proto, head->local_addr,
                   head->rem_addr, head->pid, head->p_name, head->cmdline);
    } else if (head->pid == 0)
        printf("%5s\t%-22s\t%-22s\t-\n", proto, head->local_addr,
               head->rem_addr);
    else {
        if (strlen(head->cmdline) > 0)
            printf("%5s\t%-22s\t%-22s\t%-d/%-s:%-40s\n", proto,
                   head->local_addr, head->rem_addr, head->pid, head->p_name,
                   head->cmdline);
        else
            printf("%5s\t%-22s\t%-22s\t%-d/%-s%-40s\n", proto, head->local_addr,
                   head->rem_addr, head->pid, head->p_name, head->cmdline);
    }
    if (head->next)
        dump_list(head->next, proto, filter, reg_fail);
}

void dump_net_table(net_table *tab, int tcp_flag, int udp_flag, char *filter)
{
    int reg_fail = 1;
    if (filter)
        reg_fail = !(regcomp(&regex, filter, 0) == 0);
    for (int i = 0; i < 4; i++) {
        if (i < 2 && !tcp_flag)
            continue;
        if (i >= 2 && !udp_flag)
            continue;

        if (i == 0) {
            printf("List of TCP connections:\n");
            printf(
                "proto\tLocal Address\t\tForeign Address\t\tPID/Program name "
                "and arguments\n");
        } else if (i == 2) {
            if (tcp_flag)
                printf("\n");
            printf("List of UDP connections:\n");
            printf(
                "proto\tLocal Address\t\tForeign Address\t\tPID/Program name "
                "and arguments\n");
        }
        dump_list(tab->list[i], prototype[i], filter, reg_fail);
    }
    if (!reg_fail)
        regfree(&regex);
}

DIR *opendir_or_fail(char *path)
{
    DIR *dptr;
    if ((dptr = opendir(path)) == NULL) {
        perror("opendir");
        fprintf(stderr, "Could not open %s\n", path);
        exit(EXIT_FAILURE);
    }
    return dptr;
}

FILE *fopen_or_fail(char *path, char *mode)
{
    FILE *fptr;
    if ((fptr = fopen(path, mode)) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return fptr;
}

int hex_to_bin(char ch)
{
    if (ch >= '0' && ch <= '9')
        return (int) ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return (int) ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return (int) ch - 'A' + 10;
    return -1;
}

int hex2bin(char *src, size_t count)
{
    int ret;
    while (count--) {
        int hi = hex_to_bin(*src++);
        int lo = hex_to_bin(*src++);
        /* bad input */
        if (hi < 0 || lo < 0)
            return -EINVAL;
        ret = (hi << 4) | lo;
    }
    return ret;
}

void convert_ipv4(char *addr, size_t len)
{
    struct in_addr ipv4;
    ipv4.s_addr = (in_addr_t) 0;
    int port = 0;
    for (int i = 0; i < 2; i++) {
        port |= (hex2bin(addr + 9 + 2 * i, 1) << (1 - i) * 8);
    }

    for (int i = 0; i < 4; i++) {
        ipv4.s_addr |= ((hex2bin(addr + 2 * i, 1)) << ((3 - i) * 8));
    }

    inet_ntop(AF_INET, &ipv4, addr, len);
    if (port != 0) {
        sprintf(addr + strlen(addr), ":%d", port);
    } else {
        sprintf(addr + strlen(addr), ":*");
    }
}

void convert_ipv6(char *addr, size_t len)
{
    struct in6_addr ipv6;
    int port = 0;
    for (int i = 0; i < 2; i++) {
        port |= (hex2bin(addr + 33 + 2 * i, 1) << (1 - i) * 8);
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            ipv6.__in6_u.__u6_addr8[i * 4 + (3 - j)] =
                hex2bin(addr + (i * 4 + j) * 2, 1);
    }
    inet_ntop(AF_INET6, &ipv6, addr, len);
    if (port != 0) {
        sprintf(addr + strlen(addr), ":%d", port);
    } else {
        sprintf(addr + strlen(addr), ":*");
    }
}

void load_net_table(net_table *tab)
{
    char *path = NULL;
    FILE *fptr = NULL;

    for (int i = 0; i < 4; i++) {
        path = malloc(sizeof(char) * 10 + strlen(prototype[i]) + 1);
        sprintf(path, "/proc/net/%s", prototype[i]);
        fptr = fopen_or_fail(path, "r");
        fscanf(fptr, "%*[^\n]\n");

        socket_info *new;
        size_t line_buf_size = 1024;
        char *line_buf = malloc(1024 * sizeof(char));
        while (getline(&line_buf, &line_buf_size, fptr) > 0) {
            new = malloc(sizeof(socket_info));
            new->next = NULL;
            sscanf(line_buf, info_fmt[i / 2], new->local_addr, new->rem_addr,
                   &new->inode_id);
            if (i % 2 == 0) {
                convert_ipv4(new->local_addr, sizeof(new->local_addr));
                convert_ipv4(new->rem_addr, sizeof(new->rem_addr));
            } else {
                convert_ipv6(new->local_addr, sizeof(new->local_addr));
                convert_ipv6(new->rem_addr, sizeof(new->rem_addr));
            }
            tab->list[i] = insert_socket_list(tab->list[i], new);
        }
        free(line_buf);
        fclose(fptr);
        free(path);
    }
}

socket_info *find_target_socket_info(socket_info *head, int inode_target)
{
    for (socket_info *iter = head; iter; iter = iter->next)
        if (iter->inode_id == inode_target)
            return iter;
    return NULL;
}

socket_info *find_target(net_table *tab, int inode_target)
{
    socket_info *target = NULL;
    for (int i = 0; i < 4; i++) {
        if ((target = find_target_socket_info(tab->list[i], inode_target)))
            return target;
    }
    return target;
}

void netstate(net_table *tab)
{
    DIR *proc;
    struct dirent *proc_dirent;
    proc = opendir_or_fail("/proc");

    regcomp(&regex, "[0-9]", 0);
    while ((proc_dirent = readdir(proc)) != NULL) {
        /* read /proc/[pid] */
        if (proc_dirent->d_type == DT_DIR &&
            regexec(&regex, proc_dirent->d_name, 0, NULL, 0) == 0) {
            char *pid_fd = NULL;

            /* set path /proc/[pid]/fd */
            pid_fd = realloc(pid_fd, strlen(proc_dirent->d_name) + 4);
            sprintf(pid_fd, "/proc/%s/fd", proc_dirent->d_name);
            DIR *dir_fd;

            /* traverse /proc/[pid]/fd */
            dir_fd = opendir(pid_fd);
            if (dir_fd == NULL)
                continue;
            struct dirent *fd_dirent;

            /* read /proc/[pid]/fd/... */
            while ((fd_dirent = readdir(dir_fd)) != NULL) {
                /* check symbolic link or not */
                if (fd_dirent->d_type == DT_LNK) {
                    size_t buf_size = 64;
                    char *lnk_buf = calloc(buf_size, sizeof(char));
                    int count = 0;
                    char *link_path =
                        malloc(strlen(pid_fd) + strlen(fd_dirent->d_name) + 2);
                    sprintf(link_path, "%s/%s", pid_fd, fd_dirent->d_name);
                    readlink(link_path, lnk_buf, buf_size);
                    lnk_buf[strlen(lnk_buf)] = '\0';

                    /* format: "socket:[inode]" */
                    if (strncmp(lnk_buf, "socket", 6) == 0) {
                        count++;
                        int inode;
                        sscanf(lnk_buf, "socket:[%d]", &inode);
                        socket_info *target = find_target(tab, inode);
                        if (target && target->pid == 0) {
                            char *path_comm = malloc(strlen(pid_fd) + 9);
                            char *path_cmdline = malloc(strlen(pid_fd) + 12);
                            sprintf(path_comm, "%s/../comm", pid_fd);
                            sprintf(path_cmdline, "%s/../cmdline", pid_fd);

                            FILE *f_comm = fopen_or_fail(path_comm, "r");
                            FILE *f_cmdline = fopen_or_fail(path_cmdline, "r");
                            /* set target's socket_info */
                            target->pid = (pid_t) atoi(proc_dirent->d_name);
                            target->inode_id = inode;
                            fscanf(f_comm, "%s", target->p_name);
                            fscanf(f_cmdline, "%*s%[^\n]s", target->cmdline);

                            fclose(f_cmdline);
                            fclose(f_comm);
                            free(path_cmdline);
                            free(path_comm);
                        }
                    }
                    free(link_path);
                    free(lnk_buf);
                }
            }
        }
    }
}
