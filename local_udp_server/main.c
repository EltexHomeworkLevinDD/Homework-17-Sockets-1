#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/sock_local_udp_server"
#define BACKLOG 10
#define BUFFER_SIZE 32

int main(){
    // Создаём локальный потоковый сокет
    unlink(SOCKET_PATH);
    int socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
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

    char recv_buffer[BUFFER_SIZE];
    socklen_t client_addr_len = sizeof(socket_addr);
    ssize_t bytes_received = recvfrom(socket_fd, (void*)recv_buffer, BUFFER_SIZE, 0, 
        (struct sockaddr*)&client_addr, &client_addr_len);
    switch (bytes_received)
    {
    // Ошибка
    case -1:
        perror("recv");
        close(socket_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
        break;
    // Соединение разорвано второй стороной
    case 0:
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
    ssize_t bytes_sended = sendto(socket_fd, (void*)send_buffer, BUFFER_SIZE, 0, 
        (const struct sockaddr*)&client_addr, client_addr_len);
    if (bytes_sended == -1){
        perror("send()");
        close(socket_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    printf("Sendend: '%s'\n", send_buffer);

    usleep(10000);

    close(socket_fd);
    unlink(SOCKET_PATH);

    return 0;
}