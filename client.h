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
#include "identity.cpp"
class chat
{
    
public:

    // TODO : Make these vectors of identity, vectors of identity so that when one is changed, 
    // it reflects the status all across the database 
    std::vector<identity*> online; // Vector containing username of online friends
    std::vector<identity*> friends; // Vector containing username of all friends
    std::vector<identity*> all; // Vector containing username of all users who have not blocked this person
    std::vector<identity*> friendRequests; // Vector containing people who sent this person friendRequest
    std::unordered_map<std::string, identity*> username2identity; // username to *identity hash map
    std::unordered_map<std::string, std::vector<message> > group_messages; // key: group name
    
    std::thread read_th;
    std::thread write_th;
    identity id; // Identity of the client

    int portno; // Port Number of the server
    bool loggedin; // Is client logged in?
    int sockfd; //  The socket over which connection is running

    const char escape_char = '\\'; // Escape character for escaping separators in client messages

    // void initialise_online(std::vector<std::string> msg); // Initially add all people who are online
    // void initialise_all(std::vector<std::string> msg); // Add all people part of the network
    void initialise_database(std::vector<std::string> msg); // Initializes the database once `all` is filled
    void update_groups(std::vector<std::string> msg); // stores all groups this person belongs to
    std::vector<identity*> getOnlineusers();
    std::vector<identity*> getFriends();
    std::vector<identity*> getAllUsers();
    std::vector<identity*> getFriendRequests();
    void update_online(std::vector<std::string> msg); // Add a person if he comes online

    void update_offline(std::vector<std::string> msg); // Remove a person if he goes offline
    void update_friend(std::vector<std::string> msg); // Make a person friend
    void update_new(std::vector<std::string> msg); // Add if a new person joins the network
    void update_block(std::vector<std::string> msg); // Updates the edge to blocked
    void updateFriendRequests(std::vector<std::string> msg); // Someone sent a friend request to this person, so update friend
    void blockedYou(std::vector<std::string> msg); // someone blocked you, so you just remove him from all the lists/maps


    void all_group_messages(std::vector<std::string> msg);  // _ group name  (username message)
    void all_messages(std::vector<std::string> msg);    // _ username  (username message)
    void add_message(std::vector<std::string> msg); // /message username message 
    //void add_group_message(std::vector<std::string> msg);

    void print_all_users();
    void print_online_users();
    void print_friends(); 
    void print_friend_requests(); 

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