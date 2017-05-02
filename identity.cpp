struct message // Stores a messages and corresponding reply
{
    std::string msg;
    std::string username; // username of person who sent the message
};


struct identity
{
    std::string username; // Username
    std::string name; // Name
    std::string lastOnline; // timestamp of when someone logged out
    bool isOnline; // is this person online
    int friendIndicator; // what's the relation with this person?
    std::vector<message> messages;

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

struct group
{
    std::string groupname;
    std::vector<std::string> users;
    std::vector<message> messages;

    group() {

    }

    group(std::string groupname, std::vector<std::string> users) {
        this->groupname = groupname;
        this->users = users;
    }
};