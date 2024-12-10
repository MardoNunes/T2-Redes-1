#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>





#include "librede.h"




// Inicializa o baralho
void inicializar_cartas(struct Cartas *cartas) {
    int k = 0;
    for (int i = 1; i <= 13; i++) {
      for(int j = 0; j < 4; j++){
        switch (i){
            case 1:
                cartas->deck[k+j] = 'A';
                break;
            case 2:
                cartas->deck[k+j] = '2';
                break;
            case 3:
                cartas->deck[k+j] = '3';
                break;
            case 4:
                cartas->deck[k+j] = '4';
                break;
            case 5:
                cartas->deck[k+j] = '5';
                break;
            case 6:
                cartas->deck[k+j] = '6';
                break;
            case 7:
                cartas->deck[k+j] = '7';
                break;
            case 8:
                cartas->deck[k+j] = '8';
                break;
            case 9:
                cartas->deck[k+j] = '9';
                break;
            case 10:
                cartas->deck[k+j] = 'T';
                break;
            case 11:
                cartas->deck[k+j] = 'J';
                break;
            case 12:
                cartas->deck[k+j] = 'Q';
                break;
            case 13:
                cartas->deck[k+j] = 'K';
                break;
        }
      }
        k = k + 4;  

    }
    cartas->top = 0; // Topo começa no início
}

// Embaralha o baralho
void embaralhar_cartas(struct Cartas *cartas) {
    printf("[DEALER] Embaralhando cartas...\n");
    srand(time(NULL));
    for (int i = 0; i < MAX_CARDS; i++) {
        int j = rand() % MAX_CARDS;
        char temp = cartas->deck[i];
        cartas->deck[i] = cartas->deck[j];
        cartas->deck[j] = temp;
    }
}

void pegar_carta(struct Cartas *cartas, char *buffer){
    if (cartas->top >= MAX_CARDS) {
        printf("[DEALER] Baralho vazio!\n");
        buffer[0] = '\0'; // Retorna uma string vazia
    } else {
        buffer[0] = cartas->deck[cartas->top];
        cartas->top++;
        buffer[1] = '\0'; // Garante a terminação nula
    }
}

// Pega uma carta do baralho
void pegar_cartas(struct Cartas *cartas, char *buffer) {
    if (cartas->top >= MAX_CARDS) {
        printf("[DEALER] Baralho vazio!\n");
        buffer[0] = '\0'; // Retorna uma string vazia
    } else {
        buffer[0] = cartas->deck[cartas->top];
        cartas->top++;
        buffer[1] = cartas-> deck[cartas->top];
        buffer[2] = '\0'; // Garante a terminação nula
        cartas->top++;
    }
}


// Menu de opções para o dealer
void menu_dealer() {
    printf("\n[DEALER] Menu:\n");
    printf("1 - Iniciar jogo\n");
    printf("2 - Parar jogo\n");
    printf("Escolha: ");
}

// função que mostra as cartas do jogador
void mostrar_cartas(struct Jogador *player) {
    printf("[PLAYER] Suas cartas: ");
    for (int i = 0; i < player->hand_size; i++) {
        if(player->hand[i] == 'T'){
            printf("10 ");
        }
        else
            printf("%c ", player->hand[i]);
    }
    printf("\n");
}

//vai somar a valor das cartas da sua mao
short soma_cartas(struct Jogador* player){
    int soma = 0;

    for(int i = 0; i < player->hand_size; i++){
        switch (player->hand[i]){
            case 'A':
                soma += 1;
                break;
            case '2':
                soma += 2;
                break;
            case '3':
                soma += 3;
                break;
            case '4':
                soma += 4;
                break;
            case '5':
                soma += 5;
                break;
            case '6':
                soma += 6;
                break;
            case '7':
                soma += 7;
                break;
            case '8':
                soma += 8;
                break;
            case '9':
                soma += 9;
                break;
            case 'T':
                soma += 10;
                break;
            case 'J':
                soma += 10;
                break;
            case 'Q':
                soma += 10;
                break;
            case 'K':
                soma += 10;
                break;
        }
    }

    player->valor = soma;
    printf("[PLAYER] Valor das cartas: %d\n", player->valor);
    if(player->valor > 21){
        printf("[PLAYER] Estourou!\n");
        printf("[PLAYER] Você está fora do jogo!\n");
        player->out = 1;
        return 1;
    }
    
    return 0;
}

// Menu de opções para jogadores
void menu_jogador(struct Jogador *player) {
    
    printf("\n[PLAYER] Menu:\n");
    //mostra as suas cartas
    mostrar_cartas(player);
    printf("1 - Pedir carta\n");
    printf("2 - Parar\n");
}

// Função para setar os endereços
void setar_endereco(struct sockaddr_in *local_addr, struct sockaddr_in *next_addr, const char *local_ip, int local_port, const char *next_ip, int next_port) {
    
    // seta o endereço local
    memset(local_addr, 0, sizeof(*local_addr));
    local_addr->sin_family = AF_INET;   // IPv4
    local_addr->sin_addr.s_addr = inet_addr(local_ip); // IP local
    local_addr->sin_port = htons(local_port); // Porta local

    // seta o endereço do próximo jogador
    memset(next_addr, 0, sizeof(*next_addr)); 
    next_addr->sin_family = AF_INET;    // IPv4
    next_addr->sin_addr.s_addr = inet_addr(next_ip); // IP do próximo jogador
    next_addr->sin_port = htons(next_port); // Porta do próximo jogador
}

// Guarda os argumentos da linha de comando
void guardar_argumentos(int argc, char *argv[], const char **local_ip, int *local_port, const char **next_ip, int *next_port, struct Jogador *player) {
    if (argc != 6) {
        fprintf(stderr, "Uso: %s <ID do jogador> <IP local> <Porta local> <IP próximo> <Porta próxima>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    player->id = atoi(argv[1]);
    


    if(player->id == 0){
        player->dealer = 1;
        player->bastao = 1;
    }
    else{
        player->dealer = 0;
        player->bastao = 0;
    }
    player->out = 0;


    *local_ip = argv[2];
    *local_port = atoi(argv[3]);
    *next_ip = argv[4];
    *next_port = atoi(argv[5]);
}

//montar mensagem
void montar_mensagem(struct message *msg, int tipo, int origem, char *mensagem){
    msg->tipo = tipo;
    msg->origem = origem;
    strcpy(msg->msg, mensagem);
}

// Passa a vez para o próximo jogador
void passar_vez(int sock_send, struct sockaddr_in *next_addr, struct Jogador *player) {
    struct message msg;
    player->bastao = 0;  // Atualiza o estado para refletir que a vez foi passada
    montar_mensagem(&msg, 2, 0, "passar");
    sendto(sock_send, &msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
    printf("[DEALER] Passando a vez...\n");
    
}

// Distribui cartas iniciais
void distribuir_cartas(struct Cartas *cartas, int sock_send, struct sockaddr_in *next_addr) {
    struct message msg;
    msg.tipo = 1; // Distribuição de cartas
    printf("[DEALER] Distribuindo cartas...\n");

    embaralhar_cartas(cartas);

    for (int i = 1; i < NUM_PLAYERS; i++) {
        //variavel que guarda o endereço do próximo jogador
        struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        
        char carta[3];  //vai guardar 2 cartas para cada jogador

        memcpy(addr, next_addr, sizeof(struct sockaddr_in));
        addr->sin_port = htons(5000 + i + 1); // Porta do próximo jogador
        
        pegar_cartas(cartas, carta);
        montar_mensagem(&msg, 1, 0, carta);
        sendto(sock_send, &msg, sizeof(msg), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
        next_addr = addr;   // Atualiza o endereço do próximo jogador
    }
    printf("[DEALER] Cartas distribuídas!\n");
}


// recebe a msg e trata de acordo com o tipo
void recebe_msg(struct Jogador *player, struct message *msg, int sock_send, struct sockaddr_in *next_addr, struct Cartas *cartas){
        if (msg->tipo == 1){
            player->hand[player->hand_size] = msg->msg[0];
            player->hand_size++;
            player->hand[player->hand_size] = msg->msg[1];
            player->hand_size++;
        }
        else if (msg->tipo == 2){
            player->bastao = 1; // Jogador recebe o bastão
        }
        else if(msg->tipo == 3){
            // verefica se é o dealer, se sim manda a carta, se nao passa para o proximo
            // o proximo tem que passar a msg para o seguinte ate chegar no dealer
            // se for o dealer, sortea um carta e manda para o id que foi recebido
            // vai passando por cada um até encontrar o id;

            if(player->dealer == 1){
                //dealer sorteia uma carta e manda para o id que foi recebido
                struct message* msgEnviaCarta = (struct message*)malloc(sizeof(struct message));
                char carta[2];
                embaralhar_cartas(cartas);
                pegar_carta(cartas, carta);
                montar_mensagem(msgEnviaCarta, 4, 0, carta);
                sendto(sock_send, msgEnviaCarta, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
                

            }
            else{
                //tenho que remandar a msg e passar pro proximo
                sendto(sock_send, msg, sizeof(msg), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
            }  
        }
        else if(msg->tipo == 4){
            if(player->bastao == 1){
                //recebe a carta e adiciona na mão
                player->hand[player->hand_size] = msg->msg[0];
                player->hand_size++;
                if(player->hand[player->hand_size - 1] == 'T')
                    printf("[INFO] Carta recebida: 10\n");
                else
                    printf("[INFO] Carta recebida: %c\n", player->hand[player->hand_size - 1]);
            }
            else{
                //passa a msg para o proximo
                sendto(sock_send, msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
            }

        }
        else if(msg->tipo == 5){
            //recebe a msg de que o jogador estourou
            //atualiza o vetor de jogadores ativos de acordo com o id recebido
            if(player->dealer == 1){
                player->jogadores_ativos[msg->origem] = 0;
            }
            else{
                //passa pra frente
                sendto(sock_send, msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
            }
        }
        else if(msg->tipo == 6){
            //manda msg para o proximo jogador
            sendto(sock_send, msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
            //encerra o jogo
            exit(EXIT_SUCCESS);
        }
        else if(msg->tipo == 7){
            //recebe a msg de que o jogador parou
            //atualiza o vetor de jogadores parados de acordo com o id recebido
            if(player->dealer == 1){
                int mao = atoi(msg->msg);
                player->jogadores_parados[msg->origem] = mao;
            }
            else{
                //passa pra frente
                sendto(sock_send, msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
            }
        }
        else{
            printf("Tipo de mensagem inválido!\n");
        }
}

// Atualiza o vetor de jogadores ativos
int verifica_jogadores_ativos(int jogadores_ativos[]){ 
    //conta jogadores ativos sem contar o dealer
    int ativos = 0;
    for(int i = 1; i < NUM_PLAYERS; i++){
        if(jogadores_ativos[i] == 1){
            ativos++;
        }
    }
    if(ativos == 0){
        printf("[DEALER] Todos os jogadores ativos estouraram!\n");
    }

    return ativos;
}

// Função que busca o ganhador
int busca_ganhador(int jogadores_ativos[]){
    int id = 0;
    for(int i = 1; i < NUM_PLAYERS; i++){
        if(jogadores_ativos[i] == 1){
            id = i;
        }
    }
    return id;
    
}

// Encerra o jogo
void encerrar_jogo(struct Jogador *player, int sock_send, struct sockaddr_in *next_addr){
    struct message msg;
    montar_mensagem(&msg, 6, player->id, "encerrar");
    sendto(sock_send, &msg, sizeof(struct message), 0, (struct sockaddr *)next_addr, sizeof(*next_addr));
    printf("[DEALER] Encerrando jogo...\n");
}

//verifica se todos os jogadores ativos pararam
int verifica_jogadores_parados(int jogadores_parados[]){
    int parados = 0;
    for(int i = 1; i < NUM_PLAYERS; i++){
        if(jogadores_parados[i] != 0){
            parados++;
        }
    }

    return parados;
}