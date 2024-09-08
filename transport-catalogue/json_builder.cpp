#include "json_builder.h"
 
namespace json {
 
void Builder::AddNode(Node node) {
    if (!nodes_.empty()) {
        if (nodes_.back() -> IsString()) {
            std::string str = nodes_.back() -> AsString();
            nodes_.pop_back();
            if (nodes_.back()->IsDict()) {
                Dict dict = nodes_.back()->AsDict();
                dict.emplace(std::move(str), node);
                nodes_.pop_back();
                auto dict_ptr = std::make_unique<Node>(dict);
                nodes_.emplace_back(std::move(dict_ptr));
            }            
            return;
        }
        else if (nodes_.back()->IsArray()) {  
            Array arr = nodes_.back()->AsArray();
            arr.emplace_back(node);
            nodes_.pop_back();
            auto arr_ptr = std::make_unique<Node>(arr);
            nodes_.emplace_back(std::move(arr_ptr));
            return;
        }
        else if (!nodes_.back()->IsString() && !nodes_.back()->IsArray()) {
            throw std::logic_error("Error creating node");

        }        
    }
    else {
        if (!node_.IsNull())
            throw std::logic_error("Error - node is not empty");
        node_ = node;
        return;
    }
}
    
Builder& Builder::Key(std::string key) {
    const char action = 'a';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action) || !inspector_.map_or_arr_.back())
        throw std::logic_error("Error key");
    inspector_.last_action_ = action;

    if (nodes_.empty())
        throw std::logic_error("Error creating key");
    auto key_ptr = std::make_unique<Node>(key);
    if (nodes_.back()->IsDict()) {
        nodes_.emplace_back(std::move(key_ptr));
    }
    return *this;
}
    
Builder& Builder::Value(Node::Value value) {
    const char action = inspector_.map_or_arr_.empty() ? 'k' : inspector_.map_or_arr_.back() ? 'b' : 'j';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action))
        throw std::logic_error("Error value");
    inspector_.last_action_ = action;
    
    Node node;
    if (std::holds_alternative<bool>(value)) 
        node = std::get<bool>(value);
    else if (std::holds_alternative<int>(value)) 
        node = std::get<int>(value);
    else if (std::holds_alternative<double>(value)) 
        node = std::get<double>(value);
    else if (std::holds_alternative<std::string>(value)) 
        node = std::move(std::get<std::string>(value));    
    else if (std::holds_alternative<Array>(value)) 
        node = std::move(std::get<Array>(value));
    else if (std::holds_alternative<Dict>(value)) 
        node = std::move(std::get<Dict>(value));
    AddNode(node);
    return *this;
}
    
Builder& Builder::StartDict() {
    const char action = 'c';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action))
        throw std::logic_error("Error start dict");
    inspector_.map_or_arr_.emplace_back(true);
    inspector_.last_action_ = action;
    
    nodes_.emplace_back(std::move(std::make_unique<Node>(Dict())));
    return *this;    
}
    
Builder& Builder::EndDict() {
    const char action = 'd';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action) || !inspector_.map_or_arr_.back())
        throw std::logic_error("Error end dict");
    inspector_.map_or_arr_.pop_back();
    inspector_.last_action_ = action;
    
    if (nodes_.empty())
        throw std::logic_error("Error - closing without opening");
    Node node = *nodes_.back();
    if (!node.IsDict())
        throw std::logic_error("Error. Not dictionary");
    nodes_.pop_back();
    AddNode(node);
    return *this;
}
    
Builder& Builder::StartArray() {
    const char action = 'e';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action))
        throw std::logic_error("Error start arr");
    inspector_.map_or_arr_.emplace_back(false);
    inspector_.last_action_ = action;
    
    nodes_.emplace_back(std::move(std::make_unique<Node>(Array())));
    return *this;
}
    
Builder& Builder::EndArray() {
    const char action = 'f';
    if (!inspector_.valid_operation_.at(inspector_.last_action_).count(action) || inspector_.map_or_arr_.back())
        throw std::logic_error("Error end arr");
    inspector_.map_or_arr_.pop_back();
    inspector_.last_action_ = action;

    if (nodes_.empty())
        throw std::logic_error("Error - closing without opening");
    Node node = *nodes_.back();
    if (!node.IsArray())
        throw std::logic_error("Error. Not array");
    nodes_.pop_back();
    AddNode(node);
    return *this;    
}
    
Node Builder::Build() {
    if (!inspector_.map_or_arr_.empty() || inspector_.last_action_ == 'a'|| inspector_.last_action_ == 'b'
        || inspector_.last_action_ == 'j'|| inspector_.last_action_ == 'c'|| inspector_.last_action_ == 'e')
        throw std::logic_error("Error - build");
    if (node_.IsNull())
        throw std::logic_error("Error - creating empty JSON");
    if (!nodes_.empty())
        throw std::logic_error("Error - nothing to create");
    inspector_.last_action_ = 'z';
    return node_;
}
    
}