#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "HTTPMessage.hpp"
#define CRLF "\r\n"
#define HEADER_SPLIT "\r\n\r\n"

using std::string;

namespace msg_helpers
{
    void cleanStr(string& str)
    {
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
//        transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
}

HTTPMessage::HTTPMessage(const HTTPMessage& msg)
{
    prelude = string(msg.prelude);
    headers = std::map<string, string>(msg.headers);
    body = string(msg.body);
    raw_text = string(msg.raw_text);
}

int HTTPMessage::bodyLen()
{
    if (headers.find("Content-Length") == headers.end())
    {
        return -1;
    }
    return atoi(headers["Content-Length"].data());
}

bool HTTPMessage::isChunked()
{
    if (headers.find("Transfer-Encoding") != headers.end()
        && headers["Transfer-Encoding"].find("chunked") != string::npos)
    {
        return true;
    }
    return false;
}

string HTTPMessage::host()
{
    if (headers.find("Host") == headers.end())
    {
        return string();
    }
    return headers["Host"];
}

void HTTPMessage::setRawText(const string& s)
{
    raw_text = string(s);
}

string HTTPMessage::getRawText()
{
    return raw_text;
}

bool HTTPMessage::isEmpty()
{
    return raw_text.empty();
}

/// Parses headers out of the string and returns body content length included in string
/// @param s string input
int HTTPMessage::parse(const std::string& s)
{
    int split_loc = 0;
    if (headers.empty())
    {
        split_loc = s.find(HEADER_SPLIT);
        string headers = s.substr(0, split_loc);
        std::stringstream header_stream(headers);
        string line;
        std::getline(header_stream, line);
        prelude = line;
        msg_helpers::cleanStr(prelude);
        while (std::getline(header_stream, line))
        {
            int colon_loc = line.find(": ");
            string name = line.substr(0, colon_loc);
            int val_start = colon_loc + strlen(CRLF);
            string val = line.substr(val_start, line.length() - val_start);
            msg_helpers::cleanStr(name);
            msg_helpers::cleanStr(val);
            this->headers.insert(std::pair<string, string>(name, val));
        }
    }
    return s.length() - (split_loc + strlen(HEADER_SPLIT));
}

void HTTPMessage::parseBody(const std::string& s)
{
    int loc = s.find(HEADER_SPLIT) + strlen(HEADER_SPLIT);
    if (loc < 0 || s.length() < loc)
    {
        body = "";
        return;
    }
    body = s.substr(loc, s.length() - loc);
}

const std::string& HTTPMessage::getBody()
{
    return body;
}

std::ostream& operator<< (std::ostream& out, const HTTPMessage& msg)
{
    out << msg.to_string();
}

string HTTPMessage::to_string() const
{
    string s;
    s.append(prelude).append(CRLF);
    for (auto item : headers)
    {
        s.append(item.first).append(": ").append(item.second).append(CRLF);
    }
    s.append(CRLF);
    s.append(body);
    return s;
}
