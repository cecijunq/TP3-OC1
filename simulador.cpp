#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

class Bloco {
    public:
        int tag;
        int ocupado;
    Bloco() { ocupado = 0; }
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

int main(int argc, char *argv[]) {
    //if(argc != 5) return 1;

    int tam_cache = std::stoi(argv[1]);
    int tam_linha = std::stoi(argv[2]); // corresponde ao tamanho de um bloco
    int tam_grupo = std::stoi(argv[3]);

    std::string arquivo = std::string(argv[4]);
    std::ifstream file(arquivo);
    printf("%d %d %d %s\n", tam_cache, tam_grupo, tam_linha, arquivo.c_str());

    unsigned int x = 0xDEADBEEF;
    std::cout << x << std::endl;

    int n_linhas = tam_cache / tam_linha; // corresponde ao número de blocos que a cache tem
    int n_conjuntos_total = n_linhas / tam_grupo; // número de linhas que a cache tem (define o index)

    int offset = log2(tam_linha);
    int index = log2(n_conjuntos_total);
    int tag = 32 - index - offset;

    std::vector<std::vector<Bloco> > cache(n_conjuntos_total, std::vector<Bloco>(tam_grupo, Bloco())); // cache[n_conjuntos_total][tam_grupo];

    for(int i = 0; i < n_conjuntos_total; i++) {
        for(int j = 0; j < tam_grupo; j++) {
            printf("%d ", cache[i][j].ocupado);
        }
        printf("\n");
    }

    printf("%d | %d | %d\n", tag, index, offset);

    if (!file.is_open()) { // Se o arquivo foi aberto... erro
        printf("Erro ao tentar abrir o arquivo\n");
        return 1;
    }

    std::vector<int> endereco;
    std::string line;
    while (std::getline(file, line)) { // Lê o arquivo linha a linha
        std::stringstream ss;
        ss << std::hex << line; // Converte a linha para hexadecimal
        // unsigned int number;
        // ss >> number;
        // std::cout << "Hexadecimal: " << line << " Decimal: " << number << std::endl;
    
        for(int i = 0; i < endereco.size(); i++) {
            printf("%d", endereco[i]);
        }
        printf("\n");
    }

    file.close(); // Fecha o arquivo

    return 0;
}