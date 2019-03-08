#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"Ws2_32.lib")
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define closesocket close
#endif

#define PORT 80

//http://file6.goodid.com/20017/2019/03/07/1fffc2a0c45a0279a02123f9f0bad02a.1e
int main(int argc, char ** argv) {
    int sockfd, num;
    struct hostent *he;
    struct sockaddr_in server;

#ifdef _WIN32
    WSADATA wsaData = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif


    he = gethostbyname("file6.goodid.com");
    if (he == NULL) {
        printf("gethostbyname error\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket error\n");
        exit(1);
    }
    memset(&server, sizeof(server),0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr*)he->h_addr);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        printf("connect error\n");
        exit(1);
    }

    const char* req = "GET /20017/2019/03/07/1fffc2a0c45a0279a02123f9f0bad02a.1e HTTP/1.1\r\n"
        "Host: file6.goodid.com\r\n"
        "Connection: keep-alive\r\n"
        "Pragma: no-cache\r\n"
        "Cache-Control: no-cache\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36\r\n"
        "Accept: image/webp,image/apng,image/*,*/*;q=0.8\r\n\r\n";

    if (send(sockfd, req, strlen(req),0) == -1) {
        printf("send msg error: %s \n", strerror(errno));
        exit(1);
    }

    FILE* file = fopen("data.txt","wb");
    char buf[1024];
    while (1) {
        int len = recv(sockfd, buf, sizeof(buf), 0);
        if (len > 0) {
            fwrite(buf, 1, len, file);
        }
        else
            break;
    }

    fclose(file);
    closesocket(sockfd);
    return 0;
}