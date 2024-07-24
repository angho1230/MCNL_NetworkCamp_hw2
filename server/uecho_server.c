#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MSIZE 4
#define BUF_SIZE 1024

void error_handling(char *message);

typedef struct {
    char buf[BUF_SIZE];
    int size;
    int seq;
} p_st;

int main(int argc, char *argv[])
{
	int serv_sock;
	char buffer[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;


	
	struct sockaddr_in serv_adr, clnt_adr;
	if (argc != 3) {
		printf("Usage : %s <port> <file name>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (serv_sock == -1)
		error_handling("UDP socket creation error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
    int r;
    clnt_adr_sz = sizeof(clnt_adr);
    recvfrom(serv_sock, &r, sizeof(int), 0, 
                       (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    printf("received %d\n", r);

    FILE * fp;
    fp = fopen(argv[2], "rb");
    int size = 0;
    struct timeval tv_timeo = { 3, 500000 };
    setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &tv_timeo, sizeof(tv_timeo));
    int seq = 0;
	while (1) 
    {
        p_st p;
        clnt_adr_sz = sizeof(clnt_adr);
        str_len = fread(p.buf, 1, BUF_SIZE, fp);
        if(str_len == 0){
            break;
        }
        p.seq = seq;
        p.size = str_len;
        while(1){
            sendto(serv_sock, &p, sizeof(p_st), 0,
                   (struct sockaddr*)&clnt_adr, clnt_adr_sz);
            int ack;
            if(recvfrom(serv_sock, &ack, sizeof(int), 0, 
                        (struct sockaddr*)&clnt_adr, &clnt_adr_sz) != -1 && ack == seq){
                seq++;
                break;
            }
            printf("resending packet(%d)...\n", seq);
        }
    }
    fclose(fp);
    printf("size: %d\n", size);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
