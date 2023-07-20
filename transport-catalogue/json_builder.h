#pragma once

#include "json.h"

#include <algorithm>

namespace json {

	class Builder;
	class DictItemContext;
	class ArrayItemContext;
	class KeyContext;

	class BaseContext {
	public:
		BaseContext(Builder& builder)
			: builder_(builder) {}

		DictItemContext StartDict();
		Builder& EndDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
		KeyContext Key(std::string key);
		Builder& Value(Node::Value val);

		Node Build();

		Builder& GetBuilder();

	private:
		Builder& builder_;
	};

	class DictItemContext : public BaseContext {
	public:
		DictItemContext(BaseContext base);

		DictItemContext StartDict() = delete;
		ArrayItemContext StartArray() = delete;
		Builder& EndArray() = delete;
		Builder& Value(Node::Value val) = delete;
		Node Build() = delete;
		Builder& GetBuilder() = delete;
	};

	class ArrayItemContext : public BaseContext {
	public:
		ArrayItemContext(BaseContext base);

		Builder& EndDict() = delete;
		KeyContext Key(std::string key) = delete;
		Node Build() = delete;
		Builder& GetBuilder() = delete;

		ArrayItemContext Value(Node::Value val);
	};

	class KeyContext : public BaseContext {
	public:
		KeyContext(BaseContext base);

		Builder& EndDict() = delete;
		Builder& EndArray() = delete;
		KeyContext Key(std::string key) = delete;
		Node Build() = delete;
		Builder& GetBuilder() = delete;

		DictItemContext Value(Node::Value val);
	};

	class Builder {
	public:
		Builder() = default;

		DictItemContext StartDict();
		Builder& EndDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
		KeyContext Key(std::string key);
		Builder& Value(Node::Value val);

		Node Build();

	private:
		Node root_;
		std::vector<Node*> nodes_stack_;
	};

	namespace ditails {
		bool HasEmptyValues(json::Dict dic);
	}

} //namespace json