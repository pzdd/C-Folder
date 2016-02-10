#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#define MAX 300

using namespace std;

/*  Passo 1 : adquirindo os dados de entrada */

struct dados{
    int locais;
    int participantes;
    int medianas;
    int *capacidade;
    int **temp_medio_desc;
};

dados abrir(char* nome){
    int n[MAX];
    ifstream ler(nome);
    if(!ler.is_open()){
        cout <<"Falha ao abrir o arquivo!" <<endl;
        exit(1);
    }
    int i=0;
    while(!ler.eof()){
        ler >> n[i];
        i++;
    }
    //preenche a estrutura dados com as informações lidas do arquivo
    dados d;
    d.locais = n[0];
    d.participantes = n[1];
    d.medianas = n[2];
    d.capacidade = (int *)malloc(d.locais*sizeof(int));
    d.temp_medio_desc = (int **)malloc(d.participantes*sizeof(int));
    for(int i=0;i<d.locais;i++){
        d.capacidade[i] = n[3+i];
    }
    int cont= 3 + d.locais;
    for(int i=0;i<d.participantes;i++){
        d.temp_medio_desc[i] = (int*)malloc(d.locais*sizeof(int));
        for(int j=0;j<d.locais;j++){
            d.temp_medio_desc[i][j] = n[cont];
            cont ++;
        }
    }
    ler.close();
    //retornar a estrutura com os dados preenchidos
    return d;
}

/*  Passo 2 : representando uma solução */

struct solucao{
    int alocacao_candidatos[MAX];
    int medianas_utilizadas[MAX];
    int total_aloc[MAX];
    int custo;
};

/* Passo 3 : Obtendo um solução inicial para o problema */

solucao solucao_inicial(dados retorno){
    srand (time(NULL));
    int p_locais_de_prova;
    solucao s;
    //esse while trata o caso de haver números repetidos
    while(true){
        bool parada = true;
        //inicializando as medianas para pode tratar valores de lixo na memoria
        for(int i=0;i<retorno.medianas;i++){
            s.medianas_utilizadas[i] = -1;
        }
        for(int i=0;i<retorno.medianas;i++){
            s.total_aloc[i] = 0;
        }
        int soma_capacidade=0;
        int i=0;
        while(soma_capacidade < retorno.participantes){
            p_locais_de_prova = rand() % retorno.locais;
            soma_capacidade += retorno.capacidade[p_locais_de_prova];
            s.medianas_utilizadas[i] = p_locais_de_prova;
            i++;
        }
        int vetor_repetido[retorno.locais];
        for(int i=0;i<retorno.locais;i++){
            vetor_repetido[i] = 0;
        }
        for(int i=0;i<retorno.medianas;i++){
            vetor_repetido[s.medianas_utilizadas[i]] += 1 ;
        }
         for(int i=0;i<retorno.locais;i++){
            if(vetor_repetido[i] > 1 ){
                parada = false;
            }
         }
         if(parada){
            break;
         }
    }
    int contador_total=0,local;
    while(contador_total <= (retorno.participantes-1)){
        local = rand() % retorno.medianas;
        //caso -1 que dizer que aquela mediana não foi preenchida
        if(s.medianas_utilizadas[local] != -1){
                if(s.total_aloc[local] < retorno.capacidade[s.medianas_utilizadas[local]]){
                    s.alocacao_candidatos[contador_total] = s.medianas_utilizadas[local];
                    s.total_aloc[local] += 1;
                    contador_total++;
                }
        }
    }
     return s;
}

/* Passo 4 : Avaliando a solução gerada */

int custo(solucao s,dados retorno){
    s.custo = 0;
    for(int i=0;i<retorno.participantes;i++){
        s.custo += retorno.temp_medio_desc[i][s.alocacao_candidatos[i]];
    }
    return s.custo;
}

/* Passo 5 : Pertubando uma solução */

solucao pertubacao(solucao s,dados d){
    int local_rand = rand() % d.medianas;
    int sum = 0;
    while(sum < d.participantes){
        int novo_local = rand() % d.locais;
        //logica para ver quais locais podem ser utilizados
        int aux[d.locais];
        for(int j=0;j<d.locais;j++){
            aux[j] = -1;
        }
        for(int j=0;j<d.medianas;j++){
            aux[s.medianas_utilizadas[j]] += 2;
        }
        while(aux[novo_local] != -1){
            novo_local = rand() % d.locais;
        }

        s.medianas_utilizadas[local_rand] = novo_local;

        for(int i=0;i<d.medianas;i++){
                if(s.medianas_utilizadas[i] != -1){
                    sum += d.capacidade[s.medianas_utilizadas[i]];
                }
            }
        }
        for(int i=0;i<d.medianas;i++){
            s.total_aloc[i] = 0;
        }
        int contador_tot = 0;
        int local;
        while(contador_tot <= (d.participantes-1)){
            local = rand() % d.medianas;
            //caso -1 que dizer que aquela mediana não foi preenchida
            if(s.medianas_utilizadas[local] != -1){
                    if(s.total_aloc[local] < d.capacidade[s.medianas_utilizadas[local]]){
                        s.alocacao_candidatos[contador_tot] = s.medianas_utilizadas[local];
                        s.total_aloc[local] += 1;
                        contador_tot++;
                    }
            }
    }
     return s;
}

/* Passo 6: busca local */

solucao busca_local(solucao s,dados d){
    int aux;
    int custo_antigo = s.custo;
    int novo_custo = 0;
    for(int i=0;i<d.participantes;i++){
        for(int j=1;j<d.participantes;j++){
            aux = s.alocacao_candidatos[i];
            s.alocacao_candidatos[i] = s.alocacao_candidatos[j];
            s.alocacao_candidatos[j] = aux;
            novo_custo = custo(s,d);
            //caso seja melhor mantem
            if(novo_custo < s.custo){
                s.custo = novo_custo;
                //caso não seja melhor desfaz a troca
            }else if(novo_custo >= s.custo){
                aux = s.alocacao_candidatos[i];
                s.alocacao_candidatos[i] = s.alocacao_candidatos[j];
                s.alocacao_candidatos[j] = aux;
            }
        }
    }
    return s;
}

/* Passo 7: salvando a solução obtida em um arquivo */

void salvar(char * nome_arquivo,solucao s,dados d){
    ofstream escreve(nome_arquivo);
    if(!escreve.is_open() ){
        cout <<"Falha ao abrir o arquivo " << nome_arquivo <<endl;
        exit(1);
    }
    escreve << "Custo: " << s.custo <<endl;
    escreve << "Locais de prova: ";
    for(int i=0;i<d.medianas;i++){
        escreve << s.medianas_utilizadas[i] << " ";
    }
    escreve << endl << "Alocação dos candidatos: " << endl;
    for(int i=0;i<d.participantes;i++){
        escreve << "Candidato " << i+1 << " faz prova em " << s.alocacao_candidatos[i] << endl;
    }
    escreve << endl;
    escreve.close();
}

/* Passo 8 : função main */

int main(){

    char in[50],out[50];
    int tempo;
    cout << "Informe " << endl;
    cout << "Nome do arquivo que contém os dados de entrada" << endl;
    cin >> in;
    cout << "Tempo total em segundos a ser considerado para execução do algoritmo" << endl;
    cin >> tempo;
    cout << "Processando..." << endl;

    dados retorno = abrir(in);
    //gera solução inicial
    solucao s = solucao_inicial(retorno);
    solucao s2 = busca_local(s,retorno);
    int custo_s2 = custo(s2,retorno);
    int custo_s;
    clock_t goal = ( tempo*1000 + clock() );
    while(goal > clock()){
        s = pertubacao(s2,retorno);
        s = busca_local(s,retorno);
        custo_s = custo(s,retorno);
        if(custo_s < custo_s2){
            s2 = s;
        }
    }
    cout << s.custo << endl;
    cout << "Nome do arquivo de saida a ser gerado" << endl;
    cin >> out;
    salvar(out,s,retorno);


    return 0;
}
