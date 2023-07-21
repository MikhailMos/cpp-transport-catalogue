#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    using Value = variant;

    Node(Value value);
    
    const Value& GetValue() const;
    Value& GetValue();
    
    bool IsInt() const;
    int AsInt() const;

    bool IsDouble() const;
    bool IsPureDouble() const;
    double AsDouble() const;

    bool IsBool() const;
    bool AsBool() const;

    bool IsString() const;
    const std::string& AsString() const;

    bool IsNull() const;

    bool IsArray() const;
    const Array& AsArray() const;

    bool IsDict() const;
    const Dict& AsDict() const;

    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;

};


class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;
    bool operator==(const Document& rhs) const;
    bool operator!=(const Document& rhs) const;

private:
    Node root_;
};

Document Load(std::istream& input);

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const;

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const;
};

// Шаблон, подходящий для вывода double и int
template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, const PrintContext& ctx);

// Перегрузка функции PrintValue для вывода значений string
void PrintValue(const std::string& str, const PrintContext & ctx);

// Перегрузка функции PrintValue для вывода значений bool
void PrintValue(const bool flag, const PrintContext& ctx);

// Перегрузка функции PrintValue для вывода значений Array
void PrintValue(const Array& arr, const PrintContext& ctx);

// Перегрузка функции PrintValue для вывода значений map
void PrintValue(const Dict& dict, const PrintContext& ctx);

void Print(const Document& doc, std::ostream& output);

}  // namespace json