/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02
 *
 * RA: 726573
 * Aluno: Michele Argolo Carvalho
 * ========================================================================== */
 
/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
 
/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)
 
 
#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
 
 
/* Saídas do usuário */
#define OPCAO_INVALIDA                 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE         "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO         "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO                 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA             "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO                 "Arquivo vazio!"
#define INICIO_BUSCA                 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM                "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO             "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO                "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO                       "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                          "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP             "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS             "Busca por %s.\nNos percorridos:\n"
 
 
/* Registro do Produto */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;
 
 
/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
    char pk[TAM_PRIMARY_KEY];
    int rrn;
} Chave_ip;
 
/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
    char string[TAM_STRING_INDICE];
    char pk[TAM_PRIMARY_KEY];
} Chave_is;
 
/* Estrutura das Árvores-B */
typedef struct nodeip
{
    int num_chaves;        /* numero de chaves armazenadas*/
    Chave_ip *chave;        /* vetor das chaves e rrns [m-1]*/
    int *desc;    /* ponteiros para os descendentes, *desc[m]*/
    char folha;            /* flag folha da arvore*/
} node_Btree_ip;
 
typedef struct nodeis
{
    int num_chaves;        /* numero de chaves armazenadas*/
    Chave_is *chave;        /* vetor das chaves e rrns [m-1]*/
    int *desc;    /* ponteiros para os descendentes, *desc[m]*/
    char folha;            /* flag folha da arvore*/
} node_Btree_is;
 
typedef struct {
    int raiz;
} Indice;
 
/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice* iprimary, Indice* ibrand);
 
int alterar(Indice iprimary);
 
void buscar(Indice iprimary,Indice ibrand);
 
void listar(Indice iprimary,Indice ibrand);
 
/*******************************************************/
 
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();
 
/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);
 
/* (Re)faz o índice de jogos  */
void criar_ibrand(Indice *ibrand) ;
 
/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);
 
/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);
 
/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
node_Btree_ip* criar_no_Iprimary () ;
node_Btree_is* criar_no_ibrand () ;
 
node_Btree_is *read_btreeIs(int rrn);
 
/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);
 
/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/
 
 
/*Funções que auxiliam na criação da árvore B primária*/
int buscaArvoreIprimary(int raiz, char *chave);
void split_iprimary(int raiz, node_Btree_ip *noIprimary, Chave_ip **chave_ip, int *filho_direito);
node_Btree_ip *read_btreeIp(int rrn);
void liberNO_iprimary(node_Btree_ip *no);
void write_btreeIp(node_Btree_ip *salvar, int rrn);
void insere_iprimary(Indice *iprimary, char chave[], int rrn);
void insere_aux_Iprimary(int raiz, Chave_ip **chave_ip, int *filho_direito);
 
 
/*Funções que auxiliam na criação da árvore B secundária*/
void write_btreeIs(node_Btree_is *salvar, int rrn);
void inserir_registro_ibrand(Indice *ibrand, Produto novo, int i);
void insere_aux_ibrand(int raiz, Chave_is **chave_is, int *filho_direito);
void split_ibrand(int raiz, node_Btree_is *ibrand, Chave_is **chave_is, int *filho_direito);
void insere_ibrand(Indice *ibrand, char chave[], char str[]);
void liberNO_ibrand(node_Btree_is *no);
 
/*Funções auxiliares*/
void insere_arquivo(Produto novo);
int verificaDesconto(char desconto[]);
void inserir_registro_iprimary(Indice *iprimary, Produto novo);
void lista_codigo(int rrn, int nivel, int desc);
void lista_iproduct(int rrn, int nivel, int desc);
int busca_codigo(char chave[], int rrn, int achou, int tentativa);
int busca_iproduct(char str[], int rrn, int achou, int tentativa, int rrn_raiz);
/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto novo);
 
/* Exibe o jogo */
int exibir_registro(int rrn);
 
void ler_entrada(Produto *novo);
Produto recuperar_registro(int rrn);
 
// FÇ que gera chave do registro
void gerarChave(Produto *produto);
 
int main()
{
    char *p; /* # */
    /* Arquivo */
    int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
    scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
    if (carregarArquivo)
        nregistros = carregar_arquivo();
 
    scanf("%d %d%*c", &ordem_ip, &ordem_is);
 
    tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
    tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (    TAM_STRING_INDICE +9);
 
    /* Índice primário */
    Indice iprimary ;
    criar_iprimary(&iprimary);
 
    /* Índice secundário de nomes dos Produtos */
    Indice ibrand;
    criar_ibrand(&ibrand);
 
    /* Execução do programa */
    int opcao = 0;
    while(1)
    {
        scanf("%d%*c", &opcao);
        switch(opcao) {
            case 1: /* Cadastrar um novo Produto */
                cadastrar(&iprimary, &ibrand);
                break;
            case 2: /* Alterar o desconto de um Produto */
                printf(INICIO_ALTERACAO);
                if (alterar(iprimary))
                    printf(SUCESSO);
                else
                    printf(FALHA);
                break;
            case 3: /* Buscar um Produto */
                printf(INICIO_BUSCA);
                buscar(iprimary, ibrand);
                break;
            case 4: /* Listar todos os Produtos */
                printf(INICIO_LISTAGEM);
                listar(iprimary, ibrand);
                break;
            case 5: /* Imprimir o arquivo de dados */
                printf(INICIO_ARQUIVO);
                printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
                break;
            case 6: /* Imprime o Arquivo de Índice Primário*/
                printf(INICIO_INDICE_PRIMARIO);
                if(!*ARQUIVO_IP)
                    puts(ARQUIVO_VAZIO);
                else
                    for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
                    {
                        fwrite( p , 1 ,tamanho_registro_ip,stdout);
                        puts("");
                    }
                break;
            case 7: /* Imprime o Arquivo de Índice Secundário*/
                printf(INICIO_INDICE_SECUNDARIO);
                if(!*ARQUIVO_IS)
                    puts(ARQUIVO_VAZIO);
                else
                    for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
                    {
                        fwrite( p , 1 ,tamanho_registro_is,stdout);
                        puts("");
 
                    }
                break;
            case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
                return 0;
            default: /* exibe mensagem de erro */
                printf(OPCAO_INVALIDA);
                break;
        }
    }
    return -1;
}
 
/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */
 
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
    scanf("%[^\n]\n", ARQUIVO);
    return strlen(ARQUIVO) / TAM_REGISTRO;
}
void criar_iprimary(Indice *iprimary) {
    Produto j;
    iprimary->raiz = -1;
    char chave[TAM_PRIMARY_KEY];
    chave[0] = '\0';
    int rrn;
 
    if (nregistros) {        //cria o iprimary
        for (int i = 0; i < nregistros; i++) {
            j = recuperar_registro(i);
            strcpy(chave, j.pk);
            rrn = i;
            insere_iprimary(iprimary, chave, rrn);  // Insere no índice primário
        }
    } else
        iprimary->raiz = -1;
}
 
 
void criar_ibrand(Indice *ibrand){
    Produto j;
    char *percorreArquivo = ARQUIVO;
    int buscaPrimary = -1;
    ibrand->raiz = -1;
 
    if (nregistros) {        //cria o iprimary
        for (int i = 0; i < nregistros; i++) {
            sscanf(percorreArquivo, "%[^@]@", j.pk);
            j = recuperar_registro(i);
            inserir_registro_ibrand(ibrand, j, i);
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        }
    } else
        ibrand->raiz = -1;
}
 
/* Exibe o Produto */
int exibir_registro(int rrn)
{
    if(rrn < 0)
        return 0;
 
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
 
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    printf("%s\n", j.ano);
    sscanf(j.desconto,"%d",&desconto);
    sscanf(j.preco,"%f",&preco);
    preco = preco *  (100-desconto);
    preco = ((int) preco)/ (float) 100 ;
    printf("%07.2f\n",  preco);
    strcpy(categorias, j.categoria);
 
    for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
        printf("%s ", cat);
    printf("\n");
 
    return 1;
}
 
void lista_codigo(int rrn, int nivel, int desc){
    //pre-ordem(raiz, esq, dir)
    int tentativa = 0;
    int i, j=0;
    node_Btree_ip *noIprimary = NULL;
 
    // lê o nó da árvore
    noIprimary = read_btreeIp(rrn);
 
    i = 0;
    if(noIprimary) {
        while (i < noIprimary->num_chaves) {
            if (tentativa)
                printf(", ");
            if (!i)
                printf("%d - ", nivel);
            printf("%s", noIprimary->chave[i].pk);
            i++;
            tentativa = 1;
 
            if(i == noIprimary->num_chaves) {
                printf("\n");
                nivel++;
            }
        }
 
 
        while(noIprimary->folha != 'F' && j<=noIprimary->num_chaves){
            lista_codigo(noIprimary->desc[j], nivel, 0);
            j++;
        }
 
        liberNO_iprimary(noIprimary);
 
    }
 
}
 
void lista_iproduct(int rrn, int nivel, int desc){
    int tentativa = 0;
    char *p;
    char marca[TAM_MARCA];
    char nome[TAM_NOME];
    char str_marca[TAM_MARCA+TAM_NOME+1];
    str_marca[0] = '\0';
    int i, j=0;
    node_Btree_is *noIprimary = NULL;
 
    // lê o nó da árvore
    noIprimary = read_btreeIs(rrn);
 
    i = 0;
    if(noIprimary) {
        while(j<noIprimary->num_chaves) {
            if (noIprimary->folha != 'F') {
                lista_iproduct(noIprimary->desc[j], nivel + 1, 0);
            }
 
            strcpy(str_marca, noIprimary->chave[j].string);
            p = strtok(str_marca, "$");
            strcpy(marca, p);
            p = strtok(NULL,"#");
            strcpy(nome,p);
            printf("%s", marca);
            for(int k = strlen(marca); k<50; k++)
                printf("-");
            printf(" ");
            printf("%s", nome);
            for(int k = strlen(nome); k<50; k++)
                printf("-");
            printf("\n");
            j++;
        }
 
        if(noIprimary->folha != 'F')
            lista_iproduct(noIprimary->desc[j], nivel + 1, 0);
 
        // DUVIDA NISSO
        liberNO_ibrand(noIprimary);
    }
}
 
void listar(Indice iprimary, Indice ibrand){
    int op;
    scanf("%d%*c", &op);
 
    switch(op){
        case 1:   // Buscar por código
            if(iprimary.raiz != -1) {
                lista_codigo(iprimary.raiz, 1, 0);
            } else
                printf(REGISTRO_N_ENCONTRADO);
            break;
 
        case 2:
            if(ibrand.raiz != -1) {
                lista_iproduct(ibrand.raiz, 1, 0);
            } else
                printf(REGISTRO_N_ENCONTRADO);
            break;
 
    }
}
 
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.pk,p);
    p = strtok(NULL,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    //gerarChave(&j);
    return j;
}
 
// FÇ que gera chave do registro
void gerarChave(Produto *produto){
    produto->pk[0] = toupper(produto->nome[0]);
    produto->pk[1] = toupper(produto->nome[1]);
    produto->pk[2] = toupper(produto->marca[0]);
    produto->pk[3] = toupper(produto->marca[1]);
    produto->pk[4] = produto->data[0];
    produto->pk[5] = produto->data[1];
    produto->pk[6] = produto->data[3];
    produto->pk[7] = produto->data[4];
    produto->pk[8] = produto->ano[0];
    produto->pk[9] = produto->ano[1];
    produto->pk[10] = '\0';
}
 
node_Btree_is *read_btreeIs(int rrn){
    char temp[tamanho_registro_is+1], str_marca[102];
    int chave_tam = ordem_is - 1;
    char *p;
    int j=0, i=0;
 
    char num_chaves[4], chave[11], rrn_pk[5], desc[4];
    strncpy(temp, ARQUIVO_IS + ((rrn)*tamanho_registro_is), tamanho_registro_is);
    temp[tamanho_registro_is] = '\0'; //temp tem o registro inteiro correspondente ao rrn de parametro
    //   printf("\n%s\n", temp);
    // Aloca o no
    node_Btree_is *no = criar_no_ibrand();
 
    //Tenho que ler todas as variaveis separadamente e colocar em no declarado acima
    memcpy (num_chaves, &temp[j], 3);
    num_chaves[3] = '\0';
    j=j+3;
 
    no->num_chaves = atoi(num_chaves);
 
    for(i=0; i<atoi(num_chaves); i++){
        memcpy(chave, &temp[j], 10);
        chave[10] = '\0';
        j = j + 10;
        strcpy(no->chave[i].pk, chave);   // copia chave - ok
 
        memcpy(str_marca, &temp[j], 101);
        str_marca[101] = '\0';
        j = j + 101;
        strcpy(no->chave[i].string, str_marca);
        p = strtok(no->chave[i].string, "#");
    }
 
    while(i<ordem_is-1){
        j=j+111;
        i++;
    }
 
    no->folha = temp[j];
    j++;
 
    if(no->folha == 'F'){
        for(int i=0; i<ordem_is; i++) {
            if (temp[j] == '*') {
                no->desc[i] = -1;
            }
        }
    }
    else {
        for (i = 0; i < ordem_is; i++) {
            if (temp[j] == '*') {
                no->desc[i] = -1;
            } else {
                memcpy(desc, &temp[j], 3);
                desc[3] = '\0';
                no->desc[i] = atoi(desc);
            }
            j = j + 3;
        }
    }
 
 
    return no;
}
node_Btree_ip *read_btreeIp(int rrn){  // Essa função lê o nó da árvore binária
    char temp[tamanho_registro_ip+1];
    int chave_tam = ordem_ip - 1;
    int j=0, i=0;
 
    char num_chaves[4], chave[11], rrn_pk[5], desc[4];
    strncpy(temp, ARQUIVO_IP + ((rrn)*tamanho_registro_ip), tamanho_registro_ip);
    temp[tamanho_registro_ip] = '\0'; //temp tem o registro inteiro correspondente ao rrn de parametro
 
    // Aloca o no
    node_Btree_ip *no = criar_no_Iprimary();
 
    //Tenho que ler todas as variaveis separadamente e colocar em no declarado acima
    memcpy (num_chaves, &temp[j], 3);
    num_chaves[3] = '\0';
    j=j+3;
 
    no->num_chaves = atoi(num_chaves);
 
    for(i=0; i<atoi(num_chaves); i++){
        memcpy(chave, &temp[j], 10);
        chave[10] = '\0';
        j = j + 10;
        strcpy(no->chave[i].pk, chave);
 
        memcpy(rrn_pk, &temp[j], 4);
        rrn_pk[4] = '\0';
        j = j + 4;
        no->chave[i].rrn = atoi(rrn_pk);
    }
 
    while(i<ordem_ip-1){
        j=j+14;
        i++;
    }
    no->folha = temp[j];
    j++;
 
    if(no->folha == 'F'){
        for(int i=0; i<ordem_ip; i++) {
            if (temp[j] == '*') {
                no->desc[i] = -1;
            }
        }
    }
    else {
        for (i = 0; i < ordem_ip; i++) {
            if (temp[j] == '*') {
                no->desc[i] = -1;
            } else {
                memcpy(desc, &temp[j], 3);
                desc[3] = '\0';
                no->desc[i] = atoi(desc);
            }
            j = j + 3;
        }
    }
 
    return no;
 
}
 
int busca_codigo(char chave[], int rrn, int achou, int tentativa){
    int arvore = 0;
    int i, j=0;
    int tenta = 0;
    node_Btree_ip *noIprimary = NULL;
 
    noIprimary = read_btreeIp(rrn);
    i = 0;
 
    if(noIprimary) {
        while (i < noIprimary->num_chaves && !achou) {
            if (strcmp(noIprimary->chave[i].pk, chave) == 0) {    // Achou a chave
                if (tentativa)
                    printf(", ");
                for (int k = i; k < noIprimary->num_chaves; k++) {
                    if (tenta)
                        printf(", ");
                    printf("%s", noIprimary->chave[k].pk);
                    tenta = 1;
                }
                tentativa = 1;
                achou = 1;
                printf("\n\n");
                exibir_registro(noIprimary->chave[i].rrn);
                return 1;
            } else {
                if (strcmp(noIprimary->chave[i].pk, chave) < 0)
                    arvore = i + 1;
                if (tentativa)
                    printf(", ");
                tentativa = 1;
                printf("%s", noIprimary->chave[i].pk);
            }
            i++;
            if(i == noIprimary->num_chaves)
                printf("\n");
        }
 
        if (!achou) {
            if (noIprimary->folha != 'F' && arvore < noIprimary->num_chaves + 1) {
                busca_codigo(chave, noIprimary->desc[arvore], achou, 0);
            }
        }
    }
    if(noIprimary) {
        if (noIprimary->folha == 'F' && !achou) {
            printf("\n");
            printf(REGISTRO_N_ENCONTRADO);
            return 0;
        }
    }
 
    liberNO_iprimary(noIprimary);
}
 
int busca_iproduct(char str[], int rrn, int achou, int tentativa, int rrn_raiz) {
    int arvore = 0;
    int ip = 0;
    int i, j = 0;
    int tenta = 0;
    node_Btree_is *noIprimary = NULL;
 
    noIprimary = read_btreeIs(rrn);
    i = 0;
 
    if (noIprimary) {
        while (i < noIprimary->num_chaves && !achou) {
            if (strcmp(noIprimary->chave[i].string, str) == 0) {    // Achou a chave
                if (tentativa)
                    printf(", ");
                for (int k = i; k < noIprimary->num_chaves; k++) {
                    if (tenta)
                        printf(", ");
                    printf("%s", noIprimary->chave[k].string);
                    tenta = 1;
                }
                tentativa = 1;
                achou = 1;
                printf("\n\n");
                ip = buscaArvoreIprimary(rrn_raiz, noIprimary->chave[i].pk);
                exibir_registro(ip);
                return 1;
            } else {
                if (strcmp(noIprimary->chave[i].string, str) < 0)
                    arvore = i + 1;
                if (tentativa)
                    printf(", ");
                tentativa = 1;
                printf("%s", noIprimary->chave[i].string);
            }
            i++;
            if(i == noIprimary->num_chaves)
                printf("\n");
        }
        if (!achou) {
            if (noIprimary->folha != 'F' && arvore < noIprimary->num_chaves + 1) {
                busca_iproduct(str, noIprimary->desc[arvore], achou, 0, rrn_raiz);
            }
        }
    }
    if(noIprimary) {
        if (noIprimary->folha == 'F' && !achou) {
            printf("\n");
            printf(REGISTRO_N_ENCONTRADO);
            return 0;
        }
    }
 
    liberNO_ibrand(noIprimary);
 
}
 
void buscar(Indice iprimary, Indice ibrand){
    char chave[TAM_PRIMARY_KEY] = "\0", marca[TAM_MARCA] = "\0", produto[TAM_NOME] = "\0";
    char str[TAM_MARCA+TAM_NOME+1];
    str[0] = '\0';
    int op;
    scanf("%d%*c", &op);
 
    switch(op){
        case 1:   // Buscar por código
            scanf("%11[^\n]%*c", chave);
            printf(NOS_PERCORRIDOS_IP, chave);
            if(iprimary.raiz != -1) {
                busca_codigo(chave, iprimary.raiz, 0, 0);
            } else
                printf(REGISTRO_N_ENCONTRADO);
            break;
 
        case 2:  // Busca por nome da marca seguido por nome do produto/modelo
            scanf("%51[^\n]%*c", marca);
            scanf("%51[^\n]%*c", produto);
            sprintf(str, "%s$%s", marca, produto);
            printf(NOS_PERCORRIDOS_IS, str);
            if(ibrand.raiz != -1) {
                busca_iproduct(str, ibrand.raiz, 0, 0, iprimary.raiz);
            } else
                printf(REGISTRO_N_ENCONTRADO);
            break;
 
    }
 
}
// Função que escreve no arquivo de índice secundário as informações
void write_btreeIs(node_Btree_is *salvar, int rrn){
    char *registroIs = ARQUIVO_IS + ((rrn)*tamanho_registro_is);
    char str[tamanho_registro_is+1];
    str[0] = '\0';
    char num_chaves[4];
    int i=0, j=0;
 
    sprintf(num_chaves, "%03d", salvar->num_chaves);
    num_chaves[3] = '\0';
    strcpy(str, num_chaves);
 
    while(i<salvar->num_chaves){
        strcat(str, salvar->chave[i].pk);
        strcat(str, salvar->chave[i].string); // Completa com # os caractere não preenchidos
        for(j = strlen(salvar->chave[i].string); j<101; j++)
            strcat(str, "#");
        i++;
    }
 
    while(i<ordem_is-1){  // preenche os espaços não usados
        for(j=0; j < 111; j++)
            strcat(str, "#");
        i++;
    }
 
    char folha[2];
    sprintf(folha, "%c", salvar->folha);
    folha[1] = '\0';
    strcat(str, folha);
 
    for(i=0; i<ordem_is; i++){
        if(salvar->desc[i] != -1 && i<=salvar->num_chaves){   // Se for diferente de -1 tem filhos
            sprintf(str + strlen(str), "%03d", salvar->desc[i]);
        } else {
            int j=0;
            while(j<3){
                sprintf(str + strlen(str), "%c", '*');
                j++;
            }
        }
    }
    // Agora str tem a string inteira a ser inserida no arquivo
    for(i = 0; i < tamanho_registro_is; i++)
        registroIs[i] = str[i];
}
// Função que escreve no arquivo de índice primário as informações
void write_btreeIp(node_Btree_ip *salvar, int rrn){
    char *registroIp = ARQUIVO_IP + ((rrn)*tamanho_registro_ip);
    char str[tamanho_registro_ip+1], rrn_chave[5];
    str[0] = '\0';
    char num_chaves[4], chave[11], desc[4];
    int i=0;
 
    // Leva até onde deve ser escrito no arquivo de índice primario
    // Passa primeiro para uma string cpntendo as informações necessarias
    sprintf(num_chaves, "%03d", salvar->num_chaves);
    num_chaves[3] = '\0';
    strcpy(str, num_chaves);
 
    while(i<salvar->num_chaves){
        strcat(str, salvar->chave[i].pk);
        sprintf(rrn_chave, "%04d", salvar->chave[i].rrn);
        rrn_chave[4] = '\0';
        strcat(str, rrn_chave);
        i++;
    }
 
    int numeroChaves = salvar->num_chaves;
    while(numeroChaves < (ordem_ip-1)){
        for(i=0; i < 14; i++)
            strcat(str, "#");
        numeroChaves++;
    }
 
    char folha[2];
    sprintf(folha, "%c", salvar->folha);
    folha[1] = '\0';
    strcat(str, folha);
 
    for(i=0; i<ordem_ip; i++){
        if(salvar->desc[i] != -1 && i<=salvar->num_chaves){   // Se for diferente de -1 tem filhos
            sprintf(str + strlen(str), "%03d", salvar->desc[i]);
        } else {
            int j=0;
            while(j<3){
                sprintf(str + strlen(str), "%c", '*');
                j++;
            }
        }
    }
 
    // Agora str tem a string inteira a ser inserida no arquivo
    for(i = 0; i < tamanho_registro_ip; i++)
        registroIp[i] = str[i];
 
}
 
void cadastrar(Indice* iprimary, Indice* ibrand){
    int buscaPrimary = -1;
    Produto novo;
 
    ler_entrada(&novo);  // Lê as entradas específicas e coloca as informações na strct novo
 
    if (nregistros != 0) {
        buscaPrimary = buscaArvoreIprimary(iprimary->raiz, novo.pk);
    }
 
    // Antes de inserir nos índices confere se a chave já existe = busca binária no iprimary
    if(buscaPrimary == -1) {
        insere_arquivo(novo);     // Insere no arquivo de dados
        inserir_registro_indices(iprimary, ibrand, novo);
        nregistros = nregistros + 1;
 
        return;
    } else {
        printf(ERRO_PK_REPETIDA, novo.pk);
    }
 
}
 
void inserir_registro_ibrand(Indice *ibrand, Produto novo, int i){
    char chave[TAM_PRIMARY_KEY];
    chave[0] = '\0';
    char str[TAM_MARCA+TAM_NOME];
    str[0] = '\0';
 
    strcpy(chave, novo.pk);
    strcat(str, novo.marca);
    strcat(str, "$");
    strcat(str, novo.nome);
    insere_ibrand(ibrand, chave, str);
}
 
 
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto novo){
    char chave[TAM_PRIMARY_KEY];
    int rrn;
    chave[0] = '\0';
    char str[100];
    str[0] = '\0';
 
    // Insere no índice primário
    strcpy(chave, novo.pk);
    rrn = nregistros;
    insere_iprimary(iprimary, chave, rrn);
 
    // Insere no índice secundário
    strcat(str, novo.marca);
    strcat(str, "$");
    strcat(str, novo.nome);
    insere_ibrand(ibrand, chave, str);
}
 
void insere_iprimary(Indice *iprimary, char chave[], int rrn) {
    Chave_ip chave_ip;
    node_Btree_ip *noIprimary;
    int filho_direito = -1;
    strcpy(chave_ip.pk, chave);
    chave_ip.rrn = rrn;
    Chave_ip *chave_promovida = &chave_ip;
 
    // Se não há nenhum registro, então é o primeiro registro a ser inserido
    if(iprimary->raiz == -1) {
        noIprimary = criar_no_Iprimary(); // Cria o nó
        noIprimary->num_chaves = 1;        // Primeiro a ser inserido tem apenas uma chave
        noIprimary->folha = 'F';      // Primeiro nó é uma folha
        // Insere as informações passadas para a função
        strcpy(noIprimary->chave[0].pk, chave);
        noIprimary->chave[0].rrn = rrn;
        iprimary->raiz = 0;
        write_btreeIp(noIprimary, nregistrosip);
        nregistrosip++;
        liberNO_iprimary(noIprimary);
    } else {
        insere_aux_Iprimary(iprimary->raiz, &chave_promovida, &filho_direito);  // Não é o primeiro nó a ser inserido
        // Filho esquerdo é retornado como o iprimary->raiz e filho direito como o filho_direito p o split
        if(chave_promovida) { // Chave promovida não nula, então temos que criar outro no
            noIprimary = criar_no_Iprimary();
            noIprimary->num_chaves = 1;
            noIprimary->folha = 'N';
            strcpy(noIprimary->chave[0].pk, chave_promovida->pk);
            noIprimary->chave[0].rrn = chave_promovida->rrn;
            noIprimary->desc[0] = iprimary->raiz;
            iprimary->raiz = nregistrosip;
            noIprimary->desc[1] = filho_direito;
            write_btreeIp(noIprimary, nregistrosip);
            nregistrosip++;
        }
    }
}
 
void insere_aux_Iprimary(int raiz, Chave_ip **chave_ip, int *filho_direito) {
    node_Btree_ip *noIprimary = read_btreeIp(raiz);
    int i = noIprimary->num_chaves-1;
    int tamanhoChaves = ordem_ip-1;
 
    if(noIprimary->folha == 'F') {
        if(noIprimary->num_chaves < tamanhoChaves) { // O nó não está cheio
            while(i >= 0 && (strcmp(noIprimary->chave[i].pk, (*chave_ip)->pk) > 0)) {
                strcpy(noIprimary->chave[i+1].pk, noIprimary->chave[i].pk);
                noIprimary->chave[i+1].rrn = noIprimary->chave[i].rrn;
                i--;
            }
            strcpy(noIprimary->chave[i+1].pk, (*chave_ip)->pk);
            noIprimary->chave[i+1].rrn = (*chave_ip)->rrn;
            noIprimary->num_chaves = noIprimary->num_chaves + 1;
            *filho_direito = -1;
            *chave_ip = NULL;
            write_btreeIp(noIprimary, raiz);
            return;
        } else {
            *filho_direito = -1;
            split_iprimary(raiz, noIprimary, chave_ip, filho_direito);
            //liberNO_iprimary(noIprimary);
            return;
        }
    } else { // O nó está cheio
        while((i>=0) && (strcmp((*chave_ip)->pk, noIprimary->chave[i].pk) < 0))
            i--;
 
        i++;
        insere_aux_Iprimary(noIprimary->desc[i], chave_ip, filho_direito);
        if(*chave_ip) {
            if(noIprimary->num_chaves < tamanhoChaves){
                i = noIprimary->num_chaves-1;
                while((i >= 0) && (strcmp(noIprimary->chave[i].pk, (*chave_ip)->pk) > 0)) {
                    strcpy(noIprimary->chave[i+1].pk, noIprimary->chave[i].pk);
                    noIprimary->chave[i+1].rrn = noIprimary->chave[i].rrn;
                    noIprimary->desc[i+2] = noIprimary->desc[i+1];
                    i--;
                }
                strcpy(noIprimary->chave[i+1].pk, (*chave_ip)->pk);
                noIprimary->chave[i+1].rrn = (*chave_ip)->rrn;
                noIprimary->num_chaves = noIprimary->num_chaves + 1;
                noIprimary->desc[i+2] = *filho_direito;
                *filho_direito = -1;
                *chave_ip = NULL;
                write_btreeIp(noIprimary, raiz);
                // liberNO_iprimary(noIprimary);
            } else {
                split_iprimary(raiz, noIprimary, chave_ip, filho_direito);
                return;
            }
        } else {
            *filho_direito = -1;
            *chave_ip = NULL;
        }
    }
    liberNO_iprimary(noIprimary);
}
 
 
void liberNO_iprimary(node_Btree_ip *no){
    free(no->desc);
    free(no->chave);
    free(no);
    //no->desc = NULL;
    //no->chave = NULL;
    no = NULL;
}
 
 
void liberNO_ibrand(node_Btree_is *no){
    free(no->desc);
    free(no->chave);
    free(no);
    //no->desc = NULL;
    //no->chave = NULL;
    no = NULL;
}
 
node_Btree_ip* criar_no_Iprimary () {
    int i, tamanhoChave = ordem_ip-1;
 
    node_Btree_ip *no = (node_Btree_ip *)malloc(sizeof(node_Btree_ip));
    no->num_chaves = 0;
    no->chave = (Chave_ip *)malloc(sizeof(Chave_ip) * (tamanhoChave));
    no->desc = (int *) malloc(sizeof(int) * ordem_ip);
    for(i=0; i < tamanhoChave; i++)
        no->chave[i].rrn = -1;
 
    for(i=0; i < ordem_ip; i++)
        no->desc[i] = -1;
 
    return no;
}
 
void insere_ibrand(Indice *ibrand, char chave[], char str[]) {
    node_Btree_is *noIbrand = NULL;
    int filho_direito = -1;
    Chave_is chave_is;
    strcpy(chave_is.pk, chave);
    strcpy(chave_is.string, str);
    Chave_is *chame_promovida = &chave_is;
 
    if(ibrand->raiz == -1) {
        noIbrand = criar_no_ibrand();
        strcpy(noIbrand->chave[0].pk, chave);
        strcpy(noIbrand->chave[0].string, str);
        noIbrand->num_chaves = 1;
        noIbrand->folha = 'F';
        ibrand->raiz = 0;
        write_btreeIs(noIbrand, nregistrosis);
        nregistrosis++;
        liberNO_ibrand(noIbrand);
    }else{
        insere_aux_ibrand(ibrand->raiz, &chame_promovida, &filho_direito);
        if(chame_promovida) {
            noIbrand = criar_no_ibrand();
            noIbrand->num_chaves = 1;
            noIbrand->folha = 'N';
            strcpy(noIbrand->chave[0].pk, chame_promovida->pk);
            strcpy(noIbrand->chave[0].string, chame_promovida->string);;
            noIbrand->desc[0] = ibrand->raiz;
            ibrand->raiz = nregistrosis;
            noIbrand->desc[1] = filho_direito;
            write_btreeIs(noIbrand, nregistrosis);
            nregistrosis++;
            //liberNO_ibrand(noIbrand);
        }
    }
}
 
 
void insere_aux_ibrand(int raiz, Chave_is **chave_is, int *filho_direito){
    node_Btree_is *noIbrand = read_btreeIs(raiz);
    int i = noIbrand->num_chaves-1;
    int tamanhoChave = ordem_is-1;
 
    if(noIbrand->folha == 'F'){
        if(noIbrand->num_chaves < tamanhoChave){
            while((i >= 0) && (strcmp(noIbrand->chave[i].string, (*chave_is)->string) > 0)){
                strcpy(noIbrand->chave[i+1].pk, noIbrand->chave[i].pk);
                strcpy(noIbrand->chave[i+1].string, noIbrand->chave[i].string);
                i--;
            }
            strcpy(noIbrand->chave[i+1].pk, (*chave_is)->pk);
            strcpy(noIbrand->chave[i+1].string, (*chave_is)->string);
            noIbrand->num_chaves = noIbrand->num_chaves + 1;
            *filho_direito = -1;
            *chave_is = NULL;
            write_btreeIs(noIbrand, raiz);
            return;
 
        } else {
            *filho_direito = -1;
            split_ibrand(raiz, noIbrand, chave_is, filho_direito);
            return;
        }
    }else{
        while(i>=0 && (strcmp((*chave_is)->string, noIbrand->chave[i].string) < 0))
            i--;
 
        i++;
        insere_aux_ibrand(noIbrand->desc[i], chave_is, filho_direito);
        if(*chave_is) {
            if(noIbrand->num_chaves < tamanhoChave) {
                i = noIbrand->num_chaves-1;
                while(i >= 0 && (strcmp(noIbrand->chave[i].string, (*chave_is)->string) > 0)) {
                    strcpy(noIbrand->chave[i+1].pk, noIbrand->chave[i].pk);
                    strcpy(noIbrand->chave[i+1].string, noIbrand->chave[i].string);
                    noIbrand->desc[i+2] = noIbrand->desc[i+1];
                    i--;
                }
                strcpy(noIbrand->chave[i+1].pk, (*chave_is)->pk);
                strcpy(noIbrand->chave[i+1].string, (*chave_is)->string);
                noIbrand->num_chaves = noIbrand->num_chaves + 1;
                noIbrand->desc[i+2] = *filho_direito;
                *filho_direito = -1;
                *chave_is = NULL;
                write_btreeIs(noIbrand, raiz);
                // liberNO_iprimary(noIprimary);
            } else {
                split_ibrand(raiz, noIbrand, chave_is, filho_direito);
                return;
            }
        } else {
            *filho_direito = -1;
            *chave_is = NULL;
        }
    }
    liberNO_ibrand(noIbrand);
}
 
void split_iprimary(int raiz, node_Btree_ip *noIprimary, Chave_ip **chave_ip, int *filho_direito) {
    int tamanhoChave = ordem_ip-1;
    int metade = ordem_ip/2;
    node_Btree_ip *novo = criar_no_Iprimary();
    novo->num_chaves = tamanhoChave/2;
    novo->folha = noIprimary->folha;
    int i = noIprimary->num_chaves-1;
    int j = 0, chave_alocada = 0;
    int tamanhoChaveNovo = novo->num_chaves-1;
 
    for(j = tamanhoChaveNovo; j >= 0; j--){
        if(chave_alocada == 0 && (strcmp((*chave_ip)->pk, noIprimary->chave[i].pk) > 0)){
            chave_alocada = 1;
            strcpy(novo->chave[j].pk, (*chave_ip)->pk);
            novo->chave[j].rrn = (*chave_ip)->rrn;
            novo->desc[j+1] = *filho_direito;
        } else {
            strcpy(novo->chave[j].pk, noIprimary->chave[i].pk);
            novo->chave[j].rrn = noIprimary->chave[i].rrn;
            novo->desc[j+1] = noIprimary->desc[i+1];
            i--;
        }
    }
    if(chave_alocada == 0){
        while(i >= 0 && (strcmp(noIprimary->chave[i].pk, (*chave_ip)->pk) > 0)) {
            strcpy(noIprimary->chave[i+1].pk, noIprimary->chave[i].pk);
            noIprimary->chave[i+1].rrn = noIprimary->chave[i].rrn;
            noIprimary->desc[i+2] = noIprimary->desc[i+1];
            i--;
        }
        noIprimary->desc[i+2] = *filho_direito;
        strcpy(noIprimary->chave[i+1].pk, (*chave_ip)->pk);
        noIprimary->chave[i+1].rrn = (*chave_ip)->rrn;
    }
    strcpy((*chave_ip)->pk, noIprimary->chave[metade].pk);
    (*chave_ip)->rrn = noIprimary->chave[metade].rrn;
    novo->desc[0] = noIprimary->desc[metade+1];
    noIprimary->num_chaves = metade;
    *filho_direito = nregistrosip;
    write_btreeIp(noIprimary, raiz);
    write_btreeIp(novo, nregistrosip);
    nregistrosip++;
    liberNO_iprimary(noIprimary);
    liberNO_iprimary(novo);
 
}
 
void split_ibrand(int raiz, node_Btree_is *ibrand, Chave_is **chave_is, int *filho_direito) {
    int tamanhoChave = ordem_is-1;
    int metade = ordem_is/2;
    node_Btree_is *novo = criar_no_ibrand();
    novo->num_chaves = tamanhoChave/2;
    novo->folha = ibrand->folha;
    int i = ibrand->num_chaves-1;
    int j = 0, chave_alocada = 0;
    int tamanhoChaveNovo = novo->num_chaves-1;
 
    for(j = tamanhoChaveNovo; j >= 0; j--) {
        if(chave_alocada == 0 && (strcmp((*chave_is)->string, ibrand->chave[i].string) > 0)){
            chave_alocada = 1;
            strcpy(novo->chave[j].pk, (*chave_is)->pk);
            strcpy(novo->chave[j].string, (*chave_is)->string);
            novo->desc[j+1] = *filho_direito;
        } else {
            strcpy(novo->chave[j].pk, ibrand->chave[i].pk);
            strcpy(novo->chave[j].string, ibrand->chave[i].string);
            novo->desc[j+1] = ibrand->desc[i+1];
            i--;
        }
    }
    if(chave_alocada == 0) {
        while(i >= 0 && (strcmp(ibrand->chave[i].string, (*chave_is)->string) > 0)) {
            strcpy(ibrand->chave[i+1].pk, ibrand->chave[i].pk);
            strcpy(ibrand->chave[i+1].string, ibrand->chave[i].string);
            ibrand->desc[i+2] = ibrand->desc[i+1];
            i--;
        }
        ibrand->desc[i+2] = *filho_direito;
        strcpy(ibrand->chave[i+1].pk, (*chave_is)->pk);
        strcpy(ibrand->chave[i+1].string, (*chave_is)->string);
    }
 
    strcpy((*chave_is)->pk, ibrand->chave[metade].pk);
    strcpy((*chave_is)->string, ibrand->chave[metade].string);
    novo->desc[0] = ibrand->desc[metade+1];
    ibrand->num_chaves = metade;
    *filho_direito = nregistrosis;
    write_btreeIs(ibrand, raiz);
    write_btreeIs(novo, nregistrosis);
    nregistrosis++;
    //liberNO_iprimary(noIprimary);
    //liberNO_iprimary(novo);
}
 
 
node_Btree_is* criar_no_ibrand () {
    int i, tamanhoChave = ordem_is-1;
 
    node_Btree_is *no = (node_Btree_is *)malloc(sizeof(node_Btree_is));
    no->num_chaves = 0;
    no->chave = (Chave_is *)malloc(sizeof(Chave_is)*(tamanhoChave));
    no->desc = (int *) malloc(sizeof(int)*ordem_is);
 
    for(i=0; i < ordem_is; i++)
        no->desc[i] = -1;
 
    return no;
}
 
void insere_arquivo(Produto novo){
    char *registro = ARQUIVO + nregistros * TAM_REGISTRO;
    char str[193] = "\0";
    int i = 0;
    // Insere em str todas as informações digitadas separadas pelo delimitador "@"
 
    sprintf(str, "%s", novo.pk);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.nome);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.marca);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.data);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.ano);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.preco);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.desconto);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.categoria);
    sprintf(str + strlen(str), "%s", "@");
 
    for(i =  strlen(str); i < TAM_REGISTRO; i++)
        str[i] = '#';
 
    str[i] = '\0';
 
    sprintf(registro, "%s" , str);
}
 
int buscaArvoreIprimary(int rrn, char chave[]){
    int i=0;
    // Lê o nó de acordo com o rrn passado
    node_Btree_ip *noIprimary = read_btreeIp(rrn);
 
    // Procura no nó enquanto a chave do nó for menor que a chave em questão
    while(i <= noIprimary->num_chaves - 1 && (strcmp(chave, noIprimary->chave[i].pk) > 0)){
        i++;
    }
 
    // Se a chave for igual, achou e retorna rrn
    if (i <= (noIprimary->num_chaves - 1) && (strcmp(chave, noIprimary->chave[i].pk) == 0)){
        return noIprimary->chave[i].rrn;
    }
 
    // Chegou na folha, a chave não encontra-se na árvore
    if(noIprimary->folha == 'F')
        return -1;
    else
        return buscaArvoreIprimary(noIprimary->desc[i], chave);  // Vai para o descendente que pode estar
 
}
 
void ler_entrada(Produto *novo){
    // Lê do usuários as informações de um registro
 
    scanf("%51[^\n]%*c", novo->nome);
    scanf("%51[^\n]%*c", novo->marca);
    scanf("%11[^\n]%*c", novo->data);
    scanf("%3[^\n]%*c", novo->ano);
    scanf("%8[^\n]%*c", novo->preco);
    scanf("%4[^\n]%*c", novo->desconto);
    scanf("%51[^\n]%*c", novo->categoria);
 
    //Gera chave
    novo->pk[0] = toupper(novo->nome[0]);
    novo->pk[1] = toupper(novo->nome[1]);
    novo->pk[2] = toupper(novo->marca[0]);
    novo->pk[3] = toupper(novo->marca[1]);
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    novo->pk[8] = novo->ano[0];
    novo->pk[9] = novo->ano[1];
    novo->pk[10] = '\0';
 
}
 
int alterar(Indice iprimary){
    char *pArquivo;
    char chave[TAM_PRIMARY_KEY];
    char desconto[TAM_DESCONTO];
    Produto recupera;
    int tamanho, tentativa = 0;
    float precoNovo;
 
    scanf("%11[^\n]%*c", chave);
    // Busca a chave primária digitada no arq. índice primário
    int buscaArquivo = buscaArvoreIprimary(iprimary.raiz, chave);
 
    if(buscaArquivo == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    do {
        scanf("%[^\n]%*c", desconto);
    } while (verificaDesconto(desconto) == 0);
 
    pArquivo = ARQUIVO + (TAM_REGISTRO * buscaArquivo);
    recupera = recuperar_registro(buscaArquivo);
 
    pArquivo = pArquivo + strlen(recupera.pk) + strlen(recupera.nome) + strlen(recupera.marca) + strlen(recupera.data) + strlen(recupera.ano) + strlen(recupera.preco) + 6;
    *pArquivo = desconto[0];
    pArquivo++;
    *pArquivo = desconto[1];
    pArquivo++;
    *pArquivo = desconto[2];
 
    return 1;
}
 
int verificaDesconto(char desconto[]){
    if(((atof(desconto) > 100) || atof(desconto) < 0) || strlen(desconto) != 3){
        printf(CAMPO_INVALIDO);
        return 0;
    }
 
    else{
        if(strlen(desconto) == 3){
            for(int i = 0; i<3; i++){
                if(desconto[i] < 48 || desconto[i] > 57) {
                    printf(CAMPO_INVALIDO);
                    return 0;
                }
            }
        }
    }
    return 1;
}