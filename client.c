#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char message[] = "End of work\n";
char buf[sizeof(message)];

struct info{
    int id;
    double x;
    double y;
    double z;
    double time;
    double temp;
    double dx;
};

int main()
{
    int sock , len;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("Socket dont created");
        exit(1);
    }
    printf("Socket created\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int z;
    while(true) {
    char r[3];
    printf("Enter request\n");
    scanf("%s",&r);
    sendto(sock, &r, sizeof(r), 0, (struct sockaddr *)&addr, sizeof(addr));
    if(strcmp(r,"NEW") == 0) {
    double X,Y,Z;
    printf("Enter X: ");
    scanf("%lf",&X);
    sendto(sock, &X, sizeof(X), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("Enter Y: ");
    scanf("%lf",&Y);
    sendto(sock, &Y, sizeof(Y), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("Enter Z: ");
    scanf("%lf",&Z);
    sendto(sock, &Z, sizeof(Z), 0, (struct sockaddr *)&addr, sizeof(addr));  
    printf("Enter file index: ");
    char index[10];
    scanf("%s",&index);
    printf("%s\n",index);
    sendto(sock, &index, sizeof(index), 0, (struct sockaddr *)&addr, sizeof(addr)); 
    sendto(sock, message, sizeof(message), 0, (struct sockaddr *)&addr, sizeof(addr));
    recvfrom(sock, buf, sizeof(message), 0, (struct sockaddr *)&addr, &len); 
    printf(buf);
    close(sock);
    }
    else if(strcmp(r,"GET") == 0) {
        printf("Enter file index: ");
        char ind[10];
        scanf("%s",&ind);
        printf("%s\n",ind);
        sendto(sock, &ind, sizeof(ind), 0, (struct sockaddr *)&addr, sizeof(addr)); 
        int c;
        recvfrom(sock, &c, sizeof(c), 0, (struct sockaddr *)&addr, &len); 
        if(c!=0) {
        printf("Time\tTemperature\tDisplacement X\n");
        int q;
        recvfrom(sock, &q, sizeof(q), 0, (struct sockaddr *)&addr, &len);
        for(int i=0;i<q;i++){
            struct info tmp;
            recvfrom(sock, &tmp, sizeof(tmp), 0, (struct sockaddr *)&addr, &len);
            printf("%3lf\t%2le\t%2lf\n",tmp.time,tmp.dx,tmp.temp);
        }
        }
        else{
            printf("ERROR");
        }
    }
    else if(strcmp(r,"DEL") == 0) {
        printf("Enter file index: ");
        char ind[10];
        scanf("%s",&ind);
        printf("%s\n",ind);
        sendto(sock, &ind, sizeof(ind), 0, (struct sockaddr *)&addr, sizeof(addr)); 
        printf("File deleted\n");
    }
    else {
        printf("Uncorrect request\n");
        return 0;
    }
    return 0;
    }
}