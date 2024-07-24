#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define MSIZE 4

void error_handling(char *message);

typedef struct {
    char buf[BUF_SIZE];
    int size;
    int seq;
} p_st;

int main(int argc, char *argv[])
{
	int sock;
	char buffer[BUF_SIZE];
	int str_len;
	socklen_t adr_sz;
	
	struct sockaddr_in serv_adr, from_adr;
	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_DGRAM, 0);   
	if (sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));
    printf("sending %s/n", argv[3]);
    FILE *fp = fopen("received.data", "wb");
    int total = 0;
    int size = 0;
    sendto(sock, &size, sizeof(int), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	int seq = 0;
    p_st p;
    while (1)
	{   
        adr_sz = sizeof(from_adr);
        size = recvfrom(sock, &p, sizeof(p_st), 0, 
                 (struct sockaddr*)&serv_adr, &adr_sz);
        if(seq == p.seq){
            total += p.size;
            fwrite(p.buf, 1, p.size, fp);
            sendto(sock, &seq, sizeof(int), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
            seq++;
            if(p.size != BUF_SIZE) break;
        }
        else{
            sendto(sock, &size, sizeof(int), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        }
	}
    fclose(fp);
    printf("closing..(%d)\n", total);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
