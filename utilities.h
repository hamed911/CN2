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

typedef enum { false, true } bool;

struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

typedef struct ip_fd{
	char ip[MAX_STR_SIZE];
	int fd;
} ip_fd;

typedef struct dst_port {
	char dst[MAX_STR_SIZE];
	char port[MAX_STR_SIZE];
} dst_port;

/* in the maping type-section to nodes:
// 00 is for client
// 01 is for service_provider
// 10 is for switch
// 11 is for server
*/

void read_entire_file(char* name,char data [MAX_ARRAY_SIZE]);
bool file_exist(char * fname);
void concat(int argn, char** args,char* c,char* concated);
void create_service_files(int argn, char** args);
int create_directories(char path_name[MAX_STR_SIZE]);
int open_or_create_file(char* name);
bool has_access(char* user,char* file,char* acess);
void initial_ip_fd(ip_fd table [MAX_ARRAY_SIZE]);
void delete_all_ip_fd( ip_fd table [MAX_ARRAY_SIZE], int fd);
void insert_dst_port(dst_port table [MAX_ARRAY_SIZE] ,char* ip,int fd);
void clear_dst_port( dst_port table [MAX_ARRAY_SIZE]);
int search_dst_port( dst_port table [MAX_ARRAY_SIZE], char * ip);
void delete_dst_port( dst_port table [MAX_ARRAY_SIZE],int index);
void insert_ip_fd( ip_fd table [MAX_ARRAY_SIZE] ,char* ip,int fd);
void clear_ip_fd( ip_fd table [MAX_ARRAY_SIZE]);
int search_ip_fd( ip_fd table [MAX_ARRAY_SIZE], char * ip);
void delete_ip_fd( ip_fd table [MAX_ARRAY_SIZE],int index);
void framing(char* type,char* dstAdd,char* srcAdd,char* data,char* sender_port,char* frame);
int change_ip_seed(int c);
int read_ip_seed();
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
