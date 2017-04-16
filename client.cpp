

/*******************************************/
/////// USE std:: WHEREVER REQUIRED /////////
/*******************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <signal.h> // For caputuring ctrl+c and freeing the port

int portno;
bool loggedin;

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

class chat
{
    struct history
    {
        struct message // Stores a messages and corresponding reply
        {
            std::string msg;
            bool mine; // = 1 if message is mine else 0
        };

        struct frnd // History of one friend
        {
            std::string username;
            std::string name;
            std::vector<message> messages;
        };

        std::unordered_map<std::string, frnd> user_to_frnd; // Conver username to index for faster access of history
    };

public:
    std::vector<std::string> online; // Vector containing username of online friends
    std::vector<std::string> all; // Vector containing list of all friends
    history hist; // Contains history of current session
    std::string username; // Username of client
    std::string name; // Name of client

    void initialise_online(std::string msg)
    {
        std::vector<std::string> v = break_string(msg);
        v.erase(v.begin());
        online.insert(online.end(), v.begin(), v.end());
    }

    void update_online(std::string msg)
    {
        std::vector<std::string> v = break_string(msg);
        online.push_back(v[1]);
    }

    void initialise_all(std::string msg)
    {
        std::vector<std::string> v = break_string(msg);
        v.erase(v.begin());
        all.insert(all.end(), v.begin(), v.end());
    }

    void update_all(std::string msg)
    {
        std::vector<std::string> v = break_string(msg);
        all.push_back(v[1]);
    }
};


void error(const char *msg)
{
    perror(msg);
    exit(0);
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

void write_helper(std::string str_buffer, int* newsockfd){ // Define this
	int n;
	char *buffer = new char[str_buffer.length() + 1];
	strcpy(buffer, str_buffer.c_str());
	//const char* buffer = str_buffer.c_str();
	n = write(*newsockfd, buffer, strlen(buffer)); // Writing to socket
	if (n < 0) error("ERROR writing to socket");
}

void write_helper(char buffer[], int *newsockfd)
{
    int n;
    n = write(*newsockfd,buffer,strlen(buffer)); // Writing to socket
    if (n < 0) error("ERROR writing to socket");
}

void read_thread(char buffer[], int *newsockfd)
{
    int n;
   	char endOfMessage = '#';
    std::string buffer_str;

    while(1)
    {
    	bzero(buffer,256);
    	n = read(*newsockfd,buffer,255); // Putting data from socket to buffer
    	if (n < 0)
    		error("ERROR reading from socket");
    	else
    		buffer_str = buffer;

    	while(buffer_str.back() != endOfMessage){
        	bzero(buffer, 256);
        	n = read(*newsockfd, buffer, 255);
        	if (n < 0)
            	error("ERROR reading from socket");
       		else
        		buffer_str.append(buffer);
    	}
    	buffer_str.pop_back();
    	//buffer[n-1] = '\0';
    	std::cout<<"Client: "<<buffer_str << n << std::endl;
 	}
}

void print_help() {
    printf("/help - To get help\n");
    printf("/register [username] [Name] [Password] - To start registration process\n");
    printf("/login [username] [password]\n");
    printf("/chat [Friend's Username] - To chat with a friend\n");
    printf("/showall - Show all registered users\n");
    printf("/showOnline - Show all online users\n");
    printf("/logout - To logout and quit\n");
    printf("/sendfile - To send file to friend\n");
}

void write_thread(char buffer[], int *newsockfd)
{
    int n;
    while(1)
    {
        bzero(buffer,256);
        std::string endOfMessage = "#";
        std::string dest_username;
        printf("You: ");
        std::string str_buffer;

        //fgets(buffer,255,stdin);
        getline (std::cin, str_buffer);
        if(str_buffer[0] == '/')
        {
        	std::string command = str_buffer;
            //std::string reg = "/register";
            //std::string login = "/login";
            if(command.substr(0, strlen("/help")).compare("/help") == 0)
            {
                print_help();
            }
            else if(command.substr(0, strlen("/register")).compare("/register") == 0)
            {
            	std::string name, password, username, message;
            	message = "/register:";
            	printf("Username: ");
            	getline(std::cin, username);
                printf("Name: ");
                getline(std::cin, name);
                printf("Password: ");
                getline(std::cin, password);

                while(password.find('#') != std::string::npos){
                	printf("'#' not allowed. Enter another password.\n");
                	getline(std::cin, password);
                }
                message.append(username.append(":"));
                message.append(name.append(":"));
                message.append(password.append(endOfMessage));

                write_helper(message, newsockfd); // Ideally we should get a "Username already exists error here"
                                                 // but due to threads it is a problem. Maybe we should make
                                                 // threads variables public and then synchronise somehow.
            }
            else if(command.substr(0, strlen("/login")).compare("/login") == 0){
            	std::string username, password, message;
            	message = "/login:";
            	printf("Username: ");
            	getline(std::cin, username);
            	printf("Password: ");
                getline(std::cin, password);
                message.append(username.append(":"));
                message.append(password.append(endOfMessage));

            	write_helper(message, newsockfd);
            }
            else if(command.substr(0, strlen("/chat")).compare("/chat") == 0){
            	printf("Friend's username: ");
            	getline(std::cin, dest_username);
            	// check if dest_username is valid?

            }
            else if(command.substr(0, strlen("/showall")).compare("/showall") == 0){

            }
            else if(command.substr(0, strlen("/showOnline")).compare("/showOnline") == 0){

            }
            else if(command.substr(0, strlen("/logout")).compare("/logout") == 0){
            	std::string message = "/logout";
            	message.append(endOfMessage);
            }
            else if(command.substr(0, strlen("/sendfile")).compare("/sendfile") == 0){

            }

        }
        else if(loggedin)
        {
        	std::string message = "/message:";
        	message.append(dest_username.append(":"));
        	message.append((str_buffer.append(endOfMessage)));
            write_helper(message, newsockfd);
        }
        else 
        {
            print_help();
        }
    }
}


int main(int argc, char *argv[])
{
    loggedin = false;
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char read_buffer[256];
    char write_buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]); // System call that does the DNS query. Will return a structure which has the IP address of the server

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Zeroing the instance out.
    // Populating server address structure
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length); // Now IP we got from gethostbyname is added to this structure

    serv_addr.sin_port = htons(portno); // Note there is ntohs() also

    // Note connect is not returning any new file descriptor.
    /* Not sure about this comment-> At server we need a listening socket since multiple clients can go there. sockfd keeps the
    server side info and for any new connection from a new client a newsockfd is created. newsockfd is unique for every client */
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    std::thread read_th(read_thread,read_buffer, &sockfd);
    std::thread write_th(write_thread,write_buffer, &sockfd);
    std::thread signal_th(signal_capture, portno);

    //read_th.join();
    while(1){;}
    close(sockfd); // Closing the socket.
    return 0;
}
