#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include "utilities.h"

int connected_to_server;
char* connected_server_port;

int exist_in_table(struct dst_port table[MAX_ARRAY_SIZE], int table_i, char* sip)//tested
{
	int i;
	for (i=0; i<table_i; i++)
	{
		int c = mystrcmp(table[i].dst, sip);
		if(c>=0)
			return 1;
	}
	return -1;
}

int forward_and_receive(char command[MAX_STR_SIZE], char res[MAX_STR_SIZE], struct dst_port table[MAX_ARRAY_SIZE], int table_i, int parents[MAX_ARRAY_SIZE])
{
	int tokens_num;
	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	tokenizer(command, "&", &tokens_num, input_tokens);
	char* f_type = input_tokens[0];
	char* f_dst_addr = input_tokens[1];
	char* f_src_addr = input_tokens[2];
	char* f_data = input_tokens[5];
	char* f_sndr_port = input_tokens[7];

	if(mystrcmp(f_type, "00")==0)//from client, so should send packet to server, through its parents
	{
		printf("\n**fahmide baste az cliente\n");

		if(connected_to_server > 0)//this server is connected to server
		{
			//send packet directly to server
			int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			struct sockaddr_in switch_addr;
			switch_addr.sin_family = AF_INET;
			switch_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			int port_no = atoi(connected_server_port);
			switch_addr.sin_port = htons(port_no);
			int status1 = connect(client_fd, (struct sockaddr *)&switch_addr, sizeof(switch_addr));
			if(status1 < 0)
			{
				write(STDOUTFD,"Error on connecting to server\n", sizeof("Error on connecting to server\n"));
				return -1;
			}
			else
			{
				write(STDOUTFD, "connecting successful\n", sizeof("connecting successful\n"));
			}

			//send command for server
			int bytes_written = write(client_fd, command, strlength(command));
			if(bytes_written < 0)
				write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));

			//get response from server
			int read_status = read(client_fd, res, MAX_STR_SIZE);
			//show the response to client
			write(STDOUTFD, res, strlength(res));

			//Disconnect from server
			int byts_written = write(client_fd, "DC", strlength("DC"));
			if(byts_written < 0)
				write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
			close(client_fd);
		}
		else
		{
			//send packet to another parent switch
		}
	}
	else if(mystrcmp(f_type, "10")==0)//from another switch, it's a connection message, connection should be confirmed
	{
		strcpy(res, "connection is confirmed");
		return 1;
	}
	else if(mystrcmp(f_type, "11"))//from server, it's a response packet
	{
		//should 
	}
	else//packet might be corrupted
	{
		strcpy(res, "packet corrupted or encrypted");
		return -1;
	}
}

int process_command(char command[MAX_STR_SIZE], char res[MAX_STR_SIZE], char* dirnm, struct dst_port table[MAX_ARRAY_SIZE], int* table_i, int parents[MAX_ARRAY_SIZE])
{
	int tokens_num;
	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	tokenizer(command, "&", &tokens_num, input_tokens);
	char* f_type = input_tokens[0];
	char* f_dst_addr = input_tokens[1];
	char* f_src_addr = input_tokens[2];
	char* f_data = input_tokens[5];
	char* f_sndr_port = input_tokens[7];

	//beased on sndr_port and src_addr, table will be updated
	//check that the src_ip is in table or not
	//if not add src_ip and corr_port to table
	if(exist_in_table(table, *table_i, f_dst_addr)<0 && mystrcmp(f_src_addr, "0000000000009999")<0 && mystrcmp(f_dst_addr, "0000000000009999")<0)
	{
		strcpy(table[*table_i].dst, f_dst_addr);
		*table_i++;
	}
	
	//the received_frame should be processed through parents and redirect the answer
	forward_and_receive(command, res, table, *table_i, parents);

	return 1;
}


void test()
{
	int rps = read_ip_seed();
	printf("old ip-seed is: %d\n", rps);
	change_ip_seed(++rps);
	printf("new ip-seed is: %d\n", rps);
	change_ip_seed(++rps);
	printf("new new ip-seed is: %d\n", rps);
}

int main(int argn, char** args)
{
	test(); return 0;

	if(argn!=2){
		print("use this format: /Switch port\n");
		return 0;
	}

	connected_to_server = -1;
	connected_server_port = "";

	int port_number = atoi(args[1]);//to be server
	int port_no;//to be client

	int parents[MAX_ARRAY_SIZE];
	clear_buff_int(parents, MAX_ARRAY_SIZE);
	int parent_i = 0;

	struct dst_port table[MAX_ARRAY_SIZE];
	int table_i = 0;

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
				if(it_fd == STDINFD)//command from terminal
				{
					char data [MAX_STR_SIZE];
					clear_buff(data, MAX_STR_SIZE);

					read(it_fd, data, MAX_STR_SIZE-1);

					int input_tokens_num;
					char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
					tokenizer(data, " ", &input_tokens_num, input_tokens);//TODO: check that command is connect

					struct sockaddr_in switch_addr;
					switch_addr.sin_family = AF_INET;
					switch_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
					port_no = atoi(input_tokens[2]);
					parents[parent_i] = port_no;
					parent_i++;
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

					//sending identity to switch
					char iden_buff[MAX_STR_SIZE];
					clear_buff(iden_buff, MAX_STR_SIZE);
					strcat(iden_buff, "10");//10 is for switch
					char frame[MAX_STR_SIZE];
					clear_buff(frame, MAX_STR_SIZE);
					//framing(char* type,char* dstAdd,char* srcAdd,char* data,char* sender_port,char* frame);
					framing(iden_buff, "9999", "9999", data, args[1], frame);

					//send command for server
					int bytes_written = write(client_fd, frame, strlength(frame));
					if(bytes_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));

					//get response from server
					char res_buff[MAX_STR_SIZE];
					clear_buff(res_buff, MAX_STR_SIZE);
					int read_status = read(client_fd, res_buff, MAX_STR_SIZE);

					//show the response to client
					write(STDOUTFD, res_buff, strlength(res_buff));

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
					//strcpy(response_buff, "SWitch mige OKKe");

					n = read(it_fd, buff_read, MAX_STR_SIZE-1);

					printf("\ndastoore khande shode az tarafe client:\n");
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
						if(mystrcmp(buff_read, "DC") < 0 && mystrcmp(buff_read, "get_ip") < 0 && (mystrcmp_to(buff_read, "11", 2)<0))
						{
							if(process_command(buff_read, response_buff, directory_name, table, &table_i, parents) < 0)
							{
								int st = write(it_fd, "Invalid command\n", sizeof("Invalid command\n"));
								if(st < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
							}

							printf("\nthe response that switch got\n");
							print(response_buff);

							int s = write(it_fd, response_buff, strlength(response_buff));
							if(s < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
						}
						else if(mystrcmp(buff_read, "get_ip") == 0)
						{
							int rps = read_ip_seed();
							change_ip_seed(++rps);
							printf("new ip-seed is: %d\n", rps);
							char res[20];
							clear_buff(res, 20);
							int_to_str(rps, res, 10);
							
							write(it_fd, res,strlen(res));
						}
						else if(mystrcmp_to(buff_read, "11", 2)==0)
						{
							int tokens_num;
							char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
							tokenizer(buff_read, "&", &tokens_num, input_tokens);
							char res[20];
							clear_buff(res, 20);
							strcpy(res, "connection to switch is confirmed");
							connected_to_server = 1;
							connected_server_port = input_tokens[1];

							printf("connected_to_server %d\n", connected_to_server);
							printf("connected_server_port %s\n", connected_server_port);

							write(it_fd, res,strlen(res));
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