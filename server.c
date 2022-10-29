#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
    int sock;
    struct sockaddr_in address, client;
    int len = sizeof(client);
    char buf[1024];
    int bytes_read;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("Socket dont created");
        exit(1);
    }
    printf("Socket created\n");
    
    address.sin_family = AF_INET;
    address.sin_port = htons(3425);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&address.sin_zero,sizeof(address.sin_zero));
    if(bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Bind error");
        exit(2);
    }
    printf("Bind completed\n");

    bool work = true;
    FILE *BD_Coords, *BD;
    double X,Y,Z;
    int z = 0;
    struct info allinfo[10825];
    char _id1[100], _id2[100] , _x[100], _y[100], _z[100],_time[100],_temp[100],_dx1[100],_dx2[100];

    while (work){
    char r[3];
    printf("Waiting request\n");
    recvfrom(sock, &r, sizeof(r), 0, (struct sockaddr *)&client, &len);
    printf("%c%c%c\n",r[0],r[1],r[2]);

    if(r[0]=='N' && r[1]=='E' && r[2]=='W') {
    recvfrom(sock, &X, sizeof(X), 0, (struct sockaddr *)&client, &len);
    printf("x : %lf\n",X);
    recvfrom(sock, &Y, sizeof(Y), 0, (struct sockaddr *)&client, &len);
    printf("y : %lf\n",Y);
    recvfrom(sock, &Z, sizeof(Z), 0, (struct sockaddr *)&client, &len);
    printf("z : %lf\n",Z);
    char ind[10];
    recvfrom(sock, &ind, sizeof(ind), 0, (struct sockaddr *)&client, &len);
    printf("Index file: %s",ind);

    printf("Start reading files\n");
    BD_Coords = fopen("BD_Coords.txt", "r");
    fscanf(BD_Coords, "%s\t%s\t%s\t%s\t%s\n", &_id1,&_id2,&_x,&_y,&_z);
    for (int i=0;i<10824;i++) {
        fscanf(BD_Coords, "%i\t%lf\t%lf\t%lf\n", &allinfo[i].id, &(allinfo[i].x), &(allinfo[i].y), &(allinfo[i].z));
    }

    BD = fopen("BD.txt", "r");
    fscanf(BD, "%s\t%s\t%s\t%s\t%s\t%s\n", &_time,&_id1,&_id2,&_temp,&_dx1,&_dx2);
    for (int i=0;i<10824;i++) {
        fscanf(BD, "%lf\t%i\t%lf\t%lf\n", &allinfo[i].time, &allinfo[i].id, &allinfo[i].temp, &allinfo[i].dx);
    }
    printf("End reading files\n");

    printf("%s %s\t%s\t%s %s\t%s\n", _id1,_id2,_time,_dx1,_dx2,_temp);

    for (int i = 0;i < 10824; i++){
        if (allinfo[i].x==X && allinfo[i].z==Z && allinfo[i].y==Y){
            z++;
            printf("%i\t%3lf\t%2le\t%2lf\n",allinfo[i].id,allinfo[i].time,allinfo[i].dx,allinfo[i].temp);
        }
    }
    FILE *SAVE;
    char *indfile = strcat(ind,"SAVE.txt");
    SAVE = fopen(indfile, "w");
    fprintf(SAVE,"%s %s\t%s\t%s %s\t%s\n", _id1,_id2,_time,_dx1,_dx2,_temp);
    for (int i = 0;i < 10824; i++){
        if (allinfo[i].x==X && allinfo[i].z==Z && allinfo[i].y==Y) {
            fprintf(SAVE,"%i\t%3lf\t%2le\t%2lf\n",allinfo[i].id,allinfo[i].time,allinfo[i].dx,allinfo[i].temp);
        }
    }
    fclose(SAVE);

    recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&client, &len);
    sendto(sock, buf, bytes_read, 0, (struct sockaddr *)&client, len);
    printf(buf);
    }
    else if(r[0]=='G' && r[1]=='E' && r[2]=='T') {
        char ind[10];
        recvfrom(sock, &ind, sizeof(ind), 0, (struct sockaddr *)&client, &len);
        printf("Index file: %s\n",ind);
        struct info f[10824];
        FILE *SAVE;
        char *f_ = strcat(ind,"SAVE.txt");
        SAVE = fopen(f_, "r");
        int correct = 0;
        if(SAVE == NULL){
            printf("Error index file\n");
            sendto(sock, &correct, sizeof(correct), 0, (struct sockaddr *)&client, sizeof(client)); 
            continue;
        }
        correct = 1;
        sendto(sock, &correct, sizeof(correct), 0, (struct sockaddr *)&client, sizeof(client)); 
        int q = 0;
        fscanf(SAVE,"%s %s\t%s\t%s %s\t%s\n", &_id1,&_id2,&_time,&_dx1,&_dx2,&_temp);
        for(int i =0;i<10824;i++){
            fscanf(SAVE,"%i\t%lf\t%lf\t%lf\n",&f[i].id,&f[i].time,&f[i].dx,&f[i].temp);
            if (f[i].id<=0)
                break;
            q++;
        }
        sendto(sock, &q, sizeof(q), 0, (struct sockaddr *)&client, len);
        for (int i=0;i<q;i++){
            sendto(sock, &f[i], sizeof(f[i]), 0, (struct sockaddr *)&client, len);
        }
        fclose(SAVE);
    }
    else if(r[0]=='D' && r[1]=='E' && r[2]=='L') {
        char ind[10];
        recvfrom(sock, &ind, sizeof(ind), 0, (struct sockaddr *)&client, &len);
        printf("Index file: %s",ind);
        char *f_ = strcat(ind,"SAVE.txt");
        remove(f_);
    }
    else {
        printf("\nUncorrect request\n");
            fclose(BD_Coords);
        fclose(BD);
        close(sock);
    }
    }
    fclose(BD_Coords);
    fclose(BD);
    close(sock);
    return 0;
}