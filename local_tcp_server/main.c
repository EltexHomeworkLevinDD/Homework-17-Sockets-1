#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/sock_local_tcp_server"
#define BACKLOG 10
#define BUFFER_SIZE 32

int main(){
    // Создаём локальный потоковый сокет
    unlink(SOCKET_PATH);
    int socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socket_fd == -1){
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un socket_addr, client_addr;

    // Заполняем структуру собственного адреса
    socket_addr.sun_family = AF_LOCAL;
    strncpy(socket_addr.sun_path, SOCKET_PATH, sizeof(socket_addr.sun_path));

    // Привязываем сокет к адресу
    if (bind(socket_fd, (const struct sockaddr*)&socket_addr, sizeof(struct sockaddr_un)) == -1){
        close(socket_fd);
        unlink(SOCKET_PATH);
        perror("Bind()");
        exit(EXIT_FAILURE);
    }

    // Переводим в режим ожидания
    if (listen(socket_fd, BACKLOG) == -1){
        close(socket_fd);
        unlink(SOCKET_PATH);
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
        unlink(SOCKET_PATH);
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
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
        break;
    // Соединение разорвано второй стороной
    case 0:
        close(client_fd);
        close(socket_fd);
        unlink(SOCKET_PATH);
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
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    printf("Sendend: '%s'\n", send_buffer);

    usleep(10000);

    close(client_fd);
    close(socket_fd);
    unlink(SOCKET_PATH);

    return 0;
}