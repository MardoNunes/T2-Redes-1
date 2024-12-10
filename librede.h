


#ifndef LIBREDE_H
#define LIBREDE_H


#define MAX_CARDS 52
#define BUFFER_SIZE 64
#define NUM_PLAYERS 4

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Protocolo da mensagem
struct message {
    int tipo;           // Tipo da mensagem - pedido de carta, passar vez, etc.
    int destino;        // ID do jogador que vai receber a mensagem
    int origem;         // ID do jogador que enviou a mensagem
    char msg[BUFFER_SIZE]; // Mensagem
};

// Estrutura do jogador
struct Jogador {
    int id;              // ID do jogador
    int creditos;        // Créditos do jogador
    int hand_size;       // Tamanho da mão do jogador
    int valor;     // Valor da soma de suas cartas
    char hand[MAX_CARDS]; // Mão do jogador
    short dealer;        // Se o jogador é o dealer
    short bastao;           // Se é a vez do jogador
    int rodada;          // Rodada atual
    int out;
    int parar;
    int jogadores_ativos[NUM_PLAYERS];  // jogadores ativos - ou seja ainda nao estouraram
    int jogadores_parados[NUM_PLAYERS]; //jogadores que nao estouraram porem pararam
};

// Estrutura das cartas
struct Cartas {
    char deck[MAX_CARDS]; // Deck de cartas
    int top;             // Topo do baralho
};

void inicializar_cartas(struct Cartas *cartas);
void embaralhar_cartas(struct Cartas *cartas);
void pegar_carta(struct Cartas *cartas, char *buffer);
void pegar_cartas(struct Cartas *cartas, char *buffer);
void menu_dealer();
void mostrar_cartas(struct Jogador *player);
short soma_cartas(struct Jogador* player);
void menu_jogador(struct Jogador *player);
void setar_endereco(struct sockaddr_in *local_addr, struct sockaddr_in *next_addr, const char *local_ip, int local_port, const char *next_ip, int next_port);
void guardar_argumentos(int argc, char *argv[], const char **local_ip, int *local_port, const char **next_ip, int *next_port, struct Jogador *player);
void montar_mensagem(struct message *msg, int tipo, int destino, int origem, char *mensagem);
void passar_vez(int sock_send, struct sockaddr_in *next_addr, struct Jogador *player);
void distribuir_cartas(struct Cartas *cartas, int sock_send, struct sockaddr_in *next_addr);
void recebe_msg(struct Jogador *player, struct message *msg, int sock_send, struct sockaddr_in *next_addr, struct Cartas *cartas);
int verifica_jogadores_ativos(int jogadores_ativos[]);
int busca_ganhador(int jogadores_ativos[]);
void encerrar_jogo(struct Jogador *player, int sock_send, struct sockaddr_in *next_addr);
int verifica_jogadores_parados(int jogadores_parados[]);

#endif
