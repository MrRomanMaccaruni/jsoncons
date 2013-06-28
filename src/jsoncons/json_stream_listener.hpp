// Copyright 2013 Daniel Parker
// Distributed under Boost license

#ifndef JSONCONS_JSON_STREAM_LISTENER_HPP
#define JSONCONS_JSON_STREAM_LISTENER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <istream>
#include <cstdlib>
#include "jsoncons/json_char_traits.hpp"
#include "jsoncons/json1.hpp"

namespace jsoncons {

template <class Char>
class basic_json_stream_listener
{
    enum structure_type {object_t, array_t};
    struct stack_item
    {
        stack_item(structure_type type)
            : type_(type)
        {
            switch (type)
            {
            case object_t:
                structure_.object_ = new json_object<Char>();
                //structure_.object_->reserve(4);
                break;
            case array_t:
                structure_.array_ =  new json_array<Char>();
                //structure_.array_->reserve(4);
                break;
            }
        }
        bool is_object() const
        {
            return type_ == object_t;
        }
        bool is_array() const
        {
            return type_ == array_t;
        }

        std::basic_string<Char> name_;
        structure_type type_;
        union {
            json_object<Char>* object_;
            json_array<Char>* array_;
        } structure_;
    };

public:
    void begin_json()
    {
    }

    void end_json()
    {
    }

    void begin_object()
    {
        stack_.push_back(stack_item(object_t));
    }

    void end_object()
    {
		json_object<Char>* var = stack_.back().structure_.object_;
        var->sort_members();
        basic_json<Char> val(var);	    
		stack_.pop_back();
        if (stack_.size() > 0)
        {
            if (stack_.back().is_object())
            {
                stack_.back().structure_.object_->push_back(std::pair<std::basic_string<Char>,basic_json<Char>>(std::move(stack_.back().name_),std::move(val)));
            }
            else
            {
                stack_.back().structure_.array_->push_back(std::move(val));
            }
        }
        else
        {
            root_ = std::move(val);
        }
    }

    void begin_array()
    {
        stack_.push_back(stack_item(array_t));
    }

    void end_array()
    {
        json_array<Char>* var = stack_.back().structure_.array_;
        stack_.pop_back();
        basic_json<Char> val(var);	    
        if (stack_.size() > 0)
        {
            if (stack_.back().is_object())
            {
                std::pair<std::basic_string<Char>,basic_json<Char>> pair(std::move(stack_.back().name_),std::move(val));
                stack_.back().structure_.object_->push_back(std::move(pair));
            }
            else
            {
                stack_.back().structure_.array_->push_back(std::move(val));
            }
        }
        else
        {
            root_ = std::move(val);
        }
    }

    void name(const std::basic_string<Char>& name)
    {
        stack_.back().name_ = name;
    }

    void value(const std::basic_string<Char>& value)
    {
        basic_json<Char> val(value);
        if (stack_.back().is_object())
        {
            std::pair<std::basic_string<Char>,basic_json<Char>> pair(std::move(stack_.back().name_),std::move(val));
            stack_.back().structure_.object_->push_back(std::move(pair));
        } 
        else 
        {
            stack_.back().structure_.array_->push_back(std::move(val));
        }
    }

    void value(double value)
    {
        basic_json<Char> val(value);
        if (stack_.back().is_object())
        {
            std::pair<std::basic_string<Char>,basic_json<Char>> pair(std::move(stack_.back().name_),std::move(val));
            stack_.back().structure_.object_->push_back(std::move(pair));
        } 
        else
        {
            stack_.back().structure_.array_->push_back(val);
        }
    }

    void value(long long value)
    {
        basic_json<Char> val(value);
        if (stack_.back().is_object())
        {
            stack_.back().structure_.object_->push_back(std::pair<std::basic_string<Char>,basic_json<Char>>(std::move(stack_.back().name_),std::move(val)));
        } 
        else
        {
            stack_.back().structure_.array_->push_back(val);
        }
    }

    void value(unsigned long long value)
    {
        basic_json<Char> val(value);
        if (stack_.back().is_object())
        {
            stack_.back().structure_.object_->push_back(std::pair<std::basic_string<Char>,basic_json<Char>>(std::move(stack_.back().name_),std::move(val)));
        } 
        else
        {
            stack_.back().structure_.array_->push_back(val);
        }
    }

    void value(bool value)
    {
        basic_json<Char> val(value);
        if (stack_.back().is_object())
        {
            stack_.back().structure_.object_->push_back(std::pair<std::basic_string<Char>,basic_json<Char>>(std::move(stack_.back().name_),std::move(val)));
        } 
        else
        {
            stack_.back().structure_.array_->push_back(val);
        }
    }

    void null()
    {
        basic_json<Char> val;
        if (stack_.back().is_object())
        {
            stack_.back().structure_.object_->push_back(std::pair<std::basic_string<Char>,basic_json<Char>>(std::move(stack_.back().name_),std::move(val)));
        } 
        else
        {
            stack_.back().structure_.array_->push_back(val);
        }
    }

    void swap_root(basic_json<Char>& val)
    {
        val.swap(root_);
    }
private:
	basic_json<Char> root_;
    std::vector<stack_item> stack_;
};

typedef basic_json_stream_listener<char> json_stream_listener;

}

#endif
