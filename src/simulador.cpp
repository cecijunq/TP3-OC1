#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <cctype>

void imprime_saida(FILE *outfile, std::vector<std::vector<std::pair<int, int> > > &cache, int tam_conj, int n_linhas) {
    int index = 0;

    // Checa se o arquivo de escrita está aberto
    if (outfile != NULL) {
        fprintf(outfile, "================\nIDX V ** ADDR **\n");
        for (int i = 0; i < n_linhas; i++) {
            for (int j = 0; j < tam_conj; j++) {
                if (cache[i][j].second == 1)
                    fprintf(outfile, "%03d 1 0x%08X\n", index, cache[i][j].first);
                else
                    fprintf(outfile, "%03d 0\n", index);
                index++;
            }
        }
    } else {
        printf("Erro ao tentar abrir o arquivo de saída.\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        printf("Erro! Quantidade de argumentos inválida, consulte instruções de execução em \"Documentação.txt\".\n");
        return 1;
    }
    FILE *outfile = fopen("output.txt", "w");
    if (outfile == NULL) {
        printf("Erro ao tentar abrir o arquivo de saída.\n");
        return 1;
    }

    int tam_cache = std::stoi(argv[1]); // armazena a quantidade de bytes que a cache consegue armazenar
    int tam_linha = std::stoi(argv[2]); // corresponde ao tamanho de um bloco
    long unsigned int tam_grupo = std::stoi(argv[3]); // indica o tamanho do conjunto

    std::string arquivo = std::string(argv[4]);
    std::ifstream file(arquivo);

    int n_linhas = tam_cache / tam_linha; // corresponde ao número de blocos que cache tem
    int n_conjuntos_total = n_linhas / tam_grupo; // número de linhas que cache tem (define o index)

    int offset = static_cast<int>(log2(tam_linha)); //tamanho do offset
    int index = static_cast<int>(log2(n_conjuntos_total)); //tamanho do index

    // Vetor de filas, indica a prioridade de saída de cada grupo
    std::vector<std::deque<std::pair<int, int> > > cache_fila(n_conjuntos_total);

    //Matriz que representa as posições da cache
    std::vector<std::vector<std::pair<int, int> > > _cache(n_conjuntos_total, std::vector<std::pair<int, int> >(tam_grupo, std::make_pair(0, 0))); 

    if (!file.is_open()) {
        printf("Erro ao tentar abrir o arquivo de entrada.\n");
        return 1;
    }

    std::string line;
    int hits = 0;
    int misses = 0;

    while(std::getline(file, line)) {
        std::stringstream ss;
        ss << std::hex << line; // Converte a linha para hexadecimal

        unsigned int number;
        ss >> number;

        unsigned int aux = number; // Será igual ao index

        int tag_end = aux >> (offset); // tag_end = tag + index
        aux = aux >> offset;           // Removemos offset do aux

        int mask = (1 << index) - 1;   // Selecionando bits referentes ao index
        aux = aux & mask;              // Aux = index

        bool found = false;
        for (auto it = cache_fila[aux].begin(); it != cache_fila[aux].end(); ++it) { //Busca pela matrix cache pelo elemento
            if (it->first == tag_end) { 
                //Se encontrado, marca como hit
                hits++;
                found = true;
                break;
            }
        }

        if (!found) { // endereço não tá na memória cache
            int pos_cache; // Variável auxiliar que armazena o índice da matriz de cache onde será armazenado o bloco
            misses++;

            // Confere se a cache é vazia, fácil inserção
            if(cache_fila[aux].size() == 0) pos_cache = 0;
            else pos_cache = cache_fila[aux].size();
            
            // Se a cache está cheia, confere a fila e remove o mais antigo
            // First-in First-out
            if (cache_fila[aux].size() == tam_grupo) {
                pos_cache = cache_fila[aux].front().second;
                cache_fila[aux].pop_front(); // Retira o primeiro elemento inserido (está na primeira posição)
            }

            // Insere o novo bloco na última posição da fila, e o insere na cache
            cache_fila[aux].push_back(std::make_pair(tag_end, cache_fila[aux].size())); 
            _cache[aux][pos_cache].first = tag_end;
            _cache[aux][pos_cache].second = 1;
            
            //Escreve no arquivo o estado atual da cache --> A IMPRESSÃO SÓ É FEITA QUANDO OCORRE UMA MUDANÇA NA CACHE
            imprime_saida(outfile, _cache, tam_grupo, n_conjuntos_total);
        }

    }

    file.close();

    //Imprime somatória de prints e misses
    fprintf(outfile, "\n#hits: %d\n", hits);
    fprintf(outfile, "#miss: %d\n", misses);
    fclose(outfile);

    return 0;
}