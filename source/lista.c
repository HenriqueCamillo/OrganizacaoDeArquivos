//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943
//  Nome: Gyovana Mayara Moriyama
//  NUSP: 10734387

#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

// Struct para representar um registro removido
typedef struct no sNo;
struct no {
    long int posicao;
    int tamanho;    
    sNo* proximo;
};

struct lista {
    int tam;
    sNo* inicio;
};

/**
 * @brief  Cria um novo nó com dados passados por parâmetro
 * @param  posicao: 
 * @param  tamanho: 
 * @param  proximo: 
 * @retval Retorna o nó criado
 */
sNo* criarNo(long int posicao, int tamanho, sNo* proximo) {
    sNo* no = malloc(sizeof(sNo));

    no->posicao = posicao;
    no->tamanho = tamanho;
    no->proximo = proximo;

    return no;
}

sLista* listaCriar() {
    sLista* lista = malloc(sizeof(sLista));
    lista->tam = 0;
    
    // Cria nó cabeçca com valores inválidos
    lista->inicio = criarNo(-1, -1, NULL);

    return lista;
}

void listaAdicionar(sLista* lista, long int posicao, int tamanho) {
    sNo* temp = lista->inicio;

    // Acha onde deve inserir o elemento
    while (temp->proximo != NULL && tamanho > temp->proximo->tamanho) {
        temp = temp->proximo;
    }

    // Insere o elemento
    sNo* no = criarNo(posicao, tamanho, temp->proximo);
    temp->proximo = no;

    lista->tam++;
}

long int listaRemover(sLista* lista, int tamanho) {
    if (lista->tam == 0) {
        return -1;
    }

    sNo* temp = lista->inicio;

    // Passa por todos os elementos até achar um maior ou igual ao tamanho desejado 
    // (ao final temp terá o nó com o maior valor menor que tamanho)
    while (temp->proximo != NULL && tamanho > temp->proximo->tamanho) {
        temp = temp->proximo;
    }

    // Se o próxmo for nulo, não há espaço o suficiente, então apenas retorna valor de erro.
    // Caso contrário, remove o nó da lista e retorna a posição
    if (temp->proximo == NULL) {
        return -1;
    } else {
        long int pos = temp->proximo->posicao;

        sNo* aux = temp->proximo->proximo;
        free(temp->proximo);
        temp->proximo = aux;

        lista->tam--;

        return pos;
    }
}

long int listaRemoverInicio(sLista* lista) {
    if (lista->tam == 0) {
        return -1;
    }

    long int pos = lista->inicio->proximo->posicao;

    sNo* aux = lista->inicio->proximo->proximo;
    free(lista->inicio->proximo);
    lista->inicio->proximo = aux;

    lista->tam--;

    return pos;
}

/**
 * @brief  Libera recursivamente todos os nós a partir de um certo nó
 * @param  no: Inicio dos nós que serão liberados
 * @retval None
 */
void liberarNos(sNo* no) {
    if (no->proximo != NULL) {
        liberarNos(no->proximo);
    }

    free(no);
}

void listaLiberar(sLista** lista) {
    liberarNos((*lista)->inicio);
    free(*lista);
    lista = NULL;
}

int listaTamanho(sLista* lista) {
    if (lista != NULL) {
        return lista->tam;
    }
    
    return -1;
}

void listaImprimir(sLista* lista) {
    sNo* temp = lista->inicio;
    for (int i = 1; temp->proximo != NULL; i++) {
        printf("\tRegistro removido %d:\n", i);
        printf("Tamanho: %d \n", temp->proximo->tamanho);
        printf("Posição: %ld \n", temp->proximo->posicao);
        printf("\n");
        temp = temp->proximo;
    }
}
