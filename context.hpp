/*
 * context.hpp
 *
 *  Created on: Jun 25, 2015
 *      Author: dimitar
 */

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include "variant.hpp"

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_


namespace templater{

struct context{

	struct exception : public std::exception{
		std::string what_;
		exception(const std::string& key){
			what_ = "no value for: ";
			what_ += key;
		}
		const char* what() const noexcept{
			return what_.c_str();
		}
	};

	struct value;
	struct value_iterator{
		variant_iterator 		variant_it_;

		value_iterator(variant_iterator variant_it):variant_it_(variant_it){
		}
		value operator*(){
			value _val(&(*variant_it_));
			return _val;
		}
		value_iterator& operator++(){
			variant_it_++;
			return *this;
		}
		value_iterator operator++(int){
			value_iterator _tmp = *this;
			variant_it_++;
			return _tmp;
		}
		bool operator==(value_iterator& it){
			return it.variant_it_ == variant_it_;
		}
		bool operator!=(value_iterator& it){
			return it.variant_it_ != variant_it_;
		}
	};
	//todo
	//values cannot live outside their context!!
	struct value {
		variant* 	variant_;
		bool		own_pointer_;

		value():variant_(nullptr), own_pointer_(false){}
		value(variant* var):variant_(var), own_pointer_(false){}
		value(const value& new_val){
			variant_ = new_val.variant_->clone();
			own_pointer_ = true;
		}
		~value(){
			if (own_pointer_ && variant_)
				delete variant_;
		}
		value& operator=(const value& new_val){
			if (own_pointer_ && variant_)
				delete variant_;
			variant_ = new_val.variant_;
			own_pointer_ = false;
			return *this;
		}

		template<class T>
		value& operator=(const T& new_val){
			if (own_pointer_ && variant_){
				delete variant_;
				variant_ = nullptr;
			}
			variant_ = new typed_variant<T>(new_val);
			own_pointer_ = true;
			return *this;
		}

		value_iterator begin(){
			value_iterator val_it(variant_->begin());
			return val_it;
		}
		value_iterator end(){
			return value_iterator(variant_->end());
		}

		std::string to_string(){
			return variant_->to_string();
		}
	};

	context* parent_;
	context():parent_(NULL){}
	std::unordered_map<std::string, value> values_;

	context(context* parent):parent_(parent){}
	value& operator[](const std::string& key){
		if(values_.count(key))
			return values_[key];
		else if (has(key))
			return parent_->operator [](key);
		else{
			return values_[key];
		}
	}

	bool has(const std::string& key){
		if(values_.count(key))
			return true;
		else if(parent_ != NULL)
			return parent_->has(key);
		else
			return false;
	}

	context create_sub_context(){
		return new context(this);
	}
};

} //namespace templater

#endif /* CONTEXT_HPP_ */
