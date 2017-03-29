#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/mman.h>
int Max_number_of_clients = 10;
int Maximum_length = 16;
int start_port = 25000;
int max_ports=10;

//takes a message encode it and return the encoded message
// format : messagetype | message length | message
uint16_t * encode_message(uint16_t type, char *message){
    uint16_t *encoded_message = (uint16_t *) malloc(sizeof(uint16_t) * (Maximum_length + 2));
    size_t len;
    len = strlen(message);
    while(len > 0 && (message[len-1] == '\n' || message[len-1] == '\r'))
        message[--len] == 0;
    encoded_message[0] = htons(type);
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


uint16_t udp(uint16_t port_number,int client_number,char *client_ip){
    int sockfd,newsockfd,clientlen;
    struct sockaddr_in server,client;

    uint16_t data[Maximum_length+2];
    int input_size = sizeof(uint16_t)*(Maximum_length+2);

    socklen_t client_addr_size;
    clientlen = sizeof(struct sockaddr);
    int n;
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if(sockfd<0){
        printf("Could not create socket\n");
        return port_number;
    }
    // assigning valuesto server 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_number);
    bzero(&server.sin_zero,8);
    // binding server
    if(bind(sockfd, (struct sockaddr *) &server,sizeof(server))<0){
        printf("Unable to bind \n");
        return port_number;
    }

    printf("%d\t\t%u\tudp stage\tudp\t\tstage stage start\n", client_number, port_number);
    
    while(1){
        int encoded_udp_message = recvfrom(sockfd,data, input_size,0,(struct sockaddr *)&client, &clientlen);
        
        if(encoded_udp_message<0){
            printf("receiving udp message error\n");
        }

        char *decoded_message = decode_message(data);
        uint16_t *message;
        int udp_size;

        if(strcmp(decoded_message,"quitudp")==0){
            message = encode_message(4,"bye");
            udp_size = sendto(sockfd, message, input_size,0,(struct sockaddr *)&client,clientlen);
            printf("%d ip: %s\t%u\tmessage type3\tudp\t\tmessage received: %s\n", client_number,client_ip, port_number, decoded_message);
            printf("%d ip: %s\t%u\tmessage type4\tudp\t\tmessage sent: %u\n", client_number,client_ip, port_number, *message);
        }else{
            message = encode_message(4, decoded_message);
            udp_size = sendto(sockfd, message, input_size, 0, (struct sockaddr *)&client,
                                      clientlen);
            printf("%d ip: %s\t%u\tmessage type3\tudp\t\tmessage received: %s\n", client_number,client_ip, port_number, decoded_message);
            printf("%d ip: %s\t%u\tmessage type4\tudp\t\tmessage sent: %s\n", client_number,client_ip, port_number, decoded_message);
        }

        if(udp_size < 0){
            printf("sending error in udp\n");;
            close(sockfd);
            exit(0);
        }else if(udp_size == 0){
            printf("%d ip: %s\t%u\tudp close\tudp\t\tclient closed arbitrarily\n", client_number,client_ip, port_number);
            close(sockfd);
        }else{
                //if message 3 is "quitudp" close the socket
                int closeudp = strcmp(decoded_message, "quitudp");
                if(closeudp == 0){
                    printf("%d ip: %s\t%u\tudp close\tudp\t\tclose udp socketfd\n", client_number,client_ip, port_number);
                    close(sockfd);
                    break;
                }

        }
    }

    return port_number;
}

void tcp(int port_number){

    uint16_t *Available_ports = mmap(NULL, max_ports * sizeof(uint16_t), PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint16_t *Free_ports = mmap(NULL, max_ports*sizeof(uint16_t), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int j;
    for(j=0;j<max_ports;j++){
    	Available_ports[j]= start_port+j;
    	Free_ports[j] = 0;
    }

    int sockfd,newsockfd,client_address_length;
    struct sockaddr_in server,client;
    int n,pid;
    uint16_t data[Maximum_length+2];

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd<0){
        printf("Could not create socket\n");
        return ;
    }

    // assigning valuesto server 
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    // binding server
    if(bind(sockfd, (struct sockaddr *) &server,sizeof(server))<0){
        printf("Unable to bind \n");
        return ;
    }

    // max client to listen
    listen(sockfd,Max_number_of_clients);
    int active_clients=0;

    printf("----------------------------------------------------------------------------------\n");
    printf("Client No\tPort\tOperation\tProtocol\tMore Information\n");
    printf("----------------------------------------------------------------------------------\n");

    while(active_clients < Max_number_of_clients){
        // parent process waiting for a new connection
        printf("Server waiting for new clients\n");
        client_address_length = sizeof(client);
        newsockfd = accept(sockfd,(struct sockaddr *) &client, &client_address_length);

        if(newsockfd<0){
            printf("---\t\t%d\tconnect error\ttcp\t\tTotal clients: %d\n", port_number, active_clients);
            close(newsockfd);
            continue;
        }
        printf("Client is connected wtih address %s \n",inet_ntoa(client.sin_addr));
        char * client_ip = inet_ntoa(client.sin_addr);

        // create a child process for serving clients concurrently
        pid = fork();
        if(pid==0){
            close(sockfd);

            int len = 1;
            int close_conn = 0;
            active_clients+=1;
            int client_number = active_clients;

            printf("%d ip: %s\t%d\tconnect\t\ttcp\t\tTotal clients: %d\n",client_number,client_ip,port_number,active_clients);
            while(len){
                int i,input_size= (Maximum_length+2)*(sizeof(uint16_t));
                len = recv(newsockfd,data,input_size,0);
                char *decoded_message = decode_message(data);
                int check_udp = strcmp(decoded_message,"getport");

                if(check_udp!=0){ // getport is not in message sao simply send type2 message
                    uint16_t *encoded_message = encode_message(2,decoded_message);
                    send(newsockfd,encoded_message,input_size,0);
                    printf("%d ip: %s\t%d\tmessage type1\ttcp\t\tmessage received: %s\n", client_number,client_ip, port_number, decoded_message);
                    printf("%d ip: %s\t%d\tmessage type2\ttcp\t\tmessage sent: %s\n", client_number,client_ip, port_number, decoded_message);
         	    }else{// getudp is the message so send a free port to client for udp connection
                    uint16_t udp_port;

                    //check for available ports
                    for(i=0;i<Max_number_of_clients;i++){
                        if(Free_ports[i]==0){
                            udp_port = Available_ports[i];
                            break;
                        }
                    }
                
                    char message[Maximum_length];
                    sprintf(message,"%u",udp_port); // assign udp port to message
                    uint16_t *encoded_message = encode_message(2,message);
                    int sent = send(newsockfd,encoded_message,input_size,0);

                    if(sent<0){
                        printf("Unable to send type2 message\n");
                        exit(EXIT_FAILURE);
                    }

                    printf("%d ip: %s\t%d\tmessage type1\ttcp\t\tmessage received: %s\n", client_number,client_ip, port_number, decoded_message);
                    printf("%d ip: %s\t%d\tmessage type2\ttcp\t\tmessage sent: %s\n", client_number,client_ip, port_number, message);
                    printf("%d ip: %s\t%d\ttcp close\ttcp\t\tconnection closed\n", client_number,client_ip, port_number);
                    printf("%d ip: %s\t---\tport assigned\ttcp\t\tport assigned: %u\n", client_number,client_ip, udp_port);
                    
                    // //Close the TCP connection for that client 
                    close(newsockfd);

                    // start udp socket
                    uint16_t free_port = udp(udp_port,client_number,client_ip);
                    // when connection to that client is closed then free the assigned port
                    for(i=0;i<Max_number_of_clients;i++){
                        if(Available_ports[i]==free_port){
                            if(Free_ports[i]==1){
                                Free_ports[i]=0;
                            }
                        }
                    }

                    printf("%d\t\t---\tfree port\tudp\t\tport freed: %u\n", client_number, free_port);
                // reduce the number of clients by one
                active_clients -= 1;
                break;
                }
            }

            if(len<0){
                printf("Cannot read from client\n");
                exit(EXIT_FAILURE);
            }

            if(len==0){
                printf("%d\t\t%d\ttcp close\ttcp\t\tclient closed arbitarily\n", client_number, port_number);
                close(newsockfd);
            }

            printf("---\t\t---\texit child\ttcp\t\tactive clients:%d\n", active_clients);
            exit(EXIT_SUCCESS);
         }

    }
    return;
}



int main(int argc, char const *argv[])
{
    if(argc<2){
        printf("No port_number provided\n");
        return 0;
    }
    tcp(atoi(argv[1]));
    return 0;
}