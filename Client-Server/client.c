#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
int Maximum_length = 16;

//takes a message encode it and return the encoded message
// format : messagetype | message length | message
uint16_t * encode_message(uint16_t type, char *message){
    uint16_t *encoded_message = (uint16_t *) malloc(sizeof(uint16_t) * (Maximum_length + 2));
    size_t len;
    len = strlen(message);
    while(len > 0 && (message[len-1] == '\n' || message[len-1] == '\r')) // '\r' is return key
        message[--len] == 0;
    encoded_message[0] = htons(type);   //htons = host to network short
    encoded_message[1] = htons((uint16_t) len);

    uint16_t i = 0;
    for(i = 0; i < strlen(message); i++){
        encoded_message[i+2] = htons((uint16_t) message[i]);
    }
    return encoded_message;
}


// Takes encoded message, decodes the encoded message and then returns decoded message string.
// decoding a message involves removing message type and message length field
char *decode_message(uint16_t *encoded_msg){
    char *decoded_message = malloc(sizeof(char) * Maximum_length);
    int i = 0;
    for(i = 2; i < Maximum_length+2; i++){
        decoded_message[i-2] = (char) ntohs(encoded_msg[i]);
    }
    return decoded_message;
}


void udp(int port_number,const char *serverip){
    int sockfd,n;
    struct sockaddr_in server;
    struct hostent *server_ip;

    char input[Maximum_length];
    uint16_t output[Maximum_length+2];
    socklen_t addr_size=sizeof(server);
    socklen_t sockaddr_len = sizeof(struct sockaddr);
    // create a udp socket
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){
        printf("Cannot create socket for udp\n");
        return ;
    }

    server_ip = gethostbyname(serverip);
    if(server_ip==NULL){
        printf("Host does not exist\n");
        return ;
    }

    // assign values to server to which client wants to connect
    server.sin_family = AF_INET;
    bcopy((char *)server_ip->h_addr,(char *)&server.sin_addr.s_addr, server_ip->h_length);
    server.sin_port = htons(port_number);
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))<0){
        printf("Cannot connect\n");
    }
    memset(&(server.sin_zero),0,sizeof(server));


    printf("%s\tudp start\tudp\t\t%u\t\tconnection initiated\n", serverip, port_number);
    while(1){

        printf("%s\tmessage type3\tudp\t\t%u\t\tmessage sent: ", serverip, port_number);
        bzero(input, Maximum_length);
        fgets(input, Maximum_length, stdin);

        // returns the number of characters in the initial segment of string str1 which are not in the string str2.
        input[strcspn(input, "\n")] = 0;

        // encode the input message to type3 encoding
        uint16_t *encoded_message = encode_message(3, input);
        size_t input_size = sizeof(uint16_t)*(Maximum_length+2);
        ssize_t sent = sendto(sockfd, encoded_message, input_size, 0, (struct sockaddr *)&server,
                                    sockaddr_len);
        if(sent < 0){
            perror("sendto");
            exit(EXIT_FAILURE);
        }else if(sent == 0){
            
            // Zero bytes sent which is probably because client closed the connection prematurely
            //                                 or
            //                     Connection closed by server.
            
            printf("%s\tclose\tudp\t\t%u\t\tserver not active\n", serverip, port_number);
            close(sockfd);
            exit(EXIT_SUCCESS);
        }

        socklen_t size_of = sizeof(struct sockaddr);
        ssize_t received_data = recvfrom(sockfd, output, input_size, 0, (struct sockaddr *)&server,
                                          &sockaddr_len);
        if(received_data > 0){
            char *decoded_message = decode_message(output);
            printf("%s\tmessage type4\tudp\t\t%u\t\tmessage received: %s\n", serverip, port_number, decoded_message);
        }
        else if(received_data < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }
        else if (received_data == 0){
            printf("%s\trecv error\tudp\t\t%u\t\tzero bytes received\n", serverip, port_number);
            printf("%s\texit\t\tudp\t\t%u\t\tserver has closed arbitarily\n", serverip, port_number);
            /* perror("server closed"); */
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if(strcmp(input, "quitudp") == 0){
            printf("%s\tclose udp\tudp\t\t%u\t\tclosing the connection\n", serverip, port_number);
            close(sockfd);
            break;
        }
    }
    return;
}

void tcp(const char *ip_address, int port_number){
    int sockfd,n;
    struct sockaddr_in server;
    struct hostent *server_ip;

    char input[Maximum_length];
    uint16_t output[Maximum_length+2];

    // cretae tcp socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        printf("Cannot create socket\n");
        return ;
    }

    server_ip = gethostbyname(ip_address);   // do not work with ipv6
    //gethostbyname() takes a string like "www.yahoo.com", and returns a struct hostent which contains tons of information, including the IP address.
    if(server_ip==NULL){
        printf("Host does not exist\n");
        return ;
    }

    // initializing server where clients want to connect
    memset(&server, 0,sizeof(server));
    server.sin_family = AF_INET;
    bcopy((char *)server_ip->h_addr,(char *)&server.sin_addr.s_addr, server_ip->h_length);
    server.sin_port = htons(port_number);
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))<0){
        printf("Cannot connect\n");
    }

    printf("Send \"getport\" to get a UDP port from server\n");
    printf("Once UDP Connection is established, send \"quitudp\" to quit udp transmission\n");

    printf("-----------------------------------------------------------------------------------\n");
    printf("Server IP\tOperation\tProtocol\tPort\t\tMore Information\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("%s\tconnect\t\ttcp\t\t%u\t\tconnection initiated to server\n", ip_address, port_number);
    while(1){

        printf("%s\tmessage type1\ttcp\t\t%u\t\tmessage sent: ", ip_address, port_number);
        bzero(input, Maximum_length);
        fgets(input, Maximum_length, stdin);
        input[strcspn(input, "\n")] = 0;
        uint16_t *encoded_message = encode_message(1, input);
        size_t input_size = sizeof(uint16_t)*(Maximum_length+2);
        int j=0;
        printf("encoded_message \n");
        for(j=0;j<Maximum_length+2;j++){
            printf("%u\n",encoded_message[j]);
        }
        ssize_t sent = send(sockfd, encoded_message, input_size, 0);
        if(sent < 0){
            printf("sending error\n");
            exit(EXIT_FAILURE);
        }
        ssize_t received_data = recv(sockfd, output, input_size, 0);

        if(received_data > 0){

            char *decoded_message = decode_message(output);
            if(strcmp(input, "getport") == 0){
                // if client requests getport then client will get a udp port from server
                // and client will close its tcp socket 
                // and starts its udp socket 
                printf("%s\tmessage type2\ttcp\t\t%u\t\tmessage received: %s\n", ip_address, port_number, decoded_message);
                printf("%s\ttcp close\ttcp\t\t%u\t\tconnection closed\n", ip_address, port_number);
                printf("%s\tport received\t---\t\t---\t\tudp port received: %s\n", ip_address, decoded_message);
                close(sockfd);
                udp((uint16_t) atoi(decoded_message),ip_address);
                break;
            }else{
                // else client will display the type2 message sent by sender
                printf("%s\tmessage type2\ttcp\t\t%u\t\treceived message: %s\n", ip_address, port_number, decoded_message);
            }

        }
        else if(received_data < 0){
            printf("reading error\n");
            exit(EXIT_FAILURE);
        }
        else if (received_data == 0){
            printf("%s\trecv error\ttcp\t\t%u\t\tzero bytes received\n", ip_address, port_number);
            printf("%s\texit\t\ttcp\t\t%u\t\tserver has closed arbitarily\n", ip_address, port_number);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    printf("---\t\texit\t\t---\t\t---\t\texiting\n");
    return;
}

int main(int argc, char const *argv[])
{
    if(argc<3){
        printf("Please provide 3 arguments\n");
    }
    uint16_t p[3];
    /*
    p[0]='a';
    p[1]='b';
    p[2]='c';
    int i;
    for(i=0;i<3;i++){
        printf("%u\n",p[i]);
    }*/
    tcp(argv[1],atoi(argv[2]));
    return 0;
}