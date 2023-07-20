#include "json_builder.h"

namespace json {



    DictItemContext Builder::StartDict()
    {
        using namespace std::literals;

        Node node = json::Dict();

        if (root_.IsNull()) {
            root_ = std::move(node);
            nodes_stack_.push_back(&root_);
        }
        else if (!nodes_stack_.empty()) {
            Node* last_node_ptr = nodes_stack_.back();

            if (last_node_ptr->IsArray()) {
                json::Array& arr = std::get<json::Array>(last_node_ptr->GetValue());
                arr.push_back(std::move(node));
                nodes_stack_.push_back(&arr.back());
            }
            else if (last_node_ptr->IsDict()) {
                json::Dict& dic = std::get<json::Dict>(last_node_ptr->GetValue());

                auto rit = dic.rbegin();

                if (dic.empty() || !rit->second.IsNull()) {
                    throw std::logic_error("StartDict: not called Key method before"s);
                }

                rit->second = std::move(node);
                nodes_stack_.push_back(&rit->second);
            }
            else {
                throw std::logic_error("StartDict: invalid method"s);
            }
        }
        else {
            throw std::logic_error("StartDict: called after the Value method"s);
        }


        return BaseContext{ *this }; //DictItemContext (*this);
    }

    Builder& Builder::EndDict()
    {
        using namespace std::literals;

        if (nodes_stack_.empty()) {
            throw std::logic_error("EndDict: no pending calls StartDict"s);
        }
        else {
            Node* last_node_ptr = nodes_stack_.back();

            if (!last_node_ptr->IsDict()) {
                throw std::logic_error("EndDict: no pending calls StartDict"s);
            }
            // удалим завершенный узел
            nodes_stack_.pop_back();
        }


        return *this;
    }

    ArrayItemContext Builder::StartArray()
    {
        using namespace std::literals;

        Node node = json::Array();

        if (root_.IsNull()) {
            root_ = std::move(node);
            nodes_stack_.push_back(&root_);
        }
        else if (!nodes_stack_.empty()) {
            Node* last_node_ptr = nodes_stack_.back();

            if (last_node_ptr->IsArray()) {
                json::Array& arr = std::get<json::Array>(last_node_ptr->GetValue());
                arr.push_back(std::move(node));
                nodes_stack_.push_back(&arr.back());
            }
            else if (last_node_ptr->IsDict()) {
                json::Dict& dic = std::get<json::Dict>(last_node_ptr->GetValue());
                auto rit = dic.rbegin();

                if (dic.empty() || !rit->second.IsNull()) {
                    throw std::logic_error("StartArray: not called Key method before"s);
                }

                rit->second = std::move(node);
                nodes_stack_.push_back(&rit->second);
            }
            else {
                throw std::logic_error("StartArray: invalid method"s);
            }
        }
        else {
            throw std::logic_error("StartArray: called after the Value method"s);
        }

        return BaseContext{ *this }; //ArrayItemContext (*this);
    }

    Builder& Builder::EndArray()
    {
        using namespace std::literals;

        if (nodes_stack_.empty()) {
            throw std::logic_error("EndArray: no pending calls StartArray"s);
        }
        else {
            Node* last_node_ptr = nodes_stack_.back();

            if (!last_node_ptr->IsArray()) {
                throw std::logic_error("EndArray: no pending calls StartArray"s);
            }
            // удалим завершенный узел
            nodes_stack_.pop_back();
        }

        return *this;
    }

    KeyContext Builder::Key(std::string key)
    {
        using namespace std::literals;

        if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Key: call outside dictionary or after method Key"s);
        }

        json::Dict& dic = std::get<json::Dict>(nodes_stack_.back()->GetValue());

        // проверим есть ли ключи с пустым значением
        if (ditails::HasEmptyValues(dic)) {
            throw std::logic_error("Key: called some times method Key");
        }

        dic.insert(std::pair{ key, json::Node() });

        return BaseContext{ *this }; //KeyContext(*this);
    }

    Builder& Builder::Value(Node::Value val)
    {
        using namespace std::literals;

        if (root_.IsNull()) {
            root_ = json::Node(val);
        }
        else if (!nodes_stack_.empty()) {

            Node* last_node_ptr = nodes_stack_.back();

            if (last_node_ptr->IsDict()) {
                json::Dict& dic = std::get<json::Dict>(last_node_ptr->GetValue());

                if (dic.empty()) {
                    throw std::logic_error("Value: not called Key method before"s);
                }

                auto rit = dic.rbegin();
                if (rit->second.IsNull()) {
                    rit->second = val;
                }
                else {
                    throw std::logic_error("Value: method called after another Value method"s);
                }
            }
            else if (last_node_ptr->IsArray()) {
                json::Array& array = std::get<json::Array>(last_node_ptr->GetValue());
                array.emplace_back(std::move(val));
            }
            else {
                throw std::logic_error("Value: Invalid method call"s);
            }
        }
        else {
            throw std::logic_error("Value: Invalid method call"s);
        }

        return *this;
    }

    Node Builder::Build()
    {
        if (root_ == nullptr || !nodes_stack_.empty()) {
            using namespace std::literals;

            throw std::logic_error("Build: Called an unprepared object or there are unfinished nodes"s);
        }

        return root_;
    }

    namespace ditails {

        bool HasEmptyValues(json::Dict dic) {
            return std::any_of(dic.begin(), dic.end(),
                [](const std::pair<std::string, Node>& el) {
                    return el.second.IsNull();
                });
        }

    } // namespace ditails


    DictItemContext BaseContext::StartDict()
    {
        return builder_.StartDict();
    }

    Builder& BaseContext::EndDict()
    {
        return builder_.EndDict();
    }

    ArrayItemContext BaseContext::StartArray()
    {
        return builder_.StartArray();
    }

    Builder& BaseContext::EndArray()
    {
        return builder_.EndArray();
    }

    KeyContext BaseContext::Key(std::string key)
    {
        return builder_.Key(std::move(key));
    }

    Builder& BaseContext::Value(Node::Value val)
    {
        return builder_.Value(std::move(val));
    }

    Node BaseContext::Build()
    {
        return builder_.Build();
    }

    Builder& BaseContext::GetBuilder()
    {
        return builder_;
    }

    DictItemContext::DictItemContext(BaseContext base)
        : BaseContext(base)
    {}

    ArrayItemContext::ArrayItemContext(BaseContext base)
        : BaseContext(base)
    {}

    ArrayItemContext ArrayItemContext::Value(Node::Value val)
    {
        return BaseContext{ BaseContext::Value(std::move(val)) };
    }

    KeyContext::KeyContext(BaseContext base)
        : BaseContext(base)
    {}

    DictItemContext KeyContext::Value(Node::Value val)
    {
        return BaseContext{ BaseContext::Value(std::move(val)) };
    }




} // namespace json