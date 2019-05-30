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

// Tamanho do buffer
#define BUFFER_TAM 300

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
 * @brief  Lê o cabeçalho de um arquivo .csv e armazena numa struct.
 * @param  cabecalho: Ponteiro para cabeçalho que se quer preencher.
 * @param  csv: Arquivo csv de onde se lerá o cabeçalho
 * @retval None
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
bool recuperarRegistro(sRegistro* registro, FILE* bin, sPaginaDeDisco* paginaDeDisco, long int* posicaoInicial) {
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

        return recuperarRegistro(registro, bin, paginaDeDisco, posicaoInicial);
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
    if (!recuperarRegistro(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        printf("Registro inexistente.\n");
        liberarRegistro(registro);
        fclose(bin);
        return;
    }
    imprimirRegistro(registro);
    zerarRegistro(registro);

    // Lê e imprime todos os outros registros
    while (recuperarRegistro(registro, bin, &paginaDeDisco, &posicaoInicial)) {
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
 * @retval None
 */
void buscarRegistro(char* nomeDoArquivo, char* campo, char* valor) {
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
    if (!recuperarRegistro(registro, bin, &paginaDeDisco, &posicaoInicial)) {
        printf("Registro inexistente.\n");
        
        fclose(bin);
        liberarCabecalho(cabecalho);
        liberarRegistro(registro);

        return;
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
            
            return;
        }
    }
    zerarRegistro(registro);

    // Lê e imprime todos os outros registros
    while (recuperarRegistro(registro, bin, &paginaDeDisco, &posicaoInicial)) {
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

                return;
            }
        }
        
        zerarRegistro(registro);
    }

    // Verifica se foi encontrado algum registro
    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n");

        fclose(bin);    
        liberarCabecalho(cabecalho);
        liberarRegistro(registro);

        return;
    }

    // Mostra a quantidade de páginas de disco que foram acessadas
    printf("Número de páginas de disco acessadas: %d\n", paginaDeDisco.pagina);

    // Fecha o arquivo e libera memória alocada
    fclose(bin);    
    liberarCabecalho(cabecalho);
    liberarRegistro(registro);
}

//!###############################################################################################################################

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
 * @retval None
 */
void removerRegistro(int indexCampo, char* valorBusca, sLista* registrosRemovidos, FILE* bin) {
    sPaginaDeDisco paginaDeDisco;
    sRegistro* registro = criarRegistro();
    char arroba = '@';    

    // Passa pelo arquivo inteiro procurando registros a serem removidos
    while (true) {
        // Salva posição do registro
        long int posicao = ftell(bin);

        // Lê o registro e verifica condição de parada do loop (para quanod chega no fim do arquivo)
        if (!recuperarRegistro(registro, bin, &paginaDeDisco, &posicao)) {
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
        }
    }

    liberarRegistro(registro);
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

    bool contem = recuperarRegistro(registro, bin, &paginaDeDisco, &posicao);
    
    fseek(bin, posicao, SEEK_SET);
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

        removerRegistro(indexCampo, valorBusca, registrosRemovidos, bin);
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
 * @retval None
 */
void escreverRegistroEmEspacoVazio(FILE* bin, sRegistro* registro, bool insercaoNoFinal) {
    char arroba = '@';

    // Se for final de página de disco, e não couber o registro, cm
    if (insercaoNoFinal) {
        long int posicao = ftell(bin);
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
    while (recuperarRegistro(registro, bin, &paginaDeDisco, &posicaoInicial)) {
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
    while (recuperarRegistro(registro, bin, &paginaDeDisco, &posicao)) {

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

    // Como não haverá nenhum registro removido, o topo da lista será nulo
    cabecalho->topoLista = -1;

    // Lê todos os registros do arquivo binário e os ordena de acordo com i ID
    int quantidadeDeRegistros;
    sRegistro** registros = lerArquivoBinario(bin, &quantidadeDeRegistros);

    // mergeSort(registros, 0, quantidadeDeRegistros);
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

    // Libera memória alocada
    liberarCabecalho(cabecalho);
    fclose(bin);
    fclose(novoBin);

    // Escreve o arquivo binário na tela
    binarioNaTela2(nomeDoArquivoDeSaida);
}
//!###############################################################################################################################

void mergingRegistros(char *nomeDoArquivo1, char *nomeDoArquivo2, char *nomeDoArquivoDeSaida){
    
    long int posicaoInicial1, posicaoInicial2;
    int tamanhoDoRegistro = 0;
    char status;

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
        liberarCabecalho(cabecalho);

        return;
    }

    rewind(bin1);
    // Coloca o cursor na posição do primeiro registro
    fseek(bin1, PAGINA_DE_DISCO_TAM, SEEK_SET);

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
    FILE *novoBin = fopen(nomeDoArquivoDeSaida, "wb+");

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

    sPaginaDeDisco paginaDeDiscoNovo;
    paginaDeDiscoNovo.pagina = 1;
    paginaDeDiscoNovo.bytes = 0;
    cabecalho->status = '0';
    escreverCabecalho(cabecalho, novoBin, &paginaDeDiscoNovo);
    
   
    if(!contemRegistros(bin1)){
        if(contemRegistros(bin2)){
             while(recuperarRegistro(registro2, novoBin, &paginaDeDisco2, &posicaoInicial2)){
                registro2->tamanho = tamanhoRegistro(registro2);
                tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            }  
        }
        else{
            printf("Falha no processamento do arquivo.\n");

            fclose(bin1);
            fclose(bin2);
            liberarCabecalho(cabecalho);

            return;
        }
    }
    else if(!contemRegistros(bin2)){
        while(recuperarRegistro(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)){
        registro1->tamanho = tamanhoRegistro(registro1);
        tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
        }
    }

    recuperarRegistro(registro1, bin1, &paginaDeDisco1, &posicaoInicial1);
    registro1->tamanho = tamanhoRegistro(registro1);
    recuperarRegistro(registro2, bin2, &paginaDeDisco2, &posicaoInicial2);
    registro2->tamanho = tamanhoRegistro(registro2);

    while(true){

        if(compararId(&registro2, &registro1) < 0){
           tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
           if(!recuperarRegistro(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)) {
                tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;   
           }
           registro1->tamanho = tamanhoRegistro(registro1);
        }
        else if(compararId(&registro2, &registro1) > 0){
            tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            if(!recuperarRegistro(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)) {
                tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;
            }
            registro2->tamanho = tamanhoRegistro(registro2);
        }
        else{
            tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
            if(!recuperarRegistro(registro1, bin1, &paginaDeDisco1, &posicaoInicial1))  
                break;
            registro1->tamanho = tamanhoRegistro(registro1);
            if(!recuperarRegistro(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)){
                tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
                break;
            }
            registro2->tamanho = tamanhoRegistro(registro2);
            
        }
    }

    while(recuperarRegistro(registro1, bin1, &paginaDeDisco1, &posicaoInicial1)){
        registro1->tamanho = tamanhoRegistro(registro1);
        tamanhoDoRegistro = escreverRegistro(registro1, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
    }

    while(recuperarRegistro(registro2, bin2, &paginaDeDisco2, &posicaoInicial2)){
        registro2->tamanho = tamanhoRegistro(registro2);
        tamanhoDoRegistro = escreverRegistro(registro2, novoBin, &paginaDeDiscoNovo, tamanhoDoRegistro);
    }   
    // Muda a consistência do arquivo
    cabecalho->status = '1';
    rewind(novoBin);
    fwrite(&cabecalho->status, sizeof(char), 1, novoBin);

    //Libera memória alocada e escreve o novo binário na tela
    liberarCabecalho(cabecalho);
    fclose(bin1);
    fclose(bin2);
    binarioNaTela1(novoBin);
    fclose(novoBin);
}