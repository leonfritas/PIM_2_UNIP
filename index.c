#include<stdio.h>
#include<locale.h>
#include<string.h>
#include<time.h>

#define ARQUIVO_PRODUTOS "dbProduto.txt"

typedef struct Produto {
    char nomeProduto[50];
    int qtdProduto;
    char dataValidade[11]; // Formato: YYYY-MM-DD
    int vendasPorDia;
} produto;

FILE *arquivo;

void cadastrarProduto() {
    produto produto;
	//
    arquivo = fopen(ARQUIVO_PRODUTOS, "a+");
	//
    if (arquivo == NULL) {
        printf("\n Erro ao abrir o banco de dados.");
        return;
    }
	//
    printf("\n Digite o nome do produto: ");
    fflush(stdin); 
    scanf(" %[^\n]s", produto.nomeProduto); 
	//
    printf("\n Digite a quantidade: ");
    scanf("%d", &produto.qtdProduto);
	//
    printf("\n Digite a data de validade (YYYY-MM-DD): ");
    scanf("%s", produto.dataValidade); 
	//
    printf("\n Digite a quantidade de vendas por dia: ");
    scanf("%d", &produto.vendasPorDia);
	//
    fwrite(&produto, sizeof(produto), 1, arquivo);
    printf("\n Produto cadastrado com sucesso.\n");
	//
    fclose(arquivo);
}

void listarProduto() {
    produto produto;
    FILE *listarArquivo = fopen(ARQUIVO_PRODUTOS, "r"); 

    if (listarArquivo == NULL) {
        printf("Não existem produtos cadastrados.\n");
        return;
    }

    printf("\n Produtos cadastrados:\n");
    while (fread(&produto, sizeof(produto), 1, listarArquivo)) {
        printf("\n Nome: %s", produto.nomeProduto);
        printf("\n Quantidade: %d", produto.qtdProduto);
        printf("\n Data de Validade: %s", produto.dataValidade);
        printf("\n Vendas por Dia: %d\n", produto.vendasPorDia);
    }

    fclose(listarArquivo);
}

int calcularDiasRestantes(char* dataValidade) {
    struct tm validade = {0};
    time_t t = time(NULL);
    struct tm *dataAtual = localtime(&t);
	//
    sscanf(dataValidade, "%d-%d-%d", &validade.tm_year, &validade.tm_mon, &validade.tm_mday);
    validade.tm_year -= 1900; 
    validade.tm_mon -= 1; 
	//
    time_t tempoValidade = mktime(&validade);
    double diferencaSegundos = difftime(tempoValidade, mktime(dataAtual));
	//
    return diferencaSegundos / (60 * 60 * 24); 
}

void produtoProximoVencimento() {
    produto produto;
    FILE *listarArquivo = fopen(ARQUIVO_PRODUTOS, "r");
	//	
    if (listarArquivo == NULL) {
        printf("Não existem produtos cadastrados.\n");
        return;
    }   	
	//
    printf("\n Produtos próximos do vencimento:\n");
    while (fread(&produto, sizeof(produto), 1, listarArquivo)) {
        int diasRestantes = calcularDiasRestantes(produto.dataValidade);
        if (diasRestantes <= 7 && diasRestantes >= 0) {        	
            printf("\n Nome: %s", produto.nomeProduto);
            printf("\n Quantidade: %d", produto.qtdProduto);
            printf("\n Data de Validade: %s", produto.dataValidade);
            printf("\n Vendas por Dia: %d", produto.vendasPorDia);
            printf("\n Dias restantes: %d \n", diasRestantes);            
        }
    }

    fclose(listarArquivo);
}

int main() {
    setlocale(LC_ALL, "Portuguese");
	//
    int opcao = 100;
	//
    // Cria o arquivo, caso ele não exista.
    arquivo = fopen(ARQUIVO_PRODUTOS, "w");
    //
    if (arquivo == NULL) {
        printf("Erro ao criar banco de dados.");
        return 0;
    }
    fclose(arquivo);
	//
    while (opcao != 0) {
    	printf("\n --------------------------------------------------");
        printf("\nMenu:\n");
        printf("1 - Cadastrar Produto \n");
        printf("2 - Listar Produtos \n");
        printf("3 - Verificar Produtos Próximos do Vencimento \n");
        printf("0 - Sair \n");        
        printf(" -------------------------------------------------- \n");
        scanf("%d", &opcao);
		//
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
            case 0:
                printf("Saindo do sistema.\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}

