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
#include <unordered_map>

#include "lockless_queue.cpp"
#include "utils.cpp"
#include "ldap.h"

#define KEY_LEN crypto_box_SEEDBYTES

int portno;

std::unordered_map<std::string, int> online;
std::unordered_map<int, std::string> rev_online;

lockless_queue<std::vector<std::string>> control_thread_queue;

bool ldap_login(std::string cn, std::string pass)
{
    LDAP *ld;
    int  result;
    int  auth_method    = LDAP_AUTH_SIMPLE;
    int desired_version = LDAP_VERSION3;
    char *ldap_host     = "cs252lab.cse.iitb.ac.in";
    std::string root_dn = "cn=" + cn + ", dc=cs252lab, dc=cse, dc=iitb, dc=ac, dc=in";
    std::string root_pwd = pass;

    if ((ld = ldap_init(ldap_host, LDAP_PORT)) == NULL ) {
        perror( "ldap_init failed" );
        exit( EXIT_FAILURE );
    }

    if (ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version) != LDAP_OPT_SUCCESS)
    {
        ldap_perror(ld, "ldap_set_option failed!");
        exit(EXIT_FAILURE);
    }

    /* search from this point */
    char* base="dc=cs252lab, dc=cse, dc=iitb, dc=ac, dc=in";

    if (ldap_bind_s(ld, root_dn.c_str(), root_pwd.c_str(), auth_method) != LDAP_SUCCESS )
        return false;
    else
        return true;
}

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

    return std::pair<std::string, std::string>(std::string(reinterpret_cast<char*>(key), 16), std::string(reinterpret_cast<char*>(salt), 16));
}

bool check_password(std::string password, std::string key_given, std::string salt_given) {
    const char* PASSWORD = password.c_str();
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char key[KEY_LEN];

    printf("%lu\n", salt_given.size());

    memcpy(salt, salt_given.c_str(), salt_given.size() + 1);


    if (crypto_pwhash (key, sizeof key, PASSWORD, strlen(PASSWORD), salt,
         crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
         crypto_pwhash_ALG_DEFAULT) != 0) {
        /* out of memory */
    }

    return std::string(reinterpret_cast<char*>(key), 16) == key_given;
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

std::string login_func(std::vector<std::string> vec_login,sqlite3* db, char* zErrMsg)
{
    std::string username = vec_login[1];

    if(ldap_login(vec_login[1], vec_login[2]))
    {
        return vector2string(std::vector<std::string>({"login", username, username}));
    }

    std::string t1 = "Select password,salt,name from main where username = '" + username +"'";
//    char sql[t1.size()+1];
//     memcpy(sql,t1.c_str(),t1.size()+1);

    struct sqlite3_stmt *selectstmt;
    int result = sqlite3_prepare_v2(db, t1.c_str(), -1, &selectstmt, NULL);
    if(result == SQLITE_OK)
    {
       if (sqlite3_step(selectstmt) == SQLITE_ROW)
       {
            // RECORD FOUND
            std::string password_table = (char*)sqlite3_column_text(selectstmt, 0);
            std::string salt_table = (char*)sqlite3_column_text(selectstmt, 1);
            std::string name_table = (char*)sqlite3_column_text(selectstmt, 2);
            std::string password_input = vec_login[2];
            bool temp_bool = check_password(password_input, password_table, salt_table);

           if(temp_bool)
           {
               return vector2string(std::vector<std::string>({"login", username, name_table}));
           }
       }
       else
       {
            // NO RECORD FOUND
            fprintf(stderr, "Invalid Username\n");
       }
    }
    sqlite3_finalize(selectstmt);

    return "";
}

void register_func(std::vector<std::string> vec_reg,sqlite3* db, char* zErrMsg)
{
    std::vector<std::string>::iterator it = vec_reg.begin();
    it++;

    std::string temp1 = "Select * from main where username = '" + *it +"'";
    char sql[temp1.size()+1];
    memcpy(sql,temp1.c_str(),temp1.size()+1);

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
            //name 
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
            std::string temp2 = "INSERT INTO main (username,name,password,salt,last_seen,online) "  \
                 "VALUES " + str;
            char sql1[temp2.size()+1];
            memcpy(sql1,temp2.c_str(),temp2.size()+1);

            /* Execute SQL statement */
            int rc;
            //fprintf(stderr,sql1);
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

void set_user_online(std::string username, sqlite3* db, char* zErrMsg) {
    std::string query = "UPDATE main " \
                    "SET online = 1 " \
                    "WHERE username = '" + username + "'";

    int rc;
    rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
    fprintf(stderr, "%s\n", query.c_str());
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Records updated successfully\n");
    }
}

std::vector<std::string> get_online_users(sqlite3* db, char* zErrMsg) {
    std::string query = "SELECT name, username FROM main WHERE online = 1";
    std::vector<std::string> user_vector;
    user_vector.push_back("olusers");
    struct sqlite3_stmt *selectstmt;
    int result = sqlite3_prepare_v2(db, query.c_str(), -1, &selectstmt, NULL);
    if(result == SQLITE_OK) {
        while (sqlite3_step(selectstmt) == SQLITE_ROW) {
            std::string name = (char *) sqlite3_column_text(selectstmt, 0);
            std::string username = (char *) sqlite3_column_text(selectstmt, 1);
            std::cout << "Name : " << name << std::endl;
            std::cout << "Username : " << username << std::endl << std::endl; 
            user_vector.push_back(username);
            user_vector.push_back(name);
        }
    }
    sqlite3_finalize(selectstmt);
    return user_vector;
}   

void read_thread(int newsockfd)
{
    char buffer[256];
    char endOfMessage = '#';
    int n;
    std::string buffer_str;

    while(1)
    {
        bzero(buffer,256);
        n = read(newsockfd,buffer,255); // Putting data from socket to buffer
        if (n < 0)
            error("ERROR reading from socket");
        else
            buffer_str = buffer;

        std::cout << buffer_str << std::endl; 

        int find_pos = 0;
        bool endOfMessageFound = 0;
        do{
            int pos = buffer_str.find(endOfMessage, find_pos);
            if(pos != std::string::npos) {
                if(buffer_str[pos - 1] == '\\') {
                    find_pos = pos + 1;
                }
                else {
                    endOfMessageFound = 1;
                    find_pos = pos + 1;
                }
            }
            else {
                bzero(buffer, 256);
                n = read(newsockfd, buffer, 255);
                if (n < 0)
                    error("ERROR reading from socket");
                else
                    buffer_str.append(buffer);
            }
        } while(!endOfMessageFound);

        std::string message = buffer_str.substr(0, find_pos);
        buffer_str.erase(0, find_pos);


        std::vector<std::string> messageVector = string2vector(message);
        messageVector.push_back(std::to_string(newsockfd));
        for(auto x : messageVector) {
            std::cout << x << std::endl;
        }

        control_thread_queue.produce(messageVector);

    }
}

void write_to_socket(int newsockfd, std::string data) {
    int n;
    char buffer[256];
    while(data.size() > 256)
    {
        strncpy(buffer, data.c_str(), 256);
        std::cout << "Wrting to socket :" << buffer << std::endl;
        n = write(newsockfd,buffer,strlen(buffer)); // Writing to socket
        if (n < 0) error("ERROR writing to socket");
        else data.erase(0, 256);
    }
    if(data.size() > 0) {
        strcpy(buffer, data.c_str());
        std::cout << "Wrting to socket :" << buffer << std::endl;
        n = write(newsockfd,buffer,strlen(buffer)); // Writing to socket
        if (n < 0) error("ERROR writing to socket");
    }

}

void control_thread() {
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


    while(true) {
        sleep(1);
        if(!control_thread_queue.isEmpty()) { 
            auto head = control_thread_queue.consume_all();
            while(head) {
                std::vector<std::string> messageVector = head->data;  
                
                int sockfd = atoi(messageVector.back().c_str());
                messageVector.pop_back();

                if(messageVector[0] == "register") 
                {
                    register_func(messageVector, db, zErrMsg);
                }
                else if(messageVector[0] == "login")
                {
                    std::string ans = login_func(messageVector, db, zErrMsg);
                    if(ans != "") {
                        // send online data and people registered here
                        
                        std::cout << sockfd << std::endl;
                        // online[messageVector[1]] = sockfd;
                        // rev_online[sockfd] = messageVector[1];
                        set_user_online(messageVector[1], db, zErrMsg);
                        write_to_socket(sockfd, ans);

                        std::vector<std::string> userlist = get_online_users(db, zErrMsg);
                        write_to_socket(sockfd,vector2string(userlist));  
                    }
                    else {
                        // tell client wrong pass
                        std::vector<std::string> v({"wrong"});
                        write_to_socket(sockfd, vector2string(v));
                    }
                    fprintf(stderr, "%s\n", ans.c_str());

                }
                else if(messageVector[0] == "message") {

                }

                auto temp = head->next;
                delete head;
                head = head->next;   
            }
        }
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
    // std::thread signal_th(signal_capture, portno);

    std::thread controlthread(control_thread);
    std::vector<std::thread> threads;
    while(1){
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
        threads.push_back(std::thread(read_thread, newsockfd));
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
