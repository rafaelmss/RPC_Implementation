#include<stdio.h>
#include "client_stub.c"

void menu(int opcao);
void adicionarTelefone();
void pesquisarTelefone();
void excluirTelefone();


int main(){
    int opcao;
    do{
        printf("\n\t1) Adicionar telefone.");
        printf("\n\t2) Pesquisar por nome.");
        printf("\n\t3) Excluir");
        printf("\n\t4) Sair.");
        printf("\n\tOpcao: ");
        scanf("%d", &opcao);
        menu(opcao);
    }while(opcao!=4);
}

void menu(int opcao){

    switch(opcao){
        case 1: 
        		adicionarTelefone();
                break;
                
        case 2: 
        		pesquisarTelefone();
                break;
                
        case 3: 
        		excluirTelefone();
                break;
        case 4:
        		exit(0);
        		break;
        		
        default: 
        		printf("\nOpcao invalida!\n");
                break;
    }

}


void adicionarTelefone(){
    int telefone;
    char *nome;
    
	nome = malloc(100*sizeof(char));
	
    printf("\n\tNome: ");
    getchar();
    scanf("%s", nome);
    printf("\n\tTelefone: ");
    scanf("%d", &telefone);

    adicionarTelefoneRemoto(nome, telefone);
    return;
}

void pesquisarTelefone(){
    char *nome;
	int resp;
	
	nome = malloc(100*sizeof(char));
	
    printf("\n\tNome: ");
    getchar();
    scanf("%s", nome);

    resp = pesquisarTelefoneRemoto(nome);
    
    if(resp == -1){
    	printf("\nTelefone nao encontrado.\n");
    }
    else{
    	printf("\nTelefone: %d\n", resp);
    }
    
    return;
}

void excluirTelefone(){
    char *nome;
	int resp;
	
	nome = malloc(100*sizeof(char));
	
    printf("\n\tNome: ");
    getchar();
    scanf("%s", nome);

    resp = excluirTelefoneRemoto(nome);
    
    if(resp == -1){
    	printf("\nContato nao encontrado.\n");
    }
    else{
    	printf("\nContato deletado.\n");
    }
    
    return;
}
