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
        std::string lastOnline; // timestamp of when someone logged out
        bool isOnline; // is this person online
        int friendIndicator; // what's the relation with this person?

        identity()
        {
            username = name = lastOnline = "";
        }
        identity(std::string username, std::string name, std::string lastOnline, int friendIndicator, bool isOnline) {
            this->username = username;
            this->name = name;
            this->lastOnline = lastOnline;
            this->friendIndicator = friendIndicator;
            this->isOnline = isOnline;
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
    // TODO : Make these vectors of identity, vectors of identity so that when one is changed, 
    // it reflects the status all across the database 
    std::vector<identity> online; // Vector containing username of online friends
    std::vector<identity> friends; // Vector containing username of all friends
    std::vector<identity> all; // Vector containing list of all friends
    std::vector<identity> friendRequests; // Vector containing people who sent this person friendRequest
    std::unordered_map<std::string, identity> username2identity; // username to *identity hash map
    
    std::thread read_th;
    std::thread write_th;
    history hist; // Contains history of current session
    identity id; // Identity of the client

    int portno; // Port Number of the server
    bool loggedin; // Is client logged in?
    int sockfd; //  The socket over which connection is running

    const char escape_char = '\\'; // Escape character for escaping separators in client messages

    void initialise_online(std::vector<std::string> msg); // Initially add all people who are online
    void initialise_all(std::vector<std::string> msg); // Add all people part of the network
    void initialise_database(); // Initializes the database once `all` is filled

    std::vector<identity> getOnlineusers(); 
    std::vector<identity> getFriends();
    std::vector<identity> getAllUsers();
    std::vector<identity> getFriendRequests();
    void update_online(std::vector<std::string> msg); // Add a person if he comes online
    void update_offline(std::vector<std::string> msg); // Remove user from online list
    void update_all(std::vector<std::string> msg); // Add if a new person joins the network
    void updateFriendRequests(std::vector<std::string> msg); // Someone sent a friend request to this person, so update friend
    void blockedYou(std::vector<std::string> msg); // someone blocked you, so you just remove him from all the lists/maps

    void print_all_users();
    void print_online_users();

    void connect_to_server(char* server_name, int port_num);
    void start_chat();

    void write_thread();

    void write_helper(char buffer[]);
    void write_helper(std::string str_buffer);

    void read_thread();

    // Helper functions
    void error(const char *msg);
    void print_help();
};