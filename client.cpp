#include "client.h"
#include "utils.cpp"

// int chat::portno = 9399;
// bool chat::loggedin = false;
// int chat::sockfd = 0;

void chat::print_all_users() {
    std::cout << "List of ALL users: " << std::endl;
    std::cout << "Name \t username" << std::endl;
    for (auto x : all) {
        std::cout << x->name << " \t "  << x->username << std::endl;
    }
}

void chat::print_online_users() {
    std::cout << "List of Online users: " << std::endl;
    std::cout << "Name \t username" << std::endl;
    for (auto x : online) {
        std::cout << x->name << " \t "  << x->username << std::endl;
    }
}

void chat::initialise_database(std::vector<std::string> msg) {
    msg.erase(msg.begin());
    for (int i = 0; i != msg.size(); i++) {
        std::string username, name, lastOnline;
        bool isOnline;
        int friendIndicator;
        username = msg[i];
        i++;
        name = msg[i];
        i++;
        friendIndicator = stoi(msg[i]);
        i++;
        if(friendIndicator == 0) {
            lastOnline = msg[i];
            i++;
            isOnline = (bool) stoi(msg[i]);
            i++;
        }
        else {
            lastOnline = "";
            isOnline = false;
        }
        identity *person = new identity(username, name, lastOnline, friendIndicator, isOnline);
        if(isOnline) {
            online.push_back(person);
        }
        if(friendIndicator == 0) {
            friends.push_back(person);
        }
        if(friendIndicator == -1) {
            friends.push_back(person);
        }
        username2identity[username] = person;
    }
}

std::vector<identity*> chat::getOnlineusers() {
    return online;
}
std::vector<identity*> chat::getFriends() {
    return friends;
}
std::vector<identity*> chat::getAllUsers() {
    return all;
}
std::vector<identity*> chat::getFriendRequests() {
    return friendRequests;
}

void chat::error(const char *msg)
{
    perror(msg);
    exit(0);
}

void chat::write_helper(std::string str_buffer){ // Define this
    int n;
    char *buffer = new char[str_buffer.length() + 1];
    strcpy(buffer, str_buffer.c_str());
    //const char* buffer = str_buffer.c_str();
    n = write(sockfd, buffer, strlen(buffer)); // Writing to socket
    if (n < 0) error("ERROR writing to socket");
}

void chat::write_helper(char buffer[])
{
    int n;
    n = write(sockfd,buffer,strlen(buffer)); // Writing to socket
    if (n < 0) error("ERROR writing to socket");
}

void chat::read_thread()
{
    int n;
    char endOfMessage = '#';
    char buffer[256];
    std::string buffer_str;

    while(1)
    {
        bzero(buffer,256);
        n = read(sockfd,buffer,255); // Putting data from socket to buffer
        if (n < 0)
            error("ERROR reading from socket");
        else
            buffer_str = buffer;

        while(buffer_str.back() != endOfMessage){
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0)
                error("ERROR reading from socket");
            else
                buffer_str.append(buffer);
        }
        auto messageVector = string2vector(buffer_str);
        if(messageVector[0] == "login") {
            loggedin = true;
            std::cout << "Logged in as " << messageVector[1] << std::endl; 
        }
        else if(messageVector[0] == "wrong") {
            printf("Wrong username/password\n");
        }
        else if(messageVector[0] == "logout") {
            printf("You have logged out\n" );
            loggedin = false;
        }
        // else if(messageVector[0] == "olusers") {
            // initialise_online(messageVector);
        // }
        else if(messageVector[0] == "users") {
            initialise_database(messageVector);
        }
        else if(messageVector[0] == "online") {
            std::cout << messageVector[1] << " came online now" << std::endl; 
            update_online(messageVector);
        }
        else if(messageVector[0] == "offline") {
            std::cout << messageVector[1] << " went offline" << std::endl; 

        }
        else if(messageVector[0] == "message") {
            std::cout << messageVector[1] << ": " << messageVector[2] << std::endl;
            // update_message();
        }
        else if(messageVector[0] == "sendreq"){
            std:: cout << "You need to send a friend request to " + messageVector[1] + " first" << std::endl;
        }
        else if(messageVector[0] == "nfound"){
            std:: cout << "This user does not exist. Please enter a valid username to chat." << std::endl;
        }
        else if(messageVector[0] == "acreq") {
            std:: cout << "Please accept the friend request from " + messageVector[1] + " to start chatting" << std::endl;
        }
        else if(messageVector[0] == "notacreq") {
            std:: cout << messageVector[1] + " has not yet accepted your friend request. You cannot chat." << std::endl;
        }
        else if(messageVector[0] == "ublock") {
            std::cout << "You have blocked " + messageVector[1] + ". Type /unblock " + messageVector[1] + " to unblock and start chatting.";
            
        }
        else if(messageVector[0] == "blocked") {
            std::cout << messageVector[1] + " has been blocked. You will not be able to reach him/her anymore. Use /unblock to unblock" << std::endl;
            
        }
        else if(messageVector[0] == "unblocked") {
            std::cout << "Unblocked " + messageVector[1] << std::endl;
        }
        else if(messageVector[0] == "notblocked") {
            std::cout << messageVector[1] + " was never blocked.";
        }
        else if(messageVector[0] == "sentreq") {
            std::cout << "Sent a friend request to " + messageVector[1] << std::endl;
        }
        else if(messageVector[0] == "recvreq") {
            std::cout << "You received a friend request from " + messageVector[1] << std::endl;
        }
        else if(messageVector[0] == "accepted") {
            std::cout << "You are now friends with " + messageVector[1] << std::endl;
        }
        else if(messageVector[0] == "acceptedyour") {
            std::cout << messageVector[1] + " accepted your friend request" << std::endl;
        }
    }
}

void chat::print_help() {
    printf("/help - To get help\n");
    printf("/register - To start registration process\n");
    printf("/login [username] [password]\n");
    printf("/chat [Friend's Username] - To chat with a friend\n");
    printf("/showall - Lists all registered users\n");
    printf("/showFriends - Lists your friends\n");
    printf("/showOnline - Lists all online users\n");
    printf("/befriend [username] - Send a friend request to [username] \n");
    printf("/block [username] - Blocks [username] \n");
    printf("/unblock [username] - Unblocks [username] \n");
    printf("/logout - To logout and quit\n");
    printf("/sendfile - To send file to friend\n");
    printf("/setStatus - Set a status for yourself \n");
}

void chat::write_thread()
{
    int n;
    char buffer[256];

    std::string dest_username;

    while(1)
    {
        bzero(buffer,256);
        std::string endOfMessage = "#";
        printf("You: ");
        std::string str_buffer;

        getline (std::cin, str_buffer);

        if(str_buffer[0] == '/')
        {
            std::string command = str_buffer;
            if(command.substr(0, strlen("/help")).compare("/help") == 0)
            {
                print_help();
            }
            if(!loggedin) {
                if(command.substr(0, strlen("/register")).compare("/register") == 0)
                {
                    std::string name, password, username;
                    std::vector<std::string> message;
                    message.push_back("register");
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
                    message.push_back(username);
                    message.push_back(name);
                    message.push_back(password);

                    write_helper(vector2string(message));
                }
                else if(command.substr(0, strlen("/login")).compare("/login") == 0){
                    std::string username, password;
                    std::vector<std::string> msg;
                    printf("Username: ");
                    getline(std::cin, username);
                    printf("Password: ");
                    getline(std::cin, password);
                    msg.push_back("login");
                    msg.push_back(username);
                    msg.push_back(password);

                    write_helper(vector2string(msg));
                }
            }
            else {
                if(command.substr(0, strlen("/chat")).compare("/chat") == 0){
                    printf("Friend's username: ");
                    getline(std::cin, dest_username);
                    // check if dest_username is valid?
    
                }
                else if(command.substr(0, strlen("/showall")).compare("/showall") == 0){
                    print_all_users();
                }
                else if(command.substr(0, strlen("/showOnline")).compare("/showOnline") == 0){
                    print_online_users();
                }
                else if(command.substr(0, strlen("/logout")).compare("/logout") == 0){
                    write_helper(vector2string(std::vector<std::string>({"logout"})));
                }
                else if(command.substr(0, strlen("/sendfile")).compare("/sendfile") == 0){
                    
                }
                else if(command.substr(0, strlen("/friend")).compare("/friend") == 0) {
                    std::string dest;
                    printf("Username: ");
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"friend", dest})));
                }
                else if(command.substr(0, strlen("/accept")).compare("/accept") == 0) {
                    std::string dest;
                    printf("Username: ");
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"accept", dest})));
                }
                else if(command.substr(0, strlen("/block")).compare("/block") == 0) {
                    std::string dest;
                    printf("Username: ");
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"block", dest})));
                }
                else if(command.substr(0, strlen("/unblock")).compare("/unblock") == 0) {
                    std::string dest;
                    printf("Username: ");
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"unblock", dest})));
                }
            }

        }
        else if(loggedin)
        {
            if(dest_username != "") {
                std::vector<std::string> messageVector({"message"});
                messageVector.push_back(dest_username);
                messageVector.push_back(str_buffer);
                write_helper(vector2string(messageVector));
            }
            else {
                std::cout << "Please select a person to chat. More details on /help page" << std::endl;
            }
        }
        else
        {
            print_help();
        }
    }
}

void chat::connect_to_server(char *server_name, int port_num)
{
    loggedin = false;
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = port_num;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(server_name); // System call that does the DNS query. Will return a structure which has the IP address of the server

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
    server side info and for any new connection from a new client a sockfd is created. sockfd is unique for every client */
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}

void chat::start_chat()
{

    std::thread read_th(&chat::read_thread, this);
    std::thread write_th(&chat::write_thread, this);
    // std::thread signal_th(chat::signal_capture);

    while(1){;}
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }

    chat c;

    c.connect_to_server(argv[1], atoi(argv[2]));
    c.start_chat();

    return 0;
}