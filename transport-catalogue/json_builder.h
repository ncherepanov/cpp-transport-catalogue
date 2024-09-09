#pragma once
 
#include "json.h"
#include <string>
#include <memory>
 
namespace json {

class KeyItemContext;
class DictItemContext;
class ArrItemContext;
 
class Builder {
public:

    KeyItemContext Key(std::string key);
    
    DictItemContext StartDict();
    
    ArrItemContext StartArray();
 
    Builder& Value(Node::Value value);
 
    Builder& EndDict();
 
    Builder& EndArray();
 
    Node Build();
    
    void AddNode(Node node);
 
private:
    Node node_;
    std::vector<std::unique_ptr<Node>> nodes_;
 
};
 
class BaseContext {
public:
    BaseContext(Builder& builder);
 
    KeyItemContext Key(std::string key);
    
    DictItemContext StartDict();
    
    ArrItemContext StartArray();
 
    Builder& Value(Node::Value value);
 
    Builder& EndDict();
 
    Builder& EndArray();
 
private:
    Builder& builder_;
};
 
class KeyItemContext : public BaseContext {
public:
    KeyItemContext(Builder& builder);
 
    KeyItemContext Key(std::string key) = delete;
    BaseContext EndDict() = delete;
    BaseContext EndArray() = delete;
    DictItemContext Value(Node::Value value);
};
 
class DictItemContext : public BaseContext {
public:
    DictItemContext(Builder& builder);
 
    DictItemContext StartDict() = delete;
    ArrItemContext StartArray() = delete;
    Builder& EndArray() = delete;
    Builder& Value(Node::Value value) = delete;
};
 
class ArrItemContext : public BaseContext {
public:
    ArrItemContext(Builder& builder);
 
    KeyItemContext Key(std::string key) = delete;
    Builder& EndDict() = delete;
    ArrItemContext Value(Node::Value value);
};

}