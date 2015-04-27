#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"

int main(int argn, char** args){
	if(argn!=3){
		print("use this format: /Client username password\n");
		return 0;
	}
	int my_ip;
	char resip[MAX_STR_SIZE];
	clear_buff(resip, MAX_STR_SIZE);

	char input_buffer[MAX_STR_SIZE];
	clear_buff(input_buffer, MAX_STR_SIZE);

	while( read(STDINFD, input_buffer, MAX_STR_SIZE) > 0 ){
		int input_tokens_num;
		char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
		tokenizer(input_buffer, " ", &input_tokens_num, input_tokens);
		if(mystrcmp(input_tokens[0], "Login") == 0 && input_tokens_num == 3){
			if(mystrcmp(input_tokens[1],args[1])==0 && mystrcmp(input_tokens[2],args[2])==0){
				print("welcome!\n");
				break;
			}else{
				print("invalid password or username\n");
			}
		}else{
			print("you should Login first: Login asghar pass\n");
		}
	}

	while(read(STDINFD, input_buffer, MAX_STR_SIZE) > 0)
	{
		//parsing input_buffer
		int input_tokens_num;
		char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
		tokenizer(input_buffer, " ", &input_tokens_num, input_tokens);
		
		if(mystrcmp(input_tokens[0], "Connect") == 0 && input_tokens_num == 3)// don't forget to check reconnect!!
		{
			int n, m;//return value of read/write calls
			int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			printf("socket to connect is: %d\n", fd);

			int port_no = atoi(input_tokens[2]);
			struct sockaddr_in serv_addr;
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			serv_addr.sin_port = htons(port_no);

			int status = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			if(status < 0)
			{
				write(STDOUTFD,"Error on connecting\n", sizeof("Error on connecting\n"));
				continue;
				//exit
			}
			else
			{
				write(STDOUTFD, "connecting successful\n", sizeof("connecting successful\n"));

				int bytes_w = write(fd, "get_ip", sizeof("get_ip"));
				int bytes_r = read(fd, resip, MAX_STR_SIZE);
				printf("gotten ip is: %s\n", resip);
				my_ip = atoi(resip);
			}
			//sending identity to server
			char iden_buff[MAX_STR_SIZE];
			clear_buff(iden_buff, MAX_STR_SIZE);
			strcat(iden_buff, "00");//00 is for client

			while(1)
			{
				//read command
				char data[MAX_STR_SIZE];
				clear_buff(data, MAX_STR_SIZE);
				int status = read(STDINFD, data, MAX_STR_SIZE);
				char frame[MAX_STR_SIZE];
				clear_buff(frame, MAX_STR_SIZE);
				int tokens_num;
				char temptkns[MAX_ARRAY_SIZE][MAX_STR_SIZE];
				tokenizer(data, " \n", &tokens_num, temptkns);

				//send command for server
				if( (mystrcmp(temptkns[0], "DC") < 0) && (mystrcmp(temptkns[0], "Logout") < 0) )
				{
					framing(iden_buff,"0",resip,data,"cccc",frame);

					int bytes_written = write(fd, frame, strlength(frame));
					if(bytes_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));

					//get response from server
					char res_buff[MAX_STR_SIZE];
					clear_buff(res_buff, MAX_STR_SIZE);
					int read_status = read(fd, res_buff, MAX_STR_SIZE);

					//show the response to client
					write(STDOUTFD, res_buff, strlength(res_buff));
				}
				else if(mystrcmp(temptkns[0], "DC") == 0)
				{
					int bytes_written = write(fd, "DC", strlength("DC"));
					if(bytes_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
					break;
				}
				else if(mystrcmp(temptkns[0], "Logout") == 0)
				{
					int bytes_written = write(fd,"DC", strlength("DC"));
					if(bytes_written < 0)
						write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
					close(fd);
					return 0;
				}
			}
			close(fd);
		}
		else if(mystrcmp(input_tokens[0], "Logout") == 0)
		{
			return 0;
		}
		else if( (mystrcmp(input_tokens[0], "Connect") < 0) && (mystrcmp(input_tokens[0], "Logout") < 0) )
		{
			write(STDOUTFD, "You Should Login First: Login AAA 111\n", sizeof("You Should Login First: Login AAA 111\n"));
		}
	}
	return 0;
}