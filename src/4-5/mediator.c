#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NUM_CLIENTS 3
#define BUFFER_SIZE 256

enum Component
{
    Tobacco = 1,
    Paper = 2,
    Matches = 3
};

const char* getComponentName(enum Component component)
{
    switch (component)
    {
        case Tobacco:
            return "табак";
        case Paper:
            return "бумагу";
        case Matches:
            return "спички";
        default:
            return "неизвестный компонент";
    }
}

void send_message(int sockfd, struct sockaddr_in *client_addr, const char *message)
{
    int len = sizeof(*client_addr);
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)client_addr, len);
    if (n < 0)
    {
        perror("sendto");
        exit(1);
    }
}

void receive_message(int sockfd, struct sockaddr_in *client_addr, char *buffer)
{
    int len = sizeof(*client_addr);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)client_addr, &len);
    if (n < 0)
    {
        perror("recvfrom");
        exit(1);
    }
    buffer[n] = '\0';
}

void generateComponents(enum Component *selected_component1, enum Component *selected_component2)
{
    int random_component1 = rand() % 3 + 1;
    int random_component2 = rand() % 3 + 1;

    while (random_component2 == random_component1) {
        random_component2 = rand() % 3 + 1;
    }

    *selected_component1 = (enum Component)random_component1;
    *selected_component2 = (enum Component)random_component2;
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr[NUM_CLIENTS];
    char buffer[BUFFER_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "Использование: %s <адрес_сервера> <порт сервера>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    printf("Сервер запущен на адресе %s:%d\n", argv[1], atoi(argv[2]));

    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        printf("Ожидание клиента %d...\n", i + 1);
        receive_message(sockfd, &client_addr[i], buffer);
        printf("Получено: %s\n", buffer);
        snprintf(buffer, BUFFER_SIZE, "Добро пожаловать, курильщик %d!", i + 1);
        send_message(sockfd, &client_addr[i], buffer);
        printf("Отправлено: %s\n", buffer);
    }

    printf("Все клиенты подключены.\n");

    while (1)
    {
        enum Component component1, component2;
        generateComponents(&component1, &component2);

        printf("Посредник положил на стол: %s и %s\n", getComponentName(component1), getComponentName(component2));

        snprintf(buffer, BUFFER_SIZE, "%d %d", component1, component2);
        for (int i = 0; i < NUM_CLIENTS; i++)
        {
            send_message(sockfd, &client_addr[i], buffer);
        }
        receive_message(sockfd, &client_addr[NUM_CLIENTS - 1], buffer);
        printf("Один из курильщиков собрал сигарету.\n");
        sleep(5);
        printf("Сигарета докурена. Посредник снова генерирует компоненты.\n");
    }

    close(sockfd);
    return 0;
}
