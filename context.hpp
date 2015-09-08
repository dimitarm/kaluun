/*
 * context.hpp
 *
 *  Created on: Jun 25, 2015
 *      Author: dimitar
 */

#include <string>
#include <memory>
#include <map>
#include <boost/noncopyable.hpp>
#include "variant.hpp"

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

namespace templater{

template<class Holder>
struct context{
	typedef Holder			holder_type;
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
		typedef value			reference;
		variant_iterator* 		variant_it_;

		value_iterator(variant_iterator* variant_it):variant_it_(variant_it){}
		value_iterator(const value_iterator& val_it){
			variant_it_ = val_it.variant_it_->clone();
		}
		~value_iterator(){
			delete variant_it_;
		}
		value operator*(){
			value _val(&(**variant_it_));
			return _val;
		}
		value_iterator& operator++(){
			++(*variant_it_);
			return *this;
		}
		value_iterator& operator--(){
			--(*variant_it_);
			return *this;
		}
		value_iterator operator++(int){
			value_iterator _tmp = *this;
			++(*variant_it_);
			return _tmp;
		}
		value_iterator operator--(int){
			value_iterator _tmp = *this;
			--(*variant_it_);
			return _tmp;
		}
		bool operator==(const value_iterator& it){
			return *(it.variant_it_) == *variant_it_;
		}
		value_iterator& operator=(const value_iterator& val_it){
			delete variant_it_;
			variant_it_ = val_it.variant_it_->clone();
			return *this;
		}
		bool operator!=(const value_iterator& it){
			return *(it.variant_it_) != *variant_it_;
		}
	};
	//todo values cannot live outside their context!!
	struct value {
		variant* 	variant_;
		bool		own_pointer_;

		value():variant_(nullptr), own_pointer_(false){	}
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
			variant* tmp = new typed_variant<T, templater::is_iterable<T>::value>(new_val);
			if (own_pointer_ && variant_){
				delete variant_;
			}
			variant_ = tmp;
			own_pointer_ = true;
			return *this;
		}
		value_iterator begin(){
			set_default_value();
			value_iterator val_it(variant_->begin());
			return val_it;
		}
		value_iterator end(){
			set_default_value();
			return value_iterator(variant_->end());
		}
//todo overload common operators
		std::string to_string(){
			set_default_value();
			return variant_->to_string();
		}

		void set_default_value(){ //dummy default value lazily set to avoid unnecessary object creation
			if(!variant_){
				variant_ = new typed_variant<std::string, templater::is_iterable<std::string>::value>(std::string(""));
				own_pointer_ = true;
			}
		}
	};

	context* parent_;
	context():parent_(NULL){}
	std::map<Holder, value> values_;//todo evaluate hash on boost::iterator_range

	context(context* parent):parent_(parent){}
	value& operator[](const holder_type& key){
		if(values_.find(key) != values_.end())
			return values_[key];
		else if (has(key))
			return parent_->operator [](key);
		else{
			return values_[key];
		}
	}

	bool has(const holder_type& key){
		if(values_.find(key) != values_.end())
			return true;
		else if(parent_ != NULL)
			return parent_->has(key);
		else
			return false;
	}

	context create_sub_context(){
		return context(this);
	}
}; }

//todo support for range_mutable_iterator
//namespace boost
//{
//    // specialize range_mutable_iterator and range_const_iterator in namespace boost
//    template<>
//    struct range_mutable_iterator< templater::context::value >
//    {
//        typedef templater::context::value_iterator type;
//    };
//
//    template<>
//    struct range_const_iterator< templater::context::value >
//    {
//        typedef templater::context::value_iterator type;
//    };
//}


#endif /* CONTEXT_HPP_ */
