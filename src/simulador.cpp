#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>

// class Bloco {
//     public:
//         int tag;
//         bool ocupado;
//     Bloco() { ocupado = false; }
// };

void imprime_saida(std::vector<std::deque<int> > &cache, int tam_conj, int n_linhas) {
    int index = 0;
    int tam = 0;

    // printf("--> %d\n", n_linhas);
    // printf("--> %d\n", tam_conj);
    printf("================\nIDX V ** ADDR **\n");
    for(int i = 0; i < n_linhas; ++i) {
        tam = 0;
        for (auto it = cache[i].begin(); it != cache[i].end(); it++) {
            printf("%03d 1 %x\n", index, *it);
            index++;
            tam++;
        }
        // printf("--> tam = %d\n", tam);
        while(tam < tam_conj) {
            printf("%03d 0\n", index);
            index++;
            tam++;
        }
    }
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

    std::vector<std::deque<int> > cache(n_conjuntos_total);
    // std::vector<std::deque<Bloco> > cache(n_conjuntos_total);

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
        int tag_end = aux >> (offset + index);
        aux = aux >> offset;
        int mask = (1 << index) - 1;
        aux = aux & mask;

        bool found = false;
        // Bloco novo(tag_end);
        for (auto it = cache[aux].begin(); it != cache[aux].end(); ++it) {
            // if (it->ocupado && it->tag == tag_end) {
            if (*it == tag_end) {
                hits++;
                found = true;
                // Apaga o elemento da sua posição origial e adiciona-o novamente na última posição (pois foi o último elemento acessado)
                cache[aux].erase(it);
                cache[aux].push_back(tag_end);
                // cache[aux].push_back(novo);
                break;
            }
        }

        if (!found) { // endereço não tá na cache
            misses++;
            if (cache[aux].size() == tam_grupo) {
                cache[aux].pop_front(); // Retira o primeiro elemento inserido (que é o que tá na primeira posição)
            }
            cache[aux].push_back(tag_end); // Insere um novo bloco na última posição da fila
            // cache[aux].push_back(novo); // Insere um novo bloco na última posição da fila
        }
        imprime_saida(cache, tam_grupo, n_conjuntos_total);
    }

    file.close();
    printf("hits = %d\nmisses = %d\n", hits, misses);

    return 0;
}