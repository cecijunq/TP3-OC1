#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <cctype>

// class Bloco {
//     public:
//         int tag;
//         bool ocupado;
//     Bloco() { ocupado = false; }
// };

void imprime_saida(std::vector<std::vector<std::pair<int, int> > > &cache, int tam_conj, int n_linhas) {
    int index = 0;
    // int tam = 0;

    printf("================\nIDX V ** ADDR **\n");
    // tam = 0;
    for(int i = 0; i < n_linhas; i++) {
        for(int j = 0; j < tam_conj; j++) {
            if(cache[i][j].second == 1)
                // printf("%03d 1 %p\n", index, cache[i][j].first);
                printf("%03d 1 0x%08X\n", index, cache[i][j].first);
            else
                printf("%03d 0\n", index);
            index++;
            // tam++;
        }
    }
        // for (auto it = cache[i].begin(); it != cache[i].end(); it++) {
        //     printf("%03d 1 %x\n", index, *it);
        //     index++;
        //     tam++;
        // }
        // while(tam < tam_conj) {
        //     printf("%03d 0\n", index);
        //     index++;
        //     tam++;
        // }
    
}

/*

 # linhas = # blocos
 tamanho de uma linha = tamanho de um bloco

  O tamanho total da cache, em bytes. Se tivermos uma cache de 4KB, por exemplo, iremos digitar 4096.
  O tamanho de cada linha, em bytes. Uma linha de 1KB, por exemplo, seria entrada digitando-se 1024.
  O tamanho de cada grupo, em unidades. Pensando em uma memória cache de 4KB, e páginas (linhas) de 
 1KB, teremos 4 linhas. Se tivermos uma linha por grupo, teremos um sistema de mapeamento direto. Se 
 tivermos 4 por grupo, teremos um sistema de associatividade completa.

*/

int main(int argc, char *argv[]) {
    if(argc != 5) return 1;

    int tam_cache = std::stoi(argv[1]);
    int tam_linha = std::stoi(argv[2]); // corresponde ao tamanho de um bloco
    int tam_grupo = std::stoi(argv[3]);

    std::string arquivo = std::string(argv[4]);
    std::ifstream file(arquivo);

    int n_linhas = tam_cache / tam_linha; // corresponde ao número de blocos que cache tem
    int n_conjuntos_total = n_linhas / tam_grupo; // número de linhas que cache tem (define o index)

    int offset = static_cast<int>(log2(tam_linha));
    int index = static_cast<int>(log2(n_conjuntos_total));
    int tag = 32 - index - offset;

    std::vector<std::deque<std::pair<int, int> > > cache_fila(n_conjuntos_total);
    std::vector<std::vector<std::pair<int, int> > > _cache(n_conjuntos_total, std::vector<std::pair<int, int> >(tam_grupo, std::make_pair(0, 0))); // ocupado, tag

    // for(int i = 0; i < n_conjuntos_total; i++) {
    //     for(int j = 0; j < tam_grupo; j++) {
    //         printf("[%d %d] ", _cache[i][j].first, _cache[i][j].second);
    //     }
    //     printf("\n");
    // }

    printf("%d | %d | %d\n", tag, index, offset);

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

        // printf("number = %x\n", number);
        // printf("tag_end = %x\n", tag_end);
        // printf("aux = %x\n", aux);

        bool found = false;
        for (auto it = cache_fila[aux].begin(); it != cache_fila[aux].end(); ++it) {
            if (it->first == tag_end) {
                hits++;
                found = true;
                // Apaga o elemento da sua posição origial e adiciona-o novamente na última posição (pois foi o último elemento acessado)
                cache_fila[aux].erase(it);
                cache_fila[aux].push_back(std::make_pair(tag_end, cache_fila[aux].size()-1));
                break;
            }
        }

        if (!found) { // endereço não tá na memória cache
            // printf("tag_end = %x\n", tag_end);
            int pos_cache;
            if(cache_fila[aux].size() == 0) pos_cache = 0;
            else pos_cache = cache_fila[aux].size();
            misses++;
            if (cache_fila[aux].size() == tam_grupo) {
                pos_cache = cache_fila[aux].front().second;
                cache_fila[aux].pop_front(); // Retira o primeiro elemento inserido (que é o que tá na primeira posição)
            }
            // printf("tag_end = %x\n", tag_end);
            cache_fila[aux].push_back(std::make_pair(tag_end, cache_fila[aux].size())); // Insere um novo bloco na última posição da fila
            _cache[aux][pos_cache].first = tag_end;
            _cache[aux][pos_cache].second = 1;
        }
        // for(int i = 0; i < n_conjuntos_total; i++) {
        //     for(int j = 0; j < tam_grupo; j++) {
        //         printf("[%x %d] ", _cache[i][j].first, _cache[i][j].second);
        //     }
        //     printf("\n");
        // }
        // printf("\n\n");
        imprime_saida(_cache, tam_grupo, n_conjuntos_total);
    }

    file.close();
    printf("\n#hits = %d\n#misses = %d\n", hits, misses);

    return 0;
}