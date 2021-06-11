#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

1) Construa um algoritmo que através de um registro do tipo struct, armazene
para 10 filmes, o código, o nome, o gênero (ação, aventura, ...), censura (em
idade), tamanho do filme (em minutos), valor da locação, quantidade de mídias
existentes e quantidade de mídias que já estão locadas (esse campo deve vir com
valor zero automaticamente quando do início do cadastro).  

Faça um menu (utilizando a estrutura swtich-case) que:
- Execute uma rotina de cadastro dos filmes (faça uma função para isso)
- Execute uma rotina de locação de filme (faça uma outra função para isso)
- Execute uma rotina de devolução de filme (faça uma outra função para isso)
- Mostre o relatório de todos os filmes cadastrados

Exemplo de menu:

 MENU 1 – Cadastro 2 – Locação 3 – Devolução 4 - Relatório
 *
 */

#define TAMANHO_LINHA 40 

struct Filme {
    int cod;
    char nome[80];
    char genero[30];
    int censura;
    int tamanho;
    float valorLocacao;
    int qtdTotal;
    int qtdLocada;
};

struct ListaFilmes {
    struct Filme items[10];
    int qtdCadastrada;
};

struct ResultadoBusca {
    struct Filme filme;
    int posicao;
};

// Auxiares
void desenhaLinha(char caractere, int qtdCaracteres);
void desenhaCabecalho(char frase[]);
void mensagemErro();
void limpaTela();
void limparBuffer();
struct ResultadoBusca buscarFilme(int cod, struct ListaFilmes *listaFilmes);

// Atividades do sistema
void cadastrarFilme(struct ListaFilmes *lista);
void locarFilme(struct ListaFilmes *lista);
void devolverFilme(struct ListaFilmes *lista);
void relatorioFilmes(struct ListaFilmes *lista);
void exibirFilme(struct Filme filme);

// Menus
int menuPrincipal();
struct Filme menuCadastro();
int menuLocacao();
int menuLocacaoCodigo();
int menuLocacaoIdade();

// Validações
int idadePermitida(int idade, int censura);
int codigoExiste(int cod, struct ListaFilmes *listaFilmes);

//================================
// Função principal
//================================
int main() {
    int escolha, querSair;
    struct ListaFilmes listaFilmes;

    listaFilmes.qtdCadastrada = 0;

    querSair = 0;

    do {
        limpaTela();
        escolha = menuPrincipal();
        
        switch(escolha) {
            case 1:
                cadastrarFilme(&listaFilmes);
                break;
            case 2:
                locarFilme(&listaFilmes);
                break;
            case 3:
                devolverFilme(&listaFilmes);
                break;
            case 4:
                relatorioFilmes(&listaFilmes);
                break;
            case 5:
                querSair = 1;
                break;
            default:
                mensagemErro();
                break;
        }

    } while (!querSair);
}


//================================
// Atividades do sistema
//================================

// Cadastra um filme e incrementa o contador
void cadastrarFilme(struct ListaFilmes *listaFilmes)
{
    struct Filme novoFilme;
    novoFilme = menuCadastro();

    if (codigoExiste(novoFilme.cod, listaFilmes)) {
        desenhaCabecalho("Já existe um filme cadastrado\n com esse código.");
        printf("Tente novamente");
        limparBuffer();

        return;
    }

    listaFilmes->items[listaFilmes->qtdCadastrada] = novoFilme;
    listaFilmes->qtdCadastrada++;

    desenhaCabecalho("Filme cadastrado com sucesso");
    exibirFilme(novoFilme);
    limparBuffer();
}

void locarFilme(struct ListaFilmes *listaFilmes)
{
    int codigoFilme = menuLocacaoCodigo();

    if (!codigoExiste(codigoFilme, listaFilmes)) {
        desenhaCabecalho("Código inválido!");

        printf("O filme escolhido não existe.\n");
        desenhaLinha(*"-", TAMANHO_LINHA);
        limparBuffer();
        
        return;
    }

    struct ResultadoBusca resultadoBusca = buscarFilme(codigoFilme, listaFilmes);
    int idade = menuLocacaoIdade();

    // Verifica a censura
    if (!idadePermitida(idade, resultadoBusca.filme.censura)) {
        desenhaCabecalho("Locação recusada!");

        printf(
                "O filme selecionado é destinado a maiores de %d anos.\n",
                resultadoBusca.filme.censura
        );
        desenhaLinha(*"-", TAMANHO_LINHA);
        limparBuffer();
        limparBuffer();

        return;
    }

    // Verifica o estoque
    if (resultadoBusca.filme.qtdLocada == resultadoBusca.filme.qtdTotal) {
        desenhaCabecalho("Locação recusada!");

        printf("Todas as cópias desse filme já estão locadas.\n");
        desenhaLinha(*"-", TAMANHO_LINHA);
        limparBuffer();
        limparBuffer();

        return;
    }

    listaFilmes->items[resultadoBusca.posicao].qtdLocada += 1;

    desenhaCabecalho("Filme locado com sucesso");
    limparBuffer();
    limparBuffer();
} 

void devolverFilme(struct ListaFilmes *listaFilmes) {
    if (listaFilmes->qtdCadastrada == 0) {
        desenhaCabecalho("Devolução recusada!");
        printf("Não existe nenhum filme cadastrado.");
        limparBuffer();

        return;
    }

    int codigoFilme = menuLocacaoCodigo();

    if (!codigoExiste(codigoFilme, listaFilmes)) {
        desenhaCabecalho("Devolução recusada!");

        printf("Não existe um filme com o código informado.\n");
        desenhaLinha(*"-", TAMANHO_LINHA);
        limparBuffer();
        
        return;
    }

    struct ResultadoBusca resultadoBusca = buscarFilme(codigoFilme, listaFilmes);
    if (resultadoBusca.filme.qtdLocada == 0) {
        desenhaCabecalho("Devolução recusada!");

        printf("O filme selecionado não possui nenhuma cópia locada.\n");
        desenhaLinha(*"-", TAMANHO_LINHA);
        limparBuffer();

        return;
    }

    listaFilmes->items[resultadoBusca.posicao].qtdLocada -= 1;

    desenhaCabecalho("Filme devulvido com sucesso");
    limparBuffer();
} 

void relatorioFilmes(struct ListaFilmes *listaFilmes) {
    desenhaCabecalho("Relatório de filmes");

    if (listaFilmes->qtdCadastrada == 0) {
        printf("Não há nenhum filme cadastrado");
        limparBuffer();
        limparBuffer();
        return;
    }

    printf("Filmes cadastrados:\n");
    for (int i = 0; i < listaFilmes->qtdCadastrada; i++) {
        exibirFilme(listaFilmes->items[i]);
    }
    limparBuffer();
    limparBuffer();
}

struct ResultadoBusca buscarFilme(int cod, struct ListaFilmes *listaFilmes)
{
    struct ResultadoBusca resultadoBusca;

    // Busca o filme escolhido e armazena em uma variável
    for (int i = 0; i < listaFilmes->qtdCadastrada; i++) {
        if (cod == listaFilmes->items[i].cod) {
            resultadoBusca.filme   = listaFilmes->items[i];
            resultadoBusca.posicao = i;
        }
    }

    return resultadoBusca;
}

void exibirFilme(struct Filme filme)
{
    desenhaLinha(*"-", TAMANHO_LINHA);

    printf("Código...........: %d\n", filme.cod);
    printf("Nome.............: %s\n", filme.nome);
    printf("Gênero...........: %s\n", filme.genero);
    printf("Censura..........: %d\n", filme.censura);
    printf("Tamanho (minutos): %d\n", filme.tamanho);
    printf("Valor (locação)..: R$ %.2f\n", filme.valorLocacao);
    printf("Quantidade total.: %d\n", filme.qtdTotal);
    printf("Quantidade locada: %d\n", filme.qtdLocada);

    desenhaLinha(*"-", TAMANHO_LINHA);

}

//================================
// Validações
//================================

// Verifica se um código já existe
int codigoExiste(int cod, struct ListaFilmes *listaFilmes)
{
    // Nenhum filme cadastrado
    if (listaFilmes->qtdCadastrada == 0) return 0;

    // Percorre a lista em busca do código
    for (int i = 0; i < listaFilmes->qtdCadastrada; i++) {
        if (cod == listaFilmes->items[i].cod) {
            return 1;
        }
    }

    // Caso nenhum seja encontrado
    return 0;
}

// Verifica se a idade pode assistir ao filme
int idadePermitida(int idade, int censura)
{
    if (idade >= censura) {
        return 1;
    } else {
        return 0;
    }
}

// Verifica a disponibilidade do filme
int filmeDisponivel(int cod, struct ListaFilmes *listaFilmes) {
    struct Filme filmeEscolhido;
    

    if (filmeEscolhido.qtdLocada < filmeEscolhido.qtdTotal) {
        return 1;
    } else {
        return 0;
    }
}


//================================
// Funções auxiliares
//================================

// Desenha uma linha com uma qtd e caractere específicos
// Inclui '\n'
void desenhaLinha(char caractere, int qtdCaracteres)
{
    for (int i = 0; i < qtdCaracteres; i++) {
        putchar(caractere);
    }
    printf("\n");
}


void desenhaCabecalho(char frase[])
{
    limpaTela();
    desenhaLinha(*"=", TAMANHO_LINHA);
    printf(" %s\n", frase);
    desenhaLinha(*"-", TAMANHO_LINHA);
}

// Limpa a tela e mostra uma mensagem de erro com pausa
void mensagemErro()
{
    limpaTela();
    desenhaLinha(*"-", TAMANHO_LINHA);
    printf("Opção inválida, tente novamente\n");
    desenhaLinha(*"-", TAMANHO_LINHA);
    limparBuffer();
    getchar();
}

// Limpa a tela no Windows e Linux
void limpaTela()
{
#ifdef WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

// Substitui o fflush(stdin) que não funciona no linux
void limparBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

//================================
// Menus
//================================
int menuPrincipal()
{
    desenhaLinha(*"=", TAMANHO_LINHA);
    printf("|    Locadora BrokeBuster \n");
    desenhaLinha(*"-", TAMANHO_LINHA);

    printf("1. Cadastro\n");
    printf("2. Locação\n");
    printf("3. Devolução\n");
    printf("4. Relatório\n");
    printf("5. Sair\n");
    desenhaLinha(*"=", TAMANHO_LINHA);

    int escolha = 0;
    scanf("%d", &escolha);

    return escolha;
}

struct Filme menuCadastro()
{
    struct Filme novoFilme;
    desenhaCabecalho(" Cadastro de filme");

    printf("Código: ");
    scanf("%d", &novoFilme.cod);
    limparBuffer();

    printf("Nome: ");
    fgets(novoFilme.nome, 80, stdin);
    novoFilme.nome[strcspn(novoFilme.nome, "\r\n")] = '\0';

    printf("Gênero: ");
    fgets(novoFilme.genero, 30, stdin);
    novoFilme.genero[strcspn(novoFilme.genero, "\r\n")] = '\0';

    printf("Censura: ");
    scanf("%d", &novoFilme.censura);
    limparBuffer();

    printf("Tamanho (minutos): ");
    scanf("%d", &novoFilme.tamanho);
    limparBuffer();

    printf("Valor da locação: R$ ");
    scanf("%f", &novoFilme.valorLocacao);
    limparBuffer();

    printf("Quantidade total: ");
    scanf("%d", &novoFilme.qtdTotal);
    limparBuffer();

    novoFilme.qtdLocada = 0;

    return novoFilme;
}

int menuLocacaoCodigo()
{
    desenhaCabecalho("Locação de filme");
    int codigoFilme = 0;

    printf("> Digite o código do filme\n");
    printf("Código: ");
    scanf("%d", &codigoFilme);
    limparBuffer();
    
    return codigoFilme;
};

int menuLocacaoIdade()
{
    desenhaCabecalho("Locação de filme");
    int idade = 0;

    printf("> Digite a sua idade\n");
    printf("Idade: ");
    scanf("%d", &idade);
    
    return idade;
}
