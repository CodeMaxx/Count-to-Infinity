#include "client.h"
#include "utils.cpp"


// int chat::portno = 9399;
// bool chat::loggedin = false;
// int chat::sockfd = 0;

bool isGroup = false;
std::string dest_username;

void chat::print_all_users() {
    if(all.size() > 0) {
        std::cout << "List of ALL users: " << std::endl;
        std::cout << "Username \t Name" << std::endl;
        for (auto x : all) {
            std::cout << x->username << " \t "  << x->name << std::endl;
        }
    }
    else {
        std::cout << termcolor::red<<"There are no users \n";
        std::cout << termcolor::red<<"It's so lonely, call over other people onto this cli rtm";
        std::cout << termcolor::reset << "\n";
    }
}

void chat::print_online_users() {
    if(online.size() > 0) {    
        std::cout << "List of Online users: " << std::endl;
        std::cout << "Username \t Name" << std::endl;
        for (auto x : online) {
            std::cout << x->username << " \t "  << x->name << std::endl;
        }
    }
    else {
        std::cout << termcolor::red<< "No users online now";
        std::cout << termcolor::reset << "\n";
    }
}

void chat::print_friends() {
    if(friends.size() > 0) {
        std::cout << "List of Friends: " << std::endl;
        std::cout << "Username \t Name" << std::endl;
        for (auto x : friends) {
            std::cout << x->username << " \t "  << x->name << std::endl;
        }
    }
    else {
        std::cout << termcolor::red<<"You have no friends" << std::endl;
        std::cout << termcolor::red<<"Oh! so lonely. Send request to random people if you will";
        std::cout << termcolor::reset << "\n";
    }
}

void chat::print_friend_requests() {
    if(friendRequests.size() > 0) {
        std::cout << "List of friend requests: " << std::endl;
        std::cout << "Username \t Name" << std::endl;
        for (auto x : friendRequests) {
            std::cout << x->username << " \t "  << x->name << std::endl;
        }   
    }
    else {
        std::cout << termcolor::red<<"Sorry, no friend requests yet";
        std::cout << termcolor::reset << "\n";
    }
}
void chat::print_all_groups() {
    if(all_groups.size() > 0) {
        std::cout << "List of groups: " << std::endl;
        std::cout << "Group name" << std::endl;
        for (auto x : all_groups) {
            std::cout << x->groupname << std::endl;
        }
    }
    else {
        std::cout << "You are not in any group as of now" << std::endl;
    }
}

void chat::print_all_messages(std::string username) {
    identity *id = username2identity[username];
    for (message msg : id->messages) {
        std::cout << msg.username << ": " << msg.msg << std::endl;
    }
}

void chat::print_group_messages(std::string group_name) {
    group *grp = groupname2group[group_name];
    for (message msg : grp->messages) {
        std::cout << msg.username << ": " << msg.msg << std::endl;
    } 
}

void chat::initialise_database(std::vector<std::string> msg) {
    msg.erase(msg.begin());
    for (int i = 0; i != msg.size();) {
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
            friendRequests.push_back(person);
        }
        all.push_back(person);
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


void chat::update_block(std::vector<std::string> msg) {
    std::string username = msg[1];
    identity *id = username2identity[username];
    id->friendIndicator = 2;
}

void chat::update_online(std::vector<std::string> msg) // Update vector containing which people are online
{
    std::string username = msg[1];
    identity *id = username2identity[username];
    if(id->friendIndicator == 0){ // friends
        id->isOnline = true;
        online.push_back(id);
    }    
}

void chat::update_offline(std::vector<std::string> msg){ 
    std::string username = msg[1];
    std::string last_seen = msg[2];
    identity *id = username2identity[username];
    if(id->friendIndicator == 0){ // friends
        id->isOnline = false;
        id->lastOnline = last_seen;
        auto pos = online.begin();
        for (;pos != online.end(); pos++) {
            if(*pos == id) {
                break;
            }
        }
        online.erase(pos);
    }
}

void chat::update_new(std::vector<std::string> msg)
{
    identity *id = new identity;
    id->username = msg[1];
    id->name = msg[2];
    id->lastOnline = "";
    id->friendIndicator = 3; // no relation
    id->isOnline = false;
    all.push_back(id);
    username2identity[msg[1]] = id;
}

void chat::update_group(std::vector<std::string> msg){
    std::string group_name = msg[1];
    std::vector<std::string> grp_users;
    for(int i = 2; i < msg.size(); i++){
        grp_users.push_back(msg[i]);
    }
    group new_group(group_name, grp_users);
    groupname2group[group_name] = &new_group;

}

void chat::remove_from_group(std::vector<std::string> msg){

}
void chat::error(const char *msg)
{
    perror(msg);
    exit(0);
}

void remove_from_list(std::vector<identity*> &list, identity* id){
   auto pos = list.begin();
    for (;pos != list.end(); pos++) {
        if(*pos == id) {
            break;
        }
    }
    list.erase(pos);
}

void chat::update_friend(std::vector<std::string> msg){
    std::string username = msg[1];
    identity* id_fr = username2identity[username];
    id_fr->friendIndicator = 0;
    //id->friendIndicator = 0;
    friends.push_back(id_fr);
    if(id_fr->isOnline){
        online.push_back(id_fr);
    }

}

void chat::updateFriendRequests(std::vector<std::string> msg){
    std::string username = msg[1];
    identity *id1 = username2identity[username];
    id1->friendIndicator = 1;
    friendRequests.push_back(id1);
}

void chat::removeFriendRequest(std::vector<std::string> msg) {
    std::string username = msg[1];
    identity *id1 = username2identity[username];
    remove_from_list(friendRequests, id1);
}


void chat::blockedYou(std::vector<std::string> msg){
    std::string username = msg[1];
    identity *id = username2identity[username];
    remove_from_list(all, id);
    remove_from_list(online, id);
    remove_from_list(friends, id);
    remove_from_list(friendRequests, id);
    username2identity.erase(username);
    delete id;
}

void chat::update_groups(std::vector<std::string> msg) {

    if(msg.size() > 1) {
        group *grp;
        grp = NULL;
        int start = 1;
        int i;
        for (i = 1; i != msg.size(); i++) {
            if(msg[i] == "group") {
                if(grp != NULL) {
                    groupname2group[grp->groupname] = grp;
                    all_groups.push_back(grp);
                }
                start = i;
                grp = new group();
            }
            else if(start + 1 == i){
                grp->groupname = msg[i];
            }
            else if(start + 1 < i) {
                grp->users.push_back(msg[i]);
            }
        }
        if(i != 1) {
            if(grp != NULL) {
                groupname2group[grp->groupname] = grp;
                all_groups.push_back(grp);
            }
        }
    }
}

void chat::all_group_messages(std::vector<std::string> msg){ 
    std::string group_name = msg[1];
    group *grp = groupname2group[group_name];
    grp->updatedMessages = true;
    for(int i = 2; i < msg.size();){
        std::string group_name = msg[i];
        i++;
        std::string message_recv = msg[i];
        i++;
        message curr_msg;
        curr_msg.msg = message_recv;
        curr_msg.username = group_name;
        (grp->messages).push_back(curr_msg);
    }
}

void chat::all_messages(std::vector<std::string> msg){ 
    std::string curr_username = msg[1];
    identity* id = username2identity[curr_username];
    id->updatedMessages = true;
    for(int i = 2; i < msg.size();){
        std::string from_username = msg[i];
        i++;
        std::string message_recv = msg[i];
        i++;
        message curr_msg;     
        curr_msg.msg = message_recv;
        curr_msg.username = from_username;
        (id->messages).push_back(curr_msg);
    }
}

void chat::add_message(std::vector<std::string> msg){
    std::string username = msg[1];
    identity* id = username2identity[username];
    if(id->updatedMessages) {    
        std::string new_msg = msg[2];
        message new_message;
        new_message.msg = new_msg;
        new_message.username = username;
        (id->messages).push_back(new_message);
    }
}

void chat::add_group_message(std::vector<std::string> msg) {
    std::string group_name = msg[1];
    group *grp = groupname2group[group_name];
    if(grp->updatedMessages) {
        std::string username = msg[2];
        std::string messag = msg[3];
        message new_message;
        new_message.msg = messag;
        new_message.username = username;
        (grp->messages).push_back(new_message);
    }
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
        printf("%s\n", buffer_str.c_str()); // TODO in Release
        auto messageVector = string2vector(buffer_str);
        if(messageVector[0] == "login") {
            loggedin = true;
            std::cout <<termcolor::green<< "Logged in as " << messageVector[1]; 
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "wrong") {
            std::cout << termcolor::red<< "Wrong username/password";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "logout") {
            std::cout <<termcolor::green<<"You have logged out";
            loggedin = false;
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "registersuccess") {
            std::cout <<termcolor::green<<"You have registered successfully";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "users") {
            initialise_database(messageVector);
        }
        else if(messageVector[0] == "online") {
            std::cout << termcolor::green<< messageVector[1] << " came online now" << std::endl << '\a'; 
            update_online(messageVector);
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "offline") {
            std::cout << termcolor::red<< messageVector[1] << " went offline" << std::endl << '\a';
            update_offline(messageVector);
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "message") {
            if(dest_username == messageVector[1])
                std::cout << termcolor::blue<<messageVector[1] << ": " << messageVector[2] << '\a';
            else
                std::cout <<termcolor::green<< "You have a new message from " + messageVector[1] << '\a';
            add_message(messageVector);
            std::cout << termcolor::reset << "\n";

        }
        else if(messageVector[0] == "all_messages") {
            all_messages(messageVector);
            print_all_messages(messageVector[1]);
        }
        else if(messageVector[0] == "group_messages") {
            all_group_messages(messageVector);
            print_group_messages(messageVector[1]);
        }
        else if(messageVector[0] == "messagegroup") {
            add_group_message(messageVector);
        }
        else if(messageVector[0] == "updategroup") {
            update_group(messageVector);
        }
        else if(messageVector[0] == "sendreq"){
            std:: cout <<termcolor::red<< "You need to send a friend request to " + messageVector[1] + " first";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "nfound"){
            std:: cout << termcolor::red<< "This user does not exist. Please enter a valid username to chat.";
            std::cout << termcolor::reset << "\n";
        } 
        else if(messageVector[0] == "acreq") {
            std:: cout << termcolor::red<< "Please accept the friend request from " + messageVector[1] + " to start chatting";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "notacreq") {
            std:: cout << termcolor::red<< messageVector[1] + " has not yet accepted your friend request. You cannot chat.";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "ublock") {
            std::cout << termcolor::red<< "You have blocked " + messageVector[1] + ". Type /unblock " + messageVector[1] + " to unblock and start chatting.";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "blocked") {
            std::cout << termcolor::red<< messageVector[1] + " has been blocked. You will not be able to reach him/her anymore. Use /unblock to unblock";
            std::cout << termcolor::reset << "\n";
            update_block(messageVector);
        }
        else if(messageVector[0] == "unblocked") {
            std::cout << termcolor::green<<"Unblocked " + messageVector[1];
            std::cout << termcolor::reset << "\n";
            update_friend(messageVector);
        }
        else if(messageVector[0] == "notblocked") {
            std::cout << termcolor::green<< messageVector[1] + " was never blocked.";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "sentreq") {
            std::cout << termcolor::green<<"Sent a friend request to " + messageVector[1];
            std::cout << termcolor::reset << "\n";
            
        }
        else if(messageVector[0] == "recvreq") {
            std::cout << termcolor::green<< "You received a friend request from " + messageVector[1];
            std::cout << termcolor::reset << "\n";
            updateFriendRequests(messageVector);
        }
        else if(messageVector[0] == "accepted") {
            std::cout << termcolor::green<< "You are now friends with " + messageVector[1];
            std::cout << termcolor::reset << "\n";
            update_friend(messageVector);
            removeFriendRequest(messageVector);
            update_online(messageVector);
        }
        else if(messageVector[0] == "acceptedyour") {
            std::cout << termcolor::green<< messageVector[1] + " accepted your friend request";
            std::cout << termcolor::reset << "\n";
            update_friend(messageVector);
        }
        else if(messageVector[0] == "rejected") {
            std::cout << termcolor::red<< "You have rejected the friend request from " + messageVector[1];
            std::cout << termcolor::reset << "\n";
            removeFriendRequest(messageVector);
        }
        else if(messageVector[0] == "rejectedyour") {
            std::cout << termcolor::red<< messageVector[1] + " rejected your friend request";
            std::cout << termcolor::reset << "\n";
        }
        else if(messageVector[0] == "groups") {
            update_groups(messageVector);
        }
        else if(messageVector[0] == "group") {
            // update_group(messageVector);
        }
        else if(messageVector[0] == "newregister") {
            update_new(messageVector);
        }
        else if(messageVector[0] == "regexists") {
            std::cout << termcolor::red<< "Username already exists. Please register with a different username.";
            std::cout << termcolor::reset << "\n";
        }
    }
}

void chat::print_help() {
    std::cout << "\n\n";
    std::cout << termcolor::yellow<<  "-----> General <----- \n"; 
    std::cout << termcolor::yellow<<"/help - To get help\n";
    std::cout << termcolor::yellow<<"/register - To start registration process\n";
    std::cout << termcolor::yellow<<"/login [username] [password]\n";
    std::cout << termcolor::yellow<<"/logout - To logout and quit\n\n";
    

    std::cout << termcolor::yellow<<"-----> Chat (one - one) <----- \n"; 
    std::cout << termcolor::yellow<<"/chat [Friend's Username] - To chat with a friend\n";
    std::cout << termcolor::yellow<<"/showall - Lists all registered users\n";
    std::cout << termcolor::yellow<<"/showOnline - Lists all online users\n";
    std::cout << termcolor::yellow<<"/showFriends - Lists your friends\n";
    std::cout << termcolor::yellow<<"/showFR (or) /showFriendRequests0 - Lists your friends\n\n";

    std::cout << termcolor::yellow<<"-----> Groups <----- \n"; 
    std::cout << termcolor::yellow<<"/showgroups - Lists all groups you are in \n";
    std::cout << termcolor::yellow<<"/creategroup [username(s)] - Creates a group containing [username(s)] \n";
    std::cout << termcolor::yellow<<"/groupchat - Lists all online users \n";
    std::cout << termcolor::yellow<<"/leavegroup - Leave the group \n\n";

    std::cout << termcolor::yellow<<"-----> Friend Requests / Blocking <----- \n"; 
    std::cout << termcolor::yellow<<"/friend [username] - Send a friend request to [username] \n";
    std::cout << termcolor::yellow<<"/accept [username] - Accepts the friend request from [username] \n";
    std::cout << termcolor::yellow<<"/reject [username] - Reject friend request from [username] \n";
    std::cout << termcolor::yellow<<"/block [username] - Blocks [username] \n";
    std::cout << termcolor::yellow<<"/unblock [username] - Unblocks [username] \n\n";

    std::cout << termcolor::yellow<<"-----> Misc <----- \n"; 
    std::cout << termcolor::yellow<<"/sendfile - To send file to friend\n";
    std::cout << termcolor::yellow<<"/setStatus - Set a status for yourself \n\n";
    std::cout << termcolor::reset << "\n";
}

bool check_valid_username(std::string username){
    int len = username.length();
    if(len < 6 || len > 30){
        std::cout << termcolor::red<<"Invalid username. Username should contain at least 6 and not more than 30 characters";
        std::cout << termcolor::reset<<"\n";
        return false;
    }
    return true;
}

bool check_valid_password(std::string password){
    int len = password.length();
    if(len < 6){
        std::cout << termcolor::red<<"Password should contain at least 6 characters";
        std::cout << termcolor::reset<<"\n";
        return false;
    }
    if(password.find('#') != std::string::npos){
        std::cout << termcolor::red<<"Password should not contain '#'";
        std::cout << termcolor::reset<<"\n";
        return false;
    }
    return true;
}

bool check_valid_name(std::string name){
    if(name.empty()){
        std::cout << termcolor::red<<"Name should contain at least one character.";
        std::cout << termcolor::reset<<"\n";
        return false;
    }
    return true;
}


void chat::write_thread()
{
    int n;
    char buffer[256];

    isGroup = false;

    while(1)
    {
        bzero(buffer,256);
        std::string endOfMessage = "#";
        std::cout << std::endl;
        std::cout << termcolor::on_yellow << "You: ";
        std::cout << termcolor::reset << "";
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
                    std::cout << termcolor::on_blue <<"Username: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, username);
                    bool username_valid = check_valid_username(username);
                    while(!username_valid){
                        std::cout << termcolor::on_blue <<"Username: ";
                        std::cout << termcolor::reset << "";
                        getline(std::cin, username);
                        username_valid = check_valid_username(username);
                    }
                    std::cout << termcolor::on_blue<<"Name: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, name);
                    bool name_valid = check_valid_name(name);
                    while(!name_valid){
                        std::cout << termcolor::on_blue << "Name: ";
                        std::cout << termcolor::reset << "";
                        getline(std::cin, name);
                        name_valid = check_valid_name(name);
                    }
                    std::cout << termcolor::on_blue<<"Password: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, password);
                    bool password_valid = check_valid_password(password);
                    while(!password_valid){
                        std::cout << termcolor::on_blue <<"Password: ";
                        std::cout << termcolor::reset << "";
                        getline(std::cin, password);
                        password_valid = check_valid_password(password);
                    }
                    message.push_back(username);
                    message.push_back(name);
                    message.push_back(password);

                    write_helper(vector2string(message));
                }
                else if(command.substr(0, strlen("/login")).compare("/login") == 0){
                    std::string username, password;
                    std::vector<std::string> msg;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, username);
                    std::cout << termcolor::on_blue << "Password: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, password);
                    msg.push_back("login");
                    msg.push_back(username);
                    msg.push_back(password);

                    write_helper(vector2string(msg));
                }
                else
                    std::cout << termcolor::red << "Please login to enjoy Count To Infinity services!";
                    std::cout << termcolor::reset << "";
            }
            else {
                if(command.substr(0, strlen("/chat")).compare("/chat") == 0){
                    isGroup = false;
                    std::cout << termcolor::on_blue << "";"Friend's username: ";
                    std::cout << termcolor::reset << "";
                    getline(std::cin, dest_username);
                    write_helper(vector2string(std::vector<std::string>({"getmessages", dest_username})));
                    // check if dest_username is valid?
    
                }
                else if(command.substr(0, strlen("/showall")).compare("/showall") == 0){
                    print_all_users();
                }
                else if(command.substr(0, strlen("/showOnline")).compare("/showOnline") == 0){
                    print_online_users();
                }
                else if(command.substr(0, strlen("/showFriendRequests")).compare("/showFriendRequests") == 0 || command.substr(0, strlen("/showFR")).compare("/showFR") == 0) {
                    print_friend_requests();
                }
                else if(command.substr(0, strlen("/showFriends")).compare("/showFriends") == 0) {
                    print_friends();
                }
                else if(command.substr(0, strlen("/logout")).compare("/logout") == 0){
                    write_helper(vector2string(std::vector<std::string>({"logout"})));
                }
                else if(command.substr(0, strlen("/sendfile")).compare("/sendfile") == 0){
                    
                }
                else if(command.substr(0, strlen("/friend")).compare("/friend") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"friend", dest})));
                }
                else if(command.substr(0, strlen("/accept")).compare("/accept") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"accept", dest})));
                }
                else if(command.substr(0, strlen("/reject")).compare("/reject") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"reject", dest})));
                }
                else if(command.substr(0, strlen("/block")).compare("/block") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"block", dest})));
                }
                else if(command.substr(0, strlen("/unblock")).compare("/unblock") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Username: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    write_helper(vector2string(std::vector<std::string>({"unblock", dest})));
                } 
                else if(command.substr(0, strlen("/creategroup")).compare("/creategroup") == 0) {
                    std::string temp;
                    std::vector<std::string> messageVector;
                    messageVector.push_back("creategroup");
                    std::cout << termcolor::on_blue<<"Group name: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> temp;
                    messageVector.push_back(temp);
                    std::cout << termcolor::on_blue << "Usernames: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> temp;

                    while(temp != "\\") {
                        messageVector.push_back(temp);
                        std::cin >> temp;
                    }

                    write_helper(vector2string(messageVector));
                }
                else if(command.substr(0, strlen("/groupchat")).compare("/groupchat") == 0) {
                    std::string dest;
                    std::cout << termcolor::on_blue<<"Group name: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> dest;
                    dest_username = dest;
                    isGroup = true;
                    write_helper(vector2string(std::vector<std::string>({"getgroupmessages", dest})));
                }
                else if(command.substr(0, strlen("/showgroups")).compare("/showgroups") == 0) {
                    print_all_groups();
                }
                else if(command.substr(0, strlen("/leavegroup")).compare("/leavegroup") == 0) {
                    std::string group;
                    std::cout << termcolor::on_blue<<"Group name: ";
                    std::cout << termcolor::reset << "";
                    std::cin >> group;
                    write_helper(vector2string(std::vector<std::string>({"leavegroup", group})));
                }

            }

            if(command.substr(0, strlen("/exit")).compare("/exit") == 0)
            {
                std::cout << termcolor:: green << "Exiting..." ;
                std::cout << termcolor::reset << "\n";
                exit(0);
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
                std::cout <<termcolor::red <<"Please select a person to chat. More details on /help page";
                std::cout << termcolor::reset << "\n";
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