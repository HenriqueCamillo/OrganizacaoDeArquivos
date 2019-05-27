//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943

#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

// Tamanho dos campos
#define STRING_TAM_MAX 150
#define TELEFONE_TAM 14
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
 * @retval None
 */
void buscarRegistro(char* nomeDoArquivo, char* campo, char* valor);

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

#endif