#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include "utilities.h"

void process_recieved_command(char buff_read[MAX_STR_SIZE],ip_fd* service_fd_table,int fd,char* final_response,char* port)
{
	char response [MAX_STR_SIZE];
	clear_buff(response,MAX_STR_SIZE);
	int input_tokens_num;
	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	replace_char(buff_read,'\n','\0' );
	tokenizer(buff_read, "&", &input_tokens_num, input_tokens);
	int data_num;
	char data[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	replace_char(buff_read,'\n','\0' );
	printf("data is:%s\n",input_tokens[5] );
	tokenizer(input_tokens[5], " ", &data_num,data );
	if(strcmp( input_tokens[0],"00")==0){//from client
		printf("client said!\n" );
		if( strcmp(data[0],"Get") ==0 && strcmp(data[1],"List") ==0 && data_num==4){
			int i;
			for(i=0; i<MAX_ARRAY_SIZE; i++)
				if(service_fd_table[i].fd!=-1){
					strcat(response,service_fd_table[i].ip);
					strcat(response," ");
				}		
		}else if( strcmp(data[0],"Request")==0 && data_num==4){
			if( has_access(data[3],data[1],data[2]) ){
				strcat(response,"You have Access to file\tfile is:\n");
				char file_name [MAX_STR_SIZE];
				clear_buff(file_name,MAX_STR_SIZE);
				strcat(file_name,"./DB/Services/");
				strcat(file_name,data[1]);
				read_entire_file(file_name,response);
				printf("in request\n");
			}
			else{
				strcat(response,"Access denied! You doesn't have permission!\n");
			}
		}else if(strcmp(data[0],"Send")==0 && data_num==4){
			if( !has_access(data[2],data[1],"Write") )
				strcat(response,"Access denied! You doesn't have permission!\n");
			else
			{
				replace_char(data[3],'^','\n');
				char fname[MAX_STR_SIZE];
				clear_buff(fname,MAX_STR_SIZE);
				strcat(fname,"./DB/Services/");
				strcat(fname,data[1]);
				int fd = open_or_create_file(fname);
				int status= write(fd,data[3],strlength(data[3]));
				if(status==-1)
					strcat(response,"error in writing in file\n");
				else
					strcat(response,"file wrote successfully!\n");
				close(fd);

			}

		}else if(strcmp(data[0],"Append")==0 && data_num>=4){
			if( !has_access(data[data_num-1],data[1],"Append") )
				strcat(response,"Access denied! You doesn't have permission!\n");
			else
			{
				char file_name [MAX_STR_SIZE];
				clear_buff(file_name,MAX_STR_SIZE);
				strcat(file_name,"./DB/Services/");
				strcat(file_name,data[1]);
				FILE* f_pointer = fopen(file_name, "a");
				int i;
				char file_data[MAX_STR_SIZE];
				clear_buff(file_data,MAX_STR_SIZE);
				for (i=2; i<data_num-1; i++){
					strcat(file_data,data[i]);
					strcat(file_data," ");
				}
				fputs( file_data,f_pointer);
				fclose(f_pointer);
				strcat(response,"data in ");
				strcat(response,data[1]);
				strcat(response," file Appended\n");
			}
		}else{
			strcat(response,"Wrong command\n");
		}
		
	}else if(strcmp( input_tokens[0],"01")==0){//provider
		printf("provider said!\n" );
		
		int i;
		for(i=0; i<data_num; i++){
			insert_ip_fd( service_fd_table ,data[i], fd);
		}
		show_table_ip_fd(service_fd_table,4);
	}
	framing("11",input_tokens[2],"0",response,port,final_response);
}

void test(ip_fd service_fd_table[MAX_ARRAY_SIZE]){
	char response[MAX_STR_SIZE];
	process_recieved_command("00&0000000000000000&0000000000000001&$$$$&1024&Append kaftar.txt jafariiiiiiiiiiiiiiiiiiiiiiiiiii hamed&$$$$$$&cccc",
			service_fd_table,2,response,"1000");
	printf("response is\n%s\n",response );
	

}

int main(int argn, char** args)
{
	ip_fd ip_fd_table[MAX_ARRAY_SIZE],service_fd_table[MAX_ARRAY_SIZE];
	initial_ip_fd(ip_fd_table);
	initial_ip_fd(service_fd_table);
	// test(service_fd_table);return;
	if(argn!=2){
		print("use this format: /Server port\n");
		return 0;
	}
	// ip_fd ip_fd_table[MAX_ARRAY_SIZE],service_fd_table[MAX_ARRAY_SIZE];
	// initial_ip_fd(ip_fd_table);
	// initial_ip_fd(service_fd_table);
	change_ip_seed(0);
	int port_number = atoi(args[1]);//to be server
	int port_no;//to be client
	const int num_of_connection = 5;
	char *directory_name = "DB";
	// make directories
	int mkdir_status = create_directories(directory_name);
	if(mkdir_status != 0)
		write(STDOUTFD, "directory exist or problem in creating directory\n", sizeof("directory exist or problem in creating directory\n"));
	else
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

					//sending identity to sswitch
					char iden_buff[MAX_STR_SIZE];
					clear_buff(iden_buff, MAX_STR_SIZE);
					strcat(iden_buff, "11&");
					strcat(iden_buff, args[1]);

					//send command for server
					int bytes_written = write(client_fd, iden_buff, strlength(iden_buff));
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

					printf("Accepted fd is: %d\n", new_sock_fd);

				}
				else
				{
					int n, m;
					char buff_read [MAX_STR_SIZE], response_buff[MAX_STR_SIZE];
					clear_buff(buff_read, MAX_STR_SIZE);
					clear_buff(response_buff, MAX_STR_SIZE);
					strcpy(response_buff, "Server mige OKKe\n");

					n = read(it_fd, buff_read, MAX_STR_SIZE-1);
					if(n == 0)
					{
						close(it_fd);
						FD_CLR(it_fd, &read_fdset);
						write(STDOUTFD, "Removing One Client_fd\n", sizeof("Removing One Client_fd\n"));
						delete_all_ip_fd( service_fd_table, it_fd);
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
							printf("recieved command is:\n%s\n",buff_read);
							process_recieved_command(buff_read,service_fd_table,it_fd,response_buff,args[1]);

							// if(process_command(buff_read, response_buff, directory_name) < 0)
							// {
							// 	int st = write(it_fd, "Invalid command\n", sizeof("Invalid command\n"));
							// 	if(st < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
							// }

							print(response_buff);

							int s = write(it_fd, response_buff, strlength(response_buff));
							if(s < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
						}
						else if(mystrcmp(buff_read, "DC") == 0)
						{
							write(it_fd, "Disconnecting in Progress ...\n",sizeof("Disconnecting in Progress ...\n"));
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