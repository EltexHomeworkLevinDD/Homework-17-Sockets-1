#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH         "/tmp/sock_local_tcp_client"
#define SERVER_SOCKET_PATH  "/tmp/sock_local_tcp_server"
#define BUFFER_SIZE 32

int main(){
    unlink(SOCKET_PATH);
    // Создаём локальный потоковый сокет
    int own_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (own_fd == -1){
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру адреса сервера
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path));
    
    // // Заполняем структуру адреса клиента
    // struct sockaddr_un own_addr;
    // own_addr.sun_family = AF_LOCAL;
    // strncpy(own_addr.sun_path, SOCKET_PATH, sizeof(own_addr.sun_path));

    // // Привязываем сокет клиента к адресу (Здесь биндить клиента не обязательно)
    // if (bind(own_fd, (const struct sockaddr*)&own_addr, sizeof(struct sockaddr_un)) == -1){
    //     close(own_fd);
    //     unlink(SOCKET_PATH);
    //     perror("Bind()");
    //     exit(EXIT_FAILURE);
    // }

    // Неблокирующе пытаемся соедениться
    if (connect(own_fd, (const struct sockaddr*)&server_addr, (socklen_t)sizeof(server_addr)) == -1){
        perror("connect()");
        close(own_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    // Отправляем данные
    char send_buffer[BUFFER_SIZE];
    strncpy(send_buffer, "Hello!", BUFFER_SIZE);
    ssize_t bytes_sended = send(own_fd, send_buffer, BUFFER_SIZE, 0);
    if (bytes_sended == -1){
        perror("send()");
        close(own_fd);
        unlink(SOCKET_PATH);
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
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
        break;
    // Соединение разорвано второй стороной
    case 0:
        close(own_fd);
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

    close(own_fd);
    unlink(SOCKET_PATH);

    return 0;
}