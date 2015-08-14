/*
 * context.hpp
 *
 *  Created on: Jun 25, 2015
 *      Author: dimitar
 */

#include <string>
#include <unordered_map>

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_


namespace templater{

struct context{

	struct variant{
		std::string value_;
		variant() {}

		std::string::iterator begin(){
			return value_.begin();
		}

		std::string::iterator end(){
			return value_.end();
		}

		variant& operator=(const std::string& str_val){
			value_ = str_val;
			return *this;
		}

		variant& operator=(const std::string::value_type& char_val){
			value_ = char_val;
			return *this;
		}

		const std::string& to_string(){
			return value_;
		}

	};


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

	context* parent_;


	context():parent_(NULL){}
	std::unordered_map<std::string, variant> values_;

	context(context* parent):parent_(parent){}
	variant& operator[](const std::string& key){
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

//std::string& operator=(std::string& str, const templater::context::variant& var){
//			return str;
//		}

#endif /* CONTEXT_HPP_ */
