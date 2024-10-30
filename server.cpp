//
// servidor_udp.cpp
// Servidor UDP básico que incrementa valores em um vetor de caracteres.
// Criado em 16/05/16 por Affonso. Modificado em 30/10/24.
//

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORTA 9734   // Porta padrão do servidor.
#define TAMANHO_VETOR 4  // Tamanho do vetor de caracteres.

int main() {
    int socket_servidor;
    struct sockaddr_in endereco_servidor, endereco_cliente;
    socklen_t tamanho_cliente;
    char buffer[TAMANHO_VETOR];

    // Criação do socket UDP.
    socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_servidor < 0) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor.
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco_servidor.sin_port = htons(PORTA);

    // Vinculação do socket ao endereço e porta especificados.
    if (bind(socket_servidor, (struct sockaddr*)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        perror("Erro no bind");
        close(socket_servidor);
        exit(EXIT_FAILURE);
    }

    printf("Servidor ativo e aguardando mensagens...\n");

    while (1) {
        // Recebimento de mensagem do cliente.
        tamanho_cliente = sizeof(endereco_cliente);
        ssize_t bytes_recebidos = recvfrom(socket_servidor, buffer, TAMANHO_VETOR, 0, 
                                           (struct sockaddr*)&endereco_cliente, &tamanho_cliente);
        if (bytes_recebidos < 0) {
            perror("Erro ao receber dados");
            continue;
        }

        // Incrementa cada caractere recebido.
        for (int i = 0; i < TAMANHO_VETOR; ++i) {
            buffer[i]++;
        }

        // Envio da resposta modificada ao cliente.
        ssize_t bytes_enviados = sendto(socket_servidor, buffer, TAMANHO_VETOR, 0, 
                                        (struct sockaddr*)&endereco_cliente, tamanho_cliente);
        if (bytes_enviados < 0) {
            perror("Erro ao enviar resposta");
        }
    }

    // Fechamento do socket (nunca será alcançado neste loop infinito).
    close(socket_servidor);
    return 0;
}
