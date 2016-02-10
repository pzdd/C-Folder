#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#define CONS 256
#define TAM_ARQUIVO 512
using namespace std;

//Passo 1
struct arquivoDados
{
    int locaisProvaDisponiveis;
    int quantidadeParticipantes;
    int quantidadeMedianas;
    int *capacidadeLocaisProva;
    int **tempoMedioDeslocamento;
};
//Passo 2
struct solucao
{
    int arrayAlocacaoCanditatos[CONS];
    int medianas[CONS];
    int alocados[CONS];
    int custo;
};
//Passo 7
void save(char * arquivo,solucao solution,arquivoDados arq);
//Passo 6
solucao localSearch(solucao solution,arquivoDados arq);
//Passo 5
solucao pertubacao(solucao solution,arquivoDados arq);
//Passo 4
int calculaCusto(solucao solution,arquivoDados arq);
//Passo 3
solucao initSolution(arquivoDados arq);
//Passo 1
arquivoDados openFILE(char* arquivo);

//Passo 8
int main()
{

    char arquivoEntrada[35],arquivoSaida[35];
    int tempoExecucao;

    cout << "Nome do arquivo que contém os dados de entrada" << endl;
    cin >> arquivoEntrada;
    cout << "Tempo total em segundos a ser considerado para execução do algoritmo" << endl;
    cin >> tempoExecucao;
        cout << "Nome do arquivo de saida a ser gerado" << endl;
    cin >> arquivoSaida;

    arquivoDados arq = openFILE(arquivoEntrada);
    solucao solution = initSolution(arq);
    solucao solution1 = localSearch(solution,arq);
    int custoSolution1 = calculaCusto(solution1,arq);
    int custoSolution;
    for(int i=0;i<(tempoExecucao*100);i++)
        {
        solution = pertubacao(solution1,arq);
        solution = localSearch(solution,arq);
        custoSolution = calculaCusto(solution,arq);
        if(custoSolution < custoSolution1)
        {
            solution1 = solution;

        }
    }
    cout << solution.custo << endl;
    save(arquivoSaida,solution,arq);

    return 0;
}

arquivoDados openFILE(char* arquivo)
{
    int tamanho[TAM_ARQUIVO];
    arquivoDados arq;
    ifstream read(arquivo);
    if(!read.is_open())
        {
        cout <<"Falha ao abrir o arquivo!" <<endl;
        exit(1);
    }
    int i=0;
    while(!read.eof())
    {
        read >> tamanho[i];
        i++;
    }
    arq.locaisProvaDisponiveis = tamanho[0];
    arq.quantidadeParticipantes = tamanho[1];
    arq.quantidadeMedianas = tamanho[2];
    int offset = 3 + arq.locaisProvaDisponiveis;
    arq.capacidadeLocaisProva = (int *)malloc(arq.locaisProvaDisponiveis*sizeof(int));
    arq.tempoMedioDeslocamento = (int **)malloc(arq.quantidadeParticipantes*sizeof(int));
    for(int i=0;i<arq.quantidadeParticipantes;i++){
        arq.tempoMedioDeslocamento[i] = (int*) malloc(arq.locaisProvaDisponiveis*sizeof(int));
        for(int j=0;j<arq.locaisProvaDisponiveis;j++){
            arq.tempoMedioDeslocamento[i][j] = tamanho[offset];
            offset ++;
        }
    }
    for(int i=0;i<arq.locaisProvaDisponiveis;i++)
    {
        arq.capacidadeLocaisProva[i] = tamanho[3+i];
    }
    read.close();
    return arq;
}


solucao initSolution(arquivoDados arq)
{
    int p;
    bool stop = false;
    int capacidadetotal = 0;
    int totalCandidatos=0;
    int medianaRand;
    solucao solution;
    srand (time(NULL));
    while(!stop)
    {
        stop = true;
        capacidadetotal = 0;
        for(int i=0;i<arq.quantidadeMedianas;i++)
        {
            solution.medianas[i] = -1;
        }
        for(int i=0;i<arq.quantidadeMedianas;i++)
        {
            solution.alocados[i] = 0;
        }
        int i=0;

        while(capacidadetotal < arq.quantidadeParticipantes)
        {
            p = rand() % arq.locaisProvaDisponiveis;
            capacidadetotal = capacidadetotal + arq.capacidadeLocaisProva[p];
            solution.medianas[i] = p;
            i++;
        }
        int equals[arq.locaisProvaDisponiveis];
        for(int i=0;i<arq.locaisProvaDisponiveis;i++){
            equals[i] = 0;
        }
        for(int i=0;i<arq.quantidadeMedianas;i++)
        {
            equals[solution.medianas[i]] = equals[solution.medianas[i]] + 1 ;
        }
        for(int i=0;i<arq.locaisProvaDisponiveis;i++)
        {
            if(equals[i] > 1 )
            {
                stop = false;
            }
         }
    }

    while(totalCandidatos < arq.quantidadeParticipantes)
    {
        medianaRand = rand() % arq.quantidadeMedianas;
        if(solution.medianas[medianaRand] != -1)
        {
                if(solution.alocados[medianaRand] < arq.capacidadeLocaisProva[solution.medianas[medianaRand]])
                {
                    solution.arrayAlocacaoCanditatos[totalCandidatos] = solution.medianas[medianaRand];
                    solution.alocados[medianaRand] = solution.alocados[medianaRand] + 1;
                    totalCandidatos++;
                }
        }
    }

    return solution;
}
int calculaCusto(solucao solution,arquivoDados arq)
{
    solution.custo = 0;
    for(int i=0;i<arq.quantidadeParticipantes;i++)
    {
        solution.custo = solution.custo + arq.tempoMedioDeslocamento[i][solution.arrayAlocacaoCanditatos[i]];
    }
    return solution.custo;
}
solucao pertubacao(solucao solution,arquivoDados arq)
{
    int medianaRand = rand() % arq.quantidadeMedianas;
    int somaCapacidade = 0;
    int totalCand = 0;
    int mRand;
    while(somaCapacidade < arq.quantidadeParticipantes)
        {
        int novoLocal = rand() % arq.locaisProvaDisponiveis;
        int equals[arq.locaisProvaDisponiveis];

        for(int i=0;i<arq.locaisProvaDisponiveis;i++){
            equals[i] = -1;
        }
        for(int j=0;j<arq.quantidadeMedianas;j++)
        {
            equals[solution.medianas[j]] = equals[solution.medianas[j]] + 2;
        }
        do
        {
            novoLocal = rand() % arq.locaisProvaDisponiveis;
        }while(equals[novoLocal] != -1);

        solution.medianas[medianaRand] = novoLocal;

        for(int i=0;i<arq.quantidadeMedianas;i++)
            {
                if(solution.medianas[i] != -1)
                {
                    somaCapacidade = somaCapacidade + arq.capacidadeLocaisProva[solution.medianas[i]];
                }
            }
        }
        for(int i=0;i<arq.quantidadeMedianas;i++)
        {
            solution.alocados[i] = 0;
        }
        while(totalCand < arq.quantidadeParticipantes)
        {
            mRand = rand() % arq.quantidadeMedianas;
            if(solution.medianas[mRand] != -1)
            {
                    if(solution.alocados[mRand] < arq.capacidadeLocaisProva[solution.medianas[mRand]])
                    {
                        solution.arrayAlocacaoCanditatos[totalCand] = solution.medianas[mRand];
                        solution.alocados[mRand] = solution.alocados[mRand] + 1;
                        totalCand++;
                    }
            }
        }
     return solution;
}

solucao localSearch(solucao solution,arquivoDados arq)
{
    int swaap;
    int custoAtual = solution.custo;
    int custoMelhor = 0;
    for(int i=0;i<arq.quantidadeParticipantes;i++)
    {
        for(int j=1;j<arq.quantidadeParticipantes;j++)
        {
            swaap = solution.arrayAlocacaoCanditatos[i];
            solution.arrayAlocacaoCanditatos[i] = solution.arrayAlocacaoCanditatos[j];
            solution.arrayAlocacaoCanditatos[j] = swaap;
            custoMelhor = calculaCusto(solution,arq);
            if(custoMelhor < solution.custo)
            {
               solution.custo = custoMelhor;
            }else
            {
                swaap = solution.arrayAlocacaoCanditatos[i];
                solution.arrayAlocacaoCanditatos[i] = solution.arrayAlocacaoCanditatos[j];
                solution.arrayAlocacaoCanditatos[j] = swaap;
            }
        }
    }

    return solution;
}
void save(char * arquivo,solucao solution, arquivoDados arq)
{
    ofstream write(arquivo);
    if(!write.is_open() )
    {
        cout <<"Falha ao abrir o arquivo !" << endl;
        exit(1);
    }
    write << "Custo: " << solution.custo <<endl;
    write << "Locais de prova: ";
    for(int i=0;i<arq.quantidadeMedianas;i++)
    {
        write << solution.medianas[i] << " ";
    }
    write << endl << "Alocação dos candidatos: " << endl;
    for(int i=0;i<arq.quantidadeParticipantes;i++)
    {
        write << "Candidato " << i+1 << " faz prova em " << solution.arrayAlocacaoCanditatos[i] << endl;
    }
    write.close();
}
