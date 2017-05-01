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