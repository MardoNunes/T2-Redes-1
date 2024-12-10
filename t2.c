#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "librede.h"


int main(int argc, char *argv[]) {
    struct Jogador* player = (struct Jogador*)malloc(sizeof(struct Jogador));
    struct Cartas* cartas = (struct Cartas*)malloc(sizeof(struct Cartas));
    inicializar_cartas(cartas);

    //inicializa vetor de jogadores ativos
    for(int i = 0; i < NUM_PLAYERS; i++){
        player->jogadores_ativos[i] = 1;
        player->jogadores_parados[i] = 0;
    }

    

    //char buffer[BUFFER_SIZE];
    struct sockaddr_in local_addr, next_addr;

    const char *local_ip, *next_ip;
    int local_port, next_port;
    guardar_argumentos(argc, argv, &local_ip, &local_port, &next_ip, &next_port, player);

    int sock_recv = socket(AF_INET, SOCK_DGRAM, 0);
    int sock_send = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_recv < 0 || sock_send < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    setar_endereco(&local_addr, &next_addr, local_ip, local_port, next_ip, next_port);

    if (bind(sock_recv, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("Erro ao fazer bind");
        exit(EXIT_FAILURE);
    }




    while (1) {
        //dealer verifica se todos os jogadores ativos estouraram
        if(player->dealer == 1){
            int cont = verifica_jogadores_ativos(player->jogadores_ativos);
            if(cont == 1){
                printf("[DEALER] O JOGADOR %d GANHOU!!!\n", busca_ganhador(player->jogadores_ativos));
                encerrar_jogo(player, sock_send, &next_addr);

            }
            else if(cont > 1 && verifica_jogadores_parados(player->jogadores_parados) == cont){
                //verifica quem tem a maior mão
                int id = 0;
                int maior = 0;
                int flag = 0;
                for(int i = 1; i < NUM_PLAYERS; i++){
                    if(player->jogadores_parados[i] != 0){
                        if(player->jogadores_parados[i] >= maior){
                            if(player->jogadores_parados[i] == maior){
                                flag = 1;
                            }
                            else{
                            maior = player->jogadores_parados[i];
                            id = i;
                            }
                        }
                    }
                }
                if(flag == 1){
                    printf("[DEALER] O DEALER GANHOU O JOGO!!!\n");
                    encerrar_jogo(player, sock_send, &next_addr);
                }
                else{
                    printf("[DEALER] O JOGADOR %d GANHOU!!!\n", id);
                    encerrar_jogo(player, sock_send, &next_addr);
                }
            }
        }

    if (player->bastao == 1) {
        if (player->dealer && player->out == 0) {
            menu_dealer();
            int opcao;
            scanf("%d", &opcao);
            switch (opcao) {
                case 1: {
                    distribuir_cartas(cartas, sock_send, &next_addr);
                    passar_vez(sock_send, &next_addr, player);
                    player->out = 1;
                    break;
                }
                case 2: {
                    printf("Jogo encerrado!\n");
                    encerrar_jogo(player, sock_send, &next_addr);
                }
                default:
                    printf("Opção inválida!\n");
            }
        } else if(player->dealer != 1 && player->out == 0 && player->parar == 0){
            

            //printf("Valor: %s\n", player->valor);
            menu_jogador(player);
            soma_cartas(player);
            int opcao;
            printf("Escolha: ");
            scanf("%d", &opcao);
            switch (opcao) {
                case 1: {
                    printf("[PLAYER] Pedindo carta ao dealer...\n");
                    struct message msg;
                    montar_mensagem(&msg, 3, 0, player->id, "pedir");
                    sendto(sock_send, &msg, sizeof(struct message), 0, (struct sockaddr *)&next_addr, sizeof(next_addr));
                    
                    //recebe msg do dealer
                    struct message msg2;
                    int bytes;
                    do {
                        bytes = -1;
                        bytes = recvfrom(sock_recv, &msg2, sizeof(msg2), 0, NULL, NULL);
                    } while (msg2.tipo != 4 || msg2.origem != 0 || bytes < 0); // Verifica o tipo e a origem da mensagem

                    recebe_msg(player, &msg2, sock_send, &next_addr, cartas);
                    mostrar_cartas(player);
                    short aux = soma_cartas(player);
                    if(aux == 1){
                        //manda msg para o dealer
                        struct message msg;
                        montar_mensagem(&msg, 5, 0, player->id, "estourou");
                        printf("Mensagem: %s\n", msg.msg);
                        printf("Tipo: %d\n", msg.tipo);
                        sendto(sock_send, &msg, sizeof(struct message), 0, (struct sockaddr *)&next_addr, sizeof(next_addr));
                    }
                    passar_vez(sock_send, &next_addr, player);
                    break;
                }
                case 2: {
                    printf("[PLAYER] Parando...\n");
                    player->parar = 1;
                    struct message msg;
                    char mao[10];
                    sprintf(mao, "%d", player->valor);
                    montar_mensagem(&msg, 7, 0, player->id, mao);
                    sendto(sock_send, &msg, sizeof(struct message), 0, (struct sockaddr *)&next_addr, sizeof(next_addr));
                    passar_vez(sock_send, &next_addr, player);
                    break;
                }
                default:
                    printf("Opção inválida!\n");
            }
        }
        else{
            passar_vez(sock_send, &next_addr, player);
        }
    } else {
        struct message msg;
        recvfrom(sock_recv, &msg, sizeof(msg), 0, NULL, NULL);
        recebe_msg(player, &msg, sock_send, &next_addr, cartas);
    }
}

    close(sock_recv);
    close(sock_send);
    return 0;
}

