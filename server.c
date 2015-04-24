#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include "utilities.h"

int process_command(char command[MAX_STR_SIZE], char res[MAX_STR_SIZE], char* dirnm)
{
	return 1;
}

int main(int argn, char** args)
{
	/*
	change_ip_seed(1234);
	int rps = read_ip_seed();
	*/

	change_ip_seed(0);
	int port_number = atoi(args[1]);//to be server
	int port_no;//to be client
	const int num_of_connection = 5;
	char *directory_name = "DB";
	// make directories
	int mkdir_status = create_directories(directory_name);
	if(mkdir_status != 0){

		write(STDOUTFD, "Error while creating directory\n", sizeof("Error while creating directory\n"));
		return 0;
	}

	write(STDOUTFD, "Directory created\n", sizeof("Directory created\n"));
	//creating socket
	int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_number);

	struct sockaddr_in client_addr;
	//binding 
	int binding_st = bind(server_fd,(struct sockaddr*) &server_addr, sizeof(server_addr));
	if(binding_st == -1)
	{
		write(STDOUTFD, "binding error\n", sizeof("binding error\n"));
		return 0;
	}
	else
		write(STDOUTFD, "binding successful\n", sizeof("binding successful\n"));
	//listenning
	int listening_st = listen(server_fd, num_of_connection);
	if(listening_st == -1)
	{
		write(STDOUTFD, "listening error\n", sizeof("listening error"));
		return 0;
	}
	else
		write(STDOUTFD, "listening successful\n", sizeof("listening successful"));

	fd_set read_fdset, temp_fdset;
	struct timeval tv;
	int ret_val;
	int new_sock_fd, it_fd;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&read_fdset);
	FD_SET(server_fd, &read_fdset);
	FD_SET(STDINFD, &read_fdset);

	/* Wait up to five seconds. */
	tv.tv_sec = 10 * 60;
	tv.tv_usec = 0;

	unsigned int size_of_client_addr = sizeof(client_addr);

	int status;

	while(1)
	{
		memcpy(&temp_fdset, &read_fdset, sizeof(temp_fdset));
		status = select(FD_SETSIZE, &temp_fdset, (fd_set *)0, (fd_set *)0, (struct timeval*) &tv);
		if(status < 1)
		{
			write(STDOUTFD, "Select Error\n", sizeof("Select Error\n"));
			break;
		}

		for(it_fd = 0; it_fd<FD_SETSIZE; it_fd++)
		{
			if(FD_ISSET(it_fd, &temp_fdset))
			{
				if(it_fd == STDINFD)
				{
					char buff_read [MAX_STR_SIZE];
					clear_buff(buff_read, MAX_STR_SIZE);

					read(it_fd, buff_read, MAX_STR_SIZE-1);

					int input_tokens_num;
					char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
					tokenizer(buff_read, " ", &input_tokens_num, input_tokens);

					struct sockaddr_in switch_addr;
					switch_addr.sin_family = AF_INET;
					switch_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
					port_no = atoi(input_tokens[2]);
					switch_addr.sin_port = htons(port_no);
					int status1 = connect(client_fd, (struct sockaddr *)&switch_addr, sizeof(switch_addr));
					if(status1 < 0)
					{
						write(STDOUTFD,"Error on connecting\n", sizeof("Error on connecting\n"));
						continue;
						//exit
					}
					else
					{
						write(STDOUTFD, "connecting successful\n", sizeof("connecting successful\n"));
					}
					//do something

					//sending identity to server
					char iden_buff[MAX_STR_SIZE];
					clear_buff(iden_buff, MAX_STR_SIZE);
					strcat(iden_buff, "switch ");
					strcat(iden_buff, " \0");
					char request[MAX_STR_SIZE];
					strcpy(request, iden_buff);
					strcat(request, buff_read);

					//send command for server
					int bytes_written = write(client_fd, request, strlength(request));
					if(bytes_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));

					//get response from server
					char res_buff[MAX_STR_SIZE];
					clear_buff(res_buff, MAX_STR_SIZE);
					int read_status = read(client_fd, res_buff, MAX_STR_SIZE);

					//show the response to client
					write(STDOUTFD, res_buff, strlength(res_buff));

					//save result

					

					//Disconnect from parent switch
					int byts_written = write(client_fd, "DC", strlength("DC"));
					if(byts_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
					continue;
				}
				if(it_fd == server_fd)
				{	
					new_sock_fd = accept(server_fd, NULL, NULL);
					if(new_sock_fd < 0)
					{
						write(STDOUTFD,"Error on accepting\n", sizeof("Error on accepting\n"));
						break;
					}
					else write(STDOUTFD,"accepting successful\n", sizeof("accepting successful\n"));
					FD_SET(new_sock_fd, &read_fdset);

					printf("accepted fd is: %d\n", new_sock_fd);

				}
				else
				{
					int n, m;
					char buff_read [MAX_STR_SIZE], response_buff[MAX_STR_SIZE];
					clear_buff(buff_read, MAX_STR_SIZE);
					clear_buff(response_buff, MAX_STR_SIZE);
					strcpy(response_buff, "OKKe");
					n = read(it_fd, buff_read, MAX_STR_SIZE-1);

					print(buff_read);

					if(n == 0)
					{
						close(it_fd);
						FD_CLR(it_fd, &read_fdset);
						write(STDOUTFD, "Removing One Client_fd\n", sizeof("Removing One Client_fd\n"));
					}
					else if(n < 0)
					{
						write(STDOUTFD, "Error on reading\n", sizeof("Error on reading\n"));
					}

					//after reading successfully
					else
					{
						if(mystrcmp(buff_read, "DC") < 0)
						{
							if(process_command(buff_read, response_buff, directory_name) < 0)
							{
								int st = write(it_fd, "Invalid command\n", sizeof("Invalid command\n"));
								if(st < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
							}

							print(response_buff);

							int s = write(it_fd, response_buff, strlength(response_buff));
							if(s < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
						}
						else if(mystrcmp(buff_read, "DC") == 0)
						{
							write(it_fd, "Disconnecting in Progress ...\n",
									sizeof("Disconnecting in Progress ...\n"));
							close(it_fd);
							FD_CLR(it_fd, &read_fdset);
							write(STDOUTFD, "Removing One Client_fd\n", sizeof("Removing One Client_fd\n"));
						}
					}
				}
			}
		}
	}
	close(server_fd);
	return 0;
}