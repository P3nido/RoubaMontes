#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct baralho
{
    int number;
    int naipe;
} cards;

typedef struct element
{ // Lista
    cards *card;
    struct element *next;
    struct element *previous;
} element;

typedef struct list
{
    element *start;
    element *end;
} list;

typedef struct cell
{ // Pilha
    cards *value;
    struct cell *previous;
} cell;

typedef struct pilha
{
    cell *top;
} stack;

typedef struct jogador
{
    char name[50];
    int points;
    stack *playerStack;
    cards *currentCard;
} player;

// Funções relacionadas ao baralho
cards *startCard(int baralho);                             // Função que vai calcular quantas cartas o jogo ira possuir
void fillCards(cards a[], int baralho);                    // Função que vai pegar o numero de cartas e criar o baralho de forma ordenada
void fillStackWithCards(stack *s, cards a[], int baralho); // Função que vai pegar o baralho(vetor) e passar pra uma pilha dinamica
void printStackElements(stack *s, int baralho);            // Função pra printar a pilha (provavelmente nao vou usar foi so pra teste)
stack *shuffle(stack *s, int baralho);                     // Função de embaralhar as cartas ja dentro da pilha

// Funções relacionadas à pilha
stack *startStack();         // Inicializa a pilha
player *namingPlayer(int n); // Cria o vetor de n jogadores inicializando a sua pontuação=0 e inicializando sua pilha

// Funções relacionadas à lista
list *createList();                                            // Função pra inicializar a lista (mesa)
int insertCardInList(stack *s, list *mesa);                    // Função de inserir 1 carta na lista (mesa) pra começar o jogo
void printPlayerCard(player *j, int playerPosition);           // Printa na tela a carta da mao do jogador
void StartGameList(list *mesa, stack *s, int numberOfPlayers); // Inicializa a lista (mesa) com n cartas (n=numero de jogadores)
void printList(list *mesa);                                    // Printa a lista na tela

// Funções gerais
const char *naipeConverterStack(stack *s);             // Converter o numero em naipe em variavel de pilha
const char *naipeConverterList(cards *card);           // Converter o numero em maipe em variavel de lista
int quantifyCards(player *j, int position);            // Retorna a quantidade de cartas que a pilha de jogador possui
void printOneCard(cards *c);                           // Printa apenas uma carta
void printPlayerStack(player *j, int numberOfPlayers); // Printar o topo da pilha de cada jogador

// Funções de capturar cartas
int checkCardEqualTableList(player *j, int playerPosition, list *mesa);                     // Verifica se a carta da mesa e igual a mesa (lista) para roubar apenas 1 carta
int buyCardsWithMainDeck(stack *s, player *j, int playerPosition);                          // Captura carta da pilha de compra
int checkCardEqualPlayerStack(player *j, int myPlayerPosition, int opponentPlayerPosition); // Verifica se a carta do seu topo e igual a do topo da pilha do adversario e captura a pilha
void getCardStack(player *j, int myPosition, int yourPosition);                             // Captura o monte
int compareYourCardWithOpponentCards(player *j, int myPosition, int opponentPosition);      // Comparar a carta da mao com a do topo da lista do adversario

// Funções de fim de jogo
void ranking(player *j, int numberOfPlayers);          // ordenar os jogadores de acordo com sua pontuação
void printRanking(player *j, int numberOfPlayers);     // printar o ranking dos jogadores
cards *fillVectorWithWinnerDeck(player *j);            // Passar as cartas do ganhador pra um vetor (pra fazer a ordenação posteriormente)
void fillPlayerPoints(player *j, int numberOfPlayers); // Preencher a pontuação de cada jogador
void printWinnerStack(cards *c, int len);              // Printa na tela a pilha ordenada do ganhador do jogo

// Funções de ordenação
void insertSort(cards* c, player* p, int position);

int main()
{
    int baralho;   // numero de baralho que o usuario vai escolher
    cards *winner; // vetor que sera usado apenas no final do codigo para receber as cartas do jogador ganhador dentro de um vetor
    int len = 0;   // tamanho do vetor de cartas do jogador ganhador
    srand(time(NULL));
    cards *a;                  // cartas do baralho
    stack *s = startStack();   // pilha de compra
    int numberOfPlayers;       // numero de jogadores
    player *j;                 // vetor que armazena os players
    list *mesa = createList(); // mesa de descarte (lista)
    int checkEqualTable = -1;  // recebe uma funcao em especifico de verificar se a carta da mao e igual a da lista
    int checkEqualStack = -1;  // recebe uma funcao em especifico de verificar se a carta da mao e igual o do topo da pilha de outro player
    int options;               // variavel do switch
    int rodada=0;
    int selectPlayer; // Variavel que vai ser utilizada pra ser o index de player j para roubar o monte

    printf("Digite quantos jogadores vao jogar (2 a 10 jogadores): ");
    scanf("%d", &numberOfPlayers);
    if (numberOfPlayers <= 1)
    {
        printf("Numero de jogadores inferior ao minimo necessario");
        return 0; // numero invalido de jogadores
    }
    printf("Digite quantos baralhos o jogo possuira: ");
    scanf("%d", &baralho);
    j = namingPlayer(numberOfPlayers); // inicializou os jogadores
    a = startCard(baralho);            // criou as cartas
    fillCards(a, baralho);             // preencheu o baralho
    fillStackWithCards(s, a, baralho); // passou o baralho pra pilha
    shuffle(s, baralho);               // embaralhou
    printf("Embaralhando...");
    sleep(2);
    printf("\n\n\n\n");

    // O JOGO REALMENTE COMEÇA AQUI#############################################################################################################
    while (s->top != NULL)
    {
        
        for (int i = 0; i < numberOfPlayers; i++)
        {
            printf("Vez de jogador %d:   ", i + 1);
            int x = buyCardsWithMainDeck(s, j, i);
            if (x)
            {
                printPlayerCard(j, i);
                printList(mesa);
                printPlayerStack(j, numberOfPlayers);
                printf("\n\n\n");
                printf("RODADA %d\n",rodada+1);
                rodada++;
                printf("[1]-Roubar monte: \n");
                printf("[2]-Roubar carta da lista: \n");
                printf("Selecione>>>>> \n");
                scanf("%d", &options);

                switch (options)
                {
                case 1:
                    while (1)
                    {
                        printf("Escolha a posicao do jogador em que deseja capturar o monte (0 a %d): ", numberOfPlayers - 1);
                        scanf("%d", &selectPlayer);
                        if (selectPlayer >= 1 && selectPlayer <= numberOfPlayers && selectPlayer)
                        {
                            checkEqualStack = checkCardEqualPlayerStack(j, i, selectPlayer);
                            if (checkEqualStack == 0)
                            {
                                printf("Nao e possivel roubar o monte desse jogador, tente outro jogador ou retorne para o menu inicial\n");
                                break;
                            }
                            else
                            {
                                printf("Monte roubado com sucesso\n\n");
                                i--;
                                break;
                            }
                        }
                    }
                    break;

                case 2:
                    checkEqualTable = checkCardEqualTableList(j, i, mesa);
                    if (checkEqualTable)
                    {
                        printf("Carta da mesa capturada!!!\n");
                        i--;
                    }
                    else
                    {
                        printf("Carta da mao enviada pra mesa de descarte\n");
                    }
                    break;

                default:
                    printf("Opcao invalida, tente novamente\n");
                    break;
                }
            }
        }
    }
    printf("\nJogo encerrado, calculando resultados...\n\n\n");
    sleep(2);


    fillPlayerPoints(j,numberOfPlayers);
    ranking(j, numberOfPlayers); // fazer o ranqueamento
    printRanking(j, numberOfPlayers); // printar o ranking juntamente com o ganhador
    printf("\n\n\n");
    winner = fillVectorWithWinnerDeck(j); // passa as cartas do ganhador pra um vetor
    len=quantifyCards(j,0);
    insertSort(winner,j,0);
    printWinnerStack(winner, len); // printa as cartas do vetor do jogador vencedor

    return 0;
}


void insertSort(cards* c, player* p, int position){
    int i, j;
    cards* aux = (cards*)malloc(sizeof(cards));
    for(i = 1; i < p[position].points; i++){
        *aux = c[i];
        j = i - 1;
        while(j >= 0 && aux->number < c[j].number){
            c[j + 1] = c[j];
            j--;
        }
        c[j + 1] = *aux;
    }
    free(aux);
}


int compareYourCardWithOpponentCards(player *j, int myPosition, int opponentPosition)
{
    if (j[myPosition].currentCard != NULL && j[opponentPosition].playerStack->top != NULL && j[myPosition].currentCard->number == j[opponentPosition].playerStack->top->value->number)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void printPlayerStack(player *j, int numberOfPlayers)
{
    for (int position = 0; position < numberOfPlayers; position++)
    {
        if (j[position].currentCard != NULL)
        {
            const char *n = naipeConverterList(j[position].currentCard);
            if (j[position].playerStack->top != NULL)
            {
                int c = j[position].playerStack->top->value->number;
                printf("Topo da Pilha de Jogador %d = %d de %s\n", position + 1, c, n);
            }
        }
        else
        {
            printf("A Pilha de Jogador %d esta vazia\n", position + 1);
        }
    }
}

void printOneCard(cards *c)
{
    const char *n = naipeConverterList(c);
    printf("%d de %s\n", c->number, n);
}

void printList(list *mesa)
{
    element *aux = mesa->start;
    int count = 0;
    while (aux != NULL)
    {
        printOneCard(aux->card);
        aux = aux->next;
        count++;
    }
}

int checkCardEqualTableList(player *j, int playerPosition, list *mesa)
{
    element *aux = mesa->start;
    element *prev = NULL;

    // Encontrar a carta na lista com o mesmo valor numérico
    while (aux != NULL)
    {
        if (aux->card->number == j[playerPosition].currentCard->number)
        {
            break;
        }
        else
        {
            prev = aux;
            aux = aux->next;
        }
    }

    if (aux != NULL) // mesa 
    {
        // Remover a carta da lista
        if (prev != NULL)
        {
            prev->next = aux->next;
        }
        else
        {
            mesa->start = aux->next;
        }

        if (aux->next != NULL)
        {
            aux->next->previous = prev;
        }
        else
        {
            mesa->end = prev;
        }

        // Adicionar a carta à pilha do jogador
        cell *new1 = (cell *)malloc(sizeof(cell));
        if (new1 != NULL)
        {
            new1->value = aux->card;
            
            new1->previous = j[playerPosition].playerStack->top; 
            j[playerPosition].playerStack->top = new1;
            cell* temp=(cell*)malloc(sizeof(cell));
            temp->value=j[playerPosition].currentCard;
            temp->previous=j[playerPosition].playerStack->top;
            j[playerPosition].playerStack->top=temp;

            //free(aux);
        }
        else
        {
            printf("Erro ao alocar memória para a célula.\n");
            return 0;
        }

        return 1; // Capturou a carta da lista
    }
    else
    {
        // Carta não foi encontrada, então adiciona à lista de discarte 
        element *new = (element *)malloc(sizeof(element));
        new->card = j[playerPosition].currentCard;
        new->next = NULL;

        if (mesa->start == NULL)
        {
            mesa->start = new;
        }
        else
        {
            element *temp = mesa->start;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = new;
        }
        return 0; // Passou a carta da sua mão para a lista
    }
}

void getCardStack(player *j, int myPosition, int yourPosition)
{
    if (j[yourPosition].playerStack->top != NULL)
    {
        // Obter a quantidade de cartas na pilha do adversário
        int a = quantifyCards(j, yourPosition);

        // Alocar memória para armazenar temporariamente as cartas da pilha do adversário
        cards *c = (cards *)malloc(a * sizeof(cards));

        // Copiar as cartas da pilha do adversário para um vetor temporário
        cell *aux = j[yourPosition].playerStack->top;
        for (int i = a - 1; i >= 0; i--)
        {
            c[i].naipe = aux->value->naipe;
            c[i].number = aux->value->number;
            j[yourPosition].playerStack->top= aux->previous;
        }

        // Limpar a pilha do adversário
        j[yourPosition].playerStack->top = NULL;

        // Adicionar as cartas ao topo da pilha do jogador atual
        for (int q = 0; q < a; q++)
        {
            cell *newCell = (cell *)malloc(sizeof(cell));
            if (newCell != NULL)
            {
                newCell->value = &c[q];
                newCell->previous = j[myPosition].playerStack->top;
                j[myPosition].playerStack->top = newCell;
            }
            else
            {
                printf("Erro ao alocar memória para a célula.\n");
                
            }
        }

        // Liberar a memória alocada para o array temporário
        free(c);
    }
}

int checkCardEqualPlayerStack(player *j, int myPlayerPosition, int opponentPlayerPosition)
{
    if (opponentPlayerPosition >= 0 && j[opponentPlayerPosition].playerStack->top != NULL && j[myPlayerPosition].currentCard->number == j[opponentPlayerPosition].playerStack->top->value->number)
    {
        getCardStack(j, myPlayerPosition, opponentPlayerPosition);
        return 1;
    }
    else
    {
        return 0;
    }
}

int quantifyCards(player *j, int position)
{
    int count = 0;
    cell *aux = j[position].playerStack->top;
    while (aux != NULL)
    {
        count++;
        aux = aux->previous;
    }

    return count;
}

void printPlayerCard(player *j, int playerPosition)
{
    if (j[playerPosition].currentCard != NULL)
    {
        const char *n = naipeConverterList(j[playerPosition].currentCard);
        printf("[%d] de [%s]\n", j[playerPosition].currentCard->number, n);
    }
    else
    {
        printf("O jogador nao tem cartas na mão.\n");
    }
}

int buyCardsWithMainDeck(stack *s, player *j, int playerPosition)
{
    if (s->top != NULL)
    {
        cards *c = s->top->value;

        // Inicialize o currentCard para NULL
        j[playerPosition].currentCard = NULL;

        // Aloque memória para o currentCard
        j[playerPosition].currentCard = (cards *)malloc(sizeof(cards));
        if (j[playerPosition].currentCard == NULL)
        {
            printf("Erro ao alocar memória para a carta do jogador.\n");
            return 0;
        }

        j[playerPosition].currentCard->number = c->number;
        j[playerPosition].currentCard->naipe = c->naipe;

        // Remova o topo da pilha
        cell *temp = s->top;
        s->top = s->top->previous;
        free(temp);

        return 1;
    }
    else
    {
        return 0;
    }
}

const char *naipeConverterList(cards *card)
{
    const char *n;
    if (card->naipe == 1)
    {
        n = "copas";
    }
    else if (card->naipe == 2)
    {
        n = "espadas";
    }
    else if (card->naipe == 3)
    {
        n = "ouro";
    }
    else if (card->naipe == 4)
    {
        n = "paus";
    }
    else
    {
        n = "naipe desconhecido";
    }
    return n;
}

const char *naipeConverterStack(stack *s)
{
    const char *n;
    if (s->top->value->naipe == 1)
    {
        n = "copas";
    }
    else if (s->top->value->naipe == 2)
    {
        n = "espadas";
    }
    else if (s->top->value->naipe == 3)
    {
        n = "ouro";
    }
    else if (s->top->value->naipe == 4)
    {
        n = "paus";
    }
    return n;
}

void StartGameList(list *mesa, stack *s, int numberOfPlayers)
{
    element *aux = NULL;

    for (int i = 0; i < numberOfPlayers && s->top != NULL; i++)
    {
        // Move a carta da pilha para a lista (mesa)
        element *newElement = (element *)malloc(sizeof(element));
        newElement->card = s->top->value;
        newElement->next = NULL;
        newElement->previous = NULL;

        if (aux == NULL)
        {
            mesa->start = newElement;
            mesa->end = newElement;
            aux = newElement;
        }
        else
        {
            newElement->previous = mesa->end;
            mesa->end->next = newElement;
            mesa->end = newElement;
        }

        // Atualiza o topo da pilha
        cell *temp = s->top;
        s->top = s->top->previous;
        free(temp);
    }

    // Atualiza o início da lista (mesa)
    mesa->start = aux;

    // Printa as cartas de acordo com o número de jogadores
    aux = mesa->start; // Reinicializa aux para o início da lista
    for (int i = 0; i < numberOfPlayers; i++)
    {
        const char *n = naipeConverterList(aux->card);
        printf("[%d] de [%s]\n", aux->card->number, n);
        aux = aux->next;
    }
}

int insertCardInList(stack *s, list *mesa)
{
    element *new = (element *)malloc(sizeof(element));
    new->card = (cards *)malloc(sizeof(cards));
    new->card->number = s->top->value->number;
    new->card->naipe = s->top->value->naipe;
    new->next = NULL;
    new->previous = NULL;
    if (mesa->start == NULL)
    {
        mesa->start = new;
        mesa->end = new;
    }
    else
    {
        new->previous = mesa->end;
        mesa->end->next = new;
        mesa->end = new;
    }
    return 1;
}

list *createList()
{
    list *l;
    l = (list *)malloc(sizeof(list));
    l->start = NULL;
    l->end = NULL;
    return l;
}

player *namingPlayer(int n)
{
    player *a = (player *)malloc(n * sizeof(player));
    for (int i = 0; i < n; i++)
    {
        printf("Digite o nome do %d jogador: ", i + 1);
        getchar();
        fgets(a[i].name, 50, stdin);
        a[i].points = 0;
        a[i].playerStack = startStack();
    }
    return a;
}

stack *shuffle(stack *s, int baralho)
{
    int k = baralho * 52;
    cell *temp = (cell *)malloc(k * sizeof(cell));
    cell *new = s->top;

    for (int i = 0; i < k; i++)
    {
        temp[i] = *new;
        new = new->previous;
    }

    int x;
    cell *current = s->top;

    for (int i = 0; i < k; i++)
    {
        x = rand() % k;
        current->value = temp[x].value;
        current = current->previous;
    }
    s = current;
    free(current);
    free(temp);
    free(new);

    return s;
}

void printStackElements(stack *s, int baralho)
{
    int count = 0;
    cell *new = s->top;
    const char *naipe;
    while (new != NULL && count < baralho * 52)
    {
        if (new->value->naipe == 1)
        {
            naipe = "copas";
        }
        else if (new->value->naipe == 2)
        {
            naipe = "espadas";
        }
        else if (new->value->naipe == 3)
        {
            naipe = "ouro";
        }
        else if (new->value->naipe == 4)
        {
            naipe = "paus";
        }
        printf("[%d] de [%s]\n", new->value->number, naipe);
        new = new->previous;
        count++;
    }
}

stack *startStack()
{
    stack *s;
    s = (stack *)malloc(sizeof(stack));
    s->top = NULL;
    return s;
}

void fillStackWithCards(stack *s, cards a[], int baralho)
{
    int count = 0;
    int i = baralho * 52 - 1; // Começa do final pra empilhar certo
    while (count < baralho * 52)
    {
        cell *new;
        new = (cell *)malloc(sizeof(cell));
        if (new != NULL)
        {
            new->value = &a[i];
            new->previous = s->top;
            s->top = new;
            i--;
            count++;
        }
    }
}

cards *startCard(int baralho)
{
    cards *a;
    a = (cards *)malloc(baralho * 52 * sizeof(cards));
    return a;
}

void fillCards(cards a[], int baralho)
{
    int count = 0;
    for (int b = 0; b < baralho; b++)
    {
        for (int i = 1; i <= 4; i++)
        {
            for (int j = 1; j <= 13; j++)
            {
                a[count].naipe = i;
                a[count].number = j;
                count++;
            }
        }
    }
}

void ranking(player *j, int numberOfPlayers)
{

    int aux;
    int q;
    for (int i = 0; i < numberOfPlayers; i++)
    {
        aux = j[i].points;
        q = i - 1;
        while (aux > j[q].points && q >= 0)
        {
            j[q + 1].points = j[q].points;
            q--;
        }
        j[q + 1].points = aux;
    }
}

void printRanking(player *j, int numberOfPlayers)
{
    int i = 0;
    printf("%d lugar>>>>> %s com %d pontos *Ganhou o jogo*\n", i + 1, j[i].name, j[i].points);
    for (i = 1; i < numberOfPlayers; i++)
    {
        printf("%d lugar>>>>> %s com %d pontos\n", i + 1, j[i].name, j[i].points);
    }
}

cards *fillVectorWithWinnerDeck(player *j)
{
    cards *vet;
    int len;
    len = quantifyCards(j, 0); // posicao 0 e a posicao do ganhador
    vet = (cards *)malloc(len * sizeof(cards));
    cell *aux=j[0].playerStack->top;
    for (int i = 0; i < len; i++)
    {
        vet[i].number = aux->value->number;
        vet[i].naipe = aux->value->naipe;
        aux=aux->previous;
    }
    return vet;
}

void fillPlayerPoints(player *j, int numberOfPlayers)
{
    int count;
    for (int i = 0; i < numberOfPlayers; i++)
    {
        count = quantifyCards(j, i);
        
        j[i].points = count;
    }
}

void printWinnerStack(cards *c, int len)
{
    for (int i = 1; i < len; i++)
    {
        const char *n = naipeConverterList(&c[i]);
        printf("[%d] de [%s]\n", c[i].number, n);
    }
}