#include "utilities.h"

char* crc(char* t){
	return "$$$$$$";
}

void extend_to(char* c, int to,char* ress)
{
	int size = strlen(c);
	int ex = to - size;
	
	int i;
	for (i=0; i<ex; i++)
	{
		strcat(ress, "0");
	}
	strcat(ress, c);
}

void framing(char* type,char* dstAdd,char* srcAdd,char* data,char* sender_port,char* frame){
	strcat(frame, type);
	strcat(frame, "&");
	//dst
	char ress[MAX_STR_SIZE];
	clear_buff(ress, MAX_STR_SIZE);
	extend_to(dstAdd, 16,ress);
	strcat(frame, ress);
	strcat(frame,"&");
	//src
	clear_buff(ress, MAX_STR_SIZE);
	extend_to(srcAdd, 16,ress);
	strcat(frame, ress);
	strcat(frame, "&$$$$&1024&");
	//data
	strcat(frame, data);
	strcat(frame, "&$$$$$$&");
	//port
	strcat(frame, sender_port);
}

int change_ip_seed(int c)
{
	int file_fd = open("./DB/ip_seed.txt",O_TRUNC | O_RDWR);
	//chmod(path_name, S_IRUSR | S_IWUSR );
	if(file_fd > 0)
		chmod("./DB/ip_seed.txt", S_IRUSR | S_IWUSR );
	else
		write(STDOUTFD, "Error In Opening File to Write!\n", sizeof("Error In Opening File to Write!\n"));
	char res[20];
	clear_buff(res, 20);
	int_to_str(c, res, 10);
	int w_st = write(file_fd, res, strlen(res));
	close(file_fd);
	return w_st;
}

int read_ip_seed()
{
	int file_fd = open("./DB/ip_seed.txt",O_RDWR);
	if(file_fd > 0)
		chmod("./DB/ip_seed.txt", S_IRUSR | S_IWUSR );
	else
		write(STDOUTFD, "Error In Opening File to Write!\n", sizeof("Error In Opening File to Write!\n"));
	char res[20];
	
	int r_st = read(file_fd, res, strlen(res));
	int v = atoi(res);
	close(file_fd);
	return v;
}

int create_directories(char path_name[MAX_STR_SIZE])
{
	int mkdir_status = mkdir(path_name, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
	return 0;
}

int strlength(char str[MAX_STR_SIZE])
{
	int i;
	for(i = 0; i < GETMAX(MAX_STR_SIZE, sizeof(str)); i++)
	{
		if(str[i] == '\0') break;
	}
	return i;
}
void int_to_str(int val, char res[20], int base)
{
	clear_buff(res, 20);
	char tmp[20];
	clear_buff(tmp, 20);
	if(val == 0)
	{
		strcpy(res, "0");
		return;
	}
	int index = 0;
	while (val > 0)
	{
		char digit = '0';
		digit = (char) ((val % base) + (int) digit);
		tmp[index] = digit;
		val /= base;
		index ++;
	}
	strcat(tmp, "\0");
	int i;
	int size = strlength(tmp);

	for(i = 0; i<size; i++)
	{
		res[i] = tmp[size-1-i];
	}
	strcat(res, "\0");
}
void print(char str[MAX_STR_SIZE])
{
	write(STDOUTFD, str, strlength(str));
}
void tokenizer(char str[MAX_STR_SIZE], char delim[MAX_STR_SIZE], int* num_of_tokens, char res[MAX_STR_SIZE][MAX_STR_SIZE])
{
	char tmp[MAX_STR_SIZE];
	int i, tmp_index = 0, res_index = 0;
	for(i = 0; i <= strlength(str); i++)
	{
		int cut = 0;
		int j;
		for(j = 0; j < strlength(delim); j++)
		{
			if (str[i] == delim[j] || str[i] == '\0' )
			{
				cut = 1;
				break;
			}
		}
		if(cut == 0)
		{
			tmp[tmp_index] = str[i];
			tmp_index ++;
		}
		else
		{
			int k;
			for(k = 0; k < tmp_index; k++)
				res[res_index][k] = tmp[k];
			res[res_index][k] = '\0';
			res_index ++;
			tmp_index = 0;
		}
	}
	*num_of_tokens = res_index;
}
int clear_buff(char* c, int size)
{
	if (size == 0) size = MAX_STR_SIZE;
	int i;
	for(i = 0; i < size; i++)
	{
		c[i] = 0;
	}
}
int clear_buff_int(int* c, int size)
{
	if (size == 0) size = MAX_STR_SIZE;
	int i;
	for(i = 0; i < size; i++)
	{
		c[i] = 0;
	}
}
int mystrcmp(char const*p, char const *q)
{
	int i = 0;
	for(i=0; q[i]!='\0'; i++)
	{
		if(p[i] != q[i])
			return -1;
	}
	return 0;
}

int mystrcmp_to(char const*p, char const *q, int to)
{
	int i = 0;
	for(i=0; i<to; i++)
	{
		if(p[i] != q[i])
			return -1;
	}
	return 0;
}

int days_in_month(int month, int year)
{
	if(month == 2)
		return 28 + (year % 4 == 0);
	if(month  == 4 || month == 6 || month == 9 || month == 11)
		return 30;
	return 31;
}
void get_date(char time_str [30])
{
	clear_buff(time_str, 30);

	time_t my_time = time(NULL);
	my_time += 3 * 3600 + 30 * 60;
	int second = my_time % 60;
	int minute = (my_time % 3600) / 60;
	int hour = (my_time % (3600 * 24)) / 3600;
	int days = my_time / (3600 * 24);
	int month = 1, year = 1970;
	while (days > days_in_month(month, year))
	{
		days -= days_in_month(month, year);
		month ++;
		if(month == 13)
		{
			year ++;
			month = 1;
		}
	}

	char tmp[20];
	char str[20];
	clear_buff(tmp, 20);
	clear_buff(str, 20);
	int_to_str(month, tmp, 10);
	if(month < 10)
	{
		strcpy(str, "0");
		strcat(str, tmp);
		clear_buff(tmp, 20);
		strcpy(tmp, str);
	}
	strncpy(time_str, tmp, strlength(tmp));
	strcat(time_str, "/");//month

	clear_buff(tmp, 20);
	clear_buff(str, 20);
	int_to_str(days, tmp, 10);
	if(days < 10)
	{
		strcpy(str, "0");
		strcat(str, tmp);
		clear_buff(tmp, 20);
		strcpy(tmp, str);
	}
	strncat(time_str, tmp, strlength(tmp));
	strcat(time_str, "/");//day

	clear_buff(tmp, 20);
	int_to_str(year, tmp, 10);
	strncat(time_str, tmp, strlength(tmp));
	strcat(time_str, ",");//year

	clear_buff(tmp, 20);
	clear_buff(str, 20);
	int_to_str(hour, tmp, 10);
	if(hour < 10)
	{
		strcpy(str, "0");
		strcat(str, tmp);
		clear_buff(tmp, 20);
		strcpy(tmp, str);
	}
	strncat(time_str, tmp, strlength(tmp));
	strcat(time_str, ":");//hour

	clear_buff(tmp, 20);
	clear_buff(str, 20);
	int_to_str(minute, tmp, 10);
	if(minute < 10)
	{
		strcpy(str, "0");
		strcat(str, tmp);
		clear_buff(tmp, 20);
		strcpy(tmp, str);
	}
	strncat(time_str, tmp, strlength(tmp));
	strcat(time_str, ":");//minute

	clear_buff(tmp, 20);
	clear_buff(str, 20);
	int_to_str(second, tmp, 10);
	if(second < 10)
	{
		strcpy(str, "0");
		strcat(str, tmp);
		clear_buff(tmp, 20);
		strcpy(tmp, str);
	}
	strncat(time_str, tmp, strlength(tmp));//second

	strcat(time_str, "\0");//null at the end of string
}
void list_files(char pathname[MAX_STR_SIZE], char file_names[MAX_ARRAY_SIZE][MAX_STR_SIZE], int* file_num)
{
	int fd, nread;
	char buf[MAX_STR_SIZE];
	struct linux_dirent *d;
	int bpos;
	char d_type;
	fd = open(pathname, O_RDONLY | O_DIRECTORY);
	print("\nat 1\n");
	int file_num_int = 0;
	while(1)
	{
		nread = syscall(SYS_getdents, fd, buf, MAX_STR_SIZE);
		if (nread == 0){
			break;
		}
		for (bpos = 0; bpos < nread;) 
		{
			d = (struct linux_dirent *) (buf + bpos);
			print(d->d_name);
			print("\n");
			clear_buff(file_names[file_num_int], MAX_STR_SIZE);
			strcpy(file_names[file_num_int], d->d_name);
			bpos += d->d_reclen;
			(file_num_int) ++;
		}
		break;

	}
	*file_num = file_num_int;
}
