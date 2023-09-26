#include "json_builder.h"

namespace json {


    Builder::DictItemContext Builder::StartDict()
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

                std::pair<bool, std::string> is_found = ditails::GetKeyWithEmptyValue(dic);
                if (!is_found.first) {
                    throw std::logic_error("StartDict: not called Key method before"s);
                }

                json::Node& curr_elem = dic.at(is_found.second);
                curr_elem = std::move(node);
                nodes_stack_.push_back(&curr_elem);

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

    Builder::ArrayItemContext Builder::StartArray()
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

                std::pair<bool, std::string> is_found = ditails::GetKeyWithEmptyValue(dic);
                if (!is_found.first) {
                    throw std::logic_error("StartArray: not called Key method before"s);
                }

                json::Node& curr_elem = dic.at(is_found.second);
                curr_elem = std::move(node);
                nodes_stack_.push_back(&curr_elem);

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

    Builder::DictValueContext Builder::Key(std::string key)
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

        return BaseContext{ *this }; //DictValueContext(*this);
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

                std::pair<bool, std::string> is_found = ditails::GetKeyWithEmptyValue(dic);
                if (is_found.first) {
                    dic[is_found.second] = val;
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

        bool HasEmptyValues(const json::Dict& dic) {
            return std::any_of(dic.cbegin(), dic.cend(),
                [](const std::pair<std::string, Node>& el) {
                    return el.second.IsNull();
                });
        }

        std::pair<bool, std::string> GetKeyWithEmptyValue(const json::Dict& dict) {
            std::pair<bool, std::string> pair_found{ false, "" };

            for (const auto& el : dict) {
                if (el.second.IsNull()) {
                    pair_found.first = true;
                    pair_found.second = el.first;
                    break;
                }
            }

            return pair_found;
        }


    } // namespace ditails


    Builder::DictItemContext Builder::BaseContext::StartDict()
    {
        return builder_.StartDict();
    }

    Builder& Builder::BaseContext::EndDict()
    {
        return builder_.EndDict();
    }

    Builder::ArrayItemContext Builder::BaseContext::StartArray()
    {
        return builder_.StartArray();
    }

    Builder& Builder::BaseContext::EndArray()
    {
        return builder_.EndArray();
    }

    Builder::DictValueContext Builder::BaseContext::Key(std::string key)
    {
        return builder_.Key(std::move(key));
    }

    Builder& Builder::BaseContext::Value(Node::Value val)
    {
        return builder_.Value(std::move(val));
    }

    Node Builder::BaseContext::Build()
    {
        return builder_.Build();
    }

    Builder& Builder::BaseContext::GetBuilder()
    {
        return builder_;
    }

    Builder::DictItemContext::DictItemContext(BaseContext base)
        : BaseContext(base)
    {}

    Builder::ArrayItemContext::ArrayItemContext(BaseContext base)
        : BaseContext(base)
    {}

    Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value val)
    {
        return BaseContext{ BaseContext::Value(std::move(val)) };
    }

    Builder::DictValueContext::DictValueContext(BaseContext base)
        : BaseContext(base)
    {}

    Builder::DictItemContext Builder::DictValueContext::Value(Node::Value val)
    {
        return BaseContext{ BaseContext::Value(std::move(val)) };
    }


} // namespace json