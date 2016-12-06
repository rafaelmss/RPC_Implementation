#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_stub.c"

int pesquisarTelefoneRemoto(char* nome);
int adicionarTelefoneRemoto(char* nome, int telefone);
int excluirTelefoneRemoto(char* nome);

int adicionarTelefoneRemoto(char* nome, int telefone){
    char novo[300] = "";
    char telefoneChar[100];
	FILE *agendaFile;

    strcat(novo, "\n");
	strcat(novo, nome);
	strcat(novo, "@");
	sprintf(telefoneChar, "%d", telefone);
	strcat(novo, telefoneChar);

	agendaFile = fopen("agenda.txt", "a");

	if (agendaFile == NULL){
        printf("Problemas na CRIACAO do arquivo\n");
        return -1;
    }

    fputs(novo, agendaFile);
    fclose(agendaFile);

	return 0;
}

int pesquisarTelefoneRemoto(char* nome){
    char linha[300];
    char* contato;
    char novo[300] = "";
    char* telefoneChar;
    int i;
	FILE *agendaFile;
	int resp = 0;
	
	agendaFile = fopen("agenda.txt", "r+");

	if (agendaFile == NULL){
        printf("Problemas na LEITURA do arquivo\n");
        return -1;
    }

    i = 1;

    while (!feof(agendaFile)){
        fscanf(agendaFile, "%s", linha);
        printf("\n%s\n", linha);
        contato = strtok(linha, "@");
        if(strcmp(contato, nome) == 0){
            telefoneChar = strtok(NULL, "@");
			resp = atoi(telefoneChar);
            return resp;
        }
        i++;
    }

    fclose(agendaFile);
	return -1;
}

int excluirTelefoneRemoto(char* nome){
    char linha[300];
    char* contato;
    char novo[300];
    char* telefoneChar;
    int i;
	FILE *agendaFile;
	int deletou = -1;
	int achou;
	
	agendaFile = fopen("agenda.txt", "r");
	if (agendaFile == NULL){
        printf("Problemas na LEITURA do arquivo\n");
        return -1;
    }

    strcpy(novo, "");
  	while (!feof(agendaFile)){
  		achou = 0;
  		
        fscanf(agendaFile, "%s", linha);
        for(i = 0; i < strlen(linha); i++){
        	if(linha[i] == '@'){
        		achou = 1;
        	}
        }
        
        if(achou == 0){
        	continue;
        }
        
        printf("%s\n", linha);
        contato = strtok(linha, "@");
        telefoneChar = strtok(NULL, "@");
        
        if(strcmp(contato, nome) != 0){            
            strcat(novo, contato);
            strcat(novo, "@");
            strcat(novo, telefoneChar);
            strcat(novo, "\n");
        }
        else{
        	deletou = 1;
        }
    }
	
    fclose(agendaFile);
    
    agendaFile = fopen("agenda.txt", "w");
	fputs(novo, agendaFile);
	fclose(agendaFile);
	
	return deletou;
}
