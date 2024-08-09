#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>

class Bloco {
    public:
        int tag;
        bool ocupado;
    Bloco() { ocupado = false; }
};

/*

 # linhas = # blocos
 tamanho de uma linha = tamanho de um bloco

  O tamanho total da cache, em bytes. Se tivermos uma cache de 4KB, por exemplo, iremos digitar 4096.
  O tamanho de cada linha, em bytes. Uma linha de 1KB, por exemplo, seria entrada digitando-se 1024.
  O tamanho de cada grupo, em unidades. Pensando em uma memória cache de 4KB, e páginas (linhas) de 
 1KB, teremos 4 linhas. Se tivermos uma linha por grupo, teremos um sistema de mapeamento direto. Se 
 tivermos 4 por grupo, teremos um sistema de associatividade completa.

*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
    if(argc != 5) return 1;

    int tam_cache = std::stoi(argv[1]);
    int tam_linha = std::stoi(argv[2]); // corresponds to the size of a block
    int tam_grupo = std::stoi(argv[3]);

    std::string arquivo = std::string(argv[4]);
    std::ifstream file(arquivo);

    int n_linhas = tam_cache / tam_linha; // corresponds to the number of blocks the cache has
    int n_conjuntos_total = n_linhas / tam_grupo; // number of lines the cache has (defines the index)

    int offset = static_cast<int>(log2(tam_linha));
    int index = static_cast<int>(log2(n_conjuntos_total));
    int tag = 32 - index - offset;

    // Using std::deque instead of std::queue
    std::vector<std::deque<int> > cache(n_conjuntos_total);

    printf("%d | %d | %d\n", tag, index, offset);

    if (!file.is_open()) {
        printf("Erro ao tentar abrir o arquivo\n");
        return 1;
    }

    std::string line;
    int hits = 0;
    int misses = 0;

    while (std::getline(file, line)) {
        std::stringstream ss;
        ss << std::hex << line; // Convert the line to hexadecimal

        unsigned int number;
        ss >> number;

        unsigned int aux = number;
        int tag_end = aux >> (offset + index);
        aux = aux >> offset;
        int mask = (1 << index) - 1;
        aux = aux & mask;

        bool found = false;
        for (auto it = cache[aux].begin(); it != cache[aux].end(); ++it) {
            if (*it == tag_end) {
                hits++;
                found = true;
                // Move the accessed block to the front for LRU (optional)
                cache[aux].erase(it);
                cache[aux].push_back(tag_end);
                break;
            }
        }

        if (!found) {
            misses++;
            if (cache[aux].size() >= tam_grupo) {
                cache[aux].pop_front(); // Evict the oldest block if the set is full
            }
            cache[aux].push_back(tag_end); // Insert the new block
        }
    }

    file.close();
    printf("hits = %d\nmisses = %d\n", hits, misses);

    return 0;
}