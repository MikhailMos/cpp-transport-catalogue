#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadInt(istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    }
    else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            }
            catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    }
    catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
    
}

Node LoadString(istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    string line;
    
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        }
        else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
            case 'n':
                line.push_back('\n');
                break;
            case 't':
                line.push_back('\t');
                break;
            case 'r':
                line.push_back('\r');
                break;
            case '"':
                line.push_back('"');
                break;
            case '\\':
                line.push_back('\\');
                break;
            default:
                // Встретили неизвестную escape-последовательность
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        }
        else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            line.push_back(ch);
        }
        ++it;
    }

    return Node(move(line));

}

Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }

    return Node(move(result));
}

void CheckValue(istream& input, std::string sample) {
    std::string buffer;
    for (char letter : sample) {
        char next_char = input.get();
        if (letter != next_char) {
            for (char ch : buffer) {
                input.putback(std::move(ch));
            }
            throw ParsingError("Incorrect value"s);
        }
        buffer += next_char;
    }
}

Node LoadNode(istream& input) {
    char c;
    input >> c;
    
    if (c == '[') {
        if (input.peek() == ']') { 
            input >> c; //прочитаем, чтоб последующая обработка была корректной
            return Array{}; 
        }
        return LoadArray(input);
    } else if (c == '{') {
        if (input.peek() == '}') {
            input >> c; //прочитаем, чтоб последующая обработка была корректной
            return Dict{};
        } 
        return LoadDict(input);
    }
    else if (c == '"') {
        return LoadString(input);
    }
    else if (c == 'n') { //null       
        CheckValue(input, "ull"s);
        return Node{};
    }    
    else if (c == 't') { //true       
        CheckValue(input, "rue"s);
        return Node{ true };
    } 
    else if (c == 'f') { //false
        CheckValue(input, "alse"s);
        return Node{ false };
    }
    else {        
        input.putback(c);
        return LoadInt(input);
    }
}

}  // namespace

//----- Node -----
const Node::Value& Node::GetValue() const { return *this; }

Node::Value& Node::GetValue() { return *this; }

bool Node::IsInt() const {    
    return std::holds_alternative<int>(*this);
}

bool Node::IsDouble() const {
    return IsInt() || IsPureDouble();
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(*this);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(*this);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(*this);
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(*this);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(*this);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(*this);
}

int  Node::AsInt() const {
    if (!IsInt()) {
        throw std::logic_error("Contains a value of another type"s);
    }
    
    return std::get<int>(*this);
}

bool  Node::AsBool() const {
    if (!IsBool()) {
        throw std::logic_error("Contains a value of another type"s);
    }
    
    return std::get<bool>(*this);
}

double  Node::AsDouble() const {
    if (!IsDouble()) {
        throw std::logic_error("Contains a value of another type"s);
    }
    return IsPureDouble() ? std::get<double>(*this) : AsInt();
}

const std::string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("Contains a value of another type"s);
    }

    return std::get<std::string>(*this);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw std::logic_error("Contains a value of another type"s);
    }

    return std::get<Array>(*this);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw std::logic_error("Contains a value of another type"s);
    }

    return std::get<Dict>(*this);
}

bool Node::operator==(const Node& rhs) const
{
    return GetValue() == rhs.GetValue();
}

bool Node::operator!=(const Node& rhs) const
{
    return !(*this == rhs);
}

//----- document -----
Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document& rhs) const
{
    return GetRoot() == rhs.GetRoot();
}

bool Document::operator!=(const Document& rhs) const
{
    return !(GetRoot() == rhs.GetRoot());
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

//------ PrintContext ------
void PrintContext::PrintIndent() const {
    for (int i = 0; i < indent; ++i) {
        out.put(' ');
    }
}

PrintContext PrintContext::Indented() const {
    return { out, indent_step, indent_step + indent };
}

//------ перегрузка ф-й для variant -----
void PrintValue(std::nullptr_t, const PrintContext& ctx) {
    ctx.out << "null"sv;
}

void PrintValue(const std::string& str, const PrintContext& ctx) {
    ctx.out << "\""sv;
    
    for (const char& ch : str) {
        if (ch == '\"') { ctx.out << "\\"sv << ch; }
        else if (ch == '\r') { ctx.out << "\\r"sv; } 
        else if (ch == '\n') { ctx.out << "\\n"sv; }
        else if (ch == '\\') { ctx.out << "\\"sv << ch; }
        else {
            ctx.out << ch;
        }
    }
        
    ctx.out << "\""sv;
}

void PrintValue(const bool flag, const PrintContext& ctx) {
    if (flag) {
        ctx.out << "true"sv;
    }
    else {
        ctx.out << "false"sv;
    }
}

void PrintValue(const Array& arr, const PrintContext& ctx) {
    
    bool is_first = true;
    const PrintContext& ctx_context = ctx.Indented();
    
    ctx.out << "["sv;
    if (arr.size() > 0) { ctx.out << std::endl; }

    for (const auto& val : arr) {
        if (!is_first) { ctx_context.out << ","sv << std::endl; }
        
        ctx_context.PrintIndent();
        std::visit(
            [&ctx_context](auto value) {
                PrintValue(value, ctx_context);
            }, val.GetValue());
        if (is_first) { is_first = false; }        
    }
    
    ctx_context.out << std::endl; 
    ctx.PrintIndent();
    ctx_context.out << "]"sv;
}

void PrintValue(const Dict& dict, const PrintContext& ctx) {

    bool is_first = true;
    const PrintContext& ctx_context = ctx.Indented();
        
    ctx.out << "{"sv << std::endl;

    for (const auto& [str, node] : dict) {        
        if (!is_first) { ctx_context.out << ","sv << std::endl; }
        ctx_context.PrintIndent();
        ctx_context.out << "\""sv << str << "\": "sv;
        
        std::visit(
            [&ctx_context](auto value) {
                PrintValue(value, ctx_context);
            }, node.GetValue());
        
        if (is_first) { is_first = false; }
        
    }

    ctx_context.out << std::endl;
    ctx.PrintIndent();
    ctx_context.out << "}"sv;

}
//----- конец перегрузки -----

void Print(const Document& doc, std::ostream& output) {
    
    PrintContext ctx{ output };
    const Node& node = doc.GetRoot();

    std::visit(
        [&ctx](auto value){
            PrintValue(value, ctx);
        }, node.GetValue()
    );

}


}  // namespace json