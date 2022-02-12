/*
TCP Iterative Server for client and server architecture
Developed for : DESD Sept 2021 Batch - CDAC, Pune
Server API Sequence:
1. socket
2. bind
3. listen
4. accept
5. read/write
6. close

*/
#include <sys/types.h>      //socket
#include <sys/socket.h>     // socket
#include <netinet/in.h>     // sockaddr_in
#include <netinet/ip.h>     // sockaddr_in
#include <strings.h>        // bzero
#include <arpa/inet.h>      //htons man 3 htons (host to network byte order short int)
#include <unistd.h>         // read and write system calls
#include <stdlib.h>          // exit
#include <stdio.h>

#define BACKLOGREQUEST 10
/*
Error Handler
*/

void error(char *errormsg)
{
    perror(errormsg);
    exit(1);
}

int main(int argc, char *argv[])
{
        int sockfd, retbind, data_exchange_sd, retlisten, server_port_number, total_number_of_read_bytes, total_number_of_write_bytes;

        char buffer_space[1024];  // To read and write information

        // sockaddr_in is type of structure which holds family type, IP and port number; details man 7 ip 
        
        /*
                Remember !!
                IP address and Port Number must be in the network byte order which is "Big Endian"
                So it must be uniform between client and server endianness it is good practice to 
                explicitely make it to 
                the network byte order

               struct sockaddr_in {
               sa_family_t    sin_family;  address family: AF_INET 
               in_port_t      sin_port;   port in network byte order 
               struct in_addr sin_addr;   internet address 
           };

           /* Internet address. 
           struct in_addr {
               uint32_t       s_addr;     /* address in network byte order 
           };

        */
        
        
        
        struct sockaddr_in serv_addr, cli_addr;
        if(argc < 2)
        {
            printf("Kindly provide the port number as first command line argument \n");
            printf("Use this format : ./a.out port_number e.g ./a.out 6200 \n");
        }
        //first argument must be supplied as port number through command line
        server_port_number = atoi(argv[1]); //ASCII to int type cast
        
        //Clear the buffer of  serv_addr structure of type sockaddr_in
        bzero((char *)&serv_addr,sizeof(serv_addr));
        
        serv_addr.sin_family = AF_INET; // Address family of IPV4
        
        serv_addr.sin_port = htons(server_port_number); // port number in network byte order

        serv_addr.sin_addr.s_addr = INADDR_ANY; //the socket will be bound to all local interfaces.
        
        /*

        INADDR_ANY - will accept any incoming request and bind to all the available interfaces that are active e.g
        1. Ethernet
        2. Wi-fi
        3. localhost
        4. Cellular (via USB0/WWAN)

        In nutshell : client can use IP address of any of the interface to connect the server

        here are several special addresses: INADDR_LOOPBACK (127.0.0.1) always refers to 
        the local host via the loopback device; INADDR_ANY (0.0.0.0) means any address for binding;
        INADDR_BROADCAST (255.255.255.255) means any host and has the same effect on bind as INADDR_ANY 
        for historical reasons.
        */

        /* 
         create a socket for incoming requests
         this socket will be used as a listener
         Never used for data transmission
         man 2 socket
         AF_INET = for IPV4 Family

        */
        sockfd = socket(AF_INET,SOCK_STREAM,0); // SOCK_STREAM -- TCP SOCK_DGRAM - for UDP
        if (sockfd == -1)
        {
            error("Failed to create the master/listener socket\n");
        }
        else
        {
            printf("Master socket is created with socket descriptor value : %d\n",sockfd);
        }
        // bind system call
        retbind = bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if(retbind == -1)
        {
            error("Socket failed to bind to an IP Address and Port Number\n");
        }
        else
        {
            printf("Server is running at port number : %d\n",server_port_number);
        }

        retlisten = listen(sockfd,BACKLOGREQUEST);
        if(retlisten == 0)
        {
            printf("Listen System calls working having %d connections queued\n",BACKLOGREQUEST);
        }
        else
        {
            error("Listen system call failed\n");
        }
        // Accept to fetch connection from backlog connections/queued
        int cli_addr_len = sizeof(cli_addr);
        data_exchange_sd = accept(sockfd,(struct sockaddr*)&cli_addr,&cli_addr_len);
        if(data_exchange_sd < 0)
        {
            error("Failed to Create the socket using accept system call \n");
        }
        else
        {
            printf("Accept system call create the the socket having socket descriptor as data_exchange_sd with value %d\n",data_exchange_sd);
        }

        // clear the buffer_space before using it to avoid any garbage value

        bzero(buffer_space,sizeof(buffer_space));
        
        /*
            Remember the above code skelton can be used for any use case, it is just boiler plating
            Core Logic of the code will be begin from here
            say read and write information using socket

            ssize_t read(int fd, void *buf, size_t count);
            ssize_t write(int fd, const void *buf, size_t count);
        */
          char acknowledgement[1024]  = "Hello Client We have received your Message !!\n";
          total_number_of_read_bytes = read(data_exchange_sd,&buffer_space,sizeof(buffer_space));  
          if(total_number_of_read_bytes == -1)
          {
              error("No Data has been read from the buffer space \n");
          }
          else
          {
              printf("Total number of bytes read from the buffer space is : %d\n", total_number_of_read_bytes);
              printf("Data from client : %s\n",buffer_space);
               // acknowledgement to the client using write system call
              total_number_of_write_bytes = write(data_exchange_sd,&acknowledgement,sizeof(acknowledgement));
              if(total_number_of_write_bytes == -1)
              {
                    error("Failed to send Ack !!\n");
              }
              else
              {
                  printf("%d byte data is sent as acknowledgement to the client \n",total_number_of_write_bytes);
              }
              
          }
          close(data_exchange_sd); // Close the socket created by socket()
          close(sockfd);  // CLose the socket create by accept
          return 0;
}