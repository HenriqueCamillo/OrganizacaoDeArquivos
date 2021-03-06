// O trabalho anterior escolhido foi o de Henrique Matarazo Camillo 
//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943
//  Nome: Gyovana Mayara Moriyama
//  NUSP: 10734387

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "funcionalidades.h"
#include "lista.h"
#include "escreverNaTela.h"
#include "merge_sort.h"

// Tamanho do buffer e tamanho da string de busca do índice
#define BUFFER_TAM          300
#define INDICE_BUSCA_TAM    120
#define INDICE_TAM          128

// Struct que mostra a página de disco atual e quantos bytes já foram lidos dela
typedef struct _paginadDeDisco {
    int bytes;
    int pagina;
} sPaginaDeDisco;

// Struct que representa o cabeçalho do arquivo
typedef struct _cabecalho {
    char status;
    long int topoLista;
    char tagCampo1;
    char* desCampo1;
    char tagCampo2;
    char* desCampo2;
    char tagCampo3;
    char* desCampo3;
    char tagCampo4;
    char* desCampo4;
    char tagCampo5;
    char* desCampo5;
} sCabecalho;

// Struct que representa um registro
typedef struct _registro {
    char removido;
    int tamanho;
    long int encadeamentoLista;
    int idServidor;
    double salarioServidor;
    char* telefoneServidor;
    char* nomeServidor;
    char* cargoServidor;
} sRegistro;

// Struct que representa o cabeçalho de um arquivo de índices
typedef struct _cabecalhoIndices {
    char status;
    int nroRegistros;
} sCabecalhoDeIndices;

// Struct que representa índice de um registro 
typedef struct _indice {
    char* chaveBusca;
    long int byteOffset;
} sIndice;

/**
 * @brief  Cria uma página de disco com os valores zerados
 * @retval Retorna um ponteiro apra a página de disco criada
 */
sPaginaDeDisco* criarPaginaDeDisco() {
    sPaginaDeDisco* pagina = malloc(sizeof(sPaginaDeDisco));
    pagina->bytes = 0;
    pagina->pagina = 0;
}

/**
 * @brief  Libera memória alocada pela variável página de disco
 * @param  pagina: Variável da página de disco que será desalocada
 * @retval None
 */
void liberarPaginaDeDisco(sPaginaDeDisco* pagina) {
    free(pagina);
}

/**
 * @brief  Cria um registro vazio alocando espaço na memória para ele.
 * @retval Retorna o registro criado
 */
sRegistro* criarRegistro() {
    // Aloca memória
    sRegistro* registro = malloc(sizeof(sRegistro));
    registro->telefoneServidor = malloc(sizeof(char) * (TELEFONE_TAM + 1));
    registro->nomeServidor = malloc(sizeof(char) * STRING_TAM_MAX);
    registro->cargoServidor = malloc(sizeof(char) * STRING_TAM_MAX);

    return registro;
}

/**
 * @brief  Calcula o tamanho de um registro (ignorando o campo "removido" e o indicador de tamanho do registro)
 * @param  registro: Registro cujo tamanho irá ser calculado
 * @retval Retonra o tamanho do registro passado por parâmetro
 */
int tamanhoRegistro(sRegistro* registro) {
    // O tamanho do registro sempre terá no mínimo 34 bytes. 
    // Pode aumentar esse valor se os campos nome e cargo do servidor existirem.
    int tamanhoDoRegistro = 34;

    int tamanhoDaString;

    // Verifica se o nome e cargo não são nulos.
    // Para cada um não nulo, adiciona o tamanho da string (com \0), o tamanho da tag, e o tamanho do indicador de tamanho do campo à conta
    tamanhoDaString = strlen(registro->nomeServidor);
    if (tamanhoDaString != 0) {
        tamanhoDoRegistro += tamanhoDaString + 2 * sizeof(char) + sizeof(int);
    }

    tamanhoDaString = strlen(registro->cargoServidor);
    if (tamanhoDaString != 0) {
        tamanhoDoRegistro += tamanhoDaString + 2 * sizeof(char) + sizeof(int);
    }

    return tamanhoDoRegistro;
}

/**
 * @brief  Preenche um registro com os valores passados por parâmetro
 * @param  registro: registro a ser preenchido
 * @param  id: novo id do registro
 * @param  salario: novo salário do registro
 * @param  telefone: novo telefone do registr
 * @param  nome: novo nome do registro
 * @param  cargo: novo cargo do registro
 * @retval None
 */
void preencherRegistro(sRegistro* registro, char* id, char* salario, char* telefone, char* nome, char* cargo) {
    // Atribui o valor do id servidor, que não pode ser nulo
    registro->idServidor = atoi(id);
    
    // Atribui o valor dos outros campos, verificando se são nulos ou não
    if (!strcmp(salario, "NULO")) {
        registro->salarioServidor = -1;
    } else {
        registro->salarioServidor = atof(salario);
    }

    if (!strcmp(telefone, "NULO")) {
        registro->telefoneServidor[0] = '\0';
    } else {
        registro->telefoneServidor = telefone;
    }

    if (!strcmp(nome, "NULO")) {
        registro->nomeServidor[0] = '\0';
    } else {
        registro->nomeServidor = nome;
    }

    if (!strcmp(cargo, "NULO")) {
        registro->cargoServidor[0] = '\0';
    } else {
        registro->cargoServidor = cargo;
    }

    // Calcula tamanho do registro
    registro->tamanho = tamanhoRegistro(registro);

    registro->removido = '-';
    registro->encadeamentoLista = -1;
}


/**
 * @brief  Libera memória que foi alocada para o registro
 * @param  registro: registro a ser liberado
 * @retval None
 */
void liberarRegistro(sRegistro* registro) {
    free(registro->cargoServidor);
    free(registro->nomeServidor);
    free(registro->telefoneServidor);
    free(registro);
}

/**
 * @brief  Cria um cabeçalho vazio alocando espaço na memória para ele.
 * @retval Retorna o cabeçalho criado
 */
sCabecalho* criarCabecalho() {
    sCabecalho* cabecalho = malloc(sizeof(sCabecalho));
    cabecalho->desCampo1 = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);
    cabecalho->desCampo2 = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);
    cabecalho->desCampo3 = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);
    cabecalho->desCampo4 = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);
    cabecalho->desCampo5 = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);

    return cabecalho;
}

/**
 * @brief  Libera memória que foi alocada para o cabeçalho
 * @param  cabecalho: cabeçalho a ser liberado
 * @retval None
 */
void liberarCabecalho(sCabecalho* cabecalho) {
    free(cabecalho->desCampo1);
    free(cabecalho->desCampo2);
    free(cabecalho->desCampo3);
    free(cabecalho->desCampo4);
    free(cabecalho->desCampo5);
    free(cabecalho);
}

/**
 * @brief  Cria um registro de índice, alocando memória para ele
 * @retval Retorna o registro de índice criado
 */
sIndice* criarIndice() {
    sIndice* indice = malloc(sizeof(sIndice));
    indice->chaveBusca = malloc(sizeof(char) * INDICE_BUSCA_TAM);
}

/**
 * @brief  Libera memória alocada por um registro de índice
 * @param  indiceRegistro: Índice do registro a ser liberado
 * @retval None
 */
void liberarIndice(sIndice* indice) {
    free(indice->chaveBusca);
    free(indice);
}

/**
 * @brief  Cria um cabeçalho de índice, alocando memória para ele e incializando as variáveis com 0
 * @retval 
 */
sCabecalhoDeIndices* criarCabecalhoDeIndices() {
    sCabecalhoDeIndices* cabecalho = malloc(sizeof(sCabecalhoDeIndices));
    cabecalho->nroRegistros = 0;
    cabecalho->status = '0';
}

/**
 * @brief  Libera memória alocada por um cabeçalho de índices
 * @param  cabecalho: Cabeçalho a ser liberado
 * @retval None
 */
void liberarCabecalhoDeIndices(sCabecalhoDeIndices* cabecalho) {
    free(cabecalho);
}

/**
 * @brief  Lê o cabeçalho de um arquivo .csv e armazena numa struct.
 * @param  cabecalho: Ponteiro para cabeçalho que se quer preencher.
 * @param  csv: Arquivo csv de onde se lerá o cabeçalho
 * @retval Retorna verdadeiro se leu o cabeçalho com sucesso, e falso caso contrário 
 */
bool lerCabecalho(sCabecalho* cabecalho, FILE* csv) {
    if (fscanf(csv, "%[^,],", cabecalho->desCampo1) == EOF) {
        return false;
    }
    fscanf(csv, "%[^,],", cabecalho->desCampo2);
    fscanf(csv, "%[^,],", cabecalho->desCampo3);
    fscanf(csv, "%[^,],", cabecalho->desCampo4);
    fscanf(csv, "%[^\n],", cabecalho->desCampo5);

    // Inicializa as variáveis com os valores preestabelecidos
    cabecalho->tagCampo1 = 'i';
    cabecalho->tagCampo2 = 's';
    cabecalho->tagCampo3 = 't';
    cabecalho->tagCampo4 = 'n';
    cabecalho->tagCampo5 = 'c';
    
    // O arquivo no momento ainda está inconsistente, por isso 0
    cabecalho->status = '0';

    // Começa com -1 pois ainda não se tem o topo da lista
    cabecalho->topoLista = -1;

    return true;
} 

/**
 * @brief  Escreve num arquivo binário o caebeçalho e preenche o resto da página de disco com '@'
 * @param  cabecalho: Ponteiro para o cabeçalho que irá ser escrito no arquivo
 * @param  bin: Arquivo binário (já aberto) onde será escrito o cabeçalho
 * @param  paginadeDisco: Ponteiro para struct que faz o controle de páginas de disco
 * @retval None
 */
void escreverCabecalho(sCabecalho* cabecalho, FILE* bin, sPaginaDeDisco* paginadeDisco) {
    // Espaço livre após preenchido cada campo
    int espacoLivre;

    // Arrobas que preencherão o espaços não utilizados
    char* arrobas = malloc(sizeof(char) *  CAMPO_CABECALHO_TAM);
    for (int i = 0; i < CAMPO_CABECALHO_TAM; i++) {
        arrobas[i] = '@';
    }

    // Escreve o status e o topo da lista no arquivo binário
    fwrite(&cabecalho->status, sizeof(char), 1, bin);
    fwrite(&cabecalho->topoLista, sizeof(long int), 1, bin);
    paginadeDisco->bytes += sizeof(char) + sizeof(long int);

    // Escreve no arquivo o campo (com tag antes), calcula o espaço que sobrou e preenche com '@'
    // Repete para os 5 campos.
    fwrite(&cabecalho->tagCampo1, sizeof(char), 1, bin);
    fwrite(cabecalho->desCampo1, sizeof(char), 1 + strlen(cabecalho->desCampo1), bin);
    espacoLivre = CAMPO_CABECALHO_TAM - (1 + strlen(cabecalho->desCampo1));
    fwrite(arrobas, sizeof(char), espacoLivre, bin);
    paginadeDisco->bytes += (CAMPO_CABECALHO_TAM + 1) * sizeof(char);

    fwrite(&cabecalho->tagCampo2, sizeof(char), 1, bin);
    fwrite(cabecalho->desCampo2, sizeof(char), 1 + strlen(cabecalho->desCampo2), bin);
    espacoLivre = CAMPO_CABECALHO_TAM - (1 + strlen(cabecalho->desCampo2));
    fwrite(arrobas, sizeof(char), espacoLivre, bin);
    paginadeDisco->bytes += (CAMPO_CABECALHO_TAM + 1) * sizeof(char);
    
    fwrite(&cabecalho->tagCampo3, sizeof(char), 1, bin);
    fwrite(cabecalho->desCampo3, sizeof(char), 1 + strlen(cabecalho->desCampo3), bin);
    espacoLivre = CAMPO_CABECALHO_TAM - (1 + strlen(cabecalho->desCampo3));
    fwrite(arrobas, sizeof(char), espacoLivre, bin);
    paginadeDisco->bytes += (CAMPO_CABECALHO_TAM + 1) * sizeof(char);

    fwrite(&cabecalho->tagCampo4, sizeof(char), 1, bin);
    fwrite(cabecalho->desCampo4, sizeof(char), 1 + strlen(cabecalho->desCampo4), bin);
    espacoLivre = CAMPO_CABECALHO_TAM - (1 + strlen(cabecalho->desCampo4));
    fwrite(arrobas, sizeof(char), espacoLivre, bin);
    paginadeDisco->bytes += (CAMPO_CABECALHO_TAM + 1) * sizeof(char);

    fwrite(&cabecalho->tagCampo5, sizeof(char), 1, bin);
    fwrite(cabecalho->desCampo5, sizeof(char), 1 + strlen(cabecalho->desCampo5), bin);
    espacoLivre = CAMPO_CABECALHO_TAM - (1 + strlen(cabecalho->desCampo5));
    fwrite(arrobas, sizeof(char), espacoLivre, bin);
    paginadeDisco->bytes += (CAMPO_CABECALHO_TAM + 1) * sizeof(char);

    // Preenche o resto da página de disco com '@'  
    for (int i = paginadeDisco->bytes; i < PAGINA_DE_DISCO_TAM; i++) {
        fwrite(arrobas, sizeof(char), 1, bin);
    }

    // Passa para a outra página de disco, já que a a nterior foi preenchida
    paginadeDisco->pagina++;
    paginadeDisco->bytes = 0;

    // Libera memória alocada
    free(arrobas);
}


/**
 * @brief  Lê um registro de um arquivo .csv e armazena numa struct.
 * @param  registro: Ponteiro para registro que se quer preencher.
 * @param  csv: Arquivo csv de onde se lerá o registro
 * @retval Retorna true se o registro for válido, e false se o registro for inválido (ou se chegou ao fim do arquivo)
 */
bool lerRegistro(sRegistro* registro, FILE* csv) {
    // Lê o ID e salva o valor de retorno do fscanf, para verificar se o valor é nulo ou se chegou ao fim do arquivo
    int idRetorno = fscanf(csv, "%d,", &registro->idServidor); 

    if (idRetorno == EOF) {
        return false;
    } else if (idRetorno == 0) {
        printf("Registro inválido");

        // Termina de ler o registro inválido, lendo até o final da linha
        char* buffer = malloc(sizeof(char) * BUFFER_TAM);
        fgets(buffer, BUFFER_TAM, csv);
        free(buffer);

        return false;
    }

    // Lẽ o salário, o teleone, o nome, e o cargo do servidor.
    // Caso o campo seja vazio, ignora a vírgula que não conseguiu ser ignorada no fscanf com um fseek.
    // Para strings, caso o campo seja nulo, coloca um \0 no começo da string para gerar uma string vazia.

    // Atribui valor inválido se o valor não é fornecido.
    if (fscanf(csv, "%lf,", &registro->salarioServidor) == 0) {
        fseek(csv, 1, SEEK_CUR);
        registro->salarioServidor = -1;
    } 

    if (fscanf(csv, "%[^,],", registro->telefoneServidor) == 0) {
        registro->telefoneServidor[0] = '\0';
        fseek(csv, 1, SEEK_CUR);
    }

    if (fscanf(csv, "%[^,],", registro->nomeServidor) == 0) {
        registro->nomeServidor[0] = '\0';
        fseek(csv, 1, SEEK_CUR);
    }

    if (fscanf(csv, "%[^\n]'\n'", registro->cargoServidor) == 0) {
        registro->cargoServidor[0] = '\0';
        fseek(csv, 1, SEEK_CUR);
    }

    // Calcula o tamanho do registro
    registro->tamanho = tamanhoRegistro(registro);

    registro->removido = '-';
    registro->encadeamentoLista = -1;

    return true;
}

/**
 * @brief  Escreve um registro no arquivo binário. Verifica se está no final da página de dsico, mudando o tamanho
 * do registro anterior, e verifica campos nulos, aplicando tratamento na hora de escrever no arquivo. 
 * @param  registro: Registro que seá escrito no arquivo binário
 * @param  bin: Arquivo binário onde o registro será escrito
 * @param  paginaDeDisco: Página de disco que contém as informações sobre quantos bytes já foram escritos
 * @param  tamanhoDoRegistroAnterior: Tamanho do registro anterior
 * @retval Retorna o tamanho do registro escrito
 */
int escreverRegistro(sRegistro* registro, FILE* bin, sPaginaDeDisco* paginaDeDisco, int tamanhoDoRegistroAnterior) {
    // Variável para preencher espaços vazios
    char* arrobas = malloc(sizeof(char) * CAMPO_CABECALHO_TAM);
    for (int i = 0; i < CAMPO_CABECALHO_TAM; i++) {
        arrobas[i] = '@';
    }

    // Verifica se o registro cabe na página de disco
    // Foram adicionados o tamanho do campo "removido" e do indicador de tamanho do registro ao tamanho do registro.
    if (registro->tamanho + sizeof(char) + sizeof(int) + paginaDeDisco->bytes > PAGINA_DE_DISCO_TAM) { 

        // Escreve arrobas até que preencha a página, e conta quantos foram escritos
        int i;
        for (i = 0; paginaDeDisco->bytes < PAGINA_DE_DISCO_TAM; i++) {
            fwrite(arrobas, sizeof(char), 1, bin);
            paginaDeDisco->bytes += sizeof(char);            
        }

        // Atualiza o tamanho do registro anterior
        tamanhoDoRegistroAnterior += i * sizeof(char);
        fseek(bin, -(tamanhoDoRegistroAnterior + sizeof(int)), SEEK_CUR);     // + sizeof(int) para voltar até o indicador de tamanho do registro
        fwrite(&tamanhoDoRegistroAnterior, sizeof(int), 1, bin);
        fseek(bin, 0, SEEK_END);

        // Inicia uma nova página
        paginaDeDisco->pagina++;
        paginaDeDisco->bytes = 0;
    }

    // Para campos inválidos, coloca-se um \0 antes dos @ para diferenciar um campo nulo de lixo
    arrobas[0] = '\0';

    // Escreve no arquivo o campo "removido", indicando que o registro não foi removido
    fwrite(&registro->removido, sizeof(char), 1, bin);
    paginaDeDisco->bytes += sizeof(char);

    // Escreve no arquivo o tamanho do registro
    fwrite(&registro->tamanho, sizeof(int), 1, bin);
    paginaDeDisco->bytes += sizeof(int);

    // Escreve no aqruivo o encadeamento da lista, que será nulo, já que estamos escrevendo o arquivo do zero
    fwrite(&registro->encadeamentoLista, sizeof(long int), 1, bin);
    paginaDeDisco->bytes += sizeof(long int);

    // Escreve no arquivo o id, o salário e o telefone (campos de tamanho fixo)
    fwrite(&registro->idServidor, sizeof(int), 1, bin);
    paginaDeDisco->bytes += sizeof(int);

    fwrite(&registro->salarioServidor, sizeof(double), 1, bin);
    paginaDeDisco->bytes += sizeof(double);

    if (strlen(registro->telefoneServidor) != 0) {
        fwrite(registro->telefoneServidor, sizeof(char), strlen(registro->telefoneServidor), bin);
    } else {
        fwrite(arrobas, sizeof(char), TELEFONE_TAM, bin);
    }
    paginaDeDisco->bytes += TELEFONE_TAM;

    // Escreve no arquivo o nome e o cargo, com suas tags e indicadores de tamanho (c   ampos de tamanho variável)
    int tamanho;
    char tag;

    if (strlen(registro->nomeServidor) != 0) {
        tag = 'n';
        tamanho = strlen(registro->nomeServidor) + 2 * sizeof(char);    // string + \0 + tag
        fwrite(&tamanho, sizeof(int), 1, bin);
        fwrite(&tag, sizeof(char), 1, bin);
        fwrite(registro->nomeServidor, sizeof(char), tamanho - 1, bin);
        paginaDeDisco->bytes += sizeof(int) + tamanho * sizeof(char);
    }

    if (strlen(registro->cargoServidor) != 0) {
        tag = 'c';
        tamanho = strlen(registro->cargoServidor) + 2 * sizeof(char);
        fwrite(&tamanho, sizeof(int), 1, bin);
        fwrite(&tag, sizeof(char), 1, bin);
        fwrite(registro->cargoServidor, sizeof(char), tamanho - 1, bin);
        paginaDeDisco->bytes += sizeof(int) + tamanho * sizeof(char);
    }

    // Libera memória alocada e retorna
    free(arrobas);
    return registro->tamanho;
}

/**
 * @brief  Lê registros a partir de um arquivo e os escreve em um arquivo binário.
 * @param  nomeDoArquivo: Nome do arquivo .csv de onde será lido os registros
 * @retval None
 */
void csvParaBin(char* nomeDoArquivo) {
    // Abre o arquivo csv e verifica se não ocorreu nenhum erro
    FILE* csv = fopen(nomeDoArquivo, "r");
    if (csv == NULL) {
        printf("Falha no carregamento do arquivo.");
        return;
    }

    // Cria um cabeçalho e lê suas informações a partir do arquivo .csv
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalho(cabecalho, csv)) {
        printf("Falha no carregamento do arquivo.");
        liberarCabecalho(cabecalho);
        fclose(csv);
        return;
    }

    // Cria um arquivo binário
    FILE* bin = fopen("arquivoTrab1.bin", "wb");

    // Cria uma página de disco
    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.bytes = 0;
    paginaDeDisco.pagina = 1;

    // Escreve as informações lidas do .csv em um arquivo binario
    escreverCabecalho(cabecalho, bin, &paginaDeDisco);

    // Cria um registro vazio, lê do csv e escreve no binário enquanto o programa ainda consegue ler registros válidos
    sRegistro* registro = criarRegistro();
    int tamanhoDoRegistro = 0;
    while (lerRegistro(registro, csv)) {
        tamanhoDoRegistro = escreverRegistro(registro, bin, &paginaDeDisco, tamanhoDoRegistro);
    }

    // Muda o status do arquivo para consistente e escreve no arquivo
    cabecalho->status = '1';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);


    // Imprime na tela o nome do arquivo
    printf("arquivoTrab1.bin");

    // Fecha os arquivos e libera memória alocada
    fclose(bin);
    fclose(csv);

    liberarCabecalho(cabecalho);
    liberarRegistro(registro);
}

//!###############################################################################################################################

/**
 * @brief  Zera todos os valores de um registro
 * @param  registro: Registro a ser zerado  
 * @retval None
 */
void zerarRegistro(sRegistro* registro) {
    registro->removido = '\0';
    registro->tamanho = 0;
    registro->encadeamentoLista = -1;
    registro->idServidor = -1;
    registro->salarioServidor = -1;
    registro->telefoneServidor[0] = '\0';
    registro->nomeServidor[0] = '\0';
    registro->cargoServidor[0] = '\0';
}

/**
 * @brief  Lê o próximo valor de um arquivo, verifica se é lixo ou não, e volta o curso para onde estava.
 * @param  bin: Arquivo de onde se lerá o próximo byte.
 * @retval Retorna verdadeiro caso seja detectado lixo, e falso caso nã seja
 */
bool detectarLixo(FILE* bin) {
    // Lê o próxmo char
    char c;
    fread(&c, sizeof(char), 1, bin);

    // Volta o cursor
    fseek(bin, -sizeof(char), SEEK_CUR);

    if (c == '@') {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief  Lê todos os bytes contendo '@' de um arquivo, posicionando o cursor ao final dos bytes que representam lixo
 * @param  bin: Arquivo de onde o lixo será lido
 * @retval Retorna a quantidade de bytes lidos
 */
int lerLixo(FILE* bin) {
    char c;
    int bytes;  // Quantidade de bytes de lixo lidos

    // Lê até achar algo que não é lixo
    fread(&c, sizeof(char), 1, bin);
    for (bytes = 0; c == '@'; bytes++) {
        fread(&c, sizeof(char), 1, bin);
    }

    // Volta o cursor, pois ele chegou a ler algo que não era lixo
    fseek(bin, -sizeof(char), SEEK_CUR);

    return bytes;
}

/**
 * @brief  A partir de um arquivo binário, lê um registro e salva numa struct, 
 *        controlando a qunatidade de bytes lidos na página de disco
 * @param  registro: Ponteiro para variável que conterá o registro lido
 * @param  bin: Arquivo binário
 * @param  paginaDeDisco: Variável que fará o controle das páginas de disco
 * @param  posicaoInicial: Ponteiro para variável onde se guardará a posição inicial do registro recuperado
 * @retval Retorna verdadeiro se o registro foi lido com sucesso, e falso se chegou o fim do arquivo
 */
bool lerRegistroBin(sRegistro* registro, FILE* bin, sPaginaDeDisco* paginaDeDisco, long int* posicaoInicial) {
    int bytesLidos = 0;
    zerarRegistro(registro);    
    *posicaoInicial = ftell(bin);

    // Verifica se chegou no final do arquivo, lendo o campo "removido"
    if (fread(&registro->removido, sizeof(char), 1, bin) != 1 || registro->removido == '\0') {
        return false;
    }
    paginaDeDisco->bytes += sizeof(char);

    // Lê o tamanho do registro
    fread(&registro->tamanho, sizeof(int), 1, bin);
    paginaDeDisco->bytes += sizeof(int);

    // Caso seja um registro removido, apenas pula os dados desse registro e recomeça a função
    if (registro->removido == '*') {
        fseek(bin, registro->tamanho, SEEK_CUR);
        
        paginaDeDisco->bytes += registro->tamanho;
        if (paginaDeDisco->bytes >= PAGINA_DE_DISCO_TAM) {
            paginaDeDisco->bytes = 0;
            paginaDeDisco->pagina++;
        }

        return lerRegistroBin(registro, bin, paginaDeDisco, posicaoInicial);
    }

    // Lê os campos de tamanho fixo
    fread(&registro->encadeamentoLista, sizeof(long int), 1, bin);
    bytesLidos += sizeof(long int);

    fread(&registro->idServidor, sizeof(int), 1, bin);
    bytesLidos += sizeof(int);

    fread(&registro->salarioServidor, sizeof(double), 1, bin);
    bytesLidos += sizeof(double);

    fread(registro->telefoneServidor, sizeof(char), TELEFONE_TAM, bin);
    bytesLidos += sizeof(char) * TELEFONE_TAM;
    registro->telefoneServidor[TELEFONE_TAM] = '\0';          // Coloca um '\0' no final da string do telefone

    // Se leu o arquivo completo, coloca o nome e o cargo do servidor como valores nulos e retorna
    if (bytesLidos == registro->tamanho) {
        registro->nomeServidor[0] = '\0';
        registro->cargoServidor[0] = '\0';
    } else {
        // Ou ele tem lixo ou ele tem mais campos. Se há lixo, lê todos os bytes de lixo, senão lê o campo;
        if (detectarLixo(bin)) {
            bytesLidos += lerLixo(bin);
        } else {
            int tamanho;
            char tag;

            // Lê o tamanho do próximo campo e a tag
            fread(&tamanho, sizeof(int), 1, bin);
            fread(&tag, sizeof(char), 1, bin);
            bytesLidos += sizeof(int) + sizeof(char);

            // Se a tag for 'c', o campo nome é nulo, então lê o campo cargo e coloca \0 no nome. Caso contrário, lê o nome
            if (tag == 'c') {
                // Nome é nulo
                registro->nomeServidor[0] = '\0';

                // Lê o cargo
                fread(registro->cargoServidor, sizeof(char), tamanho - 1, bin);      // (tamanho - 1) para ignorar a tag que já foi lida
                bytesLidos += sizeof(char) * (tamanho - 1);

                // Se tiver lixo depois do último campo, lê
                if (detectarLixo(bin)) {
                    bytesLidos += lerLixo(bin);
                }
            } else if (tag == 'n') {
                // Lê o nome 
                fread(registro->nomeServidor, sizeof(char), tamanho - 1, bin);      // (tamanho - 1) para ignorar a tag que já foi lida
                bytesLidos += sizeof(char) * (tamanho - 1);
                
                // Se tiver lixo, lê, senão verifica se já chegou ao fim do registro, lendo o último campo se ainda não chegou ao fim do registro
                if (detectarLixo(bin)) {
                    registro->cargoServidor[0] = '\0';

                    bytesLidos += lerLixo(bin);
                } else if (bytesLidos < registro->tamanho) {
                    fread(&tamanho, sizeof(int), 1, bin);
                    fread(&tag, sizeof(char), 1, bin);
                    bytesLidos += sizeof(int) + sizeof(char);

                    // Lê o cargo
                    fread(registro->cargoServidor, sizeof(char), tamanho - 1, bin);      // (tamanho - 1) para ignorar a tag que já foi lida
                    bytesLidos += sizeof(char) * (tamanho - 1);

                    // Verifica se há lixo no final
                    if (detectarLixo(bin)) {
                        bytesLidos += lerLixo(bin);
                    }
                }
            } 
        }
    }

    // Faz a contagem de dados lidos e verifica se chegou ao final da página de dados
    paginaDeDisco->bytes += bytesLidos;
    if (paginaDeDisco->bytes >= PAGINA_DE_DISCO_TAM) {
        paginaDeDisco->bytes = 0;
        paginaDeDisco->pagina++;
    }

    return true;
}

/**
 * @brief  Imprime um registro passado por parâmetro
 * @param  registro: Registro a ser imprimido
 * @retval None
 */
void imprimirRegistro(sRegistro* registro) {
    int tam;

    // Imprime o id, já que ele com certeza existe
    printf ("%d", registro->idServidor);

    // Imprime o salário, se válido. Senão imprime 8 espaços
    if (registro->salarioServidor != -1) {
        printf(" %.2lf", registro->salarioServidor);
    } else {
        printf("         ");
    }

    // Imprime o telefone se válido. Senão imprime 14 espaços
    if (strlen(registro->telefoneServidor) != 0) {
        printf(" %s", registro->telefoneServidor);
    } else {
        printf("               ");
    }

    // Imprime o nome e o cargo se válidos, precedidos do tamanho do campo
    tam = strlen(registro->nomeServidor);
    if (tam != 0) {
        printf(" %d", tam);
        printf(" %s", registro->nomeServidor);
    }

    tam = strlen(registro->cargoServidor);
    if (tam != 0) {
        printf(" %d", tam);
        printf(" %s", registro->cargoServidor);
    }

    printf("\n");
}

/**
 * @brief  Imprime todos os registros contidos em um arquivo binário e mostra a quatidade de páginas de disco acessadas.
 * @param  nomeDoArquivo: Nome do arquivo binário que contém os dados que serão imprimidos
 * @retval None
 */
void imprimirBin(char* nomeDoArquivo) {
    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        // printf("Falha no carregamento do arquivo.\n");
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /// Verifica consistência do arquivo
    char status;
    fread(&status, sizeof(char), 1, bin);

    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }

    // Ignora o cabeçalho, colocando o cursor no primeiro byte do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Cria uma variável para manter controle sobre as páginas de disco
    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.bytes = 0;
    paginaDeDisco.pagina = 2;

    sRegistro* registro = criarRegistro();
    // Lê e imprime o primeiro registro, já fazendo a verificação se há registros
    long int posicaoInicial;
    if (!lerRegistroBin(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        printf("Registro inexistente.\n");
        liberarRegistro(registro);
        fclose(bin);
        return;
    }
    imprimirRegistro(registro);
    zerarRegistro(registro);

    // Lê e imprime todos os outros registros
    while (lerRegistroBin(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        imprimirRegistro(registro);
    }

    printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);

    // Fecha o arquivo e libera memória alocada
    fclose(bin);    
    liberarRegistro(registro);
}

//!###############################################################################################################################

/**
 * @brief  Lê o cabeçalho de um arquivo binário e armazena em uma variável
 * @param  cabecalho: Variável onde cabeçalho ficará armazenado
 * @param  bin: Arquivo binário de onde será lido o cabeçalho
 * @retval Retorna verdadeiro se o cabeçalho foi lido com sucesso, e falso caso contrário
 */
bool lerCabecalhoBin(sCabecalho* cabecalho, FILE* bin) {
    // Lê o status checando se já chegou ao fim do arquio
    if (fread(&cabecalho->status, sizeof(char), 1, bin) == 0) {
        return false;
    }

    // Verifica consistência do arquivo
    if (cabecalho->status == '0') {
        return false;
    }

    fread(&cabecalho->topoLista, sizeof(long int), 1, bin);

    // Buffer usado para ler os campos, tratando o caso de a nome do campo não ter \0 em seu final
    char* buffer = malloc(sizeof(char) * (CAMPO_CABECALHO_TAM + 1));

    // Lê tags e campos do cabeçalho. Primeiro coloca-se o que foi lido num buffer, e depois lê-se até achar um lixo (ou o \0)
    fread(&cabecalho->tagCampo1, sizeof(char), 1, bin);

    fgets(buffer, sizeof(char) * (CAMPO_CABECALHO_TAM + 1), bin);
    sscanf(buffer, "%[^@]", cabecalho->desCampo1);

    fread(&cabecalho->tagCampo2, sizeof(char), 1, bin);

    fgets(buffer, sizeof(char) * (CAMPO_CABECALHO_TAM + 1), bin);
    sscanf(buffer, "%[^@]", cabecalho->desCampo2);

    fread(&cabecalho->tagCampo3, sizeof(char), 1, bin);

    fgets(buffer, sizeof(char) * (CAMPO_CABECALHO_TAM + 1), bin);
    sscanf(buffer, "%[^@]", cabecalho->desCampo3);

    fread(&cabecalho->tagCampo4, sizeof(char), 1, bin);

    fgets(buffer, sizeof(char) * (CAMPO_CABECALHO_TAM + 1), bin);
    sscanf(buffer, "%[^@]", cabecalho->desCampo4);


    fread(&cabecalho->tagCampo5, sizeof(char), 1, bin);

    fgets(buffer, sizeof(char) * (CAMPO_CABECALHO_TAM + 1), bin);
    sscanf(buffer, "%[^@]", cabecalho->desCampo5);

    free(buffer);
    return true;
}

/**
 * @brief  Compara o campo com so campos do cebçalho e retorna o index de qual campo for
 * @param  cabecalho: Cabeçalho que contém os campos
 * @param  campo: Campo que se deseja saber o index
 * @retval Retorna o index do campo em relação ao cabeçalho
 */
int campoIndex(sCabecalho* cabecalho, char* campo) {
    if (campo[0] == cabecalho->tagCampo1) {
        return 1;
    } else if (campo[0] == cabecalho->tagCampo2) {
        return 2;
    } else if (campo[0] == cabecalho->tagCampo3) {
        return 3;
    } else if (campo[0] == cabecalho->tagCampo4) {
        return 4;
    } else if (campo[0] == cabecalho->tagCampo5) {
        return 5;
    } else {
        // Retorna -1 em caso de campo inválido
        return -1;
    }
}

/**
 * @brief  Imprime registro indicando os campos do cabeçalho
 * @param  registro: Registro a ser impresso
 * @param  cabecalho: Cabeçalho
 * @retval None
 */
void imprimirRegistroComCabecalho(sRegistro* registro, sCabecalho* cabecalho) {
    // Imprime o id, já que ele com certeza existe
    printf ("%s: %d\n", cabecalho->desCampo1, registro->idServidor);

    // Imprime os outros campos. Se o campo for inválido diz que o valor não foi declarado.
    printf("%s: ", cabecalho->desCampo2);
    if (registro->salarioServidor != -1) {
        printf("%.2lf\n", registro->salarioServidor);
    } else {
        printf("valor nao declarado\n");
    }

    printf("%s: ", cabecalho->desCampo3);
    if (strlen(registro->telefoneServidor) != 0) {
        printf("%s\n", registro->telefoneServidor);
    } else {
        printf("valor nao declarado\n");
    }

    printf("%s: ", cabecalho->desCampo4);
    if (strlen(registro->nomeServidor)) {
        printf("%s\n", registro->nomeServidor);
    } else {
        printf("valor nao declarado\n");
    }

    printf("%s: ", cabecalho->desCampo5);
    if (strlen(registro->cargoServidor) != 0) {
        printf("%s\n", registro->cargoServidor);
    } else {
        printf("valor nao declarado\n");
    }

    printf("\n");
}

/**
 * @brief  Compara o campo de um registro a um valor.
 * @param  registro: Registro a ser comparado
 * @param  campo: Index do campo o qual valor pertence
 * @param  valor: Valor que será comparado
 * @retval Retorna verdadeiro se o registro tem o valor, e falso caso contrário
 */
bool compararRegistro(sRegistro* registro, int campo, char* valor) {
    char nulo[5] = "NULO";
    double valorDouble;

    switch (campo) {
        case 1:
            if (atoi(valor) == registro->idServidor) {
                return true;
            } else {
                return false;
            }
            break;

        case 2: 
            // Coloca o valor como nulo, caso a string seja nula
            if (!strcmp(valor, nulo)) {
                valorDouble = -1;
            } else {
                valorDouble = atof(valor);
            }

            // Compara
            if (valorDouble == registro->salarioServidor) {
                return true;
            } else {
                return false;
            }
            break;

        case 3: 
            // Coloca o valor como nulo, caso a string seja nula
            if (!strcmp(valor, nulo)) {
                valor[0] = '\0';
            }

            // Compara
            if (!strcmp(valor, registro->telefoneServidor)) {
                return true;
            } else {
                return false;
            }
            break;

        case 4: 
            // Coloca o valor como nulo, caso a string seja nula
            if (!strcmp(valor, nulo)) {
                valor[0] = '\0';
            }

            // Compara
            if (!strcmp(valor, registro->nomeServidor)) {
                return true;
            } else {
                return false;
            }
            break;

        case 5:         
            // Coloca o valor como nulo, caso a string seja nula
            if (!strcmp(valor, nulo)) {
                valor[0] = '\0';
            }

            // Compara
            if (!strcmp(valor, registro->cargoServidor)) {
                return true;
            } else {
                return false;
            }
            break;
        // Se não caiu em nenhum dos casos anteriores, o campo passado pelo usuário é inválido.
        default:
            printf("Campo inválido\n");
            return false;
    }
}

/**
 * @brief  Busca registros que contém mesmo valor que o passado por parâmetro em determinado campo.
 * @param  nomeDoArquivo: Nome do arquivo binário que contém os registros
 * @param  campo: Campo ao qual valor pertence
 * @param  valor: Valor pelo qual se quer buscar
 * @param  imprimirPaginas: Indica se deve imprimir a quantidade de páginas de disco acessadas mesmo se não foi encontrado nada
 * @retval None
 */
int buscarRegistro(char* nomeDoArquivo, char* campo, char* valor, bool imprimirPaginas) {
    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        fclose(bin);
        liberarCabecalho(cabecalho);

        return -1;
    }

    int indexCampo = campoIndex(cabecalho, campo);

    // Coloca o cursor no primeiro byte do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Cria uma variável para manter controle sobre as páginas de disco
    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.bytes = 0;
    paginaDeDisco.pagina = 2;

    int registrosEncontrados = 0;

    sRegistro* registro = criarRegistro();
    // Lê e imprime o primeiro registro, já fazendo a verificação se há registros
    long int posicaoInicial;
    if (!lerRegistroBin(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        printf("Registro inexistente.\n");
        
        fclose(bin);
        liberarCabecalho(cabecalho);
        liberarRegistro(registro);

        return -1;
    }
    // Se o registro possui o valor do campo igual ao pedido, imprime na tela o registro
    if (compararRegistro(registro, indexCampo, valor)) {
        imprimirRegistroComCabecalho(registro, cabecalho);
        registrosEncontrados++;

        // Se o campo é o id, já acaba a função porque o id é único
        if (indexCampo == 1) {
            printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);

            fclose(bin);
            liberarCabecalho(cabecalho);
            liberarRegistro(registro);
            
            return -1;
        }
    }
    zerarRegistro(registro);

    // Lê e imprime todos os outros registros
    while (lerRegistroBin(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        // Se o registro possui o valor do campo igual ao pedido, imprime na tela o registro
        if (compararRegistro(registro, indexCampo, valor)) {
            imprimirRegistroComCabecalho(registro, cabecalho);
            registrosEncontrados++;

            // Se o campo é o id, já acaba a função porque o id é único
            if (indexCampo == 1) {
                printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);

                fclose(bin);
                liberarCabecalho(cabecalho);
                liberarRegistro(registro);

                return -1;
            }
        }
        
        zerarRegistro(registro);
    }

    // Verifica se foi encontrado algum registro
    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n");

        if (imprimirPaginas) {
            printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);
        }

        fclose(bin);    
        liberarCabecalho(cabecalho);
        liberarRegistro(registro);

        return paginaDeDisco.pagina;
    }

    // Mostra a quantidade de páginas de disco que foram acessadas
    printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);

    // Fecha o arquivo e libera memória alocada
    fclose(bin);    
    liberarCabecalho(cabecalho);
    liberarRegistro(registro);

    return paginaDeDisco.pagina;
}

//!###############################################################################################################################

/**
 * @brief  Copia todos os campos de um registro para o outro
 * @param  registroDestino: Para onde será copiado
 * @param  registroOrigem: De onde será copiado
 * @retval None
 */
void copiarRegistro(sRegistro* registroDestino, sRegistro* registroOrigem) {
    registroDestino->removido           = registroOrigem->removido;
    registroDestino->idServidor         = registroOrigem->idServidor;
    registroDestino->salarioServidor    = registroOrigem->salarioServidor;
    registroDestino->encadeamentoLista  = registroOrigem->encadeamentoLista;
    registroDestino->tamanho            = registroOrigem->tamanho;

    strcpy(registroDestino->telefoneServidor,registroOrigem->telefoneServidor);
    strcpy(registroDestino->nomeServidor, registroOrigem->nomeServidor);
    strcpy(registroDestino->cargoServidor, registroOrigem->cargoServidor);
}

/**
 * @brief  Adiciona todas as posições e tamanho dos regstros removidos a uma lista
 * @param  bin: Arquivo de onde se lerá os registros removidos.
 * @param  registrosRemovidos: Lista onde serão adicionados os dados dos registros removidos.
 * @retval Retorna falso quando chegar ao fim do arquivo
 */
bool localizarRegistrosRemovidos(FILE* bin, sLista* registrosRemovidos) {
    // Salva a posição de início do registro, e cria variáveis para guardar os capos tamanho e removido
    long int posicao = ftell(bin);
    int tamanho;
    char removido;

    // Verifica se chegou no final do arquivo, lendo o campo "removido"
    if (fread(&removido, sizeof(char), 1, bin) != 1 || removido == '\0') {
        return false;
    }

    // Lê o tamanho do registro
    fread(&tamanho, sizeof(int), 1, bin);

    // Coloca o cursor no final do registro
    fseek(bin, tamanho, SEEK_CUR);

    // Caso seja um registro removido, adiciona na lista
    // Caso contrário, lê o próximo registro
    if (removido == '*') {
        listaAdicionar(registrosRemovidos, posicao, tamanho);
        return true;
    } else {
        return localizarRegistrosRemovidos(bin, registrosRemovidos); 
    }
}

/**
 * @brief  Remove logicamente registros que se encaixam com o valor de busca
 * @param  indexCampo: Index do campo que será comparado
 * @param  valorBusca: Valor do campo que será buscado
 * @param  registrosRemovidos: Lista com dados dos registros removidos
 * @param  bin: Arquivo binário
 * @param  isIndice: booleana para verificar quando é necessário alocar o array de ponteiros de registros
 * @retval array de ponteiros dos registros encontrados
 */
sRegistro** removerRegistro(int indexCampo, char* valorBusca, sLista* registrosRemovidos, FILE* bin, bool isIndice) {
    sPaginaDeDisco paginaDeDisco;
    sRegistro* registro = criarRegistro();
    char arroba = '@';
    sRegistro** listaDeRegistros = NULL;
    int i = 0;

    // Passa pelo arquivo inteiro procurando registros a serem removidos
    while (true) {
        // Salva posição do registro
        long int posicao = ftell(bin);

        // Lê o registro e verifica condição de parada do loop (para quanod chega no fim do arquivo)
        if (!lerRegistroBin(registro, bin, &paginaDeDisco, &posicao)) {
            break;
        }

        // Se for o registro, preenche volta, marca como removido, e adiciona a lista
        if (compararRegistro(registro, indexCampo, valorBusca)) {
            // Volta ao início do registro
            fseek(bin, posicao, SEEK_SET);

            // Remove logicamente o registro
            registro->removido = '*';
            fwrite(&registro->removido, sizeof(char), 1, bin);

            // Vai para a posição do registro após o encadeamento lista, e escreve arrobas
            fseek(bin, (sizeof(int) + sizeof(long int)), SEEK_CUR);
            for (int i = 0; i < (registro->tamanho - sizeof(long int)); i++) {
               fwrite(&arroba, sizeof(char), 1, bin);
            }

            listaAdicionar(registrosRemovidos, posicao, registro->tamanho);
            if(isIndice){
                listaDeRegistros = realloc(listaDeRegistros, (i + 1) * sizeof(sRegistro*));
                sRegistro* temp = criarRegistro(); 
                copiarRegistro(temp, registro);
                listaDeRegistros[i++] = temp;

            }
        }
    }
    liberarRegistro(registro);
    return listaDeRegistros;
}
/**
 * @brief  Escreve nos registros reovidos do arquivo o encadeamento da lista em ordem crescente de tamanho
 * @param  bin: Arquivo binário
 * @param  registrosRemovidos: Lista de registros removidos
 * @retval None
 */
void escreverEncadeamento(FILE* bin, sLista* registrosRemovidos) {
    long int encadeamentoLista;

    // Move o cursor para campo topoLista do cabeçalho, e escreve no arquivo
    fseek(bin, sizeof(char), SEEK_SET);
    
    encadeamentoLista = listaRemoverInicio(registrosRemovidos);
    fwrite(&encadeamentoLista, sizeof(long int), 1, bin);

    // Escreve o encadeamento para todos os outros registros removidos da lista
    while (encadeamentoLista != -1) {
        fseek(bin, (encadeamentoLista + sizeof(char) + sizeof(int)), SEEK_SET); // soma-se tamanho dos campos "removido" e "tamanho"
        
        encadeamentoLista = listaRemoverInicio(registrosRemovidos);
        fwrite(&encadeamentoLista, sizeof(long int), 1, bin);
    }
}

/**
 * @brief  Tenta ler um registro de um arquivo para ver se possui registros
 * @note   
 * @param  bin: Arquivo que se quer verificar se tem registros ou não
 * @retval Retorna verdadeiro se há registros, e falso se não há
 */
bool contemRegistros(FILE* bin) {
    sRegistro* registro = criarRegistro();
    sPaginaDeDisco paginaDeDisco;
    
    long int posicao = ftell(bin);

    bool contem = lerRegistroBin(registro, bin, &paginaDeDisco, &posicao);
    
    fseek(bin, posicao, SEEK_SET);
    liberarRegistro(registro);
    return contem;
}

void remocaoDeRegistros() {
    // Aloca memória para variáveis
    char* nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
    char* campoBusca = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* valorBusca = malloc(sizeof(char) * STRING_TAM_MAX); 
    int n;

    // Lê o nome do arquivo e a quantidade de registros a serem removidos
    scanf("%s %d", nomeDoArquivo, &n);

    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb+");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Muda a consistência do arquivo
    cabecalho->status = '0';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Coloca o cursosr na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Busca por todos os registros já removidos no arquivo, e faz uma lista
    sLista* registrosRemovidos = listaCriar();

    // Faz uma primeira leitura pra ver se há registros no arquivo, 
    // e imprime mensagem de erro, muda consistência do arquivo e libera memória caso não haja
    if (!contemRegistros(bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        // Muda a consistência do arquivo
        cabecalho->status = '1';
        rewind(bin);
        fwrite(&cabecalho->status, sizeof(char), 1, bin);
        
        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        liberarCabecalho(cabecalho);
        listaLiberar(&registrosRemovidos);
        fclose(bin);

        return;
    }

    // Lê os registros buscando por removidos até acabar o arquivo
    while(localizarRegistrosRemovidos(bin, registrosRemovidos));

    // Realiza a remoção de registros n vezes, lendo sempre novas entradas
    for (int i = 0; i < n; i++) {
        fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

        scanf("%s ", campoBusca);
        scanf("%[^\r\n]", valorBusca);

        // Se o valor vier entre aspas, retira as aspas
        if (valorBusca[0] == '\"') {
            sscanf(valorBusca ,"\"%[^\"]\"", valorBusca);
        }
        int indexCampo = campoIndex(cabecalho, campoBusca);

        removerRegistro(indexCampo, valorBusca, registrosRemovidos, bin, false);
    }

    // Escreve o encadeamento da lista no arquivo
    escreverEncadeamento(bin, registrosRemovidos);

    // Muda a consistência do arquivo
    cabecalho->status = '1';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Libera memória alocada
    free(nomeDoArquivo);
    free(campoBusca);
    free(valorBusca);
    liberarCabecalho(cabecalho);
    listaLiberar(&registrosRemovidos);

    // Escreve o arquivo na tela e fecha
    binarioNaTela1(bin);
    fclose(bin);
}

//!###############################################################################################################################

/**
 * @brief  Retorna o próximo byte a ser lido de um arquivo
 * @param  bin: Arquivo de onde se lerá o byte
 * @retval 
 */
char proximoByte(FILE* bin) {
    char c;
    fread(&c, sizeof(char), 1, bin);
    fseek(bin, -sizeof(char), SEEK_CUR);
    return c;
}

/**
 * @brief  Escreve um registro em um arquivo em uma posição especificada
 * @param  bin: Arquivo onde se escreverá o registro
 * @param  registro: Registros que será escrito
 * @param  insercaoNoFinal: Indica se o registro será escrito no final do arquivo
 * @retval Retorna a posição em que o registro foi escrito
 */
long int escreverRegistroEmEspacoVazio(FILE* bin, sRegistro* registro, bool insercaoNoFinal) {
    char arroba = '@';
    long int posicao;

    // Se for final de página de disco, e não couber o registro, preenche de arrobas
    if (insercaoNoFinal) {
        posicao = ftell(bin);
        int tamanho = registro->tamanho + sizeof(int) + sizeof(char);
        int bytesRestantes = PAGINA_DE_DISCO_TAM - (posicao % PAGINA_DE_DISCO_TAM);
        
        if (tamanho > bytesRestantes) {
            for (int i = 0; i < bytesRestantes; i++) {
                fwrite(&arroba, sizeof(char), 1, bin);
            }

            fseek(bin, posicao, SEEK_SET);

            // Procura pelo última início do último registro
            char c;
            do {
                fseek(bin, -1, SEEK_CUR);
                fread(&c, sizeof(char), 1, bin);
                fseek(bin, -1, SEEK_CUR);
            } while (c != '*' && c != '-');

            // Vai para o campo do tamanho, calcula o novo tamanho do registro, e o escreve
            fseek(bin, 1, SEEK_CUR);
            tamanho = posicao - ftell(bin);
            fwrite(&tamanho, sizeof(int), 1, bin);
        }
    }

    // Guarda a posição inicial de onde o registro será escrito
    posicao = ftell(bin);

    // Escreve o valor removido na posição inicial do registro 
    fwrite(&registro->removido, sizeof(char), 1, bin);

    // Escreve o campo de tamanho. Se ele já existir, apenas pula
    if (insercaoNoFinal) {
        fwrite(&registro->tamanho, sizeof(int), 1, bin);
    } else {
        fseek(bin, sizeof(int), SEEK_CUR);
    }
    fwrite(&registro->encadeamentoLista, sizeof(long int), 1, bin);

    // Escreve no arquivo o id, o salário e o telefone (campos de tamanho fixo)
    fwrite(&registro->idServidor, sizeof(int), 1, bin);

    fwrite(&registro->salarioServidor, sizeof(double), 1, bin);

    if (strlen(registro->telefoneServidor) != 0) {
        fwrite(registro->telefoneServidor, sizeof(char), strlen(registro->telefoneServidor), bin);
    } else {
        char barraZero = '\0';
        fwrite(&barraZero, sizeof(char), 1, bin);
        for (int i = 0; i < TELEFONE_TAM - 1; i++) {
            fwrite(&arroba, sizeof(char), 1, bin);
        }
    }

    // Escreve no arquivo o nome e o cargo, com suas tags e indicadores de tamanho (campos de tamanho variável)
    int tamanho;
    char tag;

    if (strlen(registro->nomeServidor) != 0) {
        tag = 'n';
        tamanho = strlen(registro->nomeServidor) + 2 * sizeof(char);    // string + \0 + tag
        fwrite(&tamanho, sizeof(int), 1, bin);
        fwrite(&tag, sizeof(char), 1, bin);
        fwrite(registro->nomeServidor, sizeof(char), tamanho - 1, bin);
    }

    if (strlen(registro->cargoServidor) != 0) {
        tag = 'c';
        tamanho = strlen(registro->cargoServidor) + 2 * sizeof(char);
        fwrite(&tamanho, sizeof(int), 1, bin);
        fwrite(&tag, sizeof(char), 1, bin);
        fwrite(registro->cargoServidor, sizeof(char), tamanho - 1, bin);
    }

    return posicao;
}

/**
 * @brief  Insere um registro (de acordo com best-fit)
 * @param  bin: Arquivo onde será inserido
 * @param  registro: Registro a ser inserido
 * @param  registrosRemovidos: Lista com as posições e tamanhos dos registros removidos
 * @retval None
 */
void inserirRegistro(FILE* bin, sRegistro* registro, sLista* registrosRemovidos) {
    bool insercaoNoFinal;
    long int posicao = listaRemover(registrosRemovidos, registro->tamanho);

    if (posicao == -1) {
        fseek(bin, 0,SEEK_END);
        insercaoNoFinal = true;
    } else {
        fseek(bin, posicao, SEEK_SET);   
        insercaoNoFinal = false;
    }
    
    escreverRegistroEmEspacoVazio(bin, registro, insercaoNoFinal);
}

/**
 * @brief  Insere um registro (de acordo com best-fit) e gera um índice para ele
 * @param  bin: Arquivo onde será inserido
 * @param  registro: Registro a ser inserido
 * @param  registrosRemovidos: Lista com as posições e tamanhos dos registros removidos
 * @retval Índice do registro inserido
 */
sIndice* inserirRegistroERetornarIndice(FILE* bin, sRegistro* registro, sLista* registrosRemovidos) {
    bool insercaoNoFinal;
    long int posicao = listaRemover(registrosRemovidos, registro->tamanho);

    if (posicao == -1) {
        fseek(bin, 0,SEEK_END);
        insercaoNoFinal = true;
    } else {
        fseek(bin, posicao, SEEK_SET);   
        insercaoNoFinal = false;
    }
    
    // Insere o registro e cria um índice para ele
    sIndice* indice = criarIndice();
    indice->byteOffset = escreverRegistroEmEspacoVazio(bin, registro, insercaoNoFinal);
    strcpy(indice->chaveBusca, registro->nomeServidor);

    return indice;
}

void insercaoDeRegistros() {
    char* nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX); 
    int n;

    // Lê o nome do arquivo e a quantidade de registros a serem adicionados
    scanf("%s %d", nomeDoArquivo, &n);

    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb+");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Muda a consistência do arquivo
    cabecalho->status = '0';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Coloca o cursosr na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Busca por todos os registros já removidos no arquivo, e faz uma lista
    sLista* registrosRemovidos = listaCriar();
    while(localizarRegistrosRemovidos(bin, registrosRemovidos));
    
    // Aloca memória
    char* id        = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* salario   = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* telefone  = malloc(sizeof(char) * STRING_TAM_MAX);  
    char* nome      = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* cargo     = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* temp      = malloc(sizeof(char) * BUFFER_TAM); 
    char* buffer    = malloc(sizeof(char) * BUFFER_TAM); 

    sRegistro* registro = criarRegistro();
    // Realiza a funcionalidade n vezes, lendo sempre novas entradas
    for (int i = 0; i < n; i++) {
        // Lê alguns parâmetros e coloca os que devem ser tratados em uma variável diferente
        scanf("%s %s %s %[^\r\n]",id, salario, telefone, buffer);

        // Trata de aspas na string do telefone
        if (telefone[0] == '\"') {
            sscanf(telefone, "\"%[^\"]\"", temp);
            strcpy(telefone, temp);
        }

        // Lê a primeira string contínua do nome, parar verificar se é nulo
        sscanf(buffer, "%s", nome);

        // Se não for nulo, lê o valor que está entre aspas e atuliza o valor restante na variável "linha"
        // Caso seja nulo, apenas atualiza a variável linha
        if (strcmp(nome, "NULO")) {
            sscanf(buffer, "\"%[^\"]\" %[^\r\n]", nome, temp);
            strcpy(buffer, temp);
        } else {
            sscanf(buffer, "%*s %[^\r\n]", temp);
            strcpy(buffer, temp);
        }

        // Faz o mesmo que foi feito para o nome, agora para o cargo
        sscanf(buffer, "%s", cargo);
        if (strcmp(nome, "NULO")) {
            sscanf(buffer, "\"%[^\"]\"", cargo);
        }

        // Preenche registro com as informações necessárias
        preencherRegistro(registro, id, salario, telefone, nome, cargo);
        inserirRegistro(bin, registro, registrosRemovidos);
    }

    // Escreve o encadeamento
    escreverEncadeamento(bin, registrosRemovidos);

    // Muda a consistência do arquivo
    cabecalho->status = '1';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Libera memória alocada
    free(buffer);
    free(temp);
    free(nomeDoArquivo);
    free(id);
    free(salario);
    liberarCabecalho(cabecalho);
    liberarRegistro(registro);


    // Ecreve binário na tela e fecha o arquivo
    binarioNaTela1(bin);
    fclose(bin);
}

//!###############################################################################################################################

/**
 * @brief  Atualiza o campo de um regsitro
 * @note   
 * @param  registro: 
 * @param  indexCampoAtualiza: 
 * @param  valorAtualiza: 
 * @retval None
 */
void atualizaCampo(sRegistro* registro, int indexCampoAtualiza, char* valorAtualiza) {
    switch (indexCampoAtualiza) {
        case 1:
            registro->idServidor = atoi(valorAtualiza);
            break;
        case 2:
            registro->salarioServidor = atof(valorAtualiza);
            break;
        case 3:
            registro->telefoneServidor = valorAtualiza;
            break;
        case 4:
            registro->nomeServidor = valorAtualiza;
            break;
        case 5:
            registro->cargoServidor = valorAtualiza;
            break;
    }

    // Atualiza o tamanho do registro
    registro->tamanho = tamanhoRegistro(registro);
}

/**
 * @brief  
 * @param  bin: Arquivo binário
 * @param  valorBusca Valor que será usado para buscar o registro que será atualizado:
 * @param  indexCampoBusca: Index do campo que será usado para a busca
 * @param  valorAtualiza: Valor que substituirá o valor anterior do registro
 * @param  indexCampoAtualiza: Index do campo que será atualizado
 * @param  registrosRemovidos: Lista de registros removidos
 * @retval None
 */
void atualizarRegistro(FILE* bin, char* valorBusca, int indexCampoBusca, char* valorAtualiza, int indexCampoAtualiza, sLista* registrosRemovidos) {
    sRegistro* registro = criarRegistro();
    long int posicaoInicial, posicaoProximo;
    int tamanho;

    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.bytes = -PAGINA_DE_DISCO_TAM;

    // Coloca o cursor na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Passa por todos os registros do arquivo
    while (lerRegistroBin(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        // Se for o registro que deve ser atualizado, verifica se 
        if (compararRegistro(registro, indexCampoBusca, valorBusca)) {
            char arroba = '@';

            // Vai para o começo do registo, pois ele será atualizado
            fseek(bin, posicaoInicial, SEEK_SET);

            // Salva o valor do espaço disponível em disco, depois atualiza o registro na RAM
            tamanho = registro->tamanho;
            atualizaCampo(registro, indexCampoAtualiza, valorAtualiza);

            // Se o registro atualizado couber em sua mesma posiçao, reescreve ele nessa mesma posição
            // Caso não caiba, remove o registro e coloca no lugar de tamanho mais adequado
            if (tamanho >= registro->tamanho) {
                // Mantém o tamanho do registro
                registro->tamanho = tamanho;
                escreverRegistro(registro, bin, &paginaDeDisco, -1);

                // Calcula quanto de lixo deve ser escrito e escreve
                int lixoTamanho = tamanho - registro->tamanho;
                for (int i = 0; i < lixoTamanho; i++) {
                    fwrite(&arroba, sizeof(char), 1, bin);
                }
            } else {
                // Salva a posição
                posicaoProximo = ftell(bin);

                // Remove logicamente o registro
                char removido = '*';
                fwrite(&removido, sizeof(char), 1, bin);

                // Vai para a posição do registro após o encadeamento lista, e escreve arrobas
                fseek(bin, (sizeof(int) + sizeof(long int)), SEEK_CUR);
                for (int i = 0; i < (tamanho - sizeof(long int)); i++) {
                    fwrite(&arroba, sizeof(char), 1, bin);
                }

                listaAdicionar(registrosRemovidos, posicaoInicial, tamanho);

                // Insere em sua nova posição e coloca o cursor na posição do próximo registro
                inserirRegistro(bin, registro, registrosRemovidos);
                fseek(bin, posicaoProximo, SEEK_SET);
            }
        }
    }
}

void atualizacaoDeRegistros() {
    // Lê o nome do arquivo e a quantidade de registros a serem atualizados
    char* nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
    int n;

    scanf("%s %d", nomeDoArquivo, &n);

    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb+");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");

        free(nomeDoArquivo) ;
        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Muda a consistência do arquivo
    cabecalho->status = '0';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Coloca o cursosr na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Busca por todos os registros já removidos no arquivo, e faz uma lista
    sLista* registrosRemovidos = listaCriar();
    while(localizarRegistrosRemovidos(bin, registrosRemovidos));

    // Aloca memória
    char* buffer            = malloc(sizeof(char) * BUFFER_TAM);
    char* campoBusca        = malloc(sizeof(char) * STRING_TAM_MAX);
    char* valorBusca        = malloc(sizeof(char) * STRING_TAM_MAX);
    char* campoAtualliza    = malloc(sizeof(char) * STRING_TAM_MAX);
    char* valorAtuzaliza    = malloc(sizeof(char) * STRING_TAM_MAX);
    
    // Realiza a funcionalidade n vezes, lendo sempre novas entradas
    // sRegistro* registro = criarRegistro();
    for (int i = 0; i < n; i++) {
        // Lê a linha completa
        scanf(" %[^\r\n]", buffer);

        // Lê os valores de campo e do valor. O valor ainda deve acabar de ser processado
        sscanf(buffer, "%s %s", campoBusca, valorBusca);

        // Se o valor da busca está entre aspas, lê tudo que está entre aspas. Dposi atualiza o buffer
        if (valorBusca[0] == '\"') {
            sscanf(buffer, "%*s \"%[^\"]\" %[^\r\n]", valorBusca, buffer);
        } else {
            sscanf(buffer, "%*s %*s %[^\r\n]", buffer);
        }

        // Repete o processo para os campos que devem ser atualizados
        sscanf(buffer, "%s %[^\r\n]", campoAtualliza, valorAtuzaliza);
        if (valorAtuzaliza[0] == '\"') {
            sscanf(valorAtuzaliza, "\"%[^\"]\"", valorAtuzaliza);
        }
        
        // Identifica o index do campo de busca e de atualização
        int indexCampoBusca = campoIndex(cabecalho, campoBusca);
        int indexCampoAtualiza = campoIndex(cabecalho, campoAtualliza);

        // Atualiza o registro
        atualizarRegistro(bin, valorBusca, indexCampoBusca, valorAtuzaliza, indexCampoAtualiza, registrosRemovidos);
    }

    // Escreve o encadeamento
    escreverEncadeamento(bin, registrosRemovidos);

    // Muda a consistência do arquivo
    cabecalho->status = '1';
    rewind(bin);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);

    // Libera memória alocada
    free(nomeDoArquivo);
    free(campoBusca);
    free(valorBusca);
    free(campoAtualliza);
    free(valorAtuzaliza);

    // Escreve o binário na tela e fecha o arquivo
    binarioNaTela1(bin);
    fclose(bin);
}

//!###############################################################################################################################

/**
 * @brief  Compara os ids de dois registros
 * @note   Compara subtraindo o o id de registro 1 do id do registro 2
 * @param  registro1: Registro 1
 * @param  registro2: Registro 2
 * @retval Diferença dos ids
 */
int compararId(const void* registro1, const void* registro2) {
    const sRegistro* r1 = *(const sRegistro**) registro1;
    const sRegistro* r2 = *(const sRegistro**) registro2;
    return r2->idServidor - r1->idServidor;
}

/**
 * @brief  
 * @note   
 * @param  bin: Arquivo binário de onde serão lidos os registros
 * @retval Vetor de ponteiros de registros
 */
sRegistro** lerArquivoBinario(FILE* bin, int* quantidadeDeRegistros) {
    *quantidadeDeRegistros = 0;
    sRegistro** arrayDeRegistros = NULL;

    // Coloca o cursor na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Cria uma variável para página de disco
    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.bytes = 0;
    paginaDeDisco.pagina = 2;

    // Cria variável para registro e seus dados
    sRegistro* registro = criarRegistro();
    long int posicao;
    
    // Passa por todo o arquivo, salvando os registros lidos em um array
    while (lerRegistroBin(registro, bin, &paginaDeDisco, &posicao)) {

        // Copia o registro lido para uma outra variável
        sRegistro* registroCopia = criarRegistro();
        copiarRegistro(registroCopia, registro);

        // Recalcula o tamanho, para casos em que há lixo no final do registro
        registroCopia->tamanho = tamanhoRegistro(registroCopia);

        // Aloca espaço para mais um registro
        arrayDeRegistros = realloc(arrayDeRegistros, (*quantidadeDeRegistros + 1) * sizeof(sRegistro*));

        // Adiciona registro no array e aumenta a quantidade de registros
        arrayDeRegistros[*quantidadeDeRegistros] = registroCopia;
        (*quantidadeDeRegistros)++;
    }

    liberarRegistro(registro);
    
    return arrayDeRegistros;
}

void ordenarRegistros(char* nomeDoArquivo, char* nomeDoArquivoDeSaida) {
    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");

        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Como não haverá nenhum registro removido, o topo da lista será nulo. Coloca o arquivo como inconsistente também
    cabecalho->topoLista = -1;
    cabecalho->status = '0';

    // Lê todos os registros do arquivo binário e os ordena de acordo com o ID
    int quantidadeDeRegistros;
    sRegistro** registros = lerArquivoBinario(bin, &quantidadeDeRegistros);

    // Ordena os registros
    MS_sort(registros, quantidadeDeRegistros, sizeof(sRegistro*), compararId);

    // Cria variável de página de disco
    sPaginaDeDisco paginaDeDisco;
    paginaDeDisco.pagina = 1;
    paginaDeDisco.bytes = 0;

    // Cria um novo arquivo binário e escreve o cabeçalho nele
    FILE* novoBin = fopen(nomeDoArquivoDeSaida, "wb");
    escreverCabecalho(cabecalho, novoBin, &paginaDeDisco);


    // Escreve todos os registros do array no arquivo binário, e já libera a memória alocada por esses regitros
    int tamanhoDoRegistro = 0;
    for (int i = 0; i < quantidadeDeRegistros; i++) {
        if (registros[i]->idServidor == 3707135){
            int a = 0;
        }
        tamanhoDoRegistro = escreverRegistro(registros[i], novoBin, &paginaDeDisco, tamanhoDoRegistro);
        liberarRegistro(registros[i]);
    }
    free(registros);

    // Muda a consistência do arquivo de escrita
    cabecalho->status = '1';
    rewind(novoBin);
    fwrite(&cabecalho->status, sizeof(char), 1, novoBin);

    // Libera memória alocada
    liberarCabecalho(cabecalho);
    fclose(bin);
    fclose(novoBin);

    // Escreve o arquivo binário na tela
    binarioNaTela2(nomeDoArquivoDeSaida);
}

//!###############################################################################################################################

void mergingAndMatchingRegistros(char *nomeDoArquivo1, char *nomeDoArquivo2, char *nomeDoArquivoDeSaida, bool isMerging){
    
    long int posicaoInicial1, posicaoInicial2;
    int tamanhoDoRegistro = 0;
    char status;

    // Abre os arquivos binários
    FILE *bin1 = fopen(nomeDoArquivo1, "rb");
    FILE *bin2 = fopen(nomeDoArquivo2, "rb");

    // Verifica se foi aberto corretamente
    if (bin1 == NULL || bin2 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin1)) {
        printf("Falha no processamento do arquivo.\n");

        fclose(bin1);
        fclose(bin2);
        liberarCabecalho(cabecalho);

        return;
    }

    rewind(bin1);
    // Coloca o cursor na posição do primeiro registro
    fseek(bin1, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Verifica o status do segundo arquivo, para saber se ele está consistente
    fread(&status, sizeof(char), 1, bin2);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");

        fclose(bin1);
        fclose(bin2);
        liberarCabecalho(cabecalho);

        return;
    }

    rewind(bin2);
    // Coloca o cursosr na posição do primeiro registro
    fseek(bin2, PAGINA_DE_DISCO_TAM, SEEK_SET);

    //abre o arquivo binário que será escrito o merging dos registros e verifica se foi aberto corretamente
    FILE *novoBin = fopen(nomeDoArquivoDeSaida, "wb");

    // Verifica se o arquivo foi aberto corretamente
    if(novoBin == NULL)
    {
        fclose(bin1);
        fclose(bin2);
        liberarCabecalho(cabecalho);
    }

    // Cria variável de página de disco referente ao primeiro arquivo binário
    sPaginaDeDisco paginaDeDisco1;
    paginaDeDisco1.pagina = 1;
    paginaDeDisco1.bytes = 0;
    
    // Cria variável de página de disco referente ao segundo arquivo binário
    sPaginaDeDisco paginaDeDisco2;
    paginaDeDisco2.pagina = 1;
    paginaDeDisco2.bytes = 0;   

    // Cria os registros
    sRegistro *registro1 = criarRegistro();
    sRegistro *registro2 = criarRegistro();

    //Cria variável de página de disco referente ao arquivo binário de escrita
    sPaginaDeDisco paginaDeDiscoNovo;
    paginaDeDiscoNovo.pagina = 1;
    paginaDeDiscoNovo.bytes = 0;
    // Escreve o cabeçalho no arquivo de escrita
    cabecalho->status = '0';
    escreverCabecalho(cabecalho, novoBin, &paginaDeDiscoNovo);
    
    //Verifica se há registro nos dois arquivos binários, caso haja só em um, copia todos os registros desse arquivo
    if(!contemRegistros(bin1)){
        // caso só o segundo arquivo tenha registros
        if(contemRegistros(bin2)){
             while(lerRegistroBin(registro2, novoBin, &paginaDeDisco2, &posicaoInicial2)){
                registro2->tamanho = tamanhoRegistro(registro2);
                tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            }  
        }
        // caso os dois arquivos não tenham registros
        else{
            printf("Falha no processamento do arquivo.\n");

            fclose(bin1);
            fclose(bin2);
            liberarCabecalho(cabecalho);
            liberarRegistro(registro1);
            liberarRegistro(registro2);

            return;
        }
    }
    // caso só o primeiro arquivo tenha registros
    else if(!contemRegistros(bin2)){
        while(lerRegistroBin(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)){
            registro1->tamanho = tamanhoRegistro(registro1);
            tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
        }
    }

    // Lê o primeiro registro de cada arquivo binário
    lerRegistroBin(registro1, bin1, &paginaDeDisco1, &posicaoInicial1);
    registro1->tamanho = tamanhoRegistro(registro1);
    lerRegistroBin(registro2, bin2, &paginaDeDisco2, &posicaoInicial2);
    registro2->tamanho = tamanhoRegistro(registro2);

    //Enquanto os dois arquivos tiverem registros
    while(true){
        //caso o id do registro 1 seja menor que o do registro 2
        if(compararId(&registro1, &registro2) > 0){
            if(isMerging){
                tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            }
            // caso não tenha mais registros no arquivo 1, escreve o que já foi lido do arquivo 2
            if(!lerRegistroBin(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)) {
                if(isMerging)
                    tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;   
           }
           registro1->tamanho = tamanhoRegistro(registro1);
        }
        // caso o id do registro 1 seja maior que o do registro 2 
        else if(compararId(&registro1, &registro2) < 0){
            if(isMerging){
                tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            }
            //caso não tenha mais registros no arquivo 2, escreve o que já foi lido do arquivo 1
            if(!lerRegistroBin(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)) {
                if(isMerging)
                    tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;
            }
            registro2->tamanho = tamanhoRegistro(registro2);
        }
        // caso o seja o mesmo id, tanto no arquivo 1 quanto no 2
        else{
            //escreve o registro do arquivo 1
            tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            //verifica se há registros no arquivo 1, caso não para
            if(!lerRegistroBin(registro1, bin1, &paginaDeDisco1, &posicaoInicial1))  
                break;
            registro1->tamanho = tamanhoRegistro(registro1);
            //verifica se há registros no arquivo 2, caso não escreve o que já foi lido do arquivo 1 e para
            if(!lerRegistroBin(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)){
                if(isMerging)
                    tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;
            }
            registro2->tamanho = tamanhoRegistro(registro2);
            
        }
    }
    // caso ainda tenha registros apenas no arquivo 1, escreve
    if(isMerging){
        while(lerRegistroBin(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)){
            registro1->tamanho = tamanhoRegistro(registro1);
            tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
        }
    }

    //caso ainda tenha registros apenas no arquivo 2, escreve
    if(isMerging){
        while(lerRegistroBin(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)){
            registro2->tamanho = tamanhoRegistro(registro2);
            tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
        }   
    }
    // Muda a consistência do arquivo de escrita
    cabecalho->status = '1';
    rewind(novoBin);
    fwrite(&cabecalho->status, sizeof(char), 1, novoBin);

    //Libera memória alocada e escreve o novo binário na tela
    liberarRegistro(registro1);
    liberarRegistro(registro2);
    liberarCabecalho(cabecalho);
    fclose(bin1);
    fclose(bin2);
    fclose(novoBin);
    binarioNaTela2(nomeDoArquivoDeSaida);
}

//!###############################################################################################################################

/**
 * @brief  Gera uma lista de índices a partir de um arquivo binário
 * @note   Salva a quantidade de registros lidos no cabeçalho de índices
 * @param  bin: Arquivo de onde se lerá os registros
 * @param  cabecalhoDeIndices: Cabeçalho que conterá a quantidade de registros
 * @retval 
 */
sIndice** gerarListaDeIndices(FILE* bin, sCabecalhoDeIndices* cabecalhoDeIndices) {
    // Coloca o cursor na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Declara variáveis usadas para a recuperação de registros    
    sRegistro* registro = criarRegistro();
    sPaginaDeDisco paginaDeDisco;
    long int byteOffset;

    // Cria uma lista de índices
    sIndice** listaDeIndices = NULL;
    int i = 0;

    // Recupera todos os registros e adiciona o índice dos que possuem nome não nulo na lista de índices
    while (lerRegistroBin(registro, bin, &paginaDeDisco, &byteOffset)) {
        // Se o nome do registro não é nulo, adiciona na lista
        if (registro->nomeServidor[0] != '\0') {
            // Cria um índice
            sIndice* indice = criarIndice();
            indice->byteOffset = byteOffset;
            strcpy(indice->chaveBusca, registro->nomeServidor);

            // Adiciona à lista
            listaDeIndices = realloc(listaDeIndices, (i + 1) * sizeof(sIndice*));
            listaDeIndices[i++] = indice;
        }
    }

    cabecalhoDeIndices->nroRegistros = i;
    liberarRegistro(registro);

    return listaDeIndices;
}

/**
 * @brief  Compara a chave de busca de dois índices
 * @param  indice1: Índice 1
 * @param  indice2: índice 2
 * @retval Retorna o valor de menos o retorno de strcmp
 */
int compararIndice(const void* indice1, const void* indice2) {
    const sIndice* i1 = *(const sIndice**) indice1;
    const sIndice* i2 = *(const sIndice**) indice2;
    return -strcmp(i1->chaveBusca, i2->chaveBusca);
}

/**
 * @brief  Escreve lixo em um arquivo
 * @param  bin: Arquivo onde será escrito lixo
 * @param  quantidade: Quantidade de lixo que será escrito
 * @param  string: Indica se o lixo será escrito após uma string, sendo assim nexessário colocar um \n no começo
 * @retval None
 */
void escreverLixo(FILE* bin, int quantidade, bool string) {
    char lixo = '@';

    // Escreve o \0 se necessário
    if (string) {
        quantidade--;
        char barraZero = '\0';
        fwrite(&barraZero, sizeof(char), 1, bin);
    }

    // Escreve lixo
    for (int i = 0; i < quantidade; i++) {
        fwrite(&lixo, sizeof(char), 1, bin);
    }
}

/**
 * @brief  Escreve o cabeçalho de índices em um arquivo binário
 * @param  arquivoDeIndices: Arquivo de índices onde o cabeçalho será escrito
 * @param  cabecalhoDeIndices: Cabeçalho que será escrito no arquivo
 * @retval None
 */
void escreverCabecalhoDeIndice(FILE* arquivoDeIndices, sCabecalhoDeIndices* cabecalhoDeIndices) {
    fwrite(&cabecalhoDeIndices->status, sizeof(char), 1, arquivoDeIndices);
    fwrite(&cabecalhoDeIndices->nroRegistros, sizeof(int), 1, arquivoDeIndices);

    int quantidadeDeLixo = PAGINA_DE_DISCO_TAM - sizeof(char) - sizeof(int);
    escreverLixo(arquivoDeIndices, quantidadeDeLixo, false);
}


/**
 * @brief  Escreve um índice no registro
 * @param  arquivoDeIndices: Arquivo onde será escrito
 * @param  indice: Índice que será escrito
 * @retval None
 */
void escreverIndice(FILE* arquivoDeIndices, sIndice* indice) {
    if(indice->byteOffset != -1){
        // Escreve a chave busca
        int tamanhoDoCampo = strlen(indice->chaveBusca);
        fwrite(indice->chaveBusca, sizeof(char), tamanhoDoCampo, arquivoDeIndices);

        // Escreve lixo no espaço que sobrar
        escreverLixo(arquivoDeIndices, INDICE_BUSCA_TAM - tamanhoDoCampo, true);

        // Escreve o byteoffset
        fwrite(&indice->byteOffset, sizeof(long int), 1, arquivoDeIndices);
    }
}

/**
 * @brief  Muda a consistência do arquivo para o valor passado por parâmetro
 * @param  bin: Arquivo binário que terá sua consistência mudada
 * @retval None
 */
void mudarConsistenciaDoArquivo(FILE* bin, char consistencia) {
    rewind(bin);
    fwrite(&consistencia, sizeof(char), 1, bin);
}

void gerarArquivoDeIndices(char* nomeDoArquivo, char* nomeDoArquivoDeSaida) {
    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");

        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Cria um caebçalho de índices e um vetor com índices
    sCabecalhoDeIndices* cabecalhoDeIndices = criarCabecalhoDeIndices();
    sIndice** listaDeIndices = gerarListaDeIndices(bin, cabecalhoDeIndices);
    
    // Fecha o arquivo, já que os registros já foram lidos
    fclose(bin);

    // Ordena os índices alfabeticamente de forma estável
    MS_sort(listaDeIndices, cabecalhoDeIndices->nroRegistros, sizeof(sIndice*), compararIndice);

    // Cria arquivo de índices
    FILE* arquivoDeindices = fopen(nomeDoArquivoDeSaida, "wb");

    // Escreve o cabeçalho
    escreverCabecalhoDeIndice(arquivoDeindices, cabecalhoDeIndices);

    // Escreve os índices e já libera memória alocada
    for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
        escreverIndice(arquivoDeindices, listaDeIndices[i]);
        liberarIndice(listaDeIndices[i]);
    }
    free(listaDeIndices);

    // Torna o arquivo consistente
    mudarConsistenciaDoArquivo(arquivoDeindices, '1');

    // Libera memória alocada e fecha os arquivos
    liberarCabecalho(cabecalho);
    liberarCabecalhoDeIndices(cabecalhoDeIndices);
    fclose(arquivoDeindices);

    // Imprime o binário
    binarioNaTela2(nomeDoArquivoDeSaida);
}

//!###############################################################################################################################

/**
 * @brief  Lê o cabeçalho de um arquivo de índices e armazena numa struct.
 * @param  cabecalho: Ponteiro para cabeçalho que se quer preencher.
 * @param  csv: Arquivo de índices de onde se lerá o cabeçalho
 * @retval Retorna verdadeiro se leu o cabeçalho com sucesso, e falso caso contrário 
 */
bool lerCabecalhoDeIndices(sCabecalhoDeIndices* cabecalho, FILE* bin) {
    // Lê o status do cabeçalho verificando se há não se chegou ao fim do arquivo, retornando neste caso
    if (fread(&cabecalho->status, sizeof(char), 1, bin) == 0) {
        return false;
    }

    // Checa se o arquivo está consistente
    if (cabecalho->status == '0') {
        return false;
    }

    // Se tudo foi lido com sucesso até o momento, lê o resto do cabecçalho e retorna verdadeiro
    fread(&cabecalho->nroRegistros, sizeof(int), 1, bin);
    return true;
}

/**
 * @brief  Lê um índice de um arquivo de índices
 * @param  bin: Arquivo binário de índices
 * @param  indice: Índice onde se armazenará os dados 
 * @retval Retorna verdadeiro se foi lido com sucesso
 */
bool lerIndice(FILE* bin, sIndice* indice) {
    if (fread(indice->chaveBusca, sizeof(char), INDICE_BUSCA_TAM, bin) == 0) {
        return false;
    }
    fread(&indice->byteOffset, sizeof(long int), 1, bin);

    return true;
}

/**
 * @brief  Verifica se um arquivo de índices contém um índice na posição onde está o cursor
 * @param  bin: Arquivo de índices
 * @retval Retorna verdadeiro se contém índices
 */
bool contemIndice(FILE* bin) {
    bool contem;
    sIndice* indice = criarIndice();

    contem = lerIndice(bin, indice);

    // Volta o cursor para onde estava antes, libera memória alocada e retorna
    fseek(bin, -(sizeof(char) * INDICE_BUSCA_TAM + sizeof(long int)), SEEK_CUR);
    liberarIndice(indice);
    return contem;
}

/**
 * @brief  Cria um array de bools para indicar quais páginas de disco já foram acessadas
 * @note   Todos os valores começam com false
 * @param  bin: 
 * @retval 
 */
int quantidadeDePaginas(FILE* bin) {
    long int posicao = ftell(bin);
    fseek(bin, 0, SEEK_END);
    long int tamanho = ftell(bin);

    return tamanho / PAGINA_DE_DISCO_TAM + 1;
}

/**
 * @brief  Retorna a posição do índice de acordo com o RRN
 * @param  rrn: RRN do índice
 * @retval A posição do índice 
 */
long int posicaoPorRRN(int rrn) {
    return rrn * INDICE_TAM +  PAGINA_DE_DISCO_TAM;
}

/**
 * @brief  Copia um indice para outro
 * @param  indiceDestino: Índice para onde será copiado
 * @param  indiceOrigem: Índice de onde será copiado
 * @retval None
 */
void copiarIndice(sIndice* indiceDestino, sIndice* indiceOrigem) {
    indiceDestino->byteOffset = indiceOrigem->byteOffset;
    strcpy(indiceDestino->chaveBusca, indiceOrigem->chaveBusca);
}

/**
 * @brief  Realiza uma busca binária em uma lista de índices e retorna os indexes dos valores encontrados
 * @param  listaDeIndices: Lista de índices onde se fará a busca binária
 * @param  valorBusca: Valor usado na busca
 * @param  numeroDeRegistros: Número de índices na lista de índices
 * @param  tamanhoDaLista: Variável de retorno: retorna o tamanho da lista de indexes retornada
 * @retval Retorna array do e indexes de todos os valores encontrados na busca
 */
int* buscaBinaria(sIndice** listaDeIndices, char* valorBusca, int numeroDeRegistros, int* tamanhoDaLista) {
    *tamanhoDaLista = 0;

    int final = numeroDeRegistros;
    int inicio = 0;
    int i = (final - inicio) / 2;

    // Realiza a busca binária
    int comparacao = strcmp(valorBusca, listaDeIndices[i]->chaveBusca);
    while (comparacao != 0) {
        // Atualiza o início ou o final dependendo da comparação
        if (comparacao < 0) {
            final = i - 1;
        } else {
            inicio = i + 1;
        }

        // Se o inicio é igual ao final, a busca binária chegou ao final
        if (inicio > final) {
            return NULL;
        }

        // Atualiza o índice e compara as strings
        i = inicio + (final - inicio) / 2;
        comparacao = strcmp(valorBusca, listaDeIndices[i]->chaveBusca);
    }

    // Busca a primeira ocorrência do índice com valor buscado
    while (!strcmp(valorBusca, listaDeIndices[i]->chaveBusca)) {
        i--;
    }
    i++;

    // Coloca todos os indexes dos índices em um array e retorna
    int* indexes = NULL;
    for (*tamanhoDaLista = 0; !strcmp(valorBusca, listaDeIndices[i]->chaveBusca); i++) {
        indexes = realloc(indexes, (*tamanhoDaLista + 1) * sizeof(int));
        indexes[*tamanhoDaLista] = i;
        (*tamanhoDaLista)++;
    }

    return indexes;
}

/**
 * @brief  Busca o primeiro índice do registro de acordo com o valor da busca
 * @param  bin: Arquivo de índices
 * @param  valorBusca: valor pelo qual está se bugando
 * @retval RRN da primeia ocorrência da condição de busca. Retorna -1 caso não ache nada.
 */
int buscaBinarioDeIndicesEmDisco(FILE* bin, char* valorBusca, int numeroDeRegistros) {
    sIndice* indice = criarIndice();

    int inicio = 0;
    int final = numeroDeRegistros;

    // Calcula o primeiro rrn
    int rrn = (final - inicio) / 2;
    long int posicao = posicaoPorRRN(rrn);
    fseek(bin, posicao, SEEK_SET);
    lerIndice(bin, indice);

    // Equanto o índice não foi encontrado, segue a busaca binária
    int comparacao = strcmp(valorBusca, indice->chaveBusca);
    while (comparacao != 0) {
        // Atualiza o início ou o final dependendo da comparação
        if (comparacao < 0) {
            final = rrn - 1;
        } else {
            inicio = rrn + 1;
        }

        // Se o inicio é igual ao final, a busca binária chegou ao final
        if (inicio > final) {
            liberarIndice(indice);
            return -1;
        }

        // Atualiza o rrn e lê o próximo índice
        rrn = inicio + (final - inicio) / 2;
        posicao = posicaoPorRRN(rrn);
        fseek(bin, posicao, SEEK_SET);
        lerIndice(bin, indice);

        // Compara novamente as strings
        comparacao = strcmp(valorBusca, indice->chaveBusca);
    }

    // Busca a primeira ocorrência do índice com valor buscado
    while (!strcmp(valorBusca, indice->chaveBusca)) {
        rrn--;
        posicao = posicaoPorRRN(rrn);
        fseek(bin, posicao, SEEK_SET);
        lerIndice(bin, indice);
    }
    rrn++;

    // Libera memória alocada e retorna a lista de índices
    liberarIndice(indice);
    return rrn;
}

/**
 * @brief  Adiciona todos os índices que satisfaçam a condição a uma lista de arrays e retorna
 * @param  bin: Arquivo de índices
 * @param  valorBusca: Valor pelo qual se quer buscar
 * @param  numeroDeRegistros: Número de registros no arquivo de índices
 * @param  tamanhoDaLista: Variável usada como valor de retorno. Tamanho do lista criada.
 * @retval Retorna a lista de índices encontrados   
 */
sIndice** buscarEGerarListaDeIndices(FILE* bin, char* valorBusca, int numeroDeRegistros, int* tamanhoDaLista) {
    // Busca o RRN do primeiro índice que satisfaz a condição de busca e lê o índice
    int rrn = buscaBinarioDeIndicesEmDisco(bin, valorBusca, numeroDeRegistros);

    // Checa se o rrn é válido
    if (rrn == -1) {
        return NULL;
    } else {
        // Lê o primeiro índice
        sIndice* indice = criarIndice();
        fseek(bin, posicaoPorRRN(rrn++), SEEK_SET);
        lerIndice(bin, indice);

        // Cria uma lista de índices
        sIndice** listaDeIndices = NULL;
        numeroDeRegistros = 0;

        // Passa por todos os índices que são compatíveis com o valor de busca
        while (!strcmp(valorBusca, indice->chaveBusca)) {
            // Copia o índice para uma variável temporária
            sIndice* temp = criarIndice();
            copiarIndice(temp, indice);

            // Realoca memória para lista e adiciona índice a ela
            listaDeIndices = realloc(listaDeIndices, sizeof(sIndice*) * numeroDeRegistros + 1);
            listaDeIndices[numeroDeRegistros++] = temp;

            // Lê o próximo índice
            long int posicao = posicaoPorRRN(rrn++);
            fseek(bin, posicao, SEEK_SET);
            lerIndice(bin, indice);
        }

        // Libera memória alocada e retorna lista de índices
        liberarIndice(indice);
        *tamanhoDaLista = numeroDeRegistros;
        return listaDeIndices;
    }
}

int buscarPeloArquivoDeIndices(char* nomeDoArquivoBinario, char* nomeDoArquivoDeIndices, char* campoBusca, char* valorBusca, bool imprimirPaginas) {
    // Abre os arquivos
    FILE* bin = fopen(nomeDoArquivoBinario, "rb");
    FILE* arquivoDeIndices = fopen(nomeDoArquivoDeIndices, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL || arquivoDeIndices == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    sCabecalhoDeIndices* cabecalhoDeIndices = criarCabecalhoDeIndices();
    if (!lerCabecalhoBin(cabecalho, bin) || !lerCabecalhoDeIndices(cabecalhoDeIndices, arquivoDeIndices)) {
        printf("Falha no processamento do arquivo.\n");

        fclose(bin);
        fclose(arquivoDeIndices);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);

        return -1;
    }

    // Coloca os dois arquivos na segunda página de disco
    fseek(arquivoDeIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Cria um array para armazenar todos os índices
    sIndice** listaDeIndices = malloc(sizeof(sIndice*) * cabecalhoDeIndices->nroRegistros);

    // Passa todos os índices para a RAM
    fseek(arquivoDeIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);
    for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
        sIndice* temp = criarIndice();
        lerIndice(arquivoDeIndices, temp);
        listaDeIndices[i] = temp;
    }

    // Realiza busca binária 
    int tamanhoDaLista;
    int* indexes = buscaBinaria(listaDeIndices, valorBusca, cabecalhoDeIndices->nroRegistros, &tamanhoDaLista);

    // Cria variáveis para armazenar dados
    sIndice* indice = criarIndice();
    sRegistro* registro = criarRegistro();
    sPaginaDeDisco* paginaDeDisco = criarPaginaDeDisco();
    long int posicao;

    // Cria array de bools para lidar com as páginas de acessadas
    int qtdDePaginas = quantidadeDePaginas(bin);
    bool* paginasAcessadas = calloc(qtdDePaginas, sizeof(bool));

    if (indexes == NULL) {
        printf("Registro inexistente.\n");

        if (imprimirPaginas) {
            // Calcula quantas páginas foram acessadas no arquivo de dados e mostra esse valor
            fseek(arquivoDeIndices, 0, SEEK_END);
            posicao = ftell(arquivoDeIndices);
            int qtdDePaginasAcessadas = posicao / PAGINA_DE_DISCO_TAM;

            if (posicao % PAGINA_DE_DISCO_TAM != 0) {
                qtdDePaginasAcessadas++;
            }

            printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", qtdDePaginasAcessadas);

            // Verifica quantas páginas foram acessadas no arquivo de dados e mostra esse valor
            qtdDePaginasAcessadas = 1;      // Começa do 1 para contar o cabeçalho
            for (int i = 0; i < qtdDePaginas; i++) {
                if (paginasAcessadas[i]) {
                    qtdDePaginasAcessadas++;
                }
            }

            printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", qtdDePaginasAcessadas);
        }

        // Libera memória alocada e fecha os arquivos
        for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
            liberarIndice(listaDeIndices[i]);
        }
        free(listaDeIndices);

        liberarPaginaDeDisco(paginaDeDisco);
        liberarRegistro(registro);
        liberarIndice(indice);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);
        free(paginasAcessadas);
        fclose(bin);
        fclose(arquivoDeIndices);

        return 1;
    } 

    // Imprime todos os regsitros, marcando as páginas acessadas
    for (int i = 0; i < tamanhoDaLista; i++) {
        posicao = listaDeIndices[indexes[i]]->byteOffset;
        fseek(bin, posicao, SEEK_SET);
        paginasAcessadas[posicao / PAGINA_DE_DISCO_TAM] = true;

        lerRegistroBin(registro, bin, paginaDeDisco, &posicao);
        imprimirRegistroComCabecalho(registro, cabecalho);
    }

    // Calcula quantas páginas foram acessadas no arquivo de dados e mostra esse valor
    fseek(arquivoDeIndices, 0, SEEK_END);
    posicao = ftell(arquivoDeIndices);
    int qtdDePaginasAcessadas = posicao / PAGINA_DE_DISCO_TAM;

    if (posicao % PAGINA_DE_DISCO_TAM != 0) {
        qtdDePaginasAcessadas++;
    }

    printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", qtdDePaginasAcessadas);

    // Verifica quantas páginas foram acessadas no arquivo de dados e mostra esse valor
    qtdDePaginasAcessadas = 1;      // Começa do 1 para contar o cabeçalho
    for (int i = 0; i < qtdDePaginas; i++) {
        if (paginasAcessadas[i]) {
            qtdDePaginasAcessadas++;
        }
    }

    printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", qtdDePaginasAcessadas);

    // Libera memória
    for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
        liberarIndice(listaDeIndices[i]);
    }
    free(listaDeIndices);

    free(indexes);
    free(paginasAcessadas);
    liberarIndice(indice);
    liberarPaginaDeDisco(paginaDeDisco);
    liberarRegistro(registro);
    liberarCabecalho(cabecalho);
    liberarCabecalhoDeIndices(cabecalhoDeIndices);
    fclose(bin);
    fclose(arquivoDeIndices);
    
    return qtdDePaginasAcessadas;
}

//!###############################################################################################################################
/**
 * @brief  Função que lê o arquivo de índices, carregando todos os índices para a ram
 * @note   
 * @param  binIndices: arquivo de índices
 * @retval retorna um array de ponteiros de índices
 */
sIndice** recuperaIndices(FILE* binIndices){
    int i = 0;
    sIndice* indice = criarIndice();
    sIndice** listaDeIndices = NULL;

    if(binIndices != NULL){
        // Coloca o cursor no início do primeiro índice
        fseek(binIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);
        // Enquanto houver índices no arquivo, lê e coloca-os no arrays
        while(lerIndice(binIndices, indice)){
            // Adiciona à lista
            listaDeIndices = realloc(listaDeIndices, (i + 1) * sizeof(sIndice*));
            sIndice* temp = criarIndice();
            copiarIndice(temp, indice);
            listaDeIndices[i++] = temp;
        }
    }
    liberarIndice(indice);
    return listaDeIndices;
}

/**
 * @brief  Função que faz a remoção lógica do índice na ram
 * @note   Essa funcão faz a remoção ao andar pelo array de índices, com base no array retornado pela
 * busca binária, que nos dá os indexes dos índices a serem removidos
 * @param  listaDeIndices: array de ponteiros de índices
 * @param  indicesRemovidos: array com os indexes do índices a serem removidos
 * @param  tamanhoDaLista: tamanho da lista de indices removidos
 * @retval None
 */
void marcarRemovido(sIndice** listaDeIndices, int* indicesRemovidos, int tamanhoDaLista){
    if(listaDeIndices != NULL && indicesRemovidos != NULL){
        for(int i = 0; i < tamanhoDaLista; i++){
            listaDeIndices[indicesRemovidos[i]]->byteOffset = -1;
        }        
    }
}

/**
 * @brief  Função que faz a busca binária do índice do registro a ser removido e faz a remoção lógica
 * desse índice
 * @note   
 * @param  listaDeRegistros: array de ponteiros de registros, guarda os registros a serem removidos
 * @param  listaDeIndices: array de ponteiros de índices, guarda todos os índices que estavam no arquivo de índices
 * @param  cabecalhoDeIndices: ponteiro para o cabeçalho de índices
 * @param  *tamanhoDaLista: Variável usada como valor de retorno. Tamanho do lista criada para guardar quais índices serão removidos
 * @retval None
 */
void removeIndice(sRegistro** listaDeRegistros, sIndice** listaDeIndices, sCabecalhoDeIndices* cabecalhoDeIndices, int *tamanhoDaLista){
    int *listaDeIndexes = NULL;
    *tamanhoDaLista = 1;

    if(listaDeRegistros != NULL && listaDeIndices != NULL){
        for(int i = 0; i < *tamanhoDaLista; i++){
            // Faz a busca binária, realiza a remoção lógica dos índices e já libera memória dos registros
            listaDeIndexes = buscaBinaria(listaDeIndices, listaDeRegistros[i]->nomeServidor, cabecalhoDeIndices->nroRegistros, tamanhoDaLista);
            marcarRemovido(listaDeIndices, listaDeIndexes, *tamanhoDaLista);
            liberarRegistro(listaDeRegistros[i]);
            free(listaDeIndexes);
        }
        free(listaDeRegistros);
        return;
    }
    // Caso não tenha nenhum registro para ser removido
    *tamanhoDaLista = 0;
}
 
void removerListaDeIndices(){

    // Aloca memória para variáveis
    char* nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
    char* nomeDoArquivoDeIndices = malloc(sizeof(char) * STRING_TAM_MAX);
    char* campoBusca = malloc(sizeof(char) * STRING_TAM_MAX); 
    char* valorBusca = malloc(sizeof(char) * STRING_TAM_MAX); 
    int n, tamanhoDaLista, nIndices;

    // Lê o nome do arquivo e a quantidade de registros a serem removidos
    scanf("%s %s %d", nomeDoArquivo, nomeDoArquivoDeIndices, &n);

    // Abre o arquivo binário 
    FILE* bin = fopen(nomeDoArquivo, "rb+");

    // Verifica se foi aberto corretamente
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");

        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    if (!lerCabecalhoBin(cabecalho, bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        fclose(bin);
        liberarCabecalho(cabecalho);

        return;
    }

    // Muda a consistência do arquivo
    mudarConsistenciaDoArquivo(bin, '0');

    // Coloca o cursor na posição do primeiro registro
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Faz uma primeira leitura pra ver se há registros no arquivo, 
    // e imprime mensagem de erro, muda consistência do arquivo e libera memória caso não haja
    if (!contemRegistros(bin)) {
        printf("Falha no processamento do arquivo.\n");
        
        // Muda a consistência do arquivo
        mudarConsistenciaDoArquivo(bin, '1');
        
        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        liberarCabecalho(cabecalho);
        fclose(bin);

        return;
    }

    // Abre o arquivo de índices
    FILE* binIndices = fopen(nomeDoArquivoDeIndices, "rb");

    // Verifica se o arquivo de índices foi aberto corretamente
    if(binIndices == NULL){
        printf("Falha no processamento do arquivo.\n");

        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        fclose(bin);
        liberarCabecalho(cabecalho);
        return;
    }

    // Cria o cabeçalho do arquivo de índices, lê e verifica a consistência do arquivo, caso haja inconsistência,
    //termina o programa
    sCabecalhoDeIndices* cabecalhoDeIndices = criarCabecalhoDeIndices();
    if(!lerCabecalhoDeIndices(cabecalhoDeIndices, binIndices)){
        printf("Falha no processamento do arquivo.\n");

        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        fclose(bin);
        fclose(binIndices);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);
        
        return;
    }

    // Guarda a quantidade de índices original que há no arquivo
    nIndices = cabecalhoDeIndices->nroRegistros;
    cabecalhoDeIndices->status = '0';
    // Coloca o cursor no início do primeiro índice
    fseek(binIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Faz uma primeira leitura pra ver se há índices no arquivo, 
    // e imprime mensagem de erro e libera memória caso não haja
    if(!contemIndice(binIndices)){
        printf("Falha no processamento do arquivo.\n");

        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);
        fclose(bin);
        fclose(binIndices);

        return;
    }

   
    // Recupera todos os índices para a ram e fecha o arquivo de índices
    sIndice** listaDeIndices = recuperaIndices(binIndices);
    fclose(binIndices);

    // Busca por todos os registros já removidos no arquivo, e faz uma lista
    sLista* registrosRemovidos = listaCriar();
    // Lê os registros buscando por removidos até acabar o arquivo
    while(localizarRegistrosRemovidos(bin, registrosRemovidos));

    sRegistro** listaDeRegistros = NULL;

    // Realiza a remoção de registros n vezes, lendo sempre novas entradas
    for (int i = 0; i < n; i++) {
        fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

        scanf("%s ", campoBusca);
        scanf("%[^\r\n]", valorBusca);

        // Se o valor vier entre aspas, retira as aspas
        if (valorBusca[0] == '\"') {
            sscanf(valorBusca ,"\"%[^\"]\"", valorBusca);
        }
        int indexCampo = campoIndex(cabecalho, campoBusca);

        // coloca no array os registros que serão removidos
        listaDeRegistros = removerRegistro(indexCampo, valorBusca, registrosRemovidos, bin, true);
        // faz a remoção lógica dos índices
        removeIndice(listaDeRegistros, listaDeIndices, cabecalhoDeIndices, &tamanhoDaLista);
        // diminui a quantidade de índices no arquivo
        cabecalhoDeIndices->nroRegistros -= tamanhoDaLista;
    }

    // Abre o arquivo de índices, agora no modo escrita
    FILE *novoBinIndices = fopen(nomeDoArquivoDeIndices, "wb");
    
    // Verifica se o arquivo de índices foi aberto corretamente, caso não
    // imprime uma mensagem de erro e libera memória alocada
    if(novoBinIndices == NULL){
        printf("Falha no processamento do arquivo.\n");

        for(int j = 0; listaDeRegistros[j] != NULL; j++){
            liberarRegistro(listaDeRegistros[j]);
        }
        free(listaDeRegistros);
        for(int j = 0; cabecalhoDeIndices->nroRegistros; j++){
            liberarIndice(listaDeIndices[j]);
        }
        free(listaDeIndices);
        // Libera memória
        free(nomeDoArquivo);
        free(campoBusca);
        free(valorBusca);
        free(nomeDoArquivoDeIndices);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);
        
        listaLiberar(&registrosRemovidos);
        fclose(bin);

        return; 
    }

     // Escreve o cabeçalho no arquivo de índices
    escreverCabecalhoDeIndice(novoBinIndices, cabecalhoDeIndices);
    
    // Escreve os índices e já libera memória alocada
    for (int i = 0; i < nIndices; i++) {
        escreverIndice(novoBinIndices, listaDeIndices[i]);
        liberarIndice(listaDeIndices[i]);
    }
    free(listaDeIndices);

    // Muda o status do cabeçalho
    mudarConsistenciaDoArquivo(novoBinIndices, '1');

    // Escreve o encadeamento da lista no arquivo
    escreverEncadeamento(bin, registrosRemovidos);

    // Muda a consistência do arquivo
    mudarConsistenciaDoArquivo(bin, '1');

    // Libera memória alocada
    free(nomeDoArquivo);
    free(campoBusca);
    free(valorBusca);
    liberarCabecalho(cabecalho);
    listaLiberar(&registrosRemovidos);

    // Escreve o arquivo na tela e fecha
    fclose(novoBinIndices);
    binarioNaTela2(nomeDoArquivoDeIndices);
    fclose(bin);
    liberarCabecalhoDeIndices(cabecalhoDeIndices);
    free(nomeDoArquivoDeIndices);
}
//!###############################################################################################################################

void inserirRegistrosComIndice(char* nomeDoArquivoBinario, char* nomeDoArquivoDeIndices, int n) {

    char id[STRING_TAM_MAX], salario[STRING_TAM_MAX], str[STRING_TAM_MAX];
    char *buffer = NULL, *linha = NULL, *nome = NULL, *telefone = NULL, *cargo = NULL;

    // Abre os arquivos
    FILE* bin = fopen(nomeDoArquivoBinario, "rb+");
    FILE* arquivoDeIndices = fopen(nomeDoArquivoDeIndices, "rb");

    // Verifica se foi aberto corretamente
    if (bin == NULL || arquivoDeIndices == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho já verificando o status do arquivo
    sCabecalho* cabecalho = criarCabecalho();
    sCabecalhoDeIndices* cabecalhoDeIndices = criarCabecalhoDeIndices();
    if (!lerCabecalhoBin(cabecalho, bin) || !lerCabecalhoDeIndices(cabecalhoDeIndices, arquivoDeIndices)) {
        printf("Falha no processamento do arquivo.\n");

        fclose(bin);
        fclose(arquivoDeIndices);
        liberarCabecalho(cabecalho);
        liberarCabecalhoDeIndices(cabecalhoDeIndices);

        return;
    }

    // Torna o arquivo de dados inconsistente
    mudarConsistenciaDoArquivo(bin, '0');

    // Coloca os dois arquivos na segunda página de disco
    fseek(arquivoDeIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);
    fseek(bin, PAGINA_DE_DISCO_TAM, SEEK_SET);

    // Cria um array para armazenar todos os índices
    sIndice** listaDeIndices = malloc(sizeof(sIndice*) * cabecalhoDeIndices->nroRegistros);

    // Passa todos os índices para a RAM
    fseek(arquivoDeIndices, PAGINA_DE_DISCO_TAM, SEEK_SET);
    for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
        sIndice* temp = criarIndice();
        lerIndice(arquivoDeIndices, temp);
        listaDeIndices[i] = temp;
    }

    // Fecha o arquivo de índices. Ele só será usado no final agora.
    fclose(arquivoDeIndices);

    // Busca por todos os registros já removidos no arquivo, e faz uma lista
    sLista* registrosRemovidos = listaCriar();
    while(localizarRegistrosRemovidos(bin, registrosRemovidos));

     sRegistro* registro = criarRegistro();
    // Realiza a funcionalidade n vezes, lendo sempre novas entradas
    for (int i = 0; i < n; i++) {

        //lê o id, salario e telefone e coloca o resto em uma única string para serem tratados depois
        scanf("%s %s %s %m[^\r\n]", id, salario, str, &buffer);
        linha = buffer;

        // trata as aspas do telefone, se ele não for nulo
        if (str[0] != 'N')
            sscanf(str, "\"%m[^\"]\"", &telefone);
        else{
            telefone = malloc(strlen(str));
            strcpy(telefone, "NULO");
        }
        //Lê a primeira string contínua, para verificar se o nome é nulo
        sscanf(linha, "%s", str);

        /*Se o nome não for nulo, coloca o nome, sem as aspas, em outra variável e tira a parte já lida 
        da string 'linha'. Caso o nome seja nulo, tira-se a parte do "NULO" da string 'linha' e deixa só o 
        resto (o cargo)
        */
        if (str[0] != 'N')
            sscanf(linha, "\"%m[^\"]\" %[^\r\n]", &nome, linha);
        else{
            sscanf(linha, "%*s %[^\r\n]", linha);
            nome = malloc(strlen(str));
            strcpy(nome, "NULO");
        }
        // Repete o que foi feito com o nome, só que agora com o cargo do servidor
        sscanf(linha, "%s", str);
        if (str[0] != 'N')
            sscanf(linha, "\"%m[^\"]\"", &cargo);
        else{
            cargo = malloc(strlen(str));
            strcpy(cargo, "NULO");
        }
        // Preenche registro com as informações necessárias e o insere
        preencherRegistro(registro, id, salario, telefone, nome, cargo);
        sIndice* temp = inserirRegistroERetornarIndice(bin, registro, registrosRemovidos);

        // Adiciona índice à lista de índices, caso o nome não seja nulo
        if (temp->chaveBusca[0] != '\0') {
            listaDeIndices = realloc(listaDeIndices, (cabecalhoDeIndices->nroRegistros + 1) * sizeof(sIndice*));
            listaDeIndices[cabecalhoDeIndices->nroRegistros++] = temp;
        }


        if(nome != NULL)        free(nome);
        if(cargo != NULL)       free(cargo);
        if(telefone != NULL)    free(telefone);
    }

    // Escreve o encadeamento no arquivo de dados
    escreverEncadeamento(bin, registrosRemovidos);

    // Muda a consistência do arquivo de dados
    mudarConsistenciaDoArquivo(bin, '1');

    // Ordena os índices alfabeticamente de forma estável
    MS_sort(listaDeIndices, cabecalhoDeIndices->nroRegistros, sizeof(sIndice*), compararIndice);

    // Abre novamente o arquivo de índices, agora reescrevendo-o do zero
    arquivoDeIndices = fopen(nomeDoArquivoDeIndices, "wb");

    // Reescreve tudo no arquivo de índices, e já libera memória alocada pelos índices
    escreverCabecalhoDeIndice(arquivoDeIndices, cabecalhoDeIndices);
    for (int i = 0; i < cabecalhoDeIndices->nroRegistros; i++) {
        escreverIndice(arquivoDeIndices, listaDeIndices[i]);
        liberarIndice(listaDeIndices[i]);
    }
    free(listaDeIndices);

    // Libera memória alocada
    free(buffer);
    listaLiberar(&registrosRemovidos);
    liberarCabecalho(cabecalho);
    liberarCabecalhoDeIndices(cabecalhoDeIndices);
    fclose(bin);
    fclose(arquivoDeIndices);

    // Escreve o binário na tela
    binarioNaTela2(nomeDoArquivoDeIndices);
}

//!###############################################################################################################################

void compararBuscas(char* nomeDoArquivoBinario, char* nomeDoArquivoDeIndices, char* campoBusca, char* valorBusca) {
    // Realiza a funcionalidae 3
    printf("*** Realizando a busca sem o auxílio de índice\n");
    int paginasAcessadaSemIndice = buscarRegistro(nomeDoArquivoBinario, campoBusca, valorBusca, true);

    // Realiza a funcionalidade 11
    printf("*** Realizando a busca com o auxílio de um índice secundário fortemente ligado\n");
    int paginasAcessadaComIndice = buscarPeloArquivoDeIndices(nomeDoArquivoBinario, nomeDoArquivoDeIndices, campoBusca, valorBusca, true);

    // Compara as duas
    int diferenca = paginasAcessadaSemIndice - paginasAcessadaComIndice;
    printf("\nA diferença no número de páginas de disco acessadas: %d\n", diferenca);
}