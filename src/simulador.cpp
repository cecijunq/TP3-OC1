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
        fprintf(stderr, "Erro ao tentar abrir o arquivo\n");
    }
}

/*void print_saida(std::vector<std::vector<std::pair<int, int> > > &cache, int tam_conj, int n_linhas) {
    int index = 0;

    printf("================\nIDX V ** ADDR **\n");
    for(int i = 0; i < n_linhas; i++) {
        for(int j = 0; j < tam_conj; j++) {
            if(cache[i][j].second == 1)
                printf("%03d 1 0x%08X\n", index, cache[i][j].first);
            else
                printf("%03d 0\n", index);
            index++;
        }
    }
}*/

int main(int argc, char *argv[]) {
    if(argc != 5) return 1;
    printf("%d\n", argc);

    FILE *outfile = fopen("output.txt", "w");
    if (outfile == NULL) {
        fprintf(stderr, "Error: Could not open file.\n");
        return 1;
    }

    int tam_cache = std::stoi(argv[1]); // armazena a quantidade de bytes que a cache consegue armazenar
    int tam_linha = std::stoi(argv[2]); // corresponde ao tamanho de um bloco
    int tam_grupo = std::stoi(argv[3]); // indica o tamanho do conjunto

    std::string arquivo = std::string(argv[4]);
    std::ifstream file(arquivo);

    int n_linhas = tam_cache / tam_linha; // corresponde ao número de blocos que cache tem
    int n_conjuntos_total = n_linhas / tam_grupo; // número de linhas que cache tem (define o index)

    int offset = static_cast<int>(log2(tam_linha));
    int index = static_cast<int>(log2(n_conjuntos_total));
    // int tag = 32 - index - offset;

    std::vector<std::deque<std::pair<int, int> > > cache_fila(n_conjuntos_total);
    std::vector<std::vector<std::pair<int, int> > > _cache(n_conjuntos_total, std::vector<std::pair<int, int> >(tam_grupo, std::make_pair(0, 0))); // ocupado, tag

    // printf("%d | %d | %d\n", tag, index, offset);

    if (!file.is_open()) {
        printf("Erro ao tentar abrir o arquivo\n");
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

        unsigned int aux = number;
        // int tag_end = aux >> (offset + index);
        int tag_end = aux >> (offset);
        aux = aux >> offset;
        // aux = aux >> (offset + index);
        int mask = (1 << index) - 1;
        aux = aux & mask;

        // printf("--> %d\n", aux);

        bool found = false;
        for (auto it = cache_fila[aux].begin(); it != cache_fila[aux].end(); ++it) {
            if (it->first == tag_end) {
                hits++;
                found = true;
                // Apaga o elemento da sua posição original e adiciona-o novamente na última posição
                //(pois foi o último elemento acessado)
                //Removidas, fazem parte de outra política (LRU)
                //cache_fila[aux].erase(it);
                //cache_fila[aux].push_back(std::make_pair(tag_end, cache_fila[aux].size()-1));
                break;
            }
        }

        if (!found) { // endereço não tá na memória cache
            int pos_cache;
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

            // Insere um novo bloco na última posição da fila
            cache_fila[aux].push_back(std::make_pair(tag_end, cache_fila[aux].size())); 
            _cache[aux][pos_cache].first = tag_end;
            _cache[aux][pos_cache].second = 1;
        }

        //Escreve no arquivo o estado atual da cache
        imprime_saida(outfile, _cache, tam_grupo, n_conjuntos_total);
    }

    file.close();

    fprintf(outfile, "\n#hits: %d\n", hits);
    fprintf(outfile, "#miss: %d\n", misses);
    fclose(outfile);

    return 0;
}