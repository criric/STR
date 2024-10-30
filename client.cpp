//
// cliente_udp.cpp
// Cliente UDP que envia e recebe mensagens aleatórias.
// Modificado em 30/10/24.
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TAMANHO_MENSAGEM 4096  // Tamanho fixo da mensagem a ser enviada.
#define IP_SERVIDOR "127.0.0.1"  // Endereço do servidor.
#define PORTA 9734  // Porta de comunicação.

// Função para gerar uma mensagem aleatória de tamanho fixo.
void gerar_mensagem_aleatoria(char *mensagem, int tamanho) {
    const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789;.,-*/+'!@#$%¨&(){}[]=<>:?";
    int limite = sizeof(caracteres) - 1;

    for (int i = 0; i < tamanho; ++i) {
        mensagem[i] = caracteres[rand() % limite];
    }
}

int main() {
    int socket_cliente;
    struct sockaddr_in endereco_servidor;
    socklen_t tamanho_endereco = sizeof(endereco_servidor);
    char buffer[TAMANHO_MENSAGEM];

    // Criação do socket UDP.
    socket_cliente = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_cliente < 0) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor.
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
    endereco_servidor.sin_port = htons(PORTA);

    // Geração da mensagem aleatória a ser enviada.
    gerar_mensagem_aleatoria(buffer, TAMANHO_MENSAGEM);
    printf("Enviando mensagem para o servidor...\n");

    // Marca o tempo de início do envio.
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Envia a mensagem ao servidor.
    if (sendto(socket_cliente, buffer, TAMANHO_MENSAGEM, 0, 
               (struct sockaddr*)&endereco_servidor, tamanho_endereco) < 0) {
        perror("Erro ao enviar a mensagem");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    // Recebe a resposta do servidor.
    ssize_t bytes_recebidos = recvfrom(socket_cliente, buffer, TAMANHO_MENSAGEM, 0, 
                                       (struct sockaddr*)&endereco_servidor, &tamanho_endereco);
    if (bytes_recebidos < 0) {
        perror("Erro ao receber a resposta");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    // Marca o tempo de fim após o recebimento.
    clock_gettime(CLOCK_MONOTONIC, &fim);

    // Calcula o tempo total em segundos com precisão.
    double tempo_rtt = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Tempo total de envio e recebimento: %.8f segundos\n", tempo_rtt);
    printf("Mensagem recebida: %.*s\n", TAMANHO_MENSAGEM, buffer);

    // Fecha o socket.
    close(socket_cliente);
    return 0;
}
