/* A simple server in the internet domain using TCP
   The port number is passed as an argument */


/*******************************************/
/////// USE std:: WHEREVER REQUIRED /////////
/*******************************************/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <signal.h>
#include <sqlite3.h>
#include <sstream>
#include <sodium.h>
#include <sstream>

#define KEY_LEN crypto_box_SEEDBYTES

int portno;

std::pair<std::string, std::string> hash_password(std::string password) {
    const char* PASSWORD = password.c_str();
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char key[KEY_LEN];

    // random salt generate
    randombytes_buf(salt, sizeof salt);

    if (crypto_pwhash (key, sizeof key, PASSWORD, strlen(PASSWORD), salt,
         crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
         crypto_pwhash_ALG_DEFAULT) != 0) {
        /* out of memory */
    }

    std::pair<std::string, std::string> p;
    p.first = (reinterpret_cast<char*>(key));
    p.second = (reinterpret_cast<char*>(salt));
    return p;
}

bool check_password(std::string password, std::string key_given, std::string salt_given) {
    const char* PASSWORD = password.c_str();
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char key[KEY_LEN];

    memcpy(salt, salt_given.c_str(), salt_given.size() + 1);

    if (crypto_pwhash (key, sizeof key, PASSWORD, strlen(PASSWORD), salt,
         crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
         crypto_pwhash_ALG_DEFAULT) != 0) {
        /* out of memory */
    }

    return std::string(reinterpret_cast<char*>(key)) == key_given;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
   printf("\n");
   return 0;
}

void register_func(std::vector<std::string> vec_reg,sqlite3* db, char* zErrMsg)
{
    std::vector<std::string>::iterator it = vec_reg.begin();
    it++;

    std::string t1 = "Select * from main where username = '" + *it +"'";
    char sql[t1.size()+1];
    memcpy(sql,t1.c_str(),t1.size()+1);

    struct sqlite3_stmt *selectstmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &selectstmt, NULL);
    if(result == SQLITE_OK)
    {
       if (sqlite3_step(selectstmt) == SQLITE_ROW)
       {
            // RECORD FOUND
            fprintf(stderr, "Username exists in database");
       }
       else
       {
            // NO RECORD FOUND
            std::string str = "(";
            // username
            str = str + '"' + *it + '"' + ", ";
            ++it;
            //password
            std::string password = *it ; 
            std::pair<std::string,std::string> temp = hash_password(password);
            std::string new_password = temp.first;
            std::string salt = temp.second;
            // std::string new_password = "madhav";
            // std::string salt = "madhav";
            str = str + '"' + new_password + '"' + ", " + '"' + salt + '"' + ", ";
            ++it;
            //name 
            str = str + '"' + *it + '"' + ", ";
            ++it;
            //lastseen
            time_t currentTime;
            struct tm *localTime;

            time( &currentTime );                   // Get the current time
            localTime = localtime( &currentTime );  // Convert the current time to the local time

            int Day    = localTime->tm_mday;
            int Month  = localTime->tm_mon + 1;
            int Year   = localTime->tm_year + 1900;
            int Hour   = localTime->tm_hour;
            int Min    = localTime->tm_min;
            str = str + "'" + std::to_string(Day) + " " + std::to_string(Month) + " " + std::to_string(Year) + " " + std::to_string(Hour) + " " + std::to_string(Min) + "'" + ", ";
            //online
            str = str + "0" + ");";

            /* Create SQL statement */
            std::string t2 = "INSERT INTO main (username,password,salt,name,last_seen,online) "  \
                 "VALUES " + str;
            char sql1[t2.size()+1];
            memcpy(sql1,t2.c_str(),t2.size()+1);

            /* Execute SQL statement */
            int rc;
            fprintf(stderr,sql1);
            rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);
            fprintf(stderr, "%s\n",sql1);
            if( rc != SQLITE_OK )
            {
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
            }
            else
            {
              fprintf(stdout, "Records created successfully\n");
            }
       }
    }
    sqlite3_finalize(selectstmt);    
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void my_handler(int s)
{
    int optval = 1;
    setsockopt(portno, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    exit(0);
}

void signal_capture(int portno)
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    while(1)
    {
        sigaction(SIGINT, &sigIntHandler, NULL);
    }
}

std::vector<std::string> break_string(std::string msg)
{
    std::stringstream strstream(msg);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(strstream, segment, ':'))
    {
       seglist.push_back(segment);
    }

    return seglist;
}

void read_thread(char buffer[], int *newsockfd)
{
    char endOfMessage = '#';
    int n;
    std::string buffer_str;

    sqlite3 *db;
    char *zErrMsg = 0;

    /* Open database */
    int rc = sqlite3_open("main.db", &db);
    if(rc)
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    while(1)
    {
        bzero(buffer,256);
        n = read(*newsockfd,buffer,255); // Putting data from socket to buffer
        if (n < 0)
            error("ERROR reading from socket");
        else
            buffer_str = buffer;

        std::cout << buffer_str << std::endl; 
        
        while(buffer_str.back() != endOfMessage){
            bzero(buffer, 256);
            n = read(*newsockfd, buffer, 255);
            if (n < 0)
                error("ERROR reading from socket");
            else
                buffer_str.append(buffer);
        }

        std::cout << buffer_str << std::endl; 

        if(strncmp(buffer_str.c_str(), "/register", strlen("/register")))
        {
            register_func(break_string(buffer_str), db, zErrMsg);
        }

        printf("Client: %s %d\n",buffer_str, n);
    }
}

void write_thread(char buffer[], int *newsockfd)
{
    int n;
    while(1)
    {
        bzero(buffer,256);
        printf("You: ");
        fgets(buffer,255,stdin);
        n = write(*newsockfd,buffer,strlen(buffer)); // Writing to socket
        if (n < 0) error("ERROR writing to socket");
    }
}

int main(int argc, char *argv[])
{
    if(sodium_init() == -1 ) { // Hashing library initialization
        return 1;
    }

    int sockfd, newsockfd; // Socket handles are also file descriptors. Port Number where the communication will happen.
    // File descriptors are used by the operating system to file information about the files.
    socklen_t clilen;
    char read_buffer[256]; // Read the socket and put characters into the buffer.
    char write_buffer[256];
    struct sockaddr_in serv_addr, cli_addr; // in stands for internet family addresses.
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // First socket programming call! AF_INET = Address Family - Internet.(Will probably change for ipv6)
    // STREAM - TCP socket and not a UDP socket. TCP sockets are called Stream sockets. UDP sockets are called datagram sockets.
    // Third argument is a protocol. It might have a variety of protocols for a stream connection as well.
    // 0 here tells us, I don't know you pick the correct one.
    if (sockfd < 0)
       error("ERROR opening socket"); // If error print it.
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Make the memory zero from a particular address and a given number of bytes from that
    portno = atoi(argv[1]); // First argument is the port number.
    // Now we populate the server address structure
    serv_addr.sin_family = AF_INET; // Always keep AF_INET
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Pick up the localhost address
    serv_addr.sin_port = htons(portno); // Computer stores in a different way. So we convert it to how the networks mananges it.
    // Computer stores as little endian. Network takes big endian.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // We are binding the file descriptor and the server address.
        error("ERROR on binding");
    listen(sockfd,5); // We will now only refer to our socket by the descriptor. At max 5 socket connections in waiting.
    // Probably 1 is connected and 5 are waiting. Check this!
    // We have not started listening yet. We are just saying it is a listening type of port.
    // 5 waiting for handshakes. After newsockfd is created we can just use multiple threads.
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr,
                &clilen); // Now we start listening on the port. This method will also set the client address structure.
    // newsockfd is the real file descriptor of the whole connection.
    // sockfd is a descriptor for half socket. It has no information about the client addrss
    // sockets are also files. You read and write into them.
    // Note that clilen is also being sent by reference.
    // Note cli_addr is also being type casted here. Maybe for a different family of addresses which might be added later.

    if (newsockfd < 0)
        error("ERROR on accept");
    // "connect" request form client is being "accpeted" by the accept() function.
    std::thread read_th(read_thread,read_buffer, &newsockfd);
    std::thread write_th(write_thread,write_buffer, &newsockfd);
    std::thread signal_th(signal_capture, portno);

    // read_th.join();
    while(1){;}
    close(newsockfd);
    close(sockfd);
    return 0;
}
