#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IP_ADDR "127.0.0.1"
#define PORT 8081
#define BACKLOG 10
#define BUFFER_SIZE 32

int main(){
    // Создаём локальный потоковый сокет
    int own_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (own_fd == -1){
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру адреса сервера
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_zero[0] = '\0';
    inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);

    // Неблокирующе пытаемся соедениться
    if (connect(own_fd, (const struct sockaddr*)&server_addr, (socklen_t)sizeof(server_addr)) == -1){
        perror("connect()");
        close(own_fd);
        exit(EXIT_FAILURE);
    }

    // Отправляем данные
    char send_buffer[BUFFER_SIZE];
    strncpy(send_buffer, "Hello!", BUFFER_SIZE);
    ssize_t bytes_sended = send(own_fd, send_buffer, BUFFER_SIZE, 0);
    if (bytes_sended == -1){
        perror("send()");
        close(own_fd);
        exit(EXIT_FAILURE);
    }
    printf("Sendend: '%s'\n", send_buffer);

    // Блокирующе получаем данные
    char recv_buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(own_fd, (void*)recv_buffer, BUFFER_SIZE, 0);
    switch (bytes_received)
    {
    // Ошибка
    case -1:
        perror("recv");
        close(own_fd);
        exit(EXIT_FAILURE);
        break;
    // Соединение разорвано второй стороной
    case 0:
        close(own_fd);
        printf("The connection is broken by the second party\n");
        exit(EXIT_SUCCESS);
        break;
    // Обрабатываем данные
    default:
        recv_buffer[BUFFER_SIZE-1] = '\0';
        printf("Received: '%s'\n", recv_buffer);
        break;
    }

    close(own_fd);

    return 0;
}