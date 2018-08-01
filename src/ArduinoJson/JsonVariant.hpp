// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>
#include <stdint.h>  // for uint8_t

#include "Data/JsonVariantData.hpp"
#include "Data/JsonVariantDefault.hpp"
#include "JsonVariant.hpp"
#include "JsonVariantBase.hpp"
#include "Serialization/DynamicStringWriter.hpp"

namespace ArduinoJson {

// Forward declarations.
class JsonArray;
class JsonObject;

// A variant that can be a any value serializable to a JSON value.
//
// It can be set to:
// - a boolean
// - a char, short, int or a long (signed or unsigned)
// - a string (const char*)
// - a reference to a JsonArray or JsonObject
class JsonVariant : public Internals::JsonVariantBase<JsonVariant> {
 public:
  // Intenal use only
  explicit JsonVariant(Internals::JsonVariantData *data) : _data(data) {}

  // Creates an uninitialized JsonVariant
  JsonVariant() : _data(0) {}

  // set(bool value)
  // set(double value);
  // set(float value);
  // set(char)
  // set(signed short)
  // set(signed int)
  // set(signed long)
  // set(signed char)
  // set(unsigned short)
  // set(unsigned int)
  // set(unsigned long)
  // set(SerializedValue<const char *>)
  template <typename T>
  void set(const T &value) {
    if (!_data) return;
    _data->set(value);
  }

  // set(const char*);
  // set(const signed char*);
  // set(const unsigned char*);
  template <typename TChar>
  void set(const TChar *value) {
    if (!_data) return;
    _data->set(value);
  }

  void set(JsonVariant value) {
    if (!_data) return;
    _data->set(value._data);
  }

  void set(const JsonArray &array);
  void set(const Internals::JsonArraySubscript &);
  void set(const JsonObject &object);
  template <typename TString>
  void set(const Internals::JsonObjectSubscript<TString> &);

  // Get the variant as the specified type.
  //
  // char as<char>() const;
  // signed char as<signed char>() const;
  // signed short as<signed short>() const;
  // signed int as<signed int>() const;
  // signed long as<signed long>() const;
  // unsigned char as<unsigned char>() const;
  // unsigned short as<unsigned short>() const;
  // unsigned int as<unsigned int>() const;
  // unsigned long as<unsigned long>() const;
  template <typename T>
  const typename Internals::enable_if<Internals::is_integral<T>::value, T>::type
  as() const {
    return _data ? _data->asInteger<T>() : 0;
  }
  // bool as<bool>() const
  template <typename T>
  const typename Internals::enable_if<Internals::is_same<T, bool>::value,
                                      T>::type
  as() const {
    return _data && _data->asInteger<T>() != 0;
  }
  //
  // double as<double>() const;
  // float as<float>() const;
  template <typename T>
  const typename Internals::enable_if<Internals::is_floating_point<T>::value,
                                      T>::type
  as() const {
    return _data ? _data->asFloat<T>() : 0;
  }
  //
  // const char* as<const char*>() const;
  // const char* as<char*>() const;
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, const char *>::value ||
                                    Internals::is_same<T, char *>::value,
                                const char *>::type
  as() const {
    return _data ? _data->asString() : 0;
  }
  //
  // std::string as<std::string>() const;
  // String as<String>() const;
  template <typename T>
  typename Internals::enable_if<Internals::IsWriteableString<T>::value, T>::type
  as() const {
    const char *cstr = _data ? _data->asString() : 0;
    if (cstr) return T(cstr);
    T s;
    serializeJson(*this, s);
    return s;
  }
  //
  // JsonArray as<JsonArray>() const;
  // const JsonArray as<const JsonArray>() const;
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonArray>::value,
      JsonArray>::type
  as() const;
  //
  // JsonObject as<JsonObject>() const;
  // const JsonObject as<const JsonObject>() const;
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonObject>::value,
      T>::type
  as() const;
  //
  // JsonVariant as<JsonVariant> const;
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonVariant>::value,
                                T>::type
  as() const {
    return *this;
  }

  // Tells weither the variant has the specified type.
  // Returns true if the variant has type type T, false otherwise.
  //
  // bool is<char>() const;
  // bool is<signed char>() const;
  // bool is<signed short>() const;
  // bool is<signed int>() const;
  // bool is<signed long>() const;
  // bool is<unsigned char>() const;
  // bool is<unsigned short>() const;
  // bool is<unsigned int>() const;
  // bool is<unsigned long>() const;
  template <typename T>
  typename Internals::enable_if<Internals::is_integral<T>::value, bool>::type
  is() const {
    return variantIsInteger();
  }
  //
  // bool is<double>() const;
  // bool is<float>() const;
  template <typename T>
  typename Internals::enable_if<Internals::is_floating_point<T>::value,
                                bool>::type
  is() const {
    return variantIsFloat();
  }
  //
  // bool is<bool>() const
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, bool>::value, bool>::type
  is() const {
    return variantIsBoolean();
  }
  //
  // bool is<const char*>() const;
  // bool is<char*>() const;
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, const char *>::value ||
                                    Internals::is_same<T, char *>::value,
                                bool>::type
  is() const {
    return variantIsString();
  }
  //
  // bool is<JsonArray> const;
  // bool is<const JsonArray> const;
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonArray>::value,
      bool>::type
  is() const {
    return variantIsArray();
  }
  //
  // bool is<JsonObject> const;
  // bool is<const JsonObject> const;
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonObject>::value,
      bool>::type
  is() const {
    return variantIsObject();
  }

  // Returns true if the variant has a value
  bool isNull() const {
    return _data == 0 || _data->type == Internals::JSON_UNDEFINED;
  }

  template <typename Visitor>
  void visit(Visitor &visitor) const {
    if (_data)
      _data->visit(visitor);
    else
      visitor.acceptNull();
  }

 private:
  bool variantIsBoolean() const;
  bool variantIsFloat() const;
  bool variantIsInteger() const;
  bool variantIsArray() const {
    return _data && _data->type == Internals::JSON_ARRAY;
  }
  bool variantIsObject() const {
    return _data && _data->type == Internals::JSON_OBJECT;
  }
  bool variantIsString() const {
    return _data && _data->type == Internals::JSON_STRING;
  }

  Internals::JsonVariantData *_data;
};
}  // namespace ArduinoJson
