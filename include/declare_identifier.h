#pragma once

#include <string>

#define DECLARE_IDENTIFIER(IdentifierType) \
struct IdentifierType { \
    std::string value; \
    inline IdentifierType() = default; \
    inline explicit IdentifierType(const std::string& value): value(value) {} \
    inline bool isValid() const { return !value.empty(); } \
    inline bool operator<  (const IdentifierType& _id)const { return value < _id.value; } \
    inline bool operator!= (const IdentifierType& _id)const { return value != _id.value; } \
    inline bool operator== (const IdentifierType& _id)const { return value == _id.value; } \
}; \
\
struct Hasher##IdentifierType \
{ \
  std::size_t operator()(const IdentifierType& k) const \
  { \
    return std::hash<std::string>()(k.value); \
  } \
};
