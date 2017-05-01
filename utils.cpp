#include <string>
#include <vector>
#include <sys/termios.h>
#include <sys/ioctl.h>

char escape_char = '\\';

void set_echoctl(const int fd, const int enable)
{
    struct termios tc; 
    tcgetattr(fd, &tc);
    tc.c_lflag &= ~ECHOCTL;
    if (enable)
    {   
        tc.c_lflag |= ECHOCTL;
    }   
    tcsetattr(fd, TCSANOW, &tc);
}

void log(const char *const msg)
{
        // Go to line start
        write(1, "\r", 1);
        // Erases from the current cursor position to the end of the current line
        write(1, "\033[K", strlen("\033[K"));

        fprintf(stdout, "%s\n", msg);

        // Move cursor one line up
        write(1, "\033[1A", strlen("\033[1A"));
        // Disable echo control characters
        set_echoctl(1, 0);
        // Ask to reprint input buffer
        termios tc;
        tcgetattr(1, &tc);
        ioctl(1, TIOCSTI, &tc.c_cc[VREPRINT]);
        // Enable echo control characters back
        set_echoctl(1, 1);
}



void print_string_as_int(std::string s){
    for(auto x : s) {
        printf("%d ", int(x));
    }
    printf("\n");
}

void print_string_as_int(char *s, int len){
    for(int i = 0; i != len; i++) {
        printf("%d ", int(s[i]));
    }
    printf("\n");
}

std::string escape_special_chars(std::string msg) {
    for(int i = 0; i != msg.size(); i++) {
        if(msg[i] == ':' or msg[i] == '#' or msg[i] == '~' or msg[i] == '\\') {
            msg.insert(i, "\\");
            i++;
        }
    }
    return msg;
}


std::string vector2string(std::vector<std::string> v) { // converts vector of strings to a string that 
    std::string ret = "/";                              // fits our protocol
    for (std::string str : v) {
        ret += escape_special_chars(str) + ':';
    }
    ret.pop_back();
    ret.push_back('#');
    return ret;
}

std::vector<std::string> string2vector(std::string msg) {   // converts string of our protocol to a vector of strings
    if(msg[0] == '/') {                                     // for easy access 
        msg.erase(0, 1);
    }
    if(msg.back() == '#') {
        msg.erase(msg.size() - 1, 1);
    }

    for(int i = 0; i != msg.size(); i++) {
        if(msg[i] == '\\') {
            if(msg[i + 1] == '#' or msg[i + 1] == '~' or msg[i + 1] == '\\') {
                msg.erase(i, 1);
                i++;
            }
        }
    }

    std::stringstream strstream(msg);
    std::string segment;
    std::vector<std::string> seglist;
    std::string buffer;
    while(std::getline(strstream, segment, ':'))
    {
        buffer += segment;
        if(segment.back() == escape_char){
            buffer.erase(buffer.size() - 1, 1);
            buffer.push_back(':');
        }
        else {
            seglist.push_back(buffer);
            buffer = "";
        }
    }

    return seglist;
}