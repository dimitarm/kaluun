/*
 * context.hpp
 *
 *  Created on: Jun 25, 2015
 *      Author: dimitar
 */

#include <string>
#include <list>
#include <boost/noncopyable.hpp>
#include "variant.hpp"
#include <rapidjson/reader.h>

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

namespace kaluun {

template<class Context>
struct JsonHandler {
	static const char delimiter = '.';
	Context& context_;
	std::string cur_obj_;
	std::string cur_path_;
	JsonHandler(Context& ctx) :
		context_(ctx) {
	}

	bool Null() {
		return true;
	}
	bool Bool(bool b) {
		add_value(b);
		return true;
	}
	bool Int(int i) {
		add_value(i);
		return true;
	}
	bool Uint(unsigned u) {
		add_value(int(u));
		return true;
	}
	bool Int64(int64_t i) {
		add_value(int(i));
		return true;
	}
	bool Uint64(uint64_t u) {
		add_value(int(u));
		return true;
	}
	bool Double(double d) {
		add_value(d);
		return true;
	}
	bool String(const char* str, rapidjson::SizeType length, bool copy) {
		add_value(str, length);
		return true;
	}
	bool StartObject() {
		if (cur_path_.size()) { //make sure we don't start with a dot
			cur_obj_ = cur_path_;
			cur_obj_.append(1, delimiter);
		} else
			cur_obj_.clear();
		return true;
	}
	bool Key(const char* str, rapidjson::SizeType length, bool copy) {
		cur_path_ = cur_obj_;
		cur_path_.append(str, length);  //current key
		return true;
	}
	bool EndObject(rapidjson::SizeType ) {
		if (!cur_obj_.size()) {
			cur_obj_.clear();
			return true;
		}
		auto pos = cur_obj_.rfind(delimiter, cur_obj_.size() - 2);  //skip the last delimiter. Should not have unnamed properties
		if (pos == std::string::npos)
			cur_obj_.clear();
		else
			cur_obj_.erase(pos);
		return true;
	}
	bool StartArray() {
		if (!context_[cur_path_].is_list()) //list
			context_[cur_path_] = std::list<std::string>(); //make sure the value is list
		return true;
	}
	bool EndArray(rapidjson::SizeType ) {
		return true;
	}

	template<class T>
	void add_value(const T& value) {
		if (context_[cur_path_].is_list()){
			context_[cur_path_].push_back(std::to_string(value));
		}
		else
			context_[cur_path_] = value;
	}
	void add_value(const char* str, rapidjson::SizeType length) {
		if (context_[cur_path_].is_list()){
			context_[cur_path_].push_back(std::string(str, length));
		}
		else
			context_[cur_path_] = std::string(str, length);
	}
};


struct value;
template<class Value = value>
struct value_iterator {
	variant_iterator* variant_it_;

	value_iterator(variant_iterator* variant_it) :
			variant_it_(variant_it) {
	}
	value_iterator(const value_iterator& val_it) {
		variant_it_ = val_it.variant_it_->clone();
	}
	~value_iterator() {
		delete variant_it_;
	}
	Value operator*() {
		Value _val(&(**variant_it_));
		return _val;
	}
	value_iterator<Value>& operator++() {
		++(*variant_it_);
		return *this;
	}
	value_iterator<Value>& operator--() {
		--(*variant_it_);
		return *this;
	}
	value_iterator<Value> operator++(int) {
		value_iterator<Value> _tmp = *this;
		++(*variant_it_);
		return _tmp;
	}
	value_iterator<Value> operator--(int) {
		value_iterator<Value> _tmp = *this;
		--(*variant_it_);
		return _tmp;
	}
	bool operator==(const value_iterator<Value>& it) {
		return *(it.variant_it_) == *variant_it_;
	}
	value_iterator<Value>& operator=(const value_iterator<Value>& val_it) {
		delete variant_it_;
		variant_it_ = val_it.variant_it_->clone();
		return *this;
	}
	bool operator!=(const value_iterator<Value>& it) {
		return *(it.variant_it_) != *variant_it_;
	}
};

struct value {
	mutable variant* variant_;
	mutable bool own_pointer_;

	//values cannot live outside their context!!
	value() :
			variant_(nullptr), own_pointer_(false) {
	}
	value(variant* var) :
			variant_(var), own_pointer_(false) {
	}
	value(const value& new_val) {
		variant_ = new_val.variant_->clone();
		own_pointer_ = true;
	}
	~value() {
		if (own_pointer_ && variant_)
			delete variant_;
	}
	value& operator=(const value& new_val) {
		if (own_pointer_ && variant_)
			delete variant_;
		variant_ = new_val.variant_;
		own_pointer_ = false;
		return *this;
	}

	value_iterator<value> begin() {
		set_default_value();
		value_iterator<value> val_it(variant_->begin());
		return val_it;
	}
	value_iterator<value> end() {
		set_default_value();
		return value_iterator<value>(variant_->end());
	}
	std::string to_string() const {
		set_default_value();
		return variant_->to_string();
	}
	bool is_list() const {
		set_default_value();
		return variant_->is_list();
	}

	void set_default_value() const { //dummy default value lazily set to avoid unnecessary object creation
		if (!variant_) {
			variant_ = new typed_variant<std::string, kaluun::is_iterable<std::string>::value>(std::string(""));
			own_pointer_ = true;
		}
	}
	void push_back(const std::string& new_val) {
		set_default_value();
		typed_variant<std::list<std::string>, true>* list_variant = dynamic_cast<typed_variant<std::list<std::string>, true>*>(variant_);
		if(list_variant)
			list_variant->push_back(new_val);
	}

	template<class T>
	value& operator=(const T& new_val) {
		variant* tmp = new typed_variant<T, kaluun::is_iterable<T>::value>(new_val);
		if (own_pointer_ && variant_) {
			delete variant_;
		}
		variant_ = tmp;
		own_pointer_ = true;
		return *this;
	}
};

template<class T>
T& operator<<(T& str, const value& val){
	str << val.to_string();
	return str;
}

template<class Holder, class Container>
struct context {
	typedef Holder holder_type;
	struct exception: public std::exception {
		std::string what_;
		exception(const std::string& key) {
			what_ = "no value for: ";
			what_ += key;
		}
		const char* what() const noexcept {
			return what_.c_str();
		}
	};

	context* parent_;
	context() :
			parent_(NULL) {
	}
	Container values_;

	context(context* parent) :
			parent_(parent) {
	}
	value& operator[](const holder_type& key) {
		if (values_.find(key) != values_.end())
			return values_[key];
		else if (has(key))
			return parent_->operator [](key);
		else {
			return values_[key];
		}
	}

	bool has(const holder_type& key) {
		if (values_.find(key) != values_.end())
			return true;
		else if (parent_ != NULL)
			return parent_->has(key);
		else
			return false;
	}

	context create_sub_context() {
		return context(this);
	}

	template<class T>
	void load_json(const T& json){
		JsonHandler<context<Holder, Container> > handler(*this);
		rapidjson::Reader reader;
		rapidjson::StringStream ss(json);
		rapidjson::ParseResult result = reader.Parse(ss, handler);
		if (result.IsError()) {
			throw std::logic_error(
					std::string("Invalid JSON object at: ").append(json + result.Offset() - std::min(10, int(result.Offset())),
							json + result.Offset() + std::min(10, int(std::strlen(json) - result.Offset()))));
		}
	}
};

}

#endif /* CONTEXT_HPP_ */
