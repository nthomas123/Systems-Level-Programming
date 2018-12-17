#include <stdio.h>
#include "csapp.h"
#include "string.h"
/*
Noble Thomas
Goal of this lab is to create a simple proxy with Caching. 
I used tiny server as a reference for proxy server. Parse request, validate request, 
establish a connection with end server, and forward the response to the client.
*/

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 16777216 
#define MAX_OBJECT_SIZE 8388608 

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

// Functions
void doit(int fd);
void read_requesthdrs(rio_t *rp);
void proxyClient(char *host, int *port, char *uri, char *pathname, int fd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void parseRequest_uri(char *uri, char *hostname, char *pathname, int *port);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    //Prints out connection
    if (argc != 2){
        fprintf(stderr,"usage: %s <port> \n", argv[0]);
        exit(1);
    }
    Signal(SIGPIPE, SIG_IGN);


    //Proxy Server
    listenfd = Open_listenfd(argv[1]); //Create a socket and listening port
    while (1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);// Accept incoming signals
           Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
            printf("Accepted connection from (%s,%s)\n", hostname, port);
        doit(connfd);
        Close(connfd);
    }
    return 0;
}
//Handles one HTTP request/response transaction
void doit(int fd){
    int port;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], hostname[MAXLINE], pathname[MAXLINE];
    rio_t rio;

    
    Rio_readinitb(&rio, fd);                    /* Read request line and headers */
    if (!Rio_readlineb(&rio, buf, MAXLINE))  //Reads the request of the header
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);  //Parse the HTTP Request
    
// Changes HTTP request version Possible error
    if(!strcasecmp(version, "HTTP/1.1")){
       strcpy(version, "HTTP/1.0");
      }

//Determines if Method is GET (Error)
    if (strcasecmp(method, "GET")) {                     
       clienterror(fd, method, "501", "Not Implemented",
                   "Tiny does not implement this method");
        return;
    }                                                    
    read_requesthdrs(&rio);  //Reads the request handler

//Gennerates a new request for server
     parseRequest_uri(uri, hostname, pathname, &port);
    printf("Hostname = %s\n", hostname);
    printf("Pathname = %s\n", pathname);
    printf("Port = %d\n", port);
    //Sends Parameters to the client side of the proxy
    proxyClient(hostname, &port, uri, pathname, fd);
} 
/*Parse HTTP Request into Hostname nand Path so it can determine if it should open a 
connection to the hostname and send the HTTP request in
in the form of GET /index.hmtl HTTP/1.0/ */
void parseRequest_uri(char *uri, char *hostname, char *pathname, int *port){
    char *hostBeginPtr;
    char *hostEndPtr;
    char *pathBegin;
    char *portPtr;
    *port = 80;
// Parse Host
    hostBeginPtr = strstr(uri, "http://"); // Finds the first occurance of http://
    if(hostBeginPtr != 0){
        hostname[0] = '\0'; // Set end of line to NULL
    } 
    hostBeginPtr = hostBeginPtr + 7; //Access to start of the Host Name
    hostEndPtr = strpbrk(hostBeginPtr, " :/\r\n\0"); // Access to the end of the Host Name
    int distanceBetweenPointer;
    distanceBetweenPointer = hostEndPtr - hostBeginPtr;
    strncpy(hostname, hostBeginPtr, distanceBetweenPointer); // Store the Host Name from the Start to the End 
// Parse Path
    pathBegin = strchr(hostBeginPtr, '/'); //Access to start of the path
    if(pathBegin == NULL){
        pathname[0] = '\0';
    }else{
    strcpy(pathname, pathBegin);//Store the Path String from the start and to the End
    }
// Parse Port
    //Find out if the uri contains a ":" starting from the start if the hostName
     portPtr = strchr(hostBeginPtr, ':');
    // if there is none then set the port to 0
    if (portPtr != NULL){
        *port = atoi(portPtr +1);
    }

}
// After getting the request from client, we must turn the proxy into the Client side of the Proxy
// to get a valid response from the web server. 
void proxyClient(char *hostname, int *port, char *uri, char *pathname, int fd){
    int clientfd;
    char buffer1[MAXLINE];
    char readBuffer[MAXLINE];
    rio_t rio2;
   // Since Open Client takes in char character, must convert port
    char portChar[10];
    sprintf(portChar, "%d", *port);
    // Opens client descriptor and establishes a connection with end server
    clientfd = Open_clientfd(hostname,portChar);
    Rio_readinitb(&rio2, clientfd);
    
   /*Store the request response in a buffer and write into 
   the file descriptor (Request header) for each response*/
    sprintf(buffer1, "GET %s HTTP/1.0\r\n", pathname);
   Rio_writen(clientfd, buffer1, strlen(buffer1));
   sprintf(buffer1, "Host: %s\r\n", hostname);
   Rio_writen(clientfd, buffer1, strlen(buffer1));
    sprintf(buffer1, "Proxy-Connection: close\r\n");
   Rio_writen(clientfd, buffer1, strlen(buffer1));
    sprintf(buffer1, "%s\r\n", user_agent_hdr);
   Rio_writen(clientfd, buffer1, strlen(buffer1));
    sprintf(buffer1, "Connection: close\r\n\r\n");
   Rio_writen(clientfd, buffer1, strlen(buffer1));

    int fdlength;
   //End server Response will be read into the file descriptor and send response back to
   // the original client fd and write to client fd
   while((fdlength = Rio_readlineb(&rio2, readBuffer, sizeof(readBuffer))) != 0){
   Rio_writen(fd, readBuffer, fdlength);
   }
   Close(clientfd);

}


/*
 * read_requesthdrs - read HTTP request headers
 * (Process request when client enters information)
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    if (!Rio_readlineb(rp, buf, MAXLINE))
      return;
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
    }
    return;
}
/* $end read_requesthdrs */


/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    sprintf(buf, "%sContent-type: text/html\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n\r\n", buf, (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
/* $end clienterror */




