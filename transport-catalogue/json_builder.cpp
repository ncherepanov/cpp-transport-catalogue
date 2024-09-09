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
    
KeyItemContext Builder::Key(std::string key) { 
    if (nodes_.empty())
        throw std::logic_error("Error creating key");
    auto key_ptr = std::make_unique<Node>(key);
    if (nodes_.back()->IsDict()) {
        nodes_.emplace_back(std::move(key_ptr));
    }
    return KeyItemContext(*this);
}
    
DictItemContext Builder::StartDict() {
    nodes_.emplace_back(std::move(std::make_unique<Node>(Dict())));
    return DictItemContext(*this);
}
    
Builder& Builder::EndDict() {
    if (nodes_.empty())
        throw std::logic_error("Error - closing without opening");
    Node node = *nodes_.back();
    if (!node.IsDict())
        throw std::logic_error("Error. Not dictionary");
    nodes_.pop_back();
    AddNode(node);
    return *this;
}
    
ArrItemContext Builder::StartArray() {
    nodes_.emplace_back(std::move(std::make_unique<Node>(Array())));
    return ArrItemContext(*this);
}
    
Builder& Builder::EndArray() {
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
    if (node_.IsNull())
        throw std::logic_error("Error - creating empty JSON");
    if (!nodes_.empty())
        throw std::logic_error("Error - nothing to create");    
    return node_;
}

Builder& Builder::Value(Node::Value value) {
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
    else node = Node();    
    AddNode(node);
    return *this;
}

BaseContext::BaseContext(Builder& builder) 
: builder_(builder) { }
KeyItemContext BaseContext::Key(std::string key) {
    return builder_.Key(key);
}
Builder& BaseContext::Value(Node::Value value) {
    return builder_.Value(value);
}
DictItemContext BaseContext::StartDict() {
    return DictItemContext(builder_.StartDict());
}
Builder& BaseContext::EndDict() {
    return builder_.EndDict();
}
ArrItemContext BaseContext::StartArray() {
    return ArrItemContext(builder_.StartArray());
}
Builder& BaseContext::EndArray() {
    return builder_.EndArray();
}
KeyItemContext::KeyItemContext(Builder& builder) 
: BaseContext(builder) { }
DictItemContext  KeyItemContext::Value(Node::Value value) {
    return BaseContext::Value(std::move(value));
}
DictItemContext::DictItemContext(Builder& builder) 
: BaseContext(builder) { }
 
ArrItemContext::ArrItemContext(Builder& builder) 
: BaseContext(builder) { }
 
ArrItemContext ArrItemContext::Value (Node::Value value) {
    return BaseContext::Value(move(value)); 
}
    
}