/*
 * variant.hpp
 *
 *  Created on: Aug 14, 2015
 *      Author: dimitar
 */
#include <boost/lexical_cast.hpp>
#include <iterator>
#include <type_traits>

#ifndef VARIANT_HPP_
#define VARIANT_HPP_

namespace templater {

//http://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time
//http://stackoverflow.com/questions/13830158/check-if-a-variable-is-iterable
namespace detail
{
    // To allow ADL with custom begin/end
    using std::begin;
    using std::end;

    template <typename T>
    auto is_iterable_impl(int)
    -> decltype (
        begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
        ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
        *begin(std::declval<T&>()), // operator*
        std::true_type{});

    template <typename T>
    std::false_type is_iterable_impl(...);

}

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

struct variant_iterator;
template<class, bool>
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
	virtual variant_iterator* begin() = 0;
	virtual variant_iterator* end() = 0;
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

template<class T, bool iterable>
struct typed_variant: public variant {
	T value_;
	typed_variant(const T& val):value_(val) {
	}

	variant* clone() const {
		return new typed_variant<T, iterable>(value_);
	}

	variant_iterator* begin() {
		return new variant_iterator(this);
	}
	variant_iterator* end() {
		variant_iterator* it = new variant_iterator(this);
		it->operator ++();
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
struct typed_variant<std::string, true> : public variant {
	std::string value_;
	typed_variant(const std::string& val) {
		value_ = val;
	}

	variant* clone() const {
		return new typed_variant<std::string, true>(value_);
	}
	variant_iterator* begin() {
		return new variant_iterator(this);
	}
	variant_iterator* end() {
		variant_iterator* it = new variant_iterator(this);
		it->operator ++();
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

template<>
struct typed_variant<std::string&, true> : public variant {
	std::string value_;
	typed_variant(const std::string& val) {
		value_ = val;
	}

	variant* clone() const {
		return new typed_variant<std::string, true>(value_);
	}
	variant_iterator* begin() {
		return new variant_iterator(this);
	}
	variant_iterator* end() {
		variant_iterator* it = new variant_iterator(this);
		it->operator ++();
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

template<class T, class V>
struct typed_variant_iterator : public variant_iterator{
	typedef typed_variant<V, templater::is_iterable<V>::value> variant_type;
	T 					it_;  //cur iterator
	T 					end_;  //end iterator
	variant_type*		variant_;

	typed_variant_iterator(const typed_variant_iterator<T, V>& tvi){
		it_ = tvi.it_;
		end_ = tvi.end_;
		variant_ = new variant_type(*it_);
	}
	typed_variant_iterator(const T& it, const T& end){
		it_ = it;
		end_ = end;
		if (it_ != end_)
			variant_ = new variant_type(*it_);  //BOOM! if it_ points to illegal location todo
		else
			variant_  = nullptr;
	}

	virtual ~typed_variant_iterator() {
		if(variant_)
			delete variant_;
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
		it_++;
		if(variant_)
			delete variant_;
		if (it_ != end_)
			variant_ = new variant_type(*it_);
		else
			variant_  = nullptr;
		return *this;
	}
	;
	virtual variant_iterator operator++(int) {
		variant_iterator _tmp = *this;
		it_++;
		if(variant_)
			delete variant_;
		if (it_ != end_)
			variant_ = new variant_type(*it_);
		else
			variant_  = nullptr;
		return _tmp;
	}
	;
	virtual bool operator==(const variant_iterator& iter) const {
		try{
			const typed_variant_iterator<T, V>& _iter = dynamic_cast<const typed_variant_iterator<T, V>&>(iter);
			return it_ == _iter.it_;
		}
		catch(std::bad_cast& ){
			return false;
		}
	}
	virtual bool operator!=(const variant_iterator& iter) const {
		try{
			const typed_variant_iterator<T, V>& _iter = dynamic_cast<const typed_variant_iterator<T, V>&>(iter);
			return it_ != _iter.it_;
		}
		catch(std::bad_cast& ){
			return true;
		}
	}

};

//iterable types
template<class T>
struct typed_variant<T, true> : public variant {
	T value_;

	typed_variant(const T& it):value_(it){	}
	variant* clone() const {
		return new typed_variant<T, true>(value_);
	}

	variant_iterator* begin() {
		return new typed_variant_iterator<decltype(value_.begin()), decltype(*(value_.begin()))>(value_.begin(), value_.end());
	}
	variant_iterator* end() {
		return new typed_variant_iterator<decltype(value_.end()), decltype(*(value_.end()))>(value_.end(), value_.end());
	}
	std::string to_string() const {
		throw std::logic_error("iterable");
	}
	int to_integer() const {
		throw std::logic_error("iterable");
	}
	long to_long() const {
		throw std::logic_error("iterable");
	}
	double to_double() const {
		throw std::logic_error("iterable");
	}
	float to_float() const {
		throw std::logic_error("iterable");
	}
	bool to_boolean() const {
		throw std::logic_error("iterable");
	}
};



}

#endif /* VARIANT_HPP_ */
