/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TypeCasts_h
#define TypeCasts_h

#include <type_traits>

namespace WTF {

template <typename ExpectedType, typename ArgType, bool isBaseType = std::is_base_of<ExpectedType, ArgType>::value>
struct TypeCastTraits {
    static bool isOfType(ArgType&)
    {
        // If you're hitting this assertion, it is likely because you used
        // is<>() or downcast<>() with a type that doesn't have the needed
        // TypeCastTraits specialization. Please use the following macro
        // to add that specialization:
        // SPECIALIZE_TYPE_TRAITS_BEGIN() / SPECIALIZE_TYPE_TRAITS_END()
        static_assert(std::is_void<ExpectedType>::value, "Missing TypeCastTraits specialization");
        return false;
    }
};

// Template specialization for the case where ExpectedType is a base of ArgType,
// so we can return return true unconditionally.
template <typename ExpectedType, typename ArgType>
struct TypeCastTraits<ExpectedType, ArgType, true /* isBaseType */> {
    static bool isOfType(ArgType&) { return true; }
};

// Type checking function, to use before casting with downcast<>().
template <typename ExpectedType, typename ArgType>
inline bool is(ArgType& source)
{
    static_assert(!std::is_base_of<ExpectedType, ArgType>::value, "Unnecessary type check");
    return TypeCastTraits<const ExpectedType, const ArgType>::isOfType(source);
}

template <typename ExpectedType, typename ArgType>
inline bool is(ArgType* source)
{
    static_assert(!std::is_base_of<ExpectedType, ArgType>::value, "Unnecessary type check");
    return source && TypeCastTraits<const ExpectedType, const ArgType>::isOfType(*source);
}

// Safe downcasting functions.
template<typename Target, typename Source>
inline typename std::conditional<std::is_const<Source>::value, const Target&, Target&>::type downcast(Source& source)
{
    static_assert(!std::is_base_of<Target, Source>::value, "Unnecessary cast");
    ASSERT_WITH_SECURITY_IMPLICATION(is<Target>(source));
    return static_cast<typename std::conditional<std::is_const<Source>::value, const Target&, Target&>::type>(source);
}
template<typename Target, typename Source> inline typename std::conditional<std::is_const<Source>::value, const Target*, Target*>::type downcast(Source* source)
{
    static_assert(!std::is_base_of<Target, Source>::value, "Unnecessary cast");
    ASSERT_WITH_SECURITY_IMPLICATION(!source || is<Target>(*source));
    return static_cast<typename std::conditional<std::is_const<Source>::value, const Target*, Target*>::type>(source);
}

// Add support for type checking / casting using is<>() / downcast<>() helpers for a specific class.
#define SPECIALIZE_TYPE_TRAITS_BEGIN(ClassName) \
namespace WTF { \
template <typename ArgType> \
class TypeCastTraits<const ClassName, ArgType, false /* isBaseType */> { \
public: \
    static bool isOfType(ArgType& source) { return isType(source); } \
private:

#define SPECIALIZE_TYPE_TRAITS_END() \
}; \
}

} // namespace WTF

using WTF::TypeCastTraits;
using WTF::is;
using WTF::downcast;

#endif // TypeCasts_h