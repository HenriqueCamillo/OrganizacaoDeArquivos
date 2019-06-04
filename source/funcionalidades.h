// O trabalho anterior escolhido foi o de Henrique Matarazo Camillo 
//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943
//  Nome: Gyovana Mayara Moriyama
//  NUSP: 10734387

#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

// Tamanho dos campos
#define STRING_TAM_MAX      150
#define TELEFONE_TAM        14
#define CAMPO_CABECALHO_TAM 40
#define PAGINA_DE_DISCO_TAM 32000

/**
 * @brief  Lê um arquivo .csv, converte para binário, e imprime na tela o nome do arquivo de saída.
 * @param  nomeDoArquivo: Nome do arquivo .csv
 * @retval None
 */
void csvParaBin(char* nomeDoArquivo);

/**
 * @brief  Lê um arquivo binário e imprime na tela todos os registros contidos nele.
 * @param  nomeDoArquivo: Nome do arquivo .bin
 * @retval None
 */
void imprimirBin(char* nomeDoArquivo);

/**
 * @brief  Lê um arquivo binário e imprime na tela todos os registros que tem o valor indicado no campo indicado.
 * @param  nomeDoArquivo: Nome do arquivo .bin
 * @param  campo: Campo pelo qual se quer buscar.
 * @param  valor: Valor pelo qual se quer buscar.
 * @param  imprimirPaginas: Indica se deve imprimir a quantidade de páginas de disco acessadas mesmo se não foi encontrado nada
 * @retval Número de páginas de disco acessadas
 */
int buscarRegistro(char* nomeDoArquivo, char* campo, char* valor, bool imprimirPaginas);

/**
 * @brief  Remove registros que contém algum dos n campos de busca fornecidos
 * @retval None
 */
void remocaoDeRegistros();

/**
 * @brief  Insere registros de um arquivo binário tentando colocá-los nos lugares em que eles mais se encaixam
 * @retval None
 */
void insercaoDeRegistros();


/**
 * @brief  Atualiza n regsitros de acordo com campos fornecidos
 * @retval None
 */
void atualizacaoDeRegistros();

/**
 * @brief  Lê todos os registros de um arquivo, ordena e escreve em um outro arquivo binário
 * @note   Faz ordenação de forma crescente a partir do ID
 * @param  nomeDoArquivo: Nome do arquivo que contém os registros
 * @param  nomeDoArquivoDeSaida: Nome do que conterá os regitros de forma ordenada
 */
void ordenarRegistros(char* nomeDoArquivo, char* nomeDoArquivoDeSaida);

/**
 * @brief  Realiza o merging ou o matching de dois arquivos binários e escreve em um outro arquivo binário
 * @note   Lê os dois arquivos binários e realiza o merging dos registros dos arquivos, comparando pelo ID, caso essa seja
 * a operação pedida (isMerging = true), ou realiza o matching dos registros dos arquivos binários, caso essa seja a operação
 * pedida (isMerging = false) 
 * @param  *nomeDoArquivo1: string com o nome do primeiro arquivo 
 * @param  *nomeDoArquivo2: string com o nome do segundo arquivo
 * @param  *nomeDoArquivoDeSaida: string com o nome do arquivo que conterá o merging dos registros
 * @param  isMerging: booleana que indica que é para realizar o merging nos arquivos
 * @retval None
 */
void mergingAndMatchingRegistros(char *nomeDoArquivo1, char *nomeDoArquivo2, char *nomeDoArquivoDeSaida, bool isMerging);

/**
 * @brief  Gera um arquivo de índices a partir de um arquivo binário
 * @param  nomeDoArquivo: Nome do arquivo binário a partir do qual se gerará o arquivo de índices
 * @param  nomeDoArquivoDeSaida: Nome do arquivo de índices gerado
 * @retval None
 */
void gerarArquivoDeIndices(char* nomeDoArquivo, char* nomeDoArquivoDeSaida);

/**
 * @brief  Busca registros a partir do arquivo de índices e imprime na tela os registros achados 
 *        e a quantidade de páginas de disco acessadas
 * @param  nomeDoArquivoBinario: Arquivo que contém os registros
 * @param  nomeDoArquivoDeIndices: Arquivo que contém os índices dos registros
 * @param  campoBusca: Campos pelo qual se buscará
 * @param  valor: Valor do campo pelo qual se buscará
 * @param  imprimirPaginas: Indica se deve imprimir a quantidade de páginas de disco acessadas mesmo se não foi encontrado nada
 * @retval Número de páginas de disco acessadas no arquivo de dados
 */
int buscarPeloArquivoDeIndices(char* nomeDoArquivoBinario, char* nomeDoArquivoDeIndices, char* campoBusca, char* valorBusca, bool imprimirPaginas);

 /**
  * @brief  Função que realiza a remoção dos índices e dos registros, tanto no arquivo de índices (que é reescrito),
  * quanto no arquivo binário (que tem os registros removidos logicamente)
  * @note   
  * @retval None
  */ 
void removerListaDeIndices();

/**
 * @brief  Compara a busca de registros com e sem um auxílio de um arquivo de índices
 * @param  nomeDoArquivoBinario: Nome do arquivo binário
 * @param  nomeDoArquivoDeIndices: Nome do arquivo de índices
 * @param  campoBusca: Campo pelo qual está se buscando
 * @param  valorBusca: Valor do campo pelo qual está se buscando
 * @retval None
 */
void compararBuscas(char* nomeDoArquivoBinario, char* nomeDoArquivoDeIndices, char* campoBusca, char* valorBusca);

#endif