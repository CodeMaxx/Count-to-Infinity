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
#include <functional> // for std::bind

class chat
{
    struct identity
    {
        std::string username; // Username
        std::string name; // Name
        std::string status;

        identity()
        {
            username = name = status = "";
        }
    };

    struct history
    {
        struct message // Stores a messages and corresponding reply
        {
            std::string msg;
            bool mine; // = 1 if message is mine else 0
        };

        struct frnd // History of one friend
        {
            identity id;
            std::vector<message> messages;
        };

        std::unordered_map<std::string, frnd> user_to_frnd; // Convert username to index for faster access of history
    };

public:
    std::vector<identity> online; // Vector containing username of online friends
    std::vector<identity> all; // Vector containing list of all friends
    std::thread read_th;
    std::thread write_th;
    history hist; // Contains history of current session
    identity id; // Identity of the client

    int portno; // Port Number of the server
    bool loggedin; // Is client logged in?
    int sockfd; //  The socket over which connection is running

    const char escape_char = '\\'; // Escape character for escaping separators in client messages

    void initialise_online(std::vector<std::string> msg); // Initially add all people who are online
    void update_online(std::vector<std::string> msg); // Add a person if he comes online
    void initialise_all(std::vector<std::string> msg); // Add all people part of the network
    void update_all(std::vector<std::string> msg); // Add if a new person joins the network

    void print_all_users();
    void print_online_users();

    void connect_to_server(char* server_name, int port_num);
    void start_chat();

    void write_thread();

    void write_helper(char buffer[]);
    void write_helper(std::string str_buffer);

    void read_thread();

    void free_port(int s);
    void signal_capture();

    // Helper functions
    void error(const char *msg);
    void print_help();
};