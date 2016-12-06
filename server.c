#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

pthread_t tid;      

#define FAILURE -1
#define BUFF_LEN (64 * 1024)
#define QLEN 100
#define PORTA 6000

void registraServidor();
char* recebeDados(int conn);
int enviaDados(int conn, char dados[1024]);
void *atende(void *param);


int sock_fd;
struct sockaddr_in clt_addr;

int iniciaServidor(){
	registraServidor();
	
	struct sockaddr_in meu_addr;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_fd == FAILURE){
		perror("socket()");
		return FAILURE;
	}

	meu_addr.sin_addr.s_addr = INADDR_ANY;

	meu_addr.sin_port = htons(PORTA);
	meu_addr.sin_family = AF_INET;

	if (bind(sock_fd, (struct sockaddr *)&meu_addr, 
		sizeof(struct sockaddr_in)) == FAILURE){
		perror("bind()");
		return FAILURE;
	}

	if (listen(sock_fd, QLEN) == FAILURE){
		perror("listen()");
		return FAILURE;
	}
}

int recebeConexoes(){
	int clt_addr_len, conn_fd;
	
	clt_addr_len = sizeof(struct sockaddr_in);
	while(1){
		printf("\nEsperando...\n");
		conn_fd = accept(sock_fd, (struct sockaddr *)&clt_addr, (socklen_t *)&clt_addr_len);
		printf("\nRecebeu o cliente...\n");
		if (conn_fd == FAILURE) {
			perror("accept()");
			return FAILURE;
		}
		pthread_create(&tid,NULL,atende,(void*)&conn_fd);
	}
}

int main(int argc, char **argv){
	
	iniciaServidor();
	recebeConexoes();


	return 0;
}

char* recebeDados(int conn) {
	char* mensagem;
	int r = 0;
	int buff_size = 2097152;
	mensagem = malloc(1024*sizeof(char));
	
	r = recv(conn, mensagem, 1024, 0);

	if (r == FAILURE) {
		perror("recv()");
		return "ERROR";
	}

	return mensagem;
}

int enviaDados(int conn, char *resposta) {
	int s = 0;
	int DATA_SIZE = (strlen(resposta)+1);
	int buff_size = 2097152;
	
	while (s < DATA_SIZE) {
		if((buff_size+s) >= DATA_SIZE){
		    if((s += send(conn,(void *)resposta, DATA_SIZE, (DATA_SIZE-s))) < 0) {
		        perror("write");
		        continue;
		    }
		}
		else if((s += send(conn,(void *)resposta, DATA_SIZE, buff_size)) < 0) {
		    perror("write");
		    continue;
		}
	}
	
	//s = send(conn, (void *)resposta, (strlen(resposta)*sizeof(char)), 0);
	
	if (s == FAILURE){
		perror("send()");
		return -1;
	}

	return 0;
}

void registraServidor(){
	int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    unsigned short broadcastPort;     /* Server port */
    char *sendString;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */

    broadcastIP = "192.168.0.0";            /* First arg:  broadcast IP address */ 
    broadcastPort = 3000;    /* Second arg:  broadcast port */
    sendString = "@servidor_rpc";             /* Third arg:  string to broadcast */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        perror("socket() failed");
        exit(-1);
    }

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0){
        perror("setsockopt() failed");
        exit(-1);
    }

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
    broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */

    sendStringLen = strlen(sendString);  /* Find length of sendString */

	/* Broadcast sendString in datagram to clients every 3 seconds*/
	if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != sendStringLen){
		perror("sendto() sent a different number of bytes than expected");
		exit(-1);
	}
}


