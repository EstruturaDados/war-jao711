
// Bibliotecas necessárias
#include <stdio.h>
#include <stdlib.h> // Para malloc, calloc, free, rand, srand
#include <string.h>
#include <ctype.h>  // Para tolower()
#include <time.h>   // Para time()

// Definição da estrutura de dados para um Território.
struct Territorio
{
    char nome[30];
    char cor[10];
    int tropas;
};

// Definição da estrutura de dados para um Jogador.
struct Jogador
{
    char cor[10];
    char *missao; // Missão alocada dinamicamente
};

// Declaração de uma função auxiliar estática para comparar strings sem diferenciar maiúsculas/minúsculas.
static int strcicmp(const char *a, const char *b);

// Função auxiliar para limpar o buffer de entrada e evitar código duplicado.
static inline void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Protótipos das funções para melhor organização
void cadastrarTerritorios(struct Territorio *mapa, int numTerritorios);
void exibirTerritorios(const struct Territorio *mapa, int numTerritorios);
void atacar(struct Territorio *atacante, struct Territorio *defensor);
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes);
int verificarMissao(const char* missao, const char* corJogador, const struct Territorio* mapa, int numTerritorios);
void liberarMemoria(struct Territorio *mapa, struct Jogador *jogadores, int numJogadores);

int main()
{
    // Inicializa o gerador de números aleatórios.
    // Essencial para que os resultados dos dados sejam diferentes a cada execução.
    srand(time(NULL));
    
    int numTerritorios;
    printf("==============================================================\n");
    printf("           CONFIGURACAO INICIAL - WARCAD\n");
    printf("==============================================================\n\n");
    
    // Loop de validação de entrada mais claro e robusto.
    while (1) {
        printf("Digite o numero total de territorios no mapa (minimo 5): ");
        if (scanf("%d", &numTerritorios) == 1 && numTerritorios >= 5) {
            limparBufferEntrada();
            break; // Sai do loop se a entrada for válida
        } else {
            printf("Entrada invalida. Por favor, digite um numero inteiro maior ou igual a 5.\n");
            limparBufferEntrada();
        }
    }

    // Alocação dinâmica de memória para o vetor de territórios.
    // calloc é usado para inicializar todos os campos com zero/NULL.
    struct Territorio *mapa = (struct Territorio *)calloc(numTerritorios, sizeof(struct Territorio));
    if (mapa == NULL)
    {
        printf("Erro: Falha na alocacao de memoria.\n");
        return 1; // Retorna um código de erro
    }

    // Cadastro dos territórios
    cadastrarTerritorios(mapa, numTerritorios);

    // Vetor de missões estratégicas disponíveis no jogo.
    const char *missoes[] = {
        "CONQUISTAR TODOS OS TERRITORIOS.",
        "ELIMINAR TODAS AS TROPAS DA COR VERMELHO.",
        "ELIMINAR TODAS AS TROPAS DA COR AZUL.",
        "ACUMULAR 30 TROPAS NO TOTAL.",
        "CONQUISTAR 6 TERRITORIOS."};
    const int totalMissoes = 5;

    // Identificar jogadores (cores únicas) a partir do mapa inicial.
    char coresUnicas[10][10]; // Suporta até 10 jogadores/cores diferentes
    int numJogadores = 0;
    for (int i = 0; i < numTerritorios; i++)
    {
        int corExiste = 0;
        for (int j = 0; j < numJogadores; j++)
        {
            if (strcmp(mapa[i].cor, coresUnicas[j]) == 0)
            {
                corExiste = 1;
                break;
            }
        }
        if (!corExiste && numJogadores < 10)
        {
            strcpy(coresUnicas[numJogadores], mapa[i].cor);
            numJogadores++;
        }
    }

    // Alocar e configurar os jogadores com suas missões.
    struct Jogador *jogadores = (struct Jogador *)calloc(numJogadores, sizeof(struct Jogador));
    if (jogadores == NULL) {
        printf("Erro: Falha na alocacao de memoria para jogadores.\n");
        free(mapa);
        return 1;
    }

    printf("\n--- MISSOES ESTRATEGICAS ATRIBUIDAS ---\n");
    for (int i = 0; i < numJogadores; i++) {
        strcpy(jogadores[i].cor, coresUnicas[i]);

        // Aloca um buffer de tamanho fixo para a missão, conforme o design solicitado.
        // A função atribuirMissao irá preencher este buffer.
        jogadores[i].missao = malloc(100 * sizeof(char));

        if (jogadores[i].missao == NULL) {
            printf("Erro de alocacao para missao do jogador %s\n", jogadores[i].cor);
            liberarMemoria(mapa, jogadores, i); // Libera o que foi alocado até agora
            return 1;
        }
        atribuirMissao(jogadores[i].missao, missoes, totalMissoes);
        printf("Jogador da cor '%s' recebeu uma missao secreta.\n", jogadores[i].cor);
    }
    printf("----------------------------------------\n");


    // Loop principal do jogo (fase de ataques)
    int opcao;
    int vencedorEncontrado = 0;
    char corVencedora[10];

    do
    {
        exibirTerritorios(mapa, numTerritorios);

        printf("\nFase de Ataque:\n");
        printf("1. Realizar um ataque\n");
        printf("0. Sair do jogo\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("\nERRO: Entrada invalida. Por favor, digite 1 ou 0.\n");
            limparBufferEntrada();
            opcao = -1; // Garante que o loop continue
            continue;
        }
        limparBufferEntrada();


        if (opcao == 1)
        {
            int idxAtacante, idxDefensor;
            printf("\n--- SELECAO DE ATAQUE ---\n");

            // Validação robusta da entrada do usuário para evitar falhas.
            printf("Digite o numero do territorio ATACANTE (1 a %d): ", numTerritorios);
            if (scanf("%d", &idxAtacante) != 1) {
                printf("\nERRO: Entrada numerica invalida.\n");
                limparBufferEntrada();
                continue;
            }
            printf("Digite o numero do territorio DEFENSOR (1 a %d): ", numTerritorios);
            if (scanf("%d", &idxDefensor) != 1) {
                printf("\nERRO: Entrada numerica invalida.\n");
                limparBufferEntrada();
                continue;
            }
            limparBufferEntrada();

            // Converte para índice do vetor (0 a N-1)
            idxAtacante--;
            idxDefensor--;

            // Validações
            if (idxAtacante < 0 || idxAtacante >= numTerritorios ||
                idxDefensor < 0 || idxDefensor >= numTerritorios)
            {
                printf("\nERRO: Numero de territorio invalido!\n");
                continue;
            }
            if (idxAtacante == idxDefensor)
            {
                printf("\nERRO: O territorio atacante e o defensor nao podem ser o mesmo.\n");
                continue;
            }
            // Validação de cor usando strcicmp para consistência.
            if (strcicmp((mapa + idxAtacante)->cor, (mapa + idxDefensor)->cor) == 0)
            {
                printf("\nERRO: Voce nao pode atacar um territorio da sua propria cor.\n");
                continue;
            }
            // Validação de tropas: precisa de mais de 1 tropa para atacar
            if ((mapa + idxAtacante)->tropas <= 1)
            {
                printf("\nERRO: Voce precisa de mais de 1 tropa para iniciar um ataque.\n");
                continue;
            }

            // Se todas as validações passaram, executa o ataque
            atacar(mapa + idxAtacante, mapa + idxDefensor);

            // Após o ataque, verifica se algum jogador cumpriu sua missão.
            for (int i = 0; i < numJogadores; i++) {
                if (verificarMissao(jogadores[i].missao, jogadores[i].cor, mapa, numTerritorios)) {
                    vencedorEncontrado = 1;
                    strcpy(corVencedora, jogadores[i].cor);
                    break; // Encontrou um vencedor, sai do 'for'.
                }
            }

            if (vencedorEncontrado) {
                exibirTerritorios(mapa, numTerritorios); // Mostra o estado final do mapa.
                printf("\n==============================================================\n");
                printf(" VITORIA! O JOGADOR DA COR '%s' CUMPRIU SUA MISSAO!\n", corVencedora);
                // Encontra a missão do vencedor para exibi-la.
                for (int i = 0; i < numJogadores; i++) {
                    // Usa strcicmp para garantir que a cor do vencedor seja encontrada corretamente
                    if (strcicmp(jogadores[i].cor, corVencedora) == 0) {
                        printf(" Missao Cumprida: %s\n", jogadores[i].missao);
                        break;
                    }
                }
                printf("==============================================================\n");
                break; // Sai do loop principal do jogo.
            }
        }

    } while (opcao != 0 && !vencedorEncontrado);

    // Libera a memória alocada dinamicamente ao final do programa.
    liberarMemoria(mapa, jogadores, numJogadores);

    printf("\nJogo finalizado. Memoria liberada. Ate a proxima!\n");

    return 0;
}

/**
 * @brief Cadastra os territórios do jogo.
 * @param mapa Ponteiro para o vetor de territórios.
 * @param numTerritorios Número total de territórios a serem cadastrados.
 *
 * Pede ao usuário para inserir nome, cor e tropas para cada território.
 */
void cadastrarTerritorios(struct Territorio *mapa, int numTerritorios)
{
    printf("\n==============================================================\n");
    printf("           CADASTRO DE TERRITORIOS - WARCAD\n");
    printf("==============================================================\n\n");

    for (int i = 0; i < numTerritorios; i++)
    {
        printf("--- Inserindo dados do Territorio %d ---\n", i + 1);

        printf("Digite o nome do territorio: ");
        fgets((mapa + i)->nome, 30, stdin);
        (mapa + i)->nome[strcspn((mapa + i)->nome, "\r\n")] = 0;

        printf("Digite a cor do exercito: ");
        fgets((mapa + i)->cor, 10, stdin);
        (mapa + i)->cor[strcspn((mapa + i)->cor, "\r\n")] = 0;

        printf("Digite a quantidade de tropas: ");
        scanf("%d", &(mapa + i)->tropas);

        limparBufferEntrada();
        printf("\n");
    }
}

/**
 * @brief Exibe a lista de todos os territórios cadastrados.
 * @param mapa Ponteiro (const) para o vetor de territórios.
 * @param numTerritorios Número total de territórios.
 *
 * Itera sobre o vetor e imprime os dados de cada território.
 */
void exibirTerritorios(const struct Territorio *mapa, int numTerritorios)
{
    printf("\n\n==============================================================\n");
    printf("                MAPA ATUAL DOS TERRITORIOS\n");
    printf("==============================================================\n");

    for (int i = 0; i < numTerritorios; i++)
    {
        printf("\n%d. Territorio: %s\n", i + 1, (mapa + i)->nome);
        printf("   Cor do Exercito: %s\n", (mapa + i)->cor);
        printf("   Quantidade de Tropas: %d\n", (mapa + i)->tropas);
        printf("--------------------------------------------------------------\n");
    }
}

/**
 * @brief Simula um ataque de um território para outro.
 * @param atacante Ponteiro para o território que está atacando.
 * @param defensor Ponteiro para o território que está defendendo.
 *
 * Rola dados aleatórios para ataque e defesa. Se o ataque vencer,
 * o território defensor muda de dono e as tropas são transferidas.
 * Se a defesa vencer, o atacante perde uma tropa.
 */
void atacar(struct Territorio *atacante, struct Territorio *defensor)
{
    printf("\n--- SIMULACAO DE BATALHA: %s ataca %s ---\n", atacante->nome, defensor->nome);

    // Simula a rolagem de dados (1 a 6)
    int dadoAtaque = (rand() % 6) + 1;
    int dadoDefesa = (rand() % 6) + 1;

    printf("Dado de Ataque: %d\n", dadoAtaque);
    printf("Dado de Defesa: %d\n", dadoDefesa);

    // Compara os resultados dos dados
    if (dadoAtaque > dadoDefesa)
    {
        printf("\nVITORIA DO ATACANTE! %s conquistou %s.\n", atacante->nome, defensor->nome);

        // Transfere a cor do exército para o território conquistado
        strcpy(defensor->cor, atacante->cor);

        // Metade das tropas do atacante (arredondado para baixo) se movem
        int tropasMovidas = atacante->tropas / 2;
        if (tropasMovidas == 0) tropasMovidas = 1; // Garante que pelo menos 1 tropa se mova

        defensor->tropas = tropasMovidas;
        atacante->tropas -= tropasMovidas;
    }
    else
    {
        printf("\nVITORIA DA DEFESA! %s repeliu o ataque.\n", defensor->nome);
        // O atacante perde 1 tropa na derrota
        atacante->tropas--;
        printf("%s perdeu uma tropa e agora tem %d.\n", atacante->nome, atacante->tropas);
    }
    printf("--------------------------------------------------------------\n");
}

/**
 * @brief Sorteia uma missão e a copia para a string de destino.
 * @param destino Ponteiro para a string de destino onde a missão será copiada.
 * @param missoes Vetor de strings contendo as missões disponíveis.
 * @param totalMissoes Número total de missões no vetor.
 */
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes)
{
    int indiceSorteado = rand() % totalMissoes;
    strcpy(destino, missoes[indiceSorteado]);
}

/**
 * @brief Compara duas strings de forma case-insensitive.
 * @param a Primeira string.
 * @param b Segunda string.
 * @return <0 se a<b, 0 se a==b, >0 se a>b.
 */
static int strcicmp(const char *a, const char *b) { // Definição da função
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

/**
 * @brief Verifica se a missão de um jogador foi cumprida.
 * @param missao Ponteiro constante para a string da missão a ser verificada.
 * @param corJogador A cor do jogador cuja missão está sendo verificada.
 * @param mapa Ponteiro constante para o vetor de territórios.
 * @param numTerritorios Número total de territórios.
 * @return 1 se a missão foi cumprida, 0 caso contrário.
 */
int verificarMissao(const char* missao, const char* corJogador, const struct Territorio* mapa, int numTerritorios)
{
    // Lógica para "CONQUISTAR TODOS OS TERRITORIOS."
    if (strstr(missao, "TODOS OS TERRITORIOS") != NULL)
    {
        for (int i = 0; i < numTerritorios; i++)
        {
            // Usa strcicmp para garantir que a comparação de cores não seja sensível a maiúsculas/minúsculas.
            if (strcicmp(mapa[i].cor, corJogador) != 0)
            {
                return 0; // Encontrou um território não pertencente ao jogador.
            }
        }
        return 1; // Todos os territórios pertencem ao jogador.
    }

    // Lógica para "ELIMINAR TODAS AS TROPAS DA COR..."
    if (strstr(missao, "ELIMINAR TODAS AS TROPAS") != NULL)
    {
        char corAlvo[10] = "";
        if (strstr(missao, "VERMELHO")) {
            strcpy(corAlvo, "vermelho");
        } else if (strstr(missao, "AZUL")) {
            strcpy(corAlvo, "azul");
        }

        // Se o jogador tem a missão de se auto-eliminar, não faz sentido.
        if (strcicmp(corJogador, corAlvo) == 0) return 0;

        // Verifica se a cor alvo ainda existe no mapa
        for (int i = 0; i < numTerritorios; i++)
        {
            if (strcicmp(mapa[i].cor, corAlvo) == 0)
            {
                return 0; // Cor alvo ainda existe no mapa.
            }
        }
        return 1; // Cor alvo foi eliminada.
    }

    // Lógica para "ACUMULAR 30 TROPAS NO TOTAL."
    if (strstr(missao, "ACUMULAR 30 TROPAS") != NULL)
    {
        int totalTropas = 0;
        for (int i = 0; i < numTerritorios; i++)
        {
            if (strcicmp(mapa[i].cor, corJogador) == 0)
            {
                totalTropas += mapa[i].tropas;
            }
        }
        return totalTropas >= 30;
    }

    // Lógica para "CONQUISTAR 6 TERRITORIOS."
    if (strstr(missao, "CONQUISTAR 6 TERRITORIOS") != NULL)
    {
        int totalTerritorios = 0;
        for (int i = 0; i < numTerritorios; i++)
        {
            if (strcicmp(mapa[i].cor, corJogador) == 0)
            {
                totalTerritorios++;
            }
        }
        return totalTerritorios >= 6;
    }

    return 0; // Nenhuma condição de missão conhecida foi atendida.
}

/**
 * @brief Libera toda a memória alocada dinamicamente (mapa e jogadores).
 * @param mapa Ponteiro para o vetor de territórios a ser liberado.
 * @param jogadores Ponteiro para o vetor de jogadores.
 * @param numJogadores Número total de jogadores.
 */
void liberarMemoria(struct Territorio *mapa, struct Jogador *jogadores, int numJogadores)
{
    // Libera a string da missão para cada jogador
    if (jogadores != NULL) {
        for (int i = 0; i < numJogadores; i++)
        {
            if (jogadores[i].missao != NULL)
            {
                free(jogadores[i].missao);
            }
        }
        // Libera o vetor de jogadores
        free(jogadores);
    }

    // Libera o vetor de territórios
    if (mapa != NULL) {
        free(mapa);
    }
}
