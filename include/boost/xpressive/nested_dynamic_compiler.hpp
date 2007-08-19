///////////////////////////////////////////////////////////////////////////////
/// \file nested_dynamic_compiler.hpp
/// Contains the definition of an extended regex_compiler, which supports 
/// nested regular expressions.
//
//  Copyright 2007 Andreas Pokorny. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_NESTED_DYNAMIC_COMPILER_HPP_INCLUDED
#define BOOST_XPRESSIVE_NESTED_DYNAMIC_COMPILER_HPP_INCLUDED 

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


#include <boost/xpressive/xpressive_fwd.hpp>
#include <boost/xpressive/regex_compiler.hpp>
#include <boost/xpressive/detail/dynamic/parser_traits.hpp>

namespace boost{ namespace xpressive{
    namespace detail{
        template<typename BidiIter, typename RegexTraits>
        struct nested_parser
          : compiler_traits<RegexTraits> 
        {
            typedef typename compiler_traits<RegexTraits>::string_type string_type;

            struct _values
            {
                string_type expression;
                basic_regex<BidiIter> regex;
            };
            typedef std::map<string_type,_values> table_type;
            typedef typename table_type::value_type value_type;
            
            nested_parser( RegexTraits const& t )
                : compiler_traits<RegexTraits>(t),
                  start(0)
            {}

            bool is_reference( string_type const& name )
            {
                return this->table.find(name) !=  this->table.end();
            }

            void add_start(string_type const& name, string_type const& regex)
            {
                table[name].expression = regex;
                start = &table[name];
            }

            void add(string_type const& name, string_type const& regex)
            {
                table[name].expression = regex;
            }

            basic_regex<BidiIter> const* get_reference( string_type const& name )
            {
                typename table_type::const_iterator it = this->table.find(name);
                if( it != this->table.end() )
                {
                    return &it->second.regex;
                }
                return NULL;
            }

            _values * start;
            table_type table;
        };
    }

    template<typename BidiIter, typename RegexTraits>
    struct nested_dynamic_compiler : 
        regex_compiler<BidiIter, RegexTraits, detail::nested_parser<BidiIter, RegexTraits> >
    {
        typedef detail::nested_parser<BidiIter,RegexTraits> traits_type;
        typedef typename traits_type::table_type table_type;
        typedef typename traits_type::string_type string_type;
        typedef regex_compiler<BidiIter, RegexTraits, traits_type > base_compiler_type;
        typedef typename base_compiler_type::flag_type flag_type;
        explicit nested_dynamic_compiler(
                RegexTraits const &traits = RegexTraits()
                )
            : regex_compiler<
                BidiIter,
                RegexTraits,
                traits_type
               >(traits)
        {
        }

        using base_compiler_type::compile;

        void compile(flag_type flags = regex_constants::ECMAScript )
        {
            for( typename table_type::iterator it = this->traits().table.begin(),
                    e = this->traits().table.end();
                    it != e;
                    ++it )
            {
                it->second.regex = this->compile(it->second.expression, flags );
            }
        }

        basic_regex<BidiIter> & regex()
        {
            BOOST_ASSERT(this->traits().start);
            return this->traits().start->regex;
        }

        void add_start(string_type const& name, string_type const& regex)
        {
            this->traits().add_start( name, regex );
        }

        void add(string_type const& name, string_type const& regex)
        {
            this->traits().add( name, regex );
        }
    };
}}
#endif

