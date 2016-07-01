#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 256
#define UBIDOTS_TRANSLATE "translate.ubidots.com"
#define UBIDOTS_PORT 9010

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}


int connectSocket(char* hostname, int portno){
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /* socket: create the socket */
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }
    
    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    
    /* connect: create a connection with the server */
    if (connect(sockfd,(struct sockaddr *) &serveraddr,sizeof(serveraddr)) < 0)  {
        //if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
        error("ERROR connecting");
    }
    return sockfd;
}


void getLastValue(int sockfd, char* buf, char* token, char* datasource, char* variable){
    int n;
    bzero(buf, BUFSIZE);
    sprintf(buf, "ONION.v1.0|LV|%s|%s:%s|end", token, datasource, variable);

    n = write(sockfd, buf, strlen(buf));
    if (n < 0) 
      error("ERROR writing to socket");

    bzero(buf, BUFSIZE);
    n = read(sockfd, buf, BUFSIZE);
    if (n < 0) {
        error("ERROR reading from socket");
    }
}

void sendData(int sockfd, char* buf, char* token, char* datasource, char* data){
    int n;
    bzero(buf, BUFSIZE);
    sprintf(buf, "ONION.v1.0|POST|%s|%s=>%s|end", token, datasource, data);

    n = write(sockfd, buf, strlen(buf));
    if (n < 0) 
      error("ERROR writing to socket");

    bzero(buf, BUFSIZE);
    n = read(sockfd, buf, BUFSIZE);
    if (n < 0) {
        error("ERROR reading from socket");
    }
}

int main(int argc, char **argv) {
    int sockfd, n;
    char buf[BUFSIZE];
    char* token;
    char* datasource;
    char* variable;
    char* data;

    token = new char[512];
    datasource = new char[512];
    variable = new char[512];
    data = new char[512];

    
    if (argc < 2) {
       fprintf(stderr,"usage: %s [send, get] <options>\n", argv[0]);
       exit(1);
    }

    for (int i=2; i<argc; i++) {
        if (strcmp(argv[i], "-t") == 0){
            strcpy(token, argv[++i] );
        }else if (strcmp(argv[i], "-ds") == 0){
            strcpy(datasource, argv[++i] );
        }else if (strcmp(argv[i], "-v") == 0){
            strcpy(variable, argv[++i] );
        }else if (strcmp(argv[i], "-d") == 0){
            strcpy(data, argv[++i] );
        }
    }

    if (!(strcmp(argv[1], "send")==0 || strcmp(argv[1], "get")==0) || strcmp(token, "")==0){
        fprintf(stderr,"usage: %s [send, get] -t <token> -d <datasource> -v <variable> -d <data>\n", argv[0]);
        exit(1);
    }

    /* check command line arguments */

    sockfd  = connectSocket(UBIDOTS_TRANSLATE, UBIDOTS_PORT);
    
    if (strcmp(argv[1], "send")==0 && !strcmp(datasource, "")==0 && !strcmp(data, "")==0){
        sendData(sockfd, buf, token, datasource, data);
        printf(buf);
    }else if(strcmp(argv[1], "get")==0  && !strcmp(datasource, "")==0 && !strcmp(variable, "")==0){
        getLastValue(sockfd, buf, token, datasource, variable);
        printf(buf);        
    }else{
       fprintf(stderr,"usage: %s [send, get] -t <token> -d <datasource> -v <variable> -d <data>\n", argv[0]);
       exit(1);
    }
    
    close(sockfd);
    return 0;
}
