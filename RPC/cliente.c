#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N (64 * 1024)
#define PORTA 6000

int sock_fd;

char* procuraServidor(){
	//envia broadcast para achar servidor
	int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    unsigned short broadcastPort;     /* Server port */
    char *sendString;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */

    broadcastIP = "192.168.0.0";            /* First arg:  broadcast IP address */ 
    broadcastPort = 3000;    /* Second arg:  broadcast port */
    sendString = "$servidor_rpc";             /* Third arg:  string to broadcast */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        perror("socket() failed");
        return NULL;
    }

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0){
        perror("setsockopt() failed");
        return NULL;
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
		return NULL;
	}
	
	
	//*******************************************************
	//recebe a resposta com o ip do servidor
	
	int sock_r;                         /* Socket */
    struct sockaddr_in broadcastAddr_r; /* Broadcast Address */
    unsigned short broadcastPort_r;     /* Port */
    char recvString[255]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */
	int i;
	

    broadcastPort_r = 3001;   /* First arg: broadcast port */

    /* Create a best-effort datagram socket using UDP */
    if ((sock_r = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        perror("socket() failed");
        return NULL;
    }
	
    /* Construct bind structure */
    memset(&broadcastAddr_r, 0, sizeof(broadcastAddr_r));   /* Zero out structure */
    broadcastAddr_r.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr_r.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr_r.sin_port = htons(broadcastPort_r);      /* Broadcast port */
    /* Bind to the broadcast port */
    if (bind(sock_r, (struct sockaddr *) &broadcastAddr_r, sizeof(broadcastAddr_r)) < 0){
        perror("bind() failed");
        return NULL;
    }
    
    if ((recvStringLen = recvfrom(sock_r, recvString, 255, 0, NULL, 0)) < 0){
		perror("recvfrom() failed");
		return NULL;
	}
	
	recvString[recvStringLen] = '\0';
	
	close(sock_r);
	
	return recvString;
}

int estabeleceConexao(){
	char *end_ip;
	
	end_ip = procuraServidor();
	
	if(end_ip == NULL){
		printf("\nServidor nao encontrado...\n");
		exit(-1);
	}
	
	struct sockaddr_in srv_addr;
	
	inet_pton(AF_INET, end_ip, &srv_addr.sin_addr); 

	srv_addr.sin_port = htons(PORTA); 
	srv_addr.sin_family = AF_INET; 

	sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(sock_fd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect()");
		return -1;
	}

}

int enviaDados(char *mensagem){
	int r = 0;
	int DATA_SIZE = (strlen(mensagem)+1);
	int buff_size = 2097152;

	while (r < DATA_SIZE) {
		if((buff_size+r) >= DATA_SIZE){
		    if((r += send(sock_fd,(void *)mensagem, DATA_SIZE, (DATA_SIZE-r))) < 0) {
		        perror("write");
		        continue;
		    }
		}
		else if((r += send(sock_fd,(void *)mensagem, DATA_SIZE, buff_size)) < 0) {
		    perror("write");
		    continue;
		}
	}
	
	
	
	//r = send(sock_fd,(void *)mensagem, strlen(mensagem), 0);

	//printf("\nENVIOU A MSG(%d bytes): %s\n", r, mensagem);

	if(r == -1){
		perror("send()");
		return -1;
	}

	return 0;
}

char* recebeDados(){
	char* dados;
	
	dados = malloc(2024*sizeof(char));
	
	if(recv(sock_fd, (void *)dados, (2024*sizeof(char)), 0) == -1){
		perror("recv()");
		return NULL;
	}
	
	//printf("\nRECEBEU A MSG: %s\n", dados);
	return dados;
}

int fechaConexao(){
	close(sock_fd);
}




