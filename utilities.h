#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string.h>
#include <dirent.h>     
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

#define STDINFD 0
#define STDOUTFD 1
#define STDERRFD 2
#define MAX_STR_SIZE 2000
#define MAX_ARRAY_SIZE 20
#define GETMAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define GETMIN(X,Y) ((Y) > (X) ? (X) : (Y))

struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

struct dst_port {
	char dst[MAX_STR_SIZE];
	char port[MAX_STR_SIZE];
};

/* in the maping type-section to nodes:
// 00 is for client
// 01 is for service_provider
// 10 is for switch
// 11 is for server
*/

void framing(char* type,char* dstAdd,char* srcAdd,char* data,char* sender_port,char* frame);
int change_ip_seed(int c);
int read_ip_seed();
int create_directories(char path_name[MAX_STR_SIZE]);
int strlength(char str[MAX_STR_SIZE]);
void tokenizer(char str[MAX_STR_SIZE], char delim[MAX_STR_SIZE], int* num_of_tokens, char res[MAX_ARRAY_SIZE][MAX_STR_SIZE]);
int clear_buff(char* c, int size);
int clear_buff_int(int* c, int size);
int mystrcmp(char const*p, char const *q);
int mystrcmp_to(char const*p, char const *q, int to);
void print(char str[MAX_STR_SIZE]);
void int_to_str(int val, char res[20], int base);
int days_in_month(int month, int year);
void get_date(char time_str [30]);
void list_files(char pathname[MAX_STR_SIZE], char file_names[MAX_ARRAY_SIZE][MAX_STR_SIZE], int* file_num);
#endif
