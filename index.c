#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <time.h>

#define ARQUIVO_PRODUTOS "dbProduto.txt"
#define ARQUIVO_DESTINOS "dbDestino.txt"

typedef struct {
    char nomeProduto[50];
    long qtdProduto;         
    char dataValidade[11];  
    int vendasPorDia;
} Produto;

typedef struct {
    char nomeDestino[50];
    long qtdProduto;         
} Destino;

FILE *arquivo;
FILE *arquivoDestino;

void cadastrarProduto() {
    Produto produto;
    arquivo = fopen(ARQUIVO_PRODUTOS, "a+");
    
    if (arquivo == NULL) {
        printf("\nErro ao abrir o banco de dados.\n");
        return;
    }
    
    printf("\nDigite o nome do produto: ");
    fflush(stdin); 
    scanf(" %[^\n]s", produto.nomeProduto);
    
    printf("\nDigite a quantidade: ");
    while (scanf("%ld", &produto.qtdProduto) != 1) {
        printf("Valor inválido. Tente novamente: ");
        fflush(stdin);
    }

    printf("\nDigite a data de validade (YYYY-MM-DD): ");
    scanf("%10s", produto.dataValidade); 

    printf("\nDigite a quantidade de vendas por dia: ");
    while (scanf("%d", &produto.vendasPorDia) != 1) {
        printf("Valor inválido. Tente novamente: ");
        fflush(stdin);
    }
    
    fwrite(&produto, sizeof(produto), 1, arquivo);
    printf("\nProduto cadastrado com sucesso.\n");
    
    fclose(arquivo);
}

void cadastrarDestino() {
    Destino destino;
    arquivoDestino = fopen(ARQUIVO_DESTINOS, "a+");
    
    if (arquivoDestino == NULL) {
        printf("\nErro ao abrir o banco de dados.\n");
        return;
    }
    
    printf("\nDigite o nome do destino: ");
    fflush(stdin); 
    scanf(" %[^\n]s", destino.nomeDestino); 

    destino.qtdProduto = 0; 
    
    fwrite(&destino, sizeof(destino), 1, arquivoDestino);
    printf("\nDestino cadastrado com sucesso.\n");
    
    fclose(arquivoDestino);
}

void listarProduto() {
    Produto produto;
    int produtoExiste = 0;
    FILE *listarArquivo = fopen(ARQUIVO_PRODUTOS, "r"); 
    
    if (listarArquivo == NULL) {
        printf("Não existem produtos cadastrados.\n");
        return;
    }
    
    printf("\nProdutos cadastrados:\n");
    while (fread(&produto, sizeof(produto), 1, listarArquivo)) {
        printf("\nNome: %s", produto.nomeProduto);
        printf("\nQuantidade: %ld", produto.qtdProduto);
        printf("\nData de Validade: %s", produto.dataValidade);
        printf("\nVendas por Dia: %d\n", produto.vendasPorDia);
        produtoExiste = 1;
    }
    
    if (!produtoExiste) {
        printf("Não existem produtos cadastrados.\n");    
    }
    
    fclose(listarArquivo);
}

void listarDestino() {
    Destino destino;
    int destinoExiste = 0;
    FILE *listaDestino = fopen(ARQUIVO_DESTINOS, "r"); 
    
    if (listaDestino == NULL) {
        printf("Não existem destinos cadastrados.\n");
        return;
    }
    
    printf("\nDestinos cadastrados:\n");
    while (fread(&destino, sizeof(destino), 1, listaDestino)) {
        printf("\nNome: %s", destino.nomeDestino);
        printf("\nQuantidade de produtos para doação: %ld\n", destino.qtdProduto);
        destinoExiste = 1;
    }
    
    if (!destinoExiste) {
        printf("Não existem destinos cadastrados.\n");    
    }
    fclose(listaDestino);
}

void destinarProduto(Produto *produtoSelecionado) {
    Destino destino;
    char nomeDestino[50];
    int destinoEncontrado = 0;

    printf("\nDigite o nome do destino: ");
    fflush(stdin);
    scanf(" %[^\n]s", nomeDestino);

    FILE *listarDestinos = fopen(ARQUIVO_DESTINOS, "r+b");
    if (listarDestinos == NULL) {
        printf("\nErro ao abrir o banco de dados de destinos.\n");
        return;
    }

    FILE *listarProdutos = fopen(ARQUIVO_PRODUTOS, "r+b");
    if (listarProdutos == NULL) {
        printf("\nErro ao abrir o banco de dados de produtos.\n");
        fclose(listarDestinos);
        return;
    }

    long qtdDestinada; // Declare como long para evitar overflow

    while (fread(&destino, sizeof(destino), 1, listarDestinos)) {
        if (strcmp(destino.nomeDestino, nomeDestino) == 0) {
            destinoEncontrado = 1;
            printf("\nQuantidade de produtos a destinar: ");
            while (scanf("%ld", &qtdDestinada) != 1 || qtdDestinada < 0) {
                printf("Valor inválido. Tente novamente: ");
                fflush(stdin);
            }

            if (qtdDestinada <= produtoSelecionado->qtdProduto) {
                produtoSelecionado->qtdProduto -= qtdDestinada;

                fseek(listarProdutos, -sizeof(Produto), SEEK_CUR);
                fwrite(produtoSelecionado, sizeof(Produto), 1, listarProdutos);

                destino.qtdProduto += qtdDestinada; 

                fseek(listarDestinos, -sizeof(destino), SEEK_CUR);
                fwrite(&destino, sizeof(destino), 1, listarDestinos);
                printf("\nProduto destinado com sucesso.\n");
                break;
            } else {
                printf("\nQuantidade a destinar maior do que a disponível.\n");
            }
        }
    }
    
    fclose(listarDestinos);
    fclose(listarProdutos);
    if (!destinoEncontrado) {
        printf("\nDestino não encontrado. Produto não destinado.\n");
    }
}

int calcularDiasRestantes(char* dataValidade) {
    struct tm validade = {0};
    time_t t = time(NULL);
    struct tm *dataAtual = localtime(&t);
    
    sscanf(dataValidade, "%d-%d-%d", &validade.tm_year, &validade.tm_mon, &validade.tm_mday);
    validade.tm_year -= 1900; 
    validade.tm_mon -= 1; 
    
    time_t tempoValidade = mktime(&validade);
    double diferencaSegundos = difftime(tempoValidade, mktime(dataAtual));
    
    return (int)(diferencaSegundos / (60 * 60 * 24)); // Cast explícito para int
}

void produtoProximoVencimento() {
    Produto produto;
    FILE *listarArquivo = fopen(ARQUIVO_PRODUTOS, "r");
    int produtoExiste = 0;

    printf("\nProdutos próximos do vencimento:\n");

    while (fread(&produto, sizeof(produto), 1, listarArquivo)) {
        int diasRestantes = calcularDiasRestantes(produto.dataValidade);
        if (diasRestantes <= 7 && diasRestantes >= 0) {        	
            printf("\nNome: %s", produto.nomeProduto);
            printf("\nQuantidade: %ld", produto.qtdProduto);
            printf("\nData de Validade: %s", produto.dataValidade);
            printf("\nVendas por Dia: %d", produto.vendasPorDia);
            printf("\nDias restantes: %d\n", diasRestantes); 
            produtoExiste = 1;

            char destinoOpt[3];
            printf("\nDeseja destinar este produto? (s/n): ");
            fflush(stdin);
            scanf("%2s", destinoOpt); 
            if (strcmp(destinoOpt, "s") == 0) {
                destinarProduto(&produto); // Passando produto como referência
            }
        }
    }

    if (!produtoExiste) {
        printf("Não existem produtos próximos do vencimento.\n");	
    }
    
    fclose(listarArquivo);
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    int opcao = 100;

    arquivo = fopen(ARQUIVO_PRODUTOS, "wb"); // Cria o arquivo, caso ele não exista.
    if (arquivo == NULL) {
        printf("Erro ao criar banco de dados: Produto.\n");
        return 0;
    }
    
    arquivoDestino = fopen(ARQUIVO_DESTINOS, "wb"); // Cria o arquivo, caso ele não exista.
    if (arquivoDestino == NULL) {
        printf("Erro ao criar banco de dados: Destino.\n");
        fclose(arquivo); 
        return 0;
    }
    
    fclose(arquivo);
    fclose(arquivoDestino);
    
    while (opcao != 0) {
        printf("\n--------------------------------------------------");
        printf("\nMenu:\n");
        printf("1 - Cadastrar Produto \n");
        printf("2 - Listar Produtos \n");
        printf("3 - Verificar Produtos Próximos do Vencimento \n");
        printf("4 - Cadastrar Destino \n");
        printf("5 - Listar Destinos \n");
        printf("0 - Sair \n");        
        printf("--------------------------------------------------\n");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Tente novamente.\n");
            fflush(stdin);
            continue;
        }
        
        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;
            case 2:
                listarProduto();
                break;
            case 3:
                produtoProximoVencimento();
                break;
            case 4:
                cadastrarDestino();
                break;
            case 5:
                listarDestino();
                break;              
            case 0:
                printf("Saindo do sistema.\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}

