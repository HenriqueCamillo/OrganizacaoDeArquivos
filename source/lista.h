//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943
//  Nome: Gyovana Mayara Moriyama
//  NUSP: 10734387

#ifndef LISTA_H
#define LISTA_H

// Struct para representar a lista
typedef struct lista sLista;

/**
 * @brief  Cria uma nova lista
 * @retval Retorna ponteiro para lista criada
 */
sLista* listaCriar();

/**
 * @brief  Adiciona um elemento na lista contendo posicao e tamanho do registro removido
 * @param  lsita: Lista onde será adicionado novo elemento
 * @param  posicao: Posição do registro removido 
 * @param  tamanho: Tamanho do registro removido
 * @retval None
 */
void listaAdicionar(sLista* lista, long int posicao, int tamanho);

/**
 * @brief  Remove um elemento da lista, dado o parâemtro tamanho, buscando o com tamanho mais próximo (maior a ele)
 * @param  lista: 
 * @param  tamanho: 
 * @retval Retorna o campo "Posição" do elemento
 */
long int listaRemover(sLista* lista, int tamanho);

/**
 * @brief  Remove o início da lista
 * @param  lista: 
 * @retval Retorna o campo "Posição" do elemento
 */
long int listaRemoverInicio(sLista* lista);

/**
 * @brief  Libera toda a memória alocada pela lista
 * @param  lista: 
 * @retval None
 */
void listaLiberar(sLista** lista);

/**
 * @brief  retorna o tamanho da lista
 * @param  lista: Lista que quer se saber o tamanho
 * @retval Tamanho da lista
 */
int listaTamanho(sLista* lista);

/**
 * @brief  Imprime a lista
 * @param  lista: Lista a ser imprimida
 * @retval None
 */
void listaImprimir(sLista* lista);

#endif
