#pragma once
 
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
 
namespace json {
 
class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class [[nodiscard]] Node {
public:
   using Value =  std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    Node() = default;
    Node(std::nullptr_t);
    Node(Array array);
    Node(Dict map);
    Node(bool value);
    Node(int value);
    Node(double value);
    Node(std::string value);
    
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const; 
    bool IsBool() const;    
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsString() const;

    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool AsBool() const;
    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const;    

    const Value& GetValue() const { return value_; };
        
private:
    Value value_;
};
 
inline bool operator==(const Node& lhs, const Node& rhs) { 
    return lhs.GetValue() == rhs.GetValue();
}  
inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
} 
    
class Document {
public:
    Document() = default;
    explicit Document(Node root);
    const Node& GetRoot() const;
private:
    Node root_;
};
 
Document Load(std::istream& input);
 
inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}
inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}
    
void Print(const Document& doc, std::ostream& output);
 
}//end namespace json