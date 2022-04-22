#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "HTTPMessage.hpp"
#define CRLF "\r\n"
#define HEADER_SPLIT "\r\n\r\n"

using std::string;
using std::cout;
using std::endl;

HTTPMessage::HTTPMessage(const HTTPMessage& msg)
{
    hostname = msg.hostname;
    raw_text = string(msg.raw_text);
}

HTTPMessage::HTTPMessage(const string& s)
{
    raw_text = string(s);
    parseHeader();
}

string HTTPMessage::host()
{
    return hostname;
}

bool HTTPMessage::isEmpty()
{
    return raw_text.empty();
}

void HTTPMessage::parseHeader()
{
    int split_loc = 0;
    if (!isEmpty())
    {
        split_loc = raw_text.find(HEADER_SPLIT);
        string headers = raw_text.substr(0, split_loc);
        const string pre = "\r\nHost: ";
        const string post = "\r\n";
        size_t pre_loc = headers.find(pre);
        if (pre_loc != string::npos)
        {
            pre_loc += pre.length();
            string pre_cut = headers.substr(pre_loc, headers.length() - pre_loc);
            size_t post_loc = pre_cut.find(post);
            if (post_loc != string::npos)
            {
                string post_cut = pre_cut.substr(0, post_loc);
                hostname = post_cut;
                cout << "Found hostname in packet: " << post_cut << endl;
            }
        }
        
    }
}

std::ostream& operator<< (std::ostream& out, const HTTPMessage& msg)
{
    return out << msg.raw_text;
}

string HTTPMessage::to_string() const
{
    return raw_text;
}
