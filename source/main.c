// O trabalho anterior escolhido foi o de Henrique Matarazo Camillo 
//  Autor:
//  Nome: Henrique Matarazo Camillo
//  NUSP: 10294943
//  Nome: Gyovana Mayara Moriyama
//  NUSP: 10734387

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "funcionalidades.h"
#include "lista.h"
#include "escreverNaTela.h"

int main() {
    // Digite a opção de funcionalidade
    int opcao;
    scanf("%d", &opcao);

    // Parâmetros da funcionalidadades
    int n;
    bool isMerging = false;
    char *nomeDoArquivo, *nomeDoArquivo2, *nomeDoArquivoDeSaida;
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
            buscarRegistro(nomeDoArquivo, campoBusca, valorBusca, false);

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
            isMerging = true;
            //Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivo2 = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivoDeSaida = malloc(sizeof(char) * STRING_TAM_MAX);

            //Lê input
            scanf("%s %s %s", nomeDoArquivo, nomeDoArquivo2, nomeDoArquivoDeSaida);

            mergingAndMatchingRegistros(nomeDoArquivo, nomeDoArquivo2, nomeDoArquivoDeSaida, isMerging);

            //Libera memória    
            free(nomeDoArquivo);
            free(nomeDoArquivo2);
            free(nomeDoArquivoDeSaida);
            break;

        // Funcionalidade 9:
        case 9:
            //Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivo2 = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivoDeSaida = malloc(sizeof(char) * STRING_TAM_MAX);

            //Lê input
            scanf("%s %s %s", nomeDoArquivo, nomeDoArquivo2, nomeDoArquivoDeSaida);
            
            mergingAndMatchingRegistros(nomeDoArquivo, nomeDoArquivo2, nomeDoArquivoDeSaida, isMerging);
            
            //Libera memória    
            free(nomeDoArquivo);
            free(nomeDoArquivo2);
            free(nomeDoArquivoDeSaida);
            break;

        // Funcionalidade 10: Cria um arquivo de índices para um arquivo que contém registros
        case 10:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivoDeSaida = malloc(sizeof(char) * STRING_TAM_MAX);

            // Lê input
            scanf("%s %s", nomeDoArquivo, nomeDoArquivoDeSaida);

            // Realiza a funcionalidade
            gerarArquivoDeIndices(nomeDoArquivo, nomeDoArquivoDeSaida);

            // Libera memória
            free(nomeDoArquivo);
            free(nomeDoArquivoDeSaida);

            break;

        // Funcionalidade 11:
        case 11:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivo2 = malloc(sizeof(char) * STRING_TAM_MAX);
            campoBusca = malloc(sizeof(char) * STRING_TAM_MAX);
            valorBusca = malloc(sizeof(char) * STRING_TAM_MAX);

            // Lê input
            scanf("%s %s %s %[^\r\n]", nomeDoArquivo, nomeDoArquivo2, campoBusca, valorBusca);
            
            // Realiza a funcionalidade
            buscarPeloArquivoDeIndices(nomeDoArquivo, nomeDoArquivo2, campoBusca, valorBusca, false);

            // Libera memória
            free(nomeDoArquivo);
            free(nomeDoArquivo2);
            free(campoBusca);
            free(valorBusca);

            break;

        // Funcionalidade 12:
        case 12:
            removerListaDeIndices();
            break;
            
        // Funcionalidade 13:
        case 13:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivo2 = malloc(sizeof(char) * STRING_TAM_MAX);

            // Lê input
            scanf("%s %s %d", nomeDoArquivo, nomeDoArquivo2, &n);

            // Realiza a funcionalidade
            inserirRegistrosComIndice(nomeDoArquivo, nomeDoArquivo2, n);

            // Libera memória
            free(nomeDoArquivo);
            free(nomeDoArquivo2);

            break;

        // Funcionalidade 14:
        case 14:
            // Aloca memória
            nomeDoArquivo = malloc(sizeof(char) * STRING_TAM_MAX);
            nomeDoArquivo2 = malloc(sizeof(char) * STRING_TAM_MAX);
            campoBusca = malloc(sizeof(char) * STRING_TAM_MAX);
            valorBusca = malloc(sizeof(char) * STRING_TAM_MAX);

            // Lê input
            scanf("%s %s %s %[^\r\n]", nomeDoArquivo, nomeDoArquivo2, campoBusca, valorBusca);
            
            // Realiza a funcionalidade
            compararBuscas(nomeDoArquivo, nomeDoArquivo2, campoBusca, valorBusca);

            // Libera memória
            free(nomeDoArquivo);
            free(nomeDoArquivo2);
            free(campoBusca);
            free(valorBusca);

            break;
            
        // Funcionalidade inválida
        default:
            printf("Opção inválida\n");
    }
    
    return 0;
}