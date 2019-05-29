//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
#include "lista.h"
#include "escreverNaTela.h"

int main() {
    // Digite a opção de funcionalidade
    int opcao;
    scanf("%d", &opcao);
    // Parâmetros da funcionalidadades
    char *nomeDoArquivo, *nomeDoArquivoDeSaida;
    char *campoBusca, *valorBusca;

    // Checa qual foi a opção digitada pelo usuário, lê os parâmetros (dependendo da funcionalidade), e chama a função.
    switch (opcao) {
        // Funcionalidade 1: Converter arquivo csv para arquivo binário
        case 1:
            // Aloca memória e lê entrada
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX); 
            scanf("%s", nomeDoArquivo);

            // Realiza a funcionalidade e libera memória alocada
            csvParaBin(nomeDoArquivo);
            free(nomeDoArquivo);

            break;

        // Funcionalidade 2: Imprimir todos os registros de um arquivo binário
        case 2:
            // Aloca memória e lê entrada
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX); 
            scanf("%s", nomeDoArquivo);

            // Realiza a funcionalidade e libera memória alocada
            imprimirBin(nomeDoArquivo);            
            free(nomeDoArquivo);

            break;

        // Funcionalidade 3: Buscar regsitros em um arqruivo binário
        case 3:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            campoBusca = malloc(sizeof(char) * STRING_TAM_MAX);
            valorBusca = malloc(sizeof(char) * STRING_TAM_MAX);
            
            // Lê input
            scanf("%s %s %[^\r\n]", nomeDoArquivo, campoBusca, valorBusca);

            // Realiza a funcionalidade
            buscarRegistro(nomeDoArquivo, campoBusca, valorBusca);

            // Libera memória alocada
            free(nomeDoArquivo);
            free(campoBusca);
            free(valorBusca);

            break;

        // Funcionalidade 4: Remover registros de um arquivos binário
        case 4:
            remocaoDeRegistros();
            break;

        // Funcionalidade 5; Adicionar registro em um arquivo binário
        case 5:
            insercaoDeRegistros();
            break;

        // Funcionalidade 6: Atualizar regsitros de um arquivo binário
        case 6:
            atualizacaoDeRegistros();
            break;
        
        // Funcionalidade 7: Ordena os registros de um arquivo binário
        case 7:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivoDeSaida = malloc(sizeof(char) * STRING_TAM_MAX);

            // Lê input
            scanf("%s %s", nomeDoArquivo, nomeDoArquivoDeSaida);

            // Realiza a funcionalidade
            ordenarRegistros(nomeDoArquivo, nomeDoArquivoDeSaida);

            // Libera memória
            free(nomeDoArquivo);
            free(nomeDoArquivoDeSaida);

            break;

        // Funcionalidade 8:
        case 8:
            break;

        // Funcionalidade 9:
        case 9:
            break;

        // Funcionalidade inválida
        default:
            printf("Opção inválida\n");
    }
    
    return 0;
}