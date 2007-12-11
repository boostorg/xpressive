///////////////////////////////////////////////////////////////////////////////
/// \file define.hpp
/// Defines some handy file-scoped macros.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define CV(T)\
    typename add_const<T>::type

#define REF(T)\
    typename add_reference<T>::type

#define CVREF(T)\
    REF(CV(T))

#define UNCV(T)\
    typename remove_cv<T>::type

#define UNREF(T)\
    typename remove_reference<T>::type

#define UNCVREF(T)\
    UNCV(UNREF(T))

