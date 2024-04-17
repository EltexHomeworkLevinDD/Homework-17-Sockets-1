#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IP_ADDR "127.0.0.1"
#define PORT 8080
#define BACKLOG 10
#define BUFFER_SIZE 32

int main(){
    // Создаём локальный потоковый сокет
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socket_addr, client_addr;

    // Заполняем структуру собственного адреса
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(PORT);
    socket_addr.sin_zero[0] = '\0';
    inet_pton(AF_INET, IP_ADDR, &socket_addr.sin_addr);

    // Привязываем сокет к адресу
    if (bind(socket_fd, (const struct sockaddr*)&socket_addr, sizeof(struct sockaddr_in)) == -1){
        close(socket_fd);
        perror("Bind()");
        exit(EXIT_FAILURE);
    }

    // Переводим в режим ожидания
    if (listen(socket_fd, BACKLOG) == -1){
        close(socket_fd);
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    // Блокирующе подключаем клиента
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1){
        if (errno == EINTR){
            perror("accept() was interrupted");
        }
        perror("accept()");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Блокирующе получаем данные от клиента
    char recv_buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv(client_fd, (void*)recv_buffer, BUFFER_SIZE, 0);
    switch (bytes_received)
    {
    // Ошибка
    case -1:
        perror("recv");
        close(client_fd);
        close(socket_fd);
        exit(EXIT_FAILURE);
        break;
    // Соединение разорвано второй стороной
    case 0:
        close(client_fd);
        close(socket_fd);
        printf("The connection is broken by the second party\n");
        exit(EXIT_SUCCESS);
        break;
    // Обрабатываем данные
    default:
        recv_buffer[BUFFER_SIZE-1] = '\0';
        printf("Received: '%s'\n", recv_buffer);
        break;
    }

    // Отправляем данные
    char send_buffer[BUFFER_SIZE];
    strncpy(send_buffer, "Hi!", BUFFER_SIZE);
    ssize_t bytes_sended = send(client_fd, send_buffer, BUFFER_SIZE, 0);
    if (bytes_sended == -1){
        perror("send()");
        close(client_fd);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Sendend: '%s'\n", send_buffer);

    usleep(10000);

    close(client_fd);
    close(socket_fd);

    return 0;
}