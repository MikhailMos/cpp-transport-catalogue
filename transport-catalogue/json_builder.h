#pragma once

#include "json.h"

#include <algorithm>

namespace json {

	class Builder {
	private:
		class BaseContext;
		class DictItemContext;
		class ArrayItemContext;
		class DictValueContext;

	public:
		Builder() = default;

		DictItemContext StartDict();
		Builder& EndDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
		DictValueContext Key(std::string key);
		Builder& Value(Node::Value val);

		Node Build();

	private:
		Node root_;
		std::vector<Node*> nodes_stack_;

		class BaseContext {
		public:
			BaseContext(Builder& builder)
				: builder_(builder) {}

			DictItemContext StartDict();
			Builder& EndDict();
			ArrayItemContext StartArray();
			Builder& EndArray();
			DictValueContext Key(std::string key);
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
			DictValueContext Key(std::string key) = delete;
			Node Build() = delete;
			Builder& GetBuilder() = delete;

			ArrayItemContext Value(Node::Value val);
		};

		class DictValueContext : public BaseContext {
		public:
			DictValueContext(BaseContext base);

			Builder& EndDict() = delete;
			Builder& EndArray() = delete;
			DictValueContext Key(std::string key) = delete;
			Node Build() = delete;
			Builder& GetBuilder() = delete;

			DictItemContext Value(Node::Value val);
		};
	};

	namespace ditails {
		// проверяет есть ли пустые значения в Dict
		bool HasEmptyValues(const json::Dict& dic);
		// возвращает ключ у которого пустое значение
		std::pair<bool, std::string> GetKeyWithEmptyValue(const json::Dict& dict);
	}

} //namespace json