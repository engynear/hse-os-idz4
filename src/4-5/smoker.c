#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 256

typedef enum {
    Tobacco = 1,
    Paper = 2,
    Matches = 3
} Component;

void send_message(int sockfd, struct sockaddr_in *server_addr, const char *message) {
    ssize_t n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)server_addr, sizeof(*server_addr));
    if (n < 0) {
        perror("Ошибка отправки сообщения");
        exit(1);
    }
}

void receive_message(int sockfd, struct sockaddr_in *server_addr, char *buffer) {
    socklen_t len = sizeof(*server_addr);
    ssize_t n = recvfrom(sockfd, buffer, MAX_BUFFER - 1, 0, (struct sockaddr *)server_addr, &len);
    if (n < 0) {
        perror("Ошибка приема сообщения");
        exit(1);
    }
    buffer[n] = '\0';
}

void smoke(int existing_component, int component1, int component2) {
    const char *names[] = {"", "табак", "бумагу", "спички"};
    printf("У клиента есть %s.\n", names[existing_component]);
    printf("Клиент берет %s и %s со стола.\n", names[component1], names[component2]);
    printf("Клиент курит...\n");
    sleep(1);
    printf("Клиент закончил курить.\n");
}

int validate_component(int component) {
    if (component < Tobacco || component > Matches) {
        fprintf(stderr, "Недопустимый имеющийся компонент: %d\n", component);
        exit(1);
    }
    return component;
}

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }
    return sockfd;
}

void bind_socket(int sockfd, struct sockaddr_in *client_addr) {
    if (bind(sockfd, (struct sockaddr *)client_addr, sizeof(*client_addr)) < 0) {
        perror("Ошибка привязки адреса");
        exit(1);
    }
}

void initialize_server_address(struct sockaddr_in *server_addr, const char *ip_address, int port) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    if (inet_aton(ip_address, &(server_addr->sin_addr)) == 0) {
        fprintf(stderr, "Ошибка в адресе IP: %s\n", ip_address);
        exit(1);
    }
}

void initialize_client_address(struct sockaddr_in *client_addr) {
    memset(client_addr, 0, sizeof(*client_addr));
    client_addr->sin_family = AF_INET;
    client_addr->sin_port = htons(0);
    client_addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

void run_client(const char *ip_address, int port, int existing_component) {
    int sockfd = create_socket();

    struct sockaddr_in server_addr, client_addr;
    initialize_server_address(&server_addr, ip_address, port);
    initialize_client_address(&client_addr);

    bind_socket(sockfd, &client_addr);

    printf("Клиент запущен\n");

    char buffer[MAX_BUFFER];
    sprintf(buffer, "Запущен клиент с %s!", (existing_component == Tobacco ? "табаком" : (existing_component == Paper ? "бумагой" : "спичками")));
    send_message(sockfd, &server_addr, buffer);
    receive_message(sockfd, &server_addr, buffer);

    while (1) {
        receive_message(sockfd, &server_addr, buffer);

        int component1, component2;
        if (sscanf(buffer, "%d %d", &component1, &component2) == 2) {
            if ((existing_component == Tobacco && ((component1 == Paper && component2 == Matches) || (component1 == Matches && component2 == Paper))) ||
                (existing_component == Paper && ((component1 == Tobacco && component2 == Matches) || (component1 == Matches && component2 == Tobacco))) ||
                (existing_component == Matches && ((component1 == Tobacco && component2 == Paper) || (component1 == Paper && component2 == Tobacco)))) {
                smoke(existing_component, component1, component2);
                send_message(sockfd, &server_addr, buffer);
            }
        }
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <IP сервера> <порт сервера> <имеющийся компонент>\n", argv[0]);
        exit(1);
    }

    int existing_component = validate_component(atoi(argv[3]));

    run_client(argv[1], atoi(argv[2]), existing_component);

    return 0;
}
