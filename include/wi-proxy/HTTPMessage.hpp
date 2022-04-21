#pragma once

#include <map>
#include <string>

class HTTPMessage
{
    std::string prelude;
    std::map<std::string, std::string> headers;
    std::string body;
    
public:
    HTTPMessage(){};
    HTTPMessage(const HTTPMessage& m);
    int parse(const std::string& s);
    void parseBody(const std::string& s);
    int bodyLen();
    std::string host();
    
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
    friend std::ostream& operator<< (std::ostream& out, const HTTPMessage& msg);
    std::string to_string() const;
};
