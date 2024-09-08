#pragma once
 
#include "json.h"
#include <string>
#include <map>
#include <memory>
#include <set>
#include <vector>
 
namespace json {
    
struct Inspector {
    char last_action_ = 'z';   //начальное значение - z
    std::vector<bool> map_or_arr_; // контроль нахождения в слоях словарей и/или массивов
    const std::map<char, std::set<char>> valid_operation_ = 
    { {'z', {'k', 'c', 'e'}}, {'a', {'b', 'c', 'e'}}, {'b', {'a', 'd'}}, {'j', {'b', 'c', 'e', 'f', 'j'}}, {'k', {'z'}}, 
      {'c', {'a', 'd'}}, {'d', {'b', 'j', 'c', 'e'}}, {'e', {'j', 'c', 'f'}}, {'f', {'b', 'j', 'c', 'e', 'd'}}};
};

class Builder {
public:
    
    Builder& Key(std::string key);         // a
    
    Builder& Value(Node::Value value);     // b j k : b-map j-arr  k - simple value
    
    Builder& StartDict();                  // c
    
    Builder& EndDict();                    // d
    
    Builder& StartArray();                 // e
    
    Builder& EndArray();                   // f
    
    Node Build();
    
    void AddNode(Node node);
    
private:
    Node node_; 
    std::vector<std::unique_ptr<Node>>nodes_;
    Inspector inspector_;
};
    
}