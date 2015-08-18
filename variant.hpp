/*
 * variant.hpp
 *
 *  Created on: Aug 14, 2015
 *      Author: dimitar
 */
#include <boost/lexical_cast.hpp>
#ifndef VARIANT_HPP_
#define VARIANT_HPP_

namespace templater {

struct variant_iterator;
template<class T>
struct typed_variant;

struct variant: public boost::noncopyable {
	//exception
	struct exception: public std::exception {
		std::string what_;
		exception(const std::string& msg) {
			what_ = msg;
		}
		const char* what() const noexcept {
			return what_.c_str();
		}
	};
	/////////////////////////

	variant() {
	}
	virtual ~variant() {
	}
	virtual variant* clone() const = 0;
	virtual variant_iterator begin() = 0;
	virtual variant_iterator end() = 0;
	virtual std::string to_string() const = 0;
	virtual int to_integer() const = 0;
	virtual long to_long() const = 0;
	virtual double to_double() const = 0;
	virtual float to_float() const = 0;
	virtual bool to_boolean() const = 0;
private:
	variant& operator=(const std::string&) {
		return *this;
	}
	variant& operator=(const std::string::value_type&) {
		return *this;
	}
	variant& operator=(const variant&) {
		return *this;
	}
};

struct variant_iterator {
	variant* variant_;
public:
	variant_iterator() :
			variant_(nullptr) {
	}
	variant_iterator(variant* variant) :
			variant_(variant) {
	}
	variant_iterator(const variant_iterator& it) :
			variant_(it.variant_) {
	}
	virtual ~variant_iterator() {
	}
	virtual variant& operator*() const {
		return *variant_;
	}
	;
	virtual variant* operator->() const {
		return variant_;
	}
	;
	virtual variant_iterator& operator++() {
		variant_++;
		return *this;
	}
	;
	virtual variant_iterator operator++(int) {
		variant_iterator _tmp = *this;
		variant_++;
		return _tmp;
	}
	;
	virtual bool operator==(const variant_iterator& iter) const {
		return variant_ == iter.variant_;
	}
	virtual bool operator!=(const variant_iterator& iter) const {
		return variant_ != iter.variant_;
	}
};

template<class T>
struct typed_variant: public variant {
	T value_;
	typed_variant(const T& val) {
		value_ = val;
	}

	variant* clone() const {
		return new typed_variant<T>(value_);
	}

	variant_iterator begin() {
		return variant_iterator(this);
	}
	variant_iterator end() {
		variant_iterator it(this);
		it++;
		return it;
	}
	std::string to_string() const {
		return std::to_string(value_);
	}
	int to_integer() const {
		return boost::lexical_cast<int>(value_);
	}
	long to_long() const {
		return boost::lexical_cast<long>(value_);
	}
	double to_double() const {
		return boost::lexical_cast<double>(value_);
	}
	float to_float() const {
		return boost::lexical_cast<float>(value_);
	}
	bool to_boolean() const {
		return boost::lexical_cast<bool>(value_);
	}
};
template<>
struct typed_variant<std::string> : public variant {
	std::string value_;
	typed_variant(const std::string& val) {
		value_ = val;
	}

	variant* clone() const {
		return new typed_variant<std::string>(value_);
	}
	variant_iterator begin() {
		return variant_iterator(this);
	}
	variant_iterator end() {
		variant_iterator it(this);
		it++;
		return it;
	}
	std::string to_string() const {
		return value_;
	}
	int to_integer() const {
		return boost::lexical_cast<int>(value_);
	}
	long to_long() const {
		return boost::lexical_cast<long>(value_);
	}
	double to_double() const {
		return boost::lexical_cast<double>(value_);
	}
	float to_float() const {
		return boost::lexical_cast<float>(value_);
	}
	bool to_boolean() const {
		return boost::lexical_cast<bool>(value_);
	}
};
}

#endif /* VARIANT_HPP_ */
