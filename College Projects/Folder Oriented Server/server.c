#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include "parser.tab.h"
#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <crypt.h>

#define _XOPEN_SOURCE
#define PORTA 8080
#define TAMANHO_MENSAGEM 1024
// Códigos de erro HTTP
#define HTTP_NOT_FOUND 404
#define HTTP_FORBIDDEN 403
#define MAX_BUFFER_SIZE 2048
#define NUM_FDS 5
#define MAX_PATH 1024
#define INVALID_CHAR 255

pthread_mutex_t lock; //mutex para controlar as threads 

struct noValor{
    char* nomeValor;
    struct noValor* proxValor;
};
struct noCampo{
    char* nomeCampo;
    struct noCampo* proxCampo;
    struct noValor* listaValor;
};
struct HttpRequest{
    char* metodo;
    char* caminho;
    char* protocolo;
};
// Estrutura para passar múltiplos parâmetros para a função da thread
typedef struct {
    int socket;
    char* registro;
    char* caminho;
} thread_args;
struct HttpRequest* request =  NULL;
struct noCampo* inicioCampo = NULL; //marca o inicio da lista
struct noCampo* fimCampo = NULL;    //marca o fim dessa lista
struct noValor* inicioValor = NULL; //marca o inicio da lista
struct noValor* fimValor = NULL;    //marca o fim dessa lista

extern char* host;
char saida[4096] = {0};
int contador = 0;


void get_requisicao(char* reqLine){
    struct HttpRequest* req = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
    // Variável temporária para manipulação da string
    char temp[256];
    strcpy(temp, reqLine);

    // Separar a primeira palavra (método)
    char *token = strtok(temp, " ");
    req->metodo = strdup(token);
    // printf("M: %s\n", req->metodo);
    token = strtok(NULL, " ");
    req->caminho = strdup(token);
    // printf("C: %s\n", req->caminho);
    token = strtok(NULL, " ");
    req->protocolo = strdup(token);
    // printf("P: %s\n", req->protocolo);
    request = req;
}

void zera_inicioValor(){
    inicioValor = NULL;
    return;
}

/* Cria e adiciona iterm na lista Campos; caso ela não exista, cria nova lista */
void Cria_e_add_no_campo(char* nome){
    struct noCampo* novoNo = (struct noCampo*)malloc(sizeof(struct noCampo));
    if (novoNo != NULL) {
        novoNo->nomeCampo = nome;  // Atribui o nome do campo ao nó
        novoNo->proxCampo = NULL;
        novoNo->listaValor = inicioValor;
        inicioValor = NULL;
        if ( inicioCampo == NULL){
            inicioCampo = novoNo;
            fimCampo = novoNo;
        } else{
            fimCampo->proxCampo = novoNo;
            fimCampo = novoNo;
        }
    } else {
        printf("sem espaco na memoria para campo");
    }
    return;
}

/* Cria e adiciona iterm na lista Campos; caso ela não exista, cria nova lista */
void Cria_e_add_no_valor(char* nome){
    struct noValor* novoNo = (struct noValor*)malloc(sizeof(struct noValor));
    if (novoNo != NULL) {
        novoNo->nomeValor = nome;  // Atribui o nome do campo ao nó
        novoNo->proxValor = NULL;
        if ( inicioValor == NULL){
            inicioValor = novoNo;
            fimValor = novoNo;
        } else{
            fimValor->proxValor = novoNo;
            fimValor = novoNo;
        }
    } else {
        printf("sem espaco na memoria para valor");
    }
    return;
}

/* Printa qualquer uma das listas*/
void printList(struct noCampo* head) {
    struct noCampo* atualCampo = head;
    struct noValor* atualValor = atualCampo->listaValor;
    char imprime[256];

    // printf("inicio while\n");
    while (atualCampo != NULL) {
        sprintf(imprime, "%s:", atualCampo->nomeCampo);
        strcat(saida, imprime);
        atualValor = atualCampo->listaValor;
        sprintf(imprime,"%s", atualValor->nomeValor);
        strcat(saida, imprime);
        atualValor = atualValor->proxValor;
        while (atualValor != NULL){
            sprintf(imprime,",%s", atualValor->nomeValor);
            strcat(saida, imprime);
            atualValor = atualValor->proxValor;
        }
        strcat(saida, "\r\n");
        atualCampo = atualCampo->proxCampo;
    }
}

char* get_formatted_time() {
    time_t t;
    static char buffer[100];
    setlocale(LC_TIME, "");

    // Variável para armazenar o tempo atual
    time(&t);
    struct tm *local_time = localtime(&t);

    // Formatar a data e hora no formato desejado
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y %Z", local_time);

    // Retornar o buffer com a data formatada
    return buffer;
}

char* get_last_mod_t(const char *recurso) {
    struct stat file_stat;
    static char buffer1[100];
    // Verificar se conseguimos obter informações sobre o arquivo
    if (stat(recurso, &file_stat) == -1) {
        // perror("Erro ao obter informações do arquivo");
        return "ERRO";
    }

    // Converter o tempo de modificação para o formato local
    struct tm *mod_time = localtime(&file_stat.st_mtime);    
    strftime(buffer1, sizeof(buffer1), "%a %b %d %H:%M:%S %Y %Z", mod_time);
    return buffer1;
}

char* get_file_extension(const char* path) {
    // Encontra a última ocorrência de '.'
    char *dot = strrchr(path, '.');
    
    // Verifica se encontrou um '.' e se não é o primeiro caractere (para evitar arquivos ocultos sem extensão)
    if (!dot || dot == path) {
        return ""; // Não tem extensão
    }
    
    // Retorna a extensão (o que vem após o '.')
    return dot + 1;
}

char* get_tipo(char *ext){
    char *tipo;
    if (strcmp(ext, "html") == 0){
        tipo = "text/html";
    }else if (strcmp(ext, "pdf") == 0){
        tipo = "application/pdf";
    }else if (strcmp(ext, "gif") == 0){
        tipo = "image/gif";
    }else if (strcmp(ext, "png") == 0){
        tipo = "image/png";
    }else if (strcmp(ext, "jpg") == 0){
        tipo = "image/jpeg";
    }else if (strcmp(ext, "txt") == 0){
        tipo = "text/plain";
    }
    return tipo;
}

char* get_connection(){
    struct noCampo* noAtual = inicioCampo;
    if (noAtual == NULL){
        return "close";
    }
    //não sei porque, mas no firefox tive algumas requisições enviando Connectio invés de Connection, bagunçando todo o meu código
    while (strcmp(noAtual->nomeCampo, "Connection") != 0 && strcmp(noAtual->nomeCampo, "Connectio") != 0 && (noAtual->proxCampo != NULL)){
        noAtual = noAtual->proxCampo;
    }
    if (noAtual->proxCampo == NULL){
        return "keep-alive";
    }

    //depois de fazer esse while o noAtual = connection que queremos
    return noAtual->listaValor->nomeValor;
}

char *get_realm(char *htaccess){
    FILE *arquivo = fopen(htaccess, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    char buffer[128];
    char *linha = NULL;

    // Lê a primeira linha do arquivo
    if (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        // Remove o caractere de nova linha, se existir
        char *nova_linha = strchr(buffer, '\n');
        if (nova_linha) {
            *nova_linha = '\0';
        }

        // Aloca memória para a linha e copia o conteúdo
        linha = malloc(strlen(buffer) + 1);
        if (linha) {
            strcpy(linha, buffer);
        } else {
            perror("Erro ao alocar memória");
        }
    }

    fclose(arquivo);
    return linha;

}

int cabecalho(int resposta, char *msg, char *recurso, int tam, char *req, char* reg_file){
    char *extensao, *tipo, *data, *conexao, *data_mod, *reg;
    char resp[20], taman[20], imprimi[256];
    struct stat statbuf;
    int registro = open(reg_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (registro < 0) {
            perror("Erro ao abrir o arquivo\n");
            return -1;
    }
    // printf("antes da data\n");
    //definição de variaveis com base em funções
    data = get_formatted_time();
    // printf("entre data e conex\n");
    conexao = get_connection();
    // printf("depois conex\n");
    sprintf(taman, "%d", tam);
    
    
    //escrevendo resposta
    snprintf(imprimi, sizeof(imprimi),  "HTTP/1.1 %d %s\r\n", resposta, msg);
    strcpy(saida, imprimi);
    if (host != NULL){
        strcat(saida, host);
    }
    if (resposta == 401){
        snprintf(imprimi, sizeof(imprimi),  "WWW-Authenticate: Basic realm=\"%s\"\r\n", get_realm(recurso));
        strcat(saida, imprimi);
    }
    if (strcmp(req, "OPTIONS") == 0){
        strcat(saida, "Allow: OPTIONS, TRACE, GET, HEAD\n");
    }
    snprintf(imprimi, sizeof(imprimi),  "Date: %s\r\n", data);
    strcat(saida, imprimi);

    strcat(saida, "Server: Servidor HTTP ver. 0.1 de Pedro Pimenta\r\n");

    snprintf(imprimi, sizeof(imprimi),  "Connection: %s\r\n", conexao);
    strcat(saida, imprimi);

    // printf("meio da resposta\n");
    
    if (recurso != NULL && resposta != 401 && ( strcmp(req, "GET") == 0 || strcmp(req, "HEAD") == 0)){
        extensao = get_file_extension(recurso);
        tipo = get_tipo(extensao);
        data_mod = get_last_mod_t(recurso);

        snprintf(imprimi, sizeof(imprimi),  "Last-Modified: %s\r\n", data_mod);
        strcat(saida, imprimi);

        snprintf(imprimi, sizeof(imprimi),  "Content-Length: %s\r\n", taman);
        strcat(saida, imprimi);

        snprintf(imprimi, sizeof(imprimi),  "Content-Type: %s\r\n", tipo);
        strcat(saida, imprimi);
    }
    strcat(saida, "\r\n");    
    printf("%s", saida);
    write(registro, saida, strlen(saida));
    // printf("fim da resposta\n");
    close(registro);
}

int retorna_trace(char *req, char* caminho, char* reg_file){
    cabecalho(200, "OK", NULL, 0, req, reg_file);
    // printf("/ex5-6-7 %s %s %s\n", req, caminho_web, recurso_solicitado);
    // printf("logo antes do printlist\n");
    printList(inicioCampo);
}

int acessar_recurso(char *req, char *caminho_completo, char* reg_file, int novo_soquete) {
    struct stat statbuf;
    int tam;
    int respN;
    // printf("começo do acessar recurso\n"); //debug
    printf("%s\n", caminho_completo); //debug
    // printf("Caminho completo: %s\n", caminho_completo);  // Depuração

    // (b) Verificar se o recurso é um arquivo ou diretório
    if (stat(caminho_completo, &statbuf) != 0) {
        if (errno == ENOENT) {
            // printf("Recurso não encontrado: %s\n", caminho_completo);  // Depuração
            cabecalho(HTTP_NOT_FOUND, "PATH NOT FOUND", caminho_completo, 0, req, reg_file);
            write(novo_soquete, saida, strlen(saida));
            return HTTP_NOT_FOUND;  // Recurso não encontrado
        } else if ((errno == EACCES)) {
            cabecalho(HTTP_FORBIDDEN, "PATH FORBIDDEN",  caminho_completo, 0, req, reg_file);
            write(novo_soquete, saida, strlen(saida));
            return HTTP_FORBIDDEN;  // caminho sem permissão de leitura
        } else if ((statbuf.st_mode & S_IRUSR)){
            printf("debuf\n");
        }
        else {
            perror("Erro ao acessar o recurso com stat aqui");
            return -1;
        }
    }
    
    // (e) Caso seja um arquivo
    if (S_ISREG(statbuf.st_mode)) {
        // (d) Verificar se o recurso possui permissão de leitura
        if (!(statbuf.st_mode & S_IRUSR)) {
            // printf("Sem permissão de leitura: %s\n", caminho_completo);  // Depuração
            cabecalho(HTTP_FORBIDDEN, "FORBIDDEN",  caminho_completo, 0, req, reg_file);
            write(novo_soquete, saida, strlen(saida));
            return HTTP_FORBIDDEN;  // Sem permissão de leitura
        }

        // printf("Recurso é um arquivo\n");  // Depuração
        int fd = open(caminho_completo, O_RDONLY);
        if (fd < 0) {
            perror("Erro ao abrir o arquivo");
            return -1;
        }
        tam = statbuf.st_size;
        cabecalho(200, "OK",  caminho_completo, tam, req, reg_file);
        write(novo_soquete, saida, strlen(saida));
        if (strcmp(req, "GET") == 0){
            char buffer[4096]={0};
            ssize_t bytes_lidos;
            memset(saida, 0, strlen(saida));
            if (tam > 4096){
                while ((bytes_lidos = read(fd, buffer, sizeof(buffer))) > 0) {
                    buffer[bytes_lidos] = '\0';
                    buffer[bytes_lidos+1] = '\0';
                    send(novo_soquete, buffer, bytes_lidos,0);
                    memset(buffer, 0, strlen(buffer));
                }
            }else{
                bytes_lidos = read(fd, buffer, sizeof(buffer));
                buffer[bytes_lidos] = '\0';
                buffer[bytes_lidos+1] = '\0';
                send(novo_soquete, buffer,bytes_lidos,0);
            }
            // printf("\n");

            close(fd);
        }
        return 200;  // Sucesso
    }
    
    // Caso seja um diretório
    if (S_ISDIR(statbuf.st_mode)) {
        // printf("Recurso é um diretório\n");  // Depuração

        // (e) Verificar permissão de execução (varredura) no diretório
        if (!(statbuf.st_mode & S_IXUSR)) {
            // printf("Sem permissão de execução no diretório: %s\n", caminho_completo);  // Depuração
            cabecalho(HTTP_FORBIDDEN, "FORBIDDEN",  caminho_completo, 0, req, reg_file);
            write(novo_soquete, saida, strlen(saida));
            return HTTP_FORBIDDEN;  // Sem permissão de varredura
        }

        // Verificar por index.html ou welcome.html
        const char *arquivos_padrao[] = {"index.html", "welcome.html"};
        for (int i = 0; i < 2; i++) {
            char caminho_arquivo[1026];
            snprintf(caminho_arquivo, sizeof(caminho_arquivo), "%s/%s", caminho_completo, arquivos_padrao[i]);
            // printf("Verificando arquivo padrão: %s\n", caminho_arquivo);  // Depuração

            if (stat(caminho_arquivo, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
                // Verificar permissão de leitura
                if (statbuf.st_mode & S_IRUSR) {
                    // printf("Arquivo encontrado com permissão de leitura: %s\n", caminho_arquivo);  // Depuração
                    int fd = open(caminho_arquivo, O_RDONLY);
                    if (fd < 0) {
                        perror("Erro ao abrir o arquivo");
                        return -1;
                    }
                    tam = statbuf.st_size;
                    cabecalho(200, "OK",  caminho_arquivo, tam, req, reg_file);
                    write(novo_soquete, saida, strlen(saida));
                    if (strcmp(req, "GET") == 0){
                        char buffer[4096];
                        ssize_t bytes_lidos;
                        memset(saida, '\0', strlen(saida));
                        if (tam > 4096){
                            while ((bytes_lidos = read(fd, buffer, sizeof(buffer))) > 0) {
                                // printf("%zd\n", bytes_lidos);
                                buffer[bytes_lidos] = '\0';
                                buffer[bytes_lidos+1] = '\0';
                                strcat(saida, buffer);
                                write(novo_soquete, saida, strlen(saida));
                                memset(saida, '\0', strlen(saida));
                            }
                        }else{
                            bytes_lidos = read(fd, buffer, sizeof(buffer));
                            buffer[bytes_lidos] = '\0';
                            buffer[bytes_lidos+1] = '\0';
                            printf("%zd\n", bytes_lidos);
                            strcat(saida, buffer);
                            write(novo_soquete, saida, strlen(saida));
                        }
                        printf("\n");
                    }

                    close(fd);
                    return 200;  // Sucesso
                } else {
                    if (i==1){
                        //printf("Sem permissão de leitura: %s\n", caminho_arquivo);  // Depuração
                        cabecalho(HTTP_FORBIDDEN, "FORBIDDEN",  caminho_completo, 0, req, reg_file);
                        write(novo_soquete, saida, strlen(saida));
                        return HTTP_FORBIDDEN;  // Sem permissão de leitura
                    }
                }
            }
        }
        //printf("Nenhum arquivo padrão encontrado\n");  // Depuração
        cabecalho(HTTP_NOT_FOUND, "FILE NOT FOUND", caminho_completo, 0, req, reg_file);
        write(novo_soquete, saida, strlen(saida));
        return HTTP_NOT_FOUND;  // Nenhum dos arquivos padrões foi encontrado
    } 
    return HTTP_NOT_FOUND;  // Recurso desconhecido
}

void registro_requisicao(char* req_file){
    /*registra a requisição que foi feita no arquivo de registro*/
    int registro = open(req_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (registro < 0) {
        perror("Erro ao abrir o arquivo aqui REG\n");
        return ;
    }
    write(registro, "\n", 1);
    write(registro, request->metodo, strlen(request->metodo));
    write(registro, " ", 1);
    write(registro, request->caminho, strlen(request->caminho));
    write(registro, " ", 1);
    write(registro, request->protocolo, strlen(request->protocolo));
    write(registro, "\n", 1);
    close(registro);
}

void limpa_lista(struct noCampo* Head){
    struct noValor *valorAtual, *valorFree;
    struct noCampo *campoAtual;
    while (Head != NULL){
        valorAtual = Head->listaValor;
        while (valorAtual != NULL){
            valorFree = valorAtual;
            valorAtual = valorAtual->proxValor;
            free(valorFree);
        }
        campoAtual = Head;
        Head = Head->proxCampo;
        campoAtual->proxCampo = NULL;
        free(campoAtual);        
    }
    inicioCampo = NULL;
    fimCampo = NULL;
    inicioValor = NULL;
    fimValor = NULL;
}

char *verificar_ultimo_htaccess(const char *caminho) {
    static char ultimo_htaccess[MAX_PATH] = ""; // Armazena o último .htaccess encontrado
    char buffer[MAX_PATH] = "";
    struct stat statbuf;

    // Cria uma cópia do caminho para manipulação
    strncpy(buffer, caminho, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0'; // Garante terminação

    char *pos = buffer;

    // Percorre cada diretório no caminho
    while ((pos = strchr(pos, '/')) != NULL) {
        *pos = '\0'; // Termina a string no diretório atual
        char htaccess_path[MAX_PATH+11];

        // Monta o caminho para o .htaccess
        snprintf(htaccess_path, sizeof(htaccess_path), "%s/.htaccess", buffer);

        // Verifica se o .htaccess existe
        if (stat(htaccess_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
            strncpy(ultimo_htaccess, htaccess_path, sizeof(ultimo_htaccess));
            ultimo_htaccess[sizeof(ultimo_htaccess) - 1] = '\0'; // Garante terminação
        }

        *pos = '/'; // Restaura a barra e avança
        pos++;
    }

    // Verifica a pasta do arquivo final
    char htaccess_path[MAX_PATH+10];
    snprintf(htaccess_path, sizeof(htaccess_path), "%s/.htaccess", buffer);
    if (stat(htaccess_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
        strncpy(ultimo_htaccess, htaccess_path, sizeof(ultimo_htaccess));
        ultimo_htaccess[sizeof(ultimo_htaccess) - 1] = '\0'; // Garante terminação
    }
    printf("%s\n", ultimo_htaccess);
    // Retorna o último .htaccess encontrado ou NULL se nenhum foi encontrado
    return strlen(ultimo_htaccess) > 0 ? ultimo_htaccess : NULL;
}

int ta_dentro(const char *path) {
    //la ele
    int count = 0;
    char temp[256];

    strncpy(temp, path, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    char *token = strtok(temp, "/");
    while (token) {
        if (strcmp(token, "..") == 0) {
            count--; // Diretório "acima"
            if (count < 0) {
                // printf("chegou no meio\n");
                return 0; // Fora do diretório "meu-webspace"
            }
        } else {
            count++; // Diretório "dentro"
        }
        token = strtok(NULL, "/");
    }
    // printf("chegou no final\n");
    return count >= 0;
}
// Tabela de decodificação para Base64
static const uint8_t base64_decoding_table[256] = {
    [0 ... 255] = INVALID_CHAR, // Inicializa todos como inválidos
    ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,
    ['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
    ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
    ['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
    ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
    ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
    ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
    ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63,
};

char* decode_base64(const char *input, size_t *output_length) {
    size_t input_length = strlen(input);

    // Verifica se o tamanho da string é válido
    if (input_length % 4 != 0) {
        fprintf(stderr, "Erro: String Base64 inválida.\n");
        return NULL;
    }

    // Calcula o tamanho do buffer de saída
    size_t padding = 0;
    if (input_length > 0 && input[input_length - 1] == '=') padding++;
    if (input_length > 1 && input[input_length - 2] == '=') padding++;
    size_t decoded_length = (input_length / 4) * 3 - padding;

    // Aloca memória para o buffer de saída
    char *decoded_data = malloc(decoded_length + 1);
    if (!decoded_data) {
        perror("Erro ao alocar memória");
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < input_length;) {
        uint32_t sextet_a = input[i] == '=' ? 0 & i++ : base64_decoding_table[(unsigned char)input[i++]];
        uint32_t sextet_b = input[i] == '=' ? 0 & i++ : base64_decoding_table[(unsigned char)input[i++]];
        uint32_t sextet_c = input[i] == '=' ? 0 & i++ : base64_decoding_table[(unsigned char)input[i++]];
        uint32_t sextet_d = input[i] == '=' ? 0 & i++ : base64_decoding_table[(unsigned char)input[i++]];

        if (sextet_a == INVALID_CHAR || sextet_b == INVALID_CHAR ||
            sextet_c == INVALID_CHAR || sextet_d == INVALID_CHAR) {
            fprintf(stderr, "Erro: Caractere inválido na string Base64.\n");
            free(decoded_data);
            return NULL;
        }

        uint32_t triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

        if (j < decoded_length) decoded_data[j++] = (triple >> 16) & 0xFF;
        if (j < decoded_length) decoded_data[j++] = (triple >> 8) & 0xFF;
        if (j < decoded_length) decoded_data[j++] = triple & 0xFF;
    }

    decoded_data[decoded_length] = '\0'; // Garante a terminação da string
    if (output_length) {
        *output_length = decoded_length;
    }
    return decoded_data;
}

char* pegar_htpassword(const char *caminho) {
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    char buffer[256];
    char *linha = NULL;
    int contador_linha = 0;

    // Lê o arquivo linha por linha
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        contador_linha++;

        // Verifica se estamos na segunda linha
        if (contador_linha == 2) {
            // Remove o caractere de nova linha, se existir
            char *nova_linha = strchr(buffer, '\n');
            if (nova_linha) {
                *nova_linha = '\0';
            }

            // Aloca memória para a linha e copia o conteúdo
            linha = malloc(strlen(buffer) + 1);
            if (linha) {
                strcpy(linha, buffer);
            } else {
                perror("Erro ao alocar memória");
            }
            break;
        }
    }

    fclose(arquivo);

    // Se não houver pelo menos duas linhas, retorna NULL
    if (contador_linha < 2) {
        fprintf(stderr, "O arquivo não contém uma segunda linha.\n");
        return NULL;
    }

    return linha;
}

// Função para buscar a senha de um usuário em um arquivo
char* obter_senha(const char *caminho_arquivo, const char *usuario) {
    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo de senha");
        return NULL;
    }

    char buffer[512];
    char *senha = NULL;

    // Lê o arquivo linha por linha
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        // Remove o caractere de nova linha, se presente
        char *nova_linha = strchr(buffer, '\n');
        if (nova_linha) {
            *nova_linha = '\0';
        }

        // Divide a linha em usuário e senha com strtok
        char *usuario_atual = strtok(buffer, ":");
        char *senha_atual = strtok(NULL, ":");

        // Verifica se o usuário corresponde
        if (usuario_atual && senha_atual && strcmp(usuario_atual, usuario) == 0) {
            // Aloca memória para a senha e copia o conteúdo
            senha = malloc(strlen(senha_atual) + 1);
            if (senha) {
                strcpy(senha, senha_atual);
            } else {
                perror("Erro ao alocar memória");
            }
            break;
        }
    }

    fclose(arquivo);

    // Retorna a senha encontrada ou NULL se não encontrada
    return senha;
}

//função passada para a threads 
void *lida_requisicao(void *args){
    char mensagem[TAMANHO_MENSAGEM] = {0};
    struct pollfd fds[NUM_FDS]; // NUM_FDS é o número de descritores que você quer monitorar
    int timeout_poll = 3000; // Timeout em milissegundos -> 3+0s (247330)    
    char caminho_completo[1024];
    thread_args *argumento = (thread_args *)args;  // Converter ponteiro
    int novo_soquete = argumento->socket;
    char* registro = argumento->registro;  
    char* caminho = argumento->caminho;  
    // Configuração de descritores
    fds[0].fd = novo_soquete; // Descritor de arquivo (por exemplo, um socket)
    fds[0].events = POLLIN; // Eventos a serem monitorados, como POLLIN para leitura
    // Chamada da função poll
    int ret;
    int liberado;
    do {
        ret = poll(fds, NUM_FDS, timeout_poll);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) {
        perror("Erro no select");
        close(novo_soquete);
        contador--;
        //depois de tudo limpo desmuta para q outra thread faça o processamento
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
        exit(1);
    } else if (ret == 0) {
        printf("Timeout: Nenhuma requisição recebida dentro do tempo limite.\n");
        close(novo_soquete);
        contador--;
        //depois de tudo limpo desmuta para q outra thread faça o processamento
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    } else {
        // Preenchendo o buffer com zeros
        memset(mensagem, 0, TAMANHO_MENSAGEM);
        // Lê a requisição
        ssize_t bytes = read(novo_soquete, mensagem, TAMANHO_MENSAGEM);
        if (bytes < 0) {
            perror("Erro ao ler do soquete");
            close(novo_soquete);
            contador--;
            //depois de tudo limpo desmuta para q outra thread faça o processamento
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        } else if (bytes > 0){
            mensagem[bytes] = '\0';
            mensagem[bytes + 1] = '\0';
            //ativa o mutex para ler e responder a requisição dessa forma apenas uma thread por vez usa o parser e a lista ligada
            pthread_mutex_lock(&lock);
            // Passa o buffer lido do socket para o parser, indo apenas até os dois '\0'
            yy_scan_buffer(mensagem, bytes + 2);
            
            // Chama o parser
            yyparse();

            // Faz o registro da requisição
            registro_requisicao(registro);

            // Imprime na tela a requisição que foi enviada
            if (inicioCampo != NULL){
                struct noCampo* ini_lista_campo = inicioCampo;
                struct noCampo* atualCampo = inicioCampo;
                struct noValor* atualValor = atualCampo->listaValor;
                printf("%s %s %s\n", request->metodo, request->caminho, request->protocolo);
                printf("%s\n", host);
                while (atualCampo != NULL) {
                    printf("%s:", atualCampo->nomeCampo);
                    atualValor = atualCampo->listaValor;
                    printf("%s", atualValor->nomeValor);
                    atualValor = atualValor->proxValor;
                    while (atualValor != NULL) {
                        printf(",%s", atualValor->nomeValor);
                        atualValor = atualValor->proxValor;
                    }
                    printf("\n");
                    atualCampo = atualCampo->proxCampo;
                }
                printf("\n");
            }
            
            snprintf(caminho_completo, sizeof(caminho_completo), "%s%s", caminho, request->caminho);
            char *htaccess = NULL;
            htaccess = verificar_ultimo_htaccess(caminho_completo);
            if (htaccess != NULL){
                struct noCampo* noAtual = inicioCampo;
                char *user_senha = NULL;
                while (noAtual != NULL){
                    if (strcmp(noAtual->nomeCampo, "Authorization") == 0 || strcmp(noAtual->nomeCampo, "Au") == 0){
                        user_senha = noAtual->listaValor->nomeValor;
                        break;
                    }else{
                        noAtual = noAtual->proxCampo;
                    }
                }
                if (user_senha != NULL){
                    char *espaco = strchr(user_senha, ' ');
                    user_senha = espaco+1;
                    espaco = strchr(user_senha, ' ');
                    user_senha = espaco+1;
                    size_t output_length;
                    char *user_senha_decoded = decode_base64(user_senha, &output_length);
                    
                    char *user = strtok(user_senha_decoded, ":");
                    char *senha = strtok(NULL,":");
                    char *htpassword = pegar_htpassword(htaccess);
                    char *senha_guardada = obter_senha(htpassword, user);
                    if (senha_guardada == NULL){
                        liberado = 0;
                    }else{
                        char temp[512];
                        strcpy(temp, senha_guardada);
                    
                        printf("1: %s\n", user);
                        char *tipo_sal = strtok(temp, "$");
                        char *sal_real = strtok(NULL, "$");
                        char *senha_cripto = strtok(NULL, "$");
                        char sal[512];
                        snprintf(sal, 512, "$%s$%s$",tipo_sal,sal_real);
                        printf("2: %s\n", sal);
                        char* crypto = crypt(senha, sal);
                        printf("3: %s\n", crypto);
                        printf("4: %s\n", senha_guardada);
                        
                        if (strcmp(crypto, senha_guardada)==0){
                            liberado = 1;
                        }else{
                            liberado = 0;
                        }
                    }
                }else{
                    liberado = 0;
                }                
            }else{
                printf("htaccess null\n");
                liberado = 1;
            }
            if (liberado == 0){
                cabecalho(401, "Authorization Required", htaccess, 0, request->metodo, registro);
                write(novo_soquete, saida, strlen(saida));
                limpa_lista(inicioCampo);
                close(novo_soquete);
                contador--;
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }else{
                if (ta_dentro(caminho_completo)) {
                    // printf("Entrou aqui \n"); //debug
                    // Verifica o método e responde adequadamente
                    if (strcmp(request->metodo, "TRACE") == 0) {
                        retorna_trace(request->metodo, request->caminho, registro);
                        write(novo_soquete, saida, strlen(saida));
                    } else if (strcmp(request->metodo, "GET") == 0 || strcmp(request->metodo, "HEAD") == 0 || strcmp(request->metodo, "OPTIONS") == 0) {
                        // printf("antes de entrar no recurso\n"); //debug
                        acessar_recurso(request->metodo, caminho_completo, registro, novo_soquete);
                    } else {
                        cabecalho(501, "NOT IMPLEMENTED", NULL, 0, request->metodo, registro);
                        write(novo_soquete, saida, strlen(saida));
                    }
                } else {
                    // printf("Entrou lá \n"); //debug
                    cabecalho(HTTP_FORBIDDEN, "FORBIDDEN", NULL, 0, request->metodo, registro);
                    write(novo_soquete, saida, strlen(saida));
                }
            }
            // Fecha os sockets e limpa a lista ligada da memória
            limpa_lista(inicioCampo);
            close(novo_soquete);
            //libera o contador
            contador--;
            //depois de tudo limpo desmuta para q outra thread faça o processamento
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
            
        }
    }
}

int main(int argc, char **argv) {
    // Ativar depuração
    // yydebug = 1;
    int soquete, *novo_soquete;
    struct sockaddr_in endereco;
    socklen_t tamanho_endereco = sizeof(endereco);
    pthread_t thread_id;
    //inicializa o mutex
    pthread_mutex_init(&lock, NULL);

    if (argc != 5) {
        printf("Uso: ./servidor <Web Space> <arquivo registro> <porta> <limite_threads>\n");
        exit(1);
    }
    int porta = atoi(argv[3]);
    int MAX_THREADS = atoi(argv[4]);  // Limite de threads simultâneas

    // Cria o socket
    soquete = socket(AF_INET, SOCK_STREAM, 0);
    if (soquete < 0) {
        perror("Erro ao criar soquete");
        exit(1);
    }

    // Configura o endereço
    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(porta); // Porta definida pelo usuário
    inet_pton(AF_INET, "0.0.0.0", &endereco.sin_addr);

    // Liga o socket ao endereço
    if (bind(soquete, (struct sockaddr *)&endereco, sizeof(endereco)) < 0) {
        perror("Erro ao ligar soquete");
        close(soquete);
        exit(1);
    }

    // Escuta por conexões
    if (listen(soquete, 5) < 0) {
        perror("Erro ao escutar");
        close(soquete);
        exit(1);
    }

    while(1){
        // Aguardar até que o semáforo permita a criação de uma nova thread
        

        thread_args *threadArgs = malloc(sizeof(thread_args));
        if (threadArgs == NULL) {
            perror("Erro ao alocar memória");
            continue;
        }
        // Aceita conexão
        threadArgs->socket = accept(soquete, (struct sockaddr *)&endereco, &tamanho_endereco);
        if (threadArgs->socket < 0) {
            perror("Erro ao aceitar conexão");
            close(soquete);
            exit(1);
        }
        threadArgs->caminho = argv[1];
        threadArgs->registro = argv[2];
        // printf("contador pai: %d\n", contador); //debug de threads
        if (contador < MAX_THREADS){
            contador++;
            // Criar uma nova thread para lidar com a conexão
            if (pthread_create(&thread_id, NULL, lida_requisicao, threadArgs) != 0) {
                perror("Erro ao criar thread");
                close(threadArgs->socket);
                free(threadArgs);
            }

            // Desanexar a thread para que os recursos sejam liberados automaticamente após a execução
            pthread_detach(thread_id);
        }else{
            while (contador >= MAX_THREADS){};
        }
    }
    close(soquete);
    return 0;
}

