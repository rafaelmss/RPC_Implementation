#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXRECVSTRING 255  /* Longest string to receive */
    
struct servico{
	char nome[50];
	char ip[15];
};

int main(int argc, char *argv[])
{
	struct servico servicos[200];
	int numServ = 0;
	
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    unsigned short broadcastPort;     /* Port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */
	int i;
	struct sockaddr_in from;
	socklen_t from_len;
	
    if (argc != 2)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Broadcast Port>\n", argv[0]);
        exit(1);
    }

    broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        perror("bind() failed");
    
	while(1){
		/* Receive a single datagram from the server */
		/*if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
		    perror("recvfrom() failed");
		recvString[recvStringLen] = '\0';
		printf("Received: %s\n", recvString);*/
		from_len = sizeof(struct sockaddr_in);
		
		if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, (struct sockaddr*)&from, &from_len)) < 0)
		    perror("recvfrom() failed");
	
	
    	recvString[recvStringLen] = '\0';
    	
    	if(recvString[0] == '@'){
    		for(i = 1; i < strlen(recvString); i++){
    			servicos[numServ].nome[i-1]=recvString[i];
    		}
    		
    		servicos[numServ].nome[i] = '\0';
    		   		
    		//servicos[numServ].ip
    		char* ipString = inet_ntoa(from.sin_addr);
    		
    		strcpy(servicos[numServ].ip, ipString);
    		
    		printf("\nMeu nome é Paraíba, ta aqui o meu cartão:\n%s: %s\n",servicos[numServ].nome,servicos[numServ].ip);
    		
    		numServ++;
    	} else if (recvString[0] == '$') {
    		char* nome = malloc(strlen(recvString)*sizeof(char));
    		
    		for(i = 1; i < strlen(recvString); i++){
    			nome[i-1]=recvString[i];
    		}
    		
    		for(i = 0; i < numServ; i++){
    			if(strcmp(nome, servicos[i].nome) == 0){
    				sleep(2);
    				
    				int sock_r;                         /* Socket */
					struct sockaddr_in broadcastAddr_r; /* Broadcast address */
					char *broadcastIP_r;                /* IP broadcast address */
					unsigned short broadcastPort_r;     /* Server port */
					char *sendString_r;                 /* String to broadcast */
					int broadcastPermission_r;          /* Socket opt to set permission to broadcast */
					unsigned int sendStringLen_r;       /* Length of string to broadcast */

					broadcastIP_r = "192.168.0.0";            /* First arg:  broadcast IP address */ 
					broadcastPort_r = 3001;    /* Second arg:  broadcast port */

					/* Create socket for sending/receiving datagrams */
					if ((sock_r = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
						perror("socket() failed");

					/* Set socket to allow broadcast */
					broadcastPermission_r = 1;
					if (setsockopt(sock_r, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission_r, 
						  sizeof(broadcastPermission_r)) < 0)
						perror("setsockopt() failed");

					/* Construct local address structure */
					memset(&broadcastAddr_r, 0, sizeof(broadcastAddr_r));   /* Zero out structure */
					broadcastAddr_r.sin_family = AF_INET;                 /* Internet address family */
					broadcastAddr_r.sin_addr.s_addr = inet_addr(broadcastIP_r);/* Broadcast IP address */
					broadcastAddr_r.sin_port = htons(broadcastPort_r);         /* Broadcast port */
					
					sendString_r = malloc((strlen(servicos[i].ip)+1)*sizeof(char));
					strcpy(sendString_r, servicos[i].ip);
					
					sendStringLen_r = strlen(sendString_r);  /* Find length of sendString */

					/* Broadcast sendString in datagram to clients every 3 seconds*/
					if (sendto(sock_r, sendString_r, sendStringLen_r, 0, (struct sockaddr *)&broadcastAddr_r, sizeof(broadcastAddr_r)) != sendStringLen_r)
						perror("sendto() sent a different number of bytes than expected");
					
    				break;
    			}
    		}
    	}
    }
    
    close(sock);
    exit(0);
}
