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

        std::unordered_map<std::string, frnd> user_to_frnd; // Convert username to index for faster access of history


    };

public:
    std::vector<std::string> online; // Vector containing username of online friends
    std::vector<std::string> all; // Vector containing list of all friends
    history hist; // Contains history of current session
    std::string username; // Username of client
    std::string name; // Name of client
    static int portno; // Port Number of the server
    static bool loggedin; // Is client logged in?
    static int sockfd; //  The socket over which connection is running

    static const char escape_char = '\\'; // Escape character for escaping separators in client messages

    void initialise_online(std::string msg); // Initially add all people who are online
    void update_online(std::string msg); // Add a person if he comes online
    void initialise_all(std::string msg); // Add all people part of the network
    void update_all(std::string msg); // Add if a new person joins the network

    void connect_to_server(char* server_name, int port_num);
    void start_chat();

    static void write_thread();

    static void write_helper(char buffer[]);
    static void write_helper(std::string str_buffer);

    static void read_thread();

    static void free_port(int s);
    static void signal_capture();

    // Helper functions
    static void error(const char *msg);
    static void print_help();
    static std::vector<std::string> break_string(std::string msg);
};