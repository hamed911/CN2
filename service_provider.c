#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"

void test(char * name){
	if( file_exist(name) )
		printf("file exist\n");;
	char data[MAX_STR_SIZE];
	clear_buff(data,MAX_STR_SIZE);
	read_entire_file(name,data);
	printf("data is:\n%s",data );

}

int main(int argn, char** args){
	// test("./DB/Services/jafar.txt"); return;
	if(argn<2){
		printf("You didn't mentioned services\nThe correct pattern is:\t ./Service_provider.out user.txt xxxx.txt \t....\n");
		return;
	}
	create_service_files(argn,args);
	char input_buffer[MAX_STR_SIZE];
	clear_buff(input_buffer, MAX_STR_SIZE);

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
			int port_no = atoi(input_tokens[2]);
			printf("%d\n",port_no );
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
				write(STDOUTFD, "connecting successful\n", sizeof("connecting successful\n"));
			//sending identity to server
			char frame[MAX_STR_SIZE];
			clear_buff(frame, MAX_STR_SIZE);
			char data[MAX_STR_SIZE];
			clear_buff(data, MAX_STR_SIZE);
			char space[10]=" ";
			concat(argn,args,space,data);
			framing("01","0","ssssssssssssssss",data,"ssss",frame);
			strcat(frame, "\0");
			int bytes_written = write(fd, frame, strlength(frame));
			if(bytes_written < 0)
				write(STDOUTFD,"unable to write in server\n", sizeof("unable to write in server\n"));

			//get response from server
			char res_buff[MAX_STR_SIZE];
			clear_buff(res_buff, MAX_STR_SIZE);
			int read_status = read(fd, res_buff, MAX_STR_SIZE);

			//show the response to client
			write(STDOUTFD, res_buff, strlength(res_buff));
			
			char iden_buff[MAX_STR_SIZE];
			clear_buff(iden_buff, MAX_STR_SIZE);
			strcat(iden_buff, "01");//01 is for provider

			while(1)
			{
				char in_buff[MAX_STR_SIZE];
				clear_buff(in_buff, MAX_STR_SIZE);
				int status = read(fd, in_buff, MAX_STR_SIZE);
				status = write (fd,"provider recieved command\n",sizeof("provider recieved command\n"));
				write(STDOUTFD,in_buff, strlength(res_buff));				
			}
			close(fd);
		}
		else if(mystrcmp(input_tokens[0], "Exit") == 0)
		{
			return 0;
		}
	}
	return 0;
}