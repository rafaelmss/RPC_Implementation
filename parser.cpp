#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <boost/regex.hpp>
#include <fstream>

using namespace std;
using namespace boost;

vector<string> split(string str, string sep);

struct parametro{
	string tipo;
	string referencia;
};

struct funcao_dado{
	string nome;
	string tipo_saida;
	vector<parametro> parametros;
};

#define DEBUG 0

int main(int argc, char *argv[]){
	ifstream reader;
	string rpc = ""; /*= "program ECHOSERVER\n{\n\n\n\t\t\t\tversion ECHOVERSION {\nint ECHO(in string, out string) = 1;\n} = 1;\n} = 0x20000001;";*/
	string linha;
	
	if(argc != 2){
		cout << "\nExemplo: " << argv[0] << " arquivo_de_entrada\n";
		exit(-1);
	}
	
	reader.open(argv[1], ifstream::in);
	
	if(!reader.is_open()){
		cout << "\nArquivo '" << argv[1] << "', nao encontrado.\n";
		exit(-1);
	}
	
	while(reader.good()){
		reader >> linha;
		if(reader.eof()){
			break;
		}
		rpc += (linha + " ");
	}
	
	rpc = rpc.substr(0, rpc.length()-1);
	
	vector<string> elem, aux, codigo, chaves, funcoesAux;
	int contador_chaves; /*sera utilizado para verificar se as chaves foram abertas e fechadas corretamente, 
							poderia ser usado pilha porem para perfomance de memoria*/
	int comp_chaves;
	vector<funcao_dado> funcoes;
	funcao_dado funcao_copia;
	parametro parametro_copia;
	
	for(int i = 0; i < elem.size(); i++){
		aux = split(elem[i], " ");
		for(int j = 0; j < aux.size(); j++){
			codigo.push_back(aux[j]);
		}
	}
	
	//remove \n substituindo por espaços
	regex exp_remove_enter("[\n]+");
	
	string programa = regex_replace(rpc, exp_remove_enter, " ");
	
	//remove espaços duplicados e tabulacao substituindo por um espaço
	regex exp_remove_espaco("\\s+");
	
	programa = regex_replace(programa, exp_remove_espaco, " ");
	
	
	regex e("((program)(\\s)([a-zA-Z]+))(\\s\\{\\s|\\{\\s|\\s\\{|\\{)(.*)");
	
	if (!regex_match (programa, e)){
		cout << "Erro de sintaxe. Código deve iniciar com 'program NOME'" << endl;
		exit(-1);
	}
	
	
	
	contador_chaves = 0;
	comp_chaves = 0;
	
	for(int i = 0; i < programa.length(); i++){
		if(programa[i] == '{'){
			contador_chaves++;
			comp_chaves++;
		}
		else if(programa[i] == '}'){
			comp_chaves--;
		}
	}
	
	if(contador_chaves < 2 && comp_chaves != 0){
		cout << "Erro de sintaxe. Verifique as chaves." << endl;
		exit(-1);
	}
	
	regex remove_e("((program)(\\s)([a-zA-Z]+))(\\s\\{\\s|\\{\\s|\\s\\{|\\{)");
		
	programa = regex_replace(programa, remove_e, "");
	
	
	regex e_fim_ref_proc("(.*)(\\s\\}\\s|\\}\\s|\\s\\}|\\})(\\=\\s|\\=)(0x[0-9]*;)");
	
	if (!regex_match (programa, e_fim_ref_proc)){
		cout << "Erro de sintaxe. Esperado '} = 0xNUMERO;'" << endl;
		exit(-1);
	}
	
	regex remove_fim_ref_proc("(\\s\\}\\s|\\}\\s|\\s\\}|\\})(\\=\\s|\\=)(0x[0-9]*;)");
	
	programa = regex_replace(programa, remove_fim_ref_proc, "");
	
	regex e_versao("((version)(\\s)([a-zA-Z]+))(\\s\\{\\s|\\{\\s|\\s\\{|\\{)(.*)");
	
	if (!regex_match (programa, e_versao)){
		cout << "Erro de sintaxe. Código deve conter 'version NOME'" << endl;
		exit(-1);
	}
	
	regex remove_versao("((version)(\\s)([a-zA-Z]+))(\\s\\{\\s|\\{\\s|\\s\\{|\\{)");
	
	programa = regex_replace(programa, remove_versao, "");
	
	regex e_fim_versao("(.*)(\\s\\}\\s|\\}\\s|\\s\\}|\\})(\\=\\s|\\=)([0-9]*;)");
	
	if (!regex_match (programa, e_fim_versao)){
		cout << "Erro de sintaxe. Esperado '} = NUMERO;'" << endl;
		exit(-1);
	}
	
	regex remove_fim_versao("(\\s\\}\\s|\\}\\s|\\s\\}|\\})(\\=\\s|\\=)([0-9]*;)");
	
	programa = regex_replace(programa, remove_fim_versao, "");
	
	regex e_sem_func("\\s*");
	
	if (regex_match (programa, e_sem_func)){
		cout << "Erro de sintaxe. Esperado declarações de funções." << endl;
		exit(-1);
	}
	
	funcoesAux = split(programa, ";");
	
	regex e_funcoes("(\\s*)((int)|(string)|(float))(\\s)(([a-zA-Z]+[0-9]*)*)(\\s*)(\\(\\s*)(.*)(\\s*\\)\\s*\\=\\s*[0-9]+)");
	regex remove_funcao("(\\s*)((int)|(string)|(float))(\\s+)(([a-zA-Z]+[0-9]*)*)(\\s*)((\\(\\s*)|(\\())");
	regex remove_fim_funcao("(\\s*\\)\\s*\\=\\s*[0-9]+)");
	regex e_funcao_vazia("\\s*");
	regex e_parametro("(\\s*)(inout|in|out)(\\s+)(int|string|float)(\\s*)");
	regex remove_parenteses("(\\()(.*)");
	
	for(int i = 0; i < funcoesAux.size(); i++){
		
		if(!regex_match (funcoesAux[i], e_funcoes)){
			cout << "Erro de sintaxe! Estrutura de função incorreta." << endl;
			exit(-1);
		}
		vector<string> aux, aux2;
		
		string linha_funcao = funcoesAux[i].c_str();
		
		aux = split(linha_funcao, " ");
		
		
		funcao_copia.tipo_saida = aux[0].c_str();
		aux[1] = regex_replace(aux[1], remove_parenteses, "");
		funcao_copia.nome = aux[1].c_str();
		
		funcoesAux[i] = regex_replace(funcoesAux[i], remove_funcao, "");
		funcoesAux[i] = regex_replace(funcoesAux[i], remove_fim_funcao, "");
		
		aux = split(funcoesAux[i], ",");
		
		if(aux.size() == 0 && !regex_match(funcoesAux[i], e_funcao_vazia)){
			cout << "Erro de sintaxe. Declaração de parametros inválida, esperado ','" << endl;
			exit(-1);
		}
		
		funcao_copia.parametros.clear();
		
		for(int j = 0; j < aux.size(); j++){
			if(!regex_match (aux[j], e_parametro)){
				cout << "Erro de sintaxe! Declaração de parametro incorreta." << endl;
				exit(-1);
			}
			
			aux2 = split(aux[j], " ");
			
			parametro_copia.referencia = aux2[0];
			
			parametro_copia.tipo = aux2[1];
			
			funcao_copia.parametros.push_back(parametro_copia);
		}
		
		
		funcoes.push_back(funcao_copia);
		
		
	}
	
	// stubs
	ofstream filec;
	filec.open ("client_stub.c", ios::out);
	
	filec << "#include <stdio.h>\n";
	filec << "#include <stdlib.h>\n";
	filec << "#include <string.h>\n";
	filec << "#include \"cliente.c\"\n";
	filec << "\n";
	for(int i = 0; i < funcoes.size(); i++){
		if(DEBUG){
			cout << funcoes[i].nome << " " << funcoes[i].tipo_saida << endl;
		}
		
		if(funcoes[i].tipo_saida == "string"){
			funcoes[i].tipo_saida = "char*";
		}
		
		filec << funcoes[i].tipo_saida << " " << funcoes[i].nome << "(" ;
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			if(DEBUG){
				cout << funcoes[i].parametros[j].referencia << " " << funcoes[i].parametros[j].tipo << endl;
			}
			
			if(j+1 == funcoes[i].parametros.size()){
				if(funcoes[i].parametros[j].referencia == "in"){
					if(funcoes[i].parametros[j].tipo == "string"){
						filec << "char* var" << j+1 << "){\n";
					}
					else{
						filec << funcoes[i].parametros[j].tipo << " " << "var" << j+1 << "){\n";
					}
				}
				else{
					if(funcoes[i].parametros[j].tipo == "string"){
						filec << "char* var" << j+1 << "){\n";
					}
					else{
						filec << funcoes[i].parametros[j].tipo << " *var" << j+1 << "){\n";
					}
				}
			}
			else{
				if(funcoes[i].parametros[j].referencia == "in"){
					if(funcoes[i].parametros[j].tipo == "string"){
						filec << "char* var" << j+1 << ", ";
					}
					else{
						filec << funcoes[i].parametros[j].tipo << " var" << j+1 << ", ";
					}
				}
				else{
					if(funcoes[i].parametros[j].tipo == "string"){
						filec << "char* var" << j+1 << ", ";
					}
					else{
						filec << funcoes[i].parametros[j].tipo << " *var" << j+1 << ", ";
					}
				}
			}
		}
		
		filec << "\t//Gerado automaticamente\n";
		filec << "\tint i;\n";
		filec << "\tchar aux[1024], *resposta;\n";
		filec << "\tchar *dados = \"\";\n";
		filec << "\tint tam = 0;\n";
		
		if(funcoes[i].tipo_saida == "int"){
			filec << "\tint retorno;\n\n";
		}
		else if(funcoes[i].tipo_saida == "float"){
			filec << "\tfloat retorno;\n\n";
		}
		else{
			filec << "\tchar *retorno;\n\n";
		}
		
		filec << "\t//Calcular o tamanho para armazenamento dos dados\n";
		
		filec << "\tsprintf(aux, \"%d\"," << i <<");\n";
		filec << "\ttam += strlen(aux);\n";
		
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			filec << "\ttam++;\n";
			if(funcoes[i].parametros[j].tipo == "int"){
				filec << "\tsprintf(aux, \"%d\", var" << j+1 <<");\n";
				filec << "\ttam += strlen(aux);\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				filec << "\tsprintf(aux, \"%f\", var" << j+1 <<");\n";
				filec << "\ttam += strlen(aux);\n";
			}
			else{
				filec << "\ttam += strlen(var" << j+1 <<");\n";
			}
		}
		
		filec << "\tdados = malloc((tam+1)*sizeof(char));\n\n";
		
		//marshall
		filec << "\tstrcat(dados, \"" << i << "\");\n";
		
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			filec << "\tstrcat(dados, \"#\");\n";
			if(funcoes[i].parametros[j].tipo == "int"){
				filec << "\tsprintf(aux, \"%d\", var" << j+1 <<");\n";
				filec << "\tstrcat(dados, aux);\n\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				filec << "\tsprintf(aux, \"%f\", var" << j+1 <<");\n";
				filec << "\tstrcat(dados, aux);\n\n";
			}
			else{
				filec << "\tstrcat(dados, var" << j+1 <<");\n\n";
			}
		}
		
		//enviado dados para servidor
		filec << "\testabeleceConexao();\n";
		filec << "\tenviaDados(dados);\n";
		filec << "\tresposta = recebeDados();\n\n";
		
		filec << "\tchar* aux2;\n\n";
		
		//unmarshall
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			if(j == 0){
				filec << "\taux2 = strtok(resposta, \"#\");\n";
			}
			else{
				filec << "\taux2 = strtok(NULL, \"#\");\n";
			}
			
			filec << "\tif(aux2 == NULL){\n";
			filec << "\t\tprintf(\"\\nErro de operacao, quantidade de parametros incorreta!\\n\");\n";
			filec << "\t\texit(-1);\n";
			filec << "\t}\n";
			
			if(funcoes[i].parametros[j].tipo == "int"){
				filec << "\tvar" << j+1 << " = atoi(aux2);\n\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				filec << "\tvar" << j+1 << " = atof(aux2);\n\n";
			}
			else{
				if(funcoes[i].parametros[j].referencia == "in"){
					filec << "\tvar" << j+1 << " = malloc((strlen(aux2)+1)*sizeof(char));\n";
				
					filec << "\tstrcpy(var" << j+1 << ", aux2);\n\n";
				}
				else{
					filec << "\t*var" << j+1 << " = *(char *)malloc((strlen(aux2)+1)*sizeof(char));\n";
				
					filec << "\tstrcpy(var" << j+1 << ", aux2);\n\n";
				}
			}
		}
		
		if(funcoes[i].parametros.size() == 0){
			filec << "\taux2 = strtok(resposta, \"#\");\n";
		}
		else{
			filec << "\taux2 = strtok(NULL, \"#\");\n";
		}
		
		filec << "\tif(aux2 == NULL){\n";
		filec << "\t\tprintf(\"\\nErro de operacao, retorno da funcao nao encontrado!\\n\");\n";
		filec << "\t\texit(-1);\n";
		filec << "\t}\n\n";
		
		
		if(funcoes[i].tipo_saida == "int"){
			filec << "\tretorno = atoi(aux2);\n\n";
		}
		else if(funcoes[i].tipo_saida == "float"){
			filec << "\tretorno = atof(aux2);\n\n";
		}
		else{
			filec << "\tretorno = malloc((strlen(aux2)+1)*sizeof(char));\n\n";
			filec << "\tstrcpy(retorno, aux);\n\n";
		}
		
		filec << "\treturn retorno;\n\n";
		
		filec << "}\n\n";
		
		if(DEBUG){
			cout << "--------------------------" << endl;
		}
		
		
		
		
		
		
		
	}
	
	filec.close();
	
	
	// ----------------------------------- servidor --------------------------------
	
	ofstream files;
	files.open ("server_stub.c", ios::out);
	
	files << "#include <stdio.h>\n";
	files << "#include <stdlib.h>\n";
	files << "#include <string.h>\n";
	files << "#include \"server.c\"\n";
	files << "\n";
	
	files << "void *atende(void* param) {\n";
	files << "\tint conn_fd = *(int *)param;\n";
	files << "\tchar *dados, *resposta;\n\n";
	files << "\tdados = recebeDados(conn_fd);\n\n";
	
	files << "\tchar* aux = strtok(dados, \"#\");\n";
	
	files << "\tif(aux == NULL){\n";
	files << "\t\tprintf(\"\\nErro de operacao, indice nulo ou incorreto!\\n\");\n";
	files << "\t\texit(-1);\n";
	files << "\t}\n";

	files << "\tint indice = atoi(aux);\n\n";
	files << "\tswitch(indice){\n";
	
	//skeleton
	for(int i = 0; i < funcoes.size(); i++){
	
		files << "\t\tcase " << i << ":\n";
		//unmarshall
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			files << "\t\t\taux = strtok(NULL, \"#\");\n";
		
			if(funcoes[i].parametros[j].tipo == "int"){
				files << "\t\t\tint " << funcoes[i].nome <<"_var" << j+1 << " = atoi(aux);\n\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				files << "\t\t\tfloat " << funcoes[i].nome <<"_var" << j+1 << " = atof(aux);\n\n";
			}
			else{
				files << "\t\t\tchar* " << funcoes[i].nome << "_var" << j+1 << ";\n";
				files << "\t\t\t" << funcoes[i].nome << "_var" << j+1 << " = malloc((strlen(aux)+1)*sizeof(char));\n";
				files << "\t\t\tstrcpy(" << funcoes[i].nome << "_var" << j+1 << ", aux);\n\n";
			}
		
			files << "\t\t\tif(aux == NULL){\n";
			files << "\t\t\t\tprintf(\"\\nErro de operacao, quantidade de parametros incorreta!\\n\");\n";
			files << "\t\t\t\texit(-1);\n";
			files << "\t\t\t}\n";
		}
		
		//chama funcao para operacao no servidor
		if(funcoes[i].tipo_saida == "int"){
			files << "\t\t\tint " << funcoes[i].nome << "_retorno;\n\n";
		}
		else if(funcoes[i].tipo_saida == "float"){
			files << "\t\t\tfloat " << funcoes[i].nome << "_retorno;\n\n";
		}
		else{
			files << "\t\t\tchar* " << funcoes[i].nome << "_retorno;\n\n";
		}
		
		files << "\t\t\t" << funcoes[i].nome << "_retorno = " << funcoes[i].nome <<"("; 
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			if(j == 0){
				if(funcoes[i].parametros[j].referencia == "in"){
					files << " " << funcoes[i].nome << "_var" << j+1;
				}
				else{
					if(funcoes[i].parametros[j].tipo == "string"){
						files << " " << funcoes[i].nome << "_var" << j+1;
					}
					else{
						files << "&" << funcoes[i].nome <<"_var" << j+1;
					}
				}
			}
			else{
				if(funcoes[i].parametros[j].referencia == "in"){
					files << ", " << funcoes[i].nome <<"_var" << j+1;
				}
				else{
					if(funcoes[i].parametros[j].tipo == "string"){
						files << ", " << funcoes[i].nome <<"_var" << j+1;
					}
					else{
						files << ", &" << funcoes[i].nome <<"_var" << j+1;
					}
				}
			}
		}
		
		files << ");\n\n";
		
		//marshall
		
		files << "\t\t\tint " << funcoes[i].nome << "_tam = 0;\n\n";
		
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			if(j != 0){
				files << "\t\t\t" << funcoes[i].nome << "_tam++;\n";
			}
				
			if(funcoes[i].parametros[j].tipo == "int"){
				files << "\t\t\tsprintf(aux, \"%d\", " << funcoes[i].nome << "_var" << j+1 <<");\n";
				files << "\t\t\t" << funcoes[i].nome << "_tam += strlen(aux);\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				files << "\t\t\tsprintf(aux, \"%f\",  " << funcoes[i].nome << "_var" << j+1 <<");\n";
				files << "\t\t\t" << funcoes[i].nome << "_tam += strlen(aux);\n";
			}
			else{
				files << "\t\t\t" << funcoes[i].nome << "_tam += strlen(" << funcoes[i].nome << "_var" << j+1 <<");\n";
			}
		}
		
		files << "\t\t\t" << funcoes[i].nome << "_tam++;\n";
		
		if(funcoes[i].tipo_saida == "int"){	
			files << "\t\t\tsprintf(aux, \"%d\", " << funcoes[i].nome << "_retorno);\n";
			files << "\t\t\t" << funcoes[i].nome << "_tam += strlen(aux);\n";
		}
		else if(funcoes[i].tipo_saida == "float"){
			files << "\t\t\tsprintf(aux, \"%f\", " << funcoes[i].nome << "_retorno);\n";
			files << "\t\t\t" << funcoes[i].nome << "_tam += strlen(aux);\n";
		}
		else{
			files << "\t\t\t" << funcoes[i].nome <<"_tam += strlen(" << funcoes[i].nome <<"_retorno);\n";
		}
		
		files << "\t\t\tresposta = malloc((" << funcoes[i].nome << "_tam+1)*sizeof(char));\n\n";
		
		for(int j = 0; j < funcoes[i].parametros.size(); j++){
			if(j != 0){
				files << "\t\t\tstrcat(resposta, \"#\");\n";
			}
				
			if(funcoes[i].parametros[j].tipo == "int"){
				files << "\t\t\tsprintf(aux, \"%d\", " << funcoes[i].nome << "_var" << j+1 <<");\n";
				files << "\t\t\tstrcat(resposta, aux);\n\n";
			}
			else if(funcoes[i].parametros[j].tipo == "float"){
				files << "\t\t\tsprintf(aux, \"%f\", " << funcoes[i].nome << "_var" << j+1 <<");\n";
				files << "\t\t\tstrcat(resposta, aux);\n\n";
			}
			else{
				files << "\t\t\tstrcat(resposta, " << funcoes[i].nome << "_var" << j+1 <<");\n\n";
			}
		}
		
		files << "\t\t\tstrcat(resposta, \"#\");\n";
		
		if(funcoes[i].tipo_saida == "int"){	
			files << "\t\t\tsprintf(aux, \"%d\", " << funcoes[i].nome <<"_retorno);\n";
			files << "\t\t\tstrcat(resposta, aux);\n";
		}
		else if(funcoes[i].tipo_saida == "float"){
			files << "\t\t\tsprintf(aux, \"%f\", " << funcoes[i].nome <<"_retorno);\n";
			files << "\t\t\tstrcat(resposta, aux);\n";
		}
		else{
			files << "\t\t\tstrcat(resposta, " << funcoes[i].nome <<"_retorno);\n";
		}
		
		files << "\t\t\tbreak;\n\n";
	}
	
	files << "\t} //fecha switch\n\n";
	
	files << "\tenviaDados(conn_fd, resposta);\n";
	files << "\tclose(conn_fd);\n";
	files << "\tpthread_exit((void*)0);\n";
	files << "}\n\n";
	
	return 0;
}

vector<string> split(string str, string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    vector<string> arr;
    current=strtok(cstr,sep.c_str());
    
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}
