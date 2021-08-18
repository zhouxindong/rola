#pragma once

#ifndef _ROCO_PARSE_JSON11_HPP_
#define _ROCO_PARSE_JSON11_HPP_

#pragma region json11

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <limits>

#ifdef _MSC_VER
#if _MSC_VER <= 1800 // VS 2013
#ifndef noexcept
#define noexcept throw()
#endif

#ifndef snprintf
#define snprintf _snprintf_s
#endif
#endif
#endif

#define _ROLA_BEGIN_ namespace rola {
#define _ROLA_END_	 }

_ROLA_BEGIN_

static const int max_depth = 200;

/* Helper for representing null - just a do-nothing struct, plus comparison
* operators so the helpers in JsonValue work. We can't use nullptr_t because
* it may not be orderable.
*/
struct NullStruct {
	bool operator==(NullStruct) const { return true; }
	bool operator<(NullStruct) const { return false; }
};


enum JsonParse {
	STANDARD, COMMENTS
};

class JsonValue;

class Json final {
public:
	// Types
	enum Type {
		NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
	};

	// Array and object typedefs
	typedef std::vector<Json> array;
	typedef std::map<std::string, Json> object;

	// Constructors for the various types of JSON value.
	Json() noexcept;                // NUL
	Json(std::nullptr_t) noexcept;  // NUL
	Json(double value);             // NUMBER
	Json(int value);                // NUMBER
	Json(bool value);               // BOOL
	Json(const std::string& value); // STRING
	Json(std::string&& value);      // STRING
	Json(const char* value);       // STRING
	Json(const array& values);      // ARRAY
	Json(array&& values);           // ARRAY
	Json(const object& values);     // OBJECT
	Json(object&& values);          // OBJECT

									// Implicit constructor: anything with a to_json() function.
	template <class T, class = decltype(&T::to_json)>
	Json(const T& t) : Json(t.to_json()) {}

	// Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
	template <class M, typename std::enable_if<
		std::is_constructible<std::string, decltype(std::declval<M>().begin()->first)>::value
		&& std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
		int>::type = 0>
		Json(const M& m) : Json(object(m.begin(), m.end())) {}

	// Implicit constructor: vector-like objects (std::list, std::vector, std::set, etc)
	template <class V, typename std::enable_if<
		std::is_constructible<Json, decltype(*std::declval<V>().begin())>::value,
		int>::type = 0>
		Json(const V& v) : Json(array(v.begin(), v.end())) {}

	// This prevents Json(some_pointer) from accidentally producing a bool. Use
	// Json(bool(some_pointer)) if that behavior is desired.
	Json(void*) = delete;

	// Accessors
	Type type() const;

	bool is_null()   const { return type() == NUL; }
	bool is_number() const { return type() == NUMBER; }
	bool is_bool()   const { return type() == BOOL; }
	bool is_string() const { return type() == STRING; }
	bool is_array()  const { return type() == ARRAY; }
	bool is_object() const { return type() == OBJECT; }

	// Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
	// distinguish between integer and non-integer numbers - number_value() and int_value()
	// can both be applied to a NUMBER-typed object.
	double number_value() const;
	double to_double() const;
	int int_value() const;
	int to_int() const;

	// Return the enclosed value if this is a boolean, false otherwise.
	bool bool_value() const;
	bool to_bool() const;
	// Return the enclosed string if this is a string, "" otherwise.
	const std::string& string_value() const;
	const std::string& to_string() const;
	// Return the enclosed std::vector if this is an array, or an empty vector otherwise.
	const array& array_items() const;
	const array& to_array() const;
	// Return the enclosed std::map if this is an object, or an empty map otherwise.
	const object& object_items() const;
	const object& to_object() const;

	// Return a reference to arr[i] if this is an array, Json() otherwise.
	const Json& operator[](size_t i) const;
	// Return a reference to obj[key] if this is an object, Json() otherwise.
	const Json& operator[](const std::string& key) const;

	const Json& value(const std::string& key) const;

	// Serialize.
	void dump(std::string& out) const;
	std::string dump() const {
		std::string out;
		dump(out);
		return out;
	}

	// Parse. If parse fails, return Json() and assign an error message to err.
	static Json parse(const std::string& in,
		std::string& err,
		JsonParse strategy = JsonParse::STANDARD);
	static Json parse(const char* in,
		std::string& err,
		JsonParse strategy = JsonParse::STANDARD) {
		if (in) {
			return parse(std::string(in), err, strategy);
		}
		else {
			err = "null input";
			return nullptr;
		}
	}
	// Parse multiple objects, concatenated or separated by whitespace
	static std::vector<Json> parse_multi(
		const std::string& in,
		std::string::size_type& parser_stop_pos,
		std::string& err,
		JsonParse strategy = JsonParse::STANDARD);

	static inline std::vector<Json> parse_multi(
		const std::string& in,
		std::string& err,
		JsonParse strategy = JsonParse::STANDARD) {
		std::string::size_type parser_stop_pos;
		return parse_multi(in, parser_stop_pos, err, strategy);
	}

	bool operator== (const Json& rhs) const;
	bool operator<  (const Json& rhs) const;
	bool operator!= (const Json& rhs) const { return !(*this == rhs); }
	bool operator<= (const Json& rhs) const { return !(rhs < *this); }
	bool operator>  (const Json& rhs) const { return  (rhs < *this); }
	bool operator>= (const Json& rhs) const { return !(*this < rhs); }

	/* has_shape(types, err)
	*
	* Return true if this is a JSON object and, for each item in types, has a field of
	* the given type. If not, return false and set err to a descriptive message.
	*/
	typedef std::initializer_list<std::pair<std::string, Type>> shape;
	bool has_shape(const shape& types, std::string& err) const;

private:
	std::shared_ptr<JsonValue> m_ptr;
};

using std::string;
using std::vector;
using std::map;
using std::make_shared;
using std::initializer_list;
using std::move;

/* * * * * * * * * * * * * * * * * * * *
* Serialization
*/

static inline void dump(NullStruct, string& out) {
	out += "null";
}

static inline void dump(double value, string& out) {
	if (std::isfinite(value)) {
		char buf[32];
		snprintf(buf, sizeof buf, "%.17g", value);
		out += buf;
	}
	else {
		out += "null";
	}
}

static inline void dump(int value, string& out) {
	char buf[32];
	snprintf(buf, sizeof buf, "%d", value);
	out += buf;
}

static inline void dump(bool value, string& out) {
	out += value ? "true" : "false";
}

static inline void dump(const string& value, string& out) {
	out += '"';
	for (size_t i = 0; i < value.length(); i++) {
		const char ch = value[i];
		if (ch == '\\') {
			out += "\\\\";
		}
		else if (ch == '"') {
			out += "\\\"";
		}
		else if (ch == '\b') {
			out += "\\b";
		}
		else if (ch == '\f') {
			out += "\\f";
		}
		else if (ch == '\n') {
			out += "\\n";
		}
		else if (ch == '\r') {
			out += "\\r";
		}
		else if (ch == '\t') {
			out += "\\t";
		}
		else if (static_cast<uint8_t>(ch) <= 0x1f) {
			char buf[8];
			snprintf(buf, sizeof buf, "\\u%04x", ch);
			out += buf;
		}
		else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80
			&& static_cast<uint8_t>(value[i + 2]) == 0xa8) {
			out += "\\u2028";
			i += 2;
		}
		else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i + 1]) == 0x80
			&& static_cast<uint8_t>(value[i + 2]) == 0xa9) {
			out += "\\u2029";
			i += 2;
		}
		else {
			out += ch;
		}
	}
	out += '"';
}

static inline void dump(const Json::array& values, string& out) {
	bool first = true;
	out += "[";
	for (const auto& value : values) {
		if (!first)
			out += ", ";
		value.dump(out);
		first = false;
	}
	out += "]";
}

static inline void dump(const Json::object& values, string& out) {
	bool first = true;
	out += "{";
	for (const auto& kv : values) {
		if (!first)
			out += ", ";
		dump(kv.first, out);
		out += ": ";
		kv.second.dump(out);
		first = false;
	}
	out += "}";
}

// Internal class hierarchy - JsonValue objects are not exposed to users of this API.
class JsonValue {
public:
	friend class Json;
	friend class JsonInt;
	friend class JsonDouble;
	virtual Json::Type type() const = 0;
	virtual bool equals(const JsonValue* other) const = 0;
	virtual bool less(const JsonValue* other) const = 0;
	virtual void dump(std::string& out) const = 0;
	virtual JsonValue* clone() const = 0;
	virtual double number_value() const;
	virtual int int_value() const;
	virtual bool bool_value() const;
	virtual const std::string& string_value() const;
	virtual const Json::array& array_items() const;
	virtual const Json& operator[](size_t i) const;
	virtual const Json::object& object_items() const;
	virtual const Json& operator[](const std::string& key) const;
public:
	virtual JsonValue& operator<<(Json const& val);
	virtual JsonValue& insert(std::string const& key, Json const& val);

	virtual ~JsonValue() {}
};

inline void Json::dump(string& out) const {
	m_ptr->dump(out);
}

template <Json::Type tag, typename T>
class Value : public JsonValue {
public:

	// Constructors
	Value(const T& value) : m_value(value) {}
	Value(T&& value) : m_value(move(value)) {}

	Value() : m_value() {}

	// Get type tag
	Json::Type type() const override {
		return tag;
	}

	Value* clone() const override
	{
		return new Value(*this);
	}

	// Comparisons
	bool equals(const JsonValue* other) const override {
		return m_value == static_cast<const Value<tag, T>*>(other)->m_value;
	}
	bool less(const JsonValue* other) const override {
		return m_value < static_cast<const Value<tag, T>*>(other)->m_value;
	}

	/*const*/ T m_value;
	void dump(string& out) const override { rola::dump(m_value, out); }
};

class JsonDouble final : public Value<Json::NUMBER, double> {
	double number_value() const override { return m_value; }
	int int_value() const override { return static_cast<int>(m_value); }
	bool equals(const JsonValue* other) const override { return m_value == other->number_value(); }
	bool less(const JsonValue* other)   const override { return m_value < other->number_value(); }
public:
	JsonDouble(double value) : Value(value) {}
};

class JsonInt final : public Value<Json::NUMBER, int> {
	double number_value() const override { return m_value; }
	int int_value() const override { return m_value; }
	bool equals(const JsonValue* other) const override { return m_value == other->number_value(); }
	bool less(const JsonValue* other)   const override { return m_value < other->number_value(); }
public:
	JsonInt(int value) : Value(value) {}
};

class JsonBoolean final : public Value<Json::BOOL, bool> {
	bool bool_value() const override { return m_value; }
public:
	JsonBoolean(bool value) : Value(value) {}
};

class JsonString final : public Value<Json::STRING, string> {
	const string& string_value() const override { return m_value; }
public:
	JsonString(const string& value) : Value(value) {}
	JsonString(string&& value) : Value(move(value)) {}
};

class JsonArray final : public Value<Json::ARRAY, Json::array> {
	const Json::array& array_items() const override { return m_value; }
	const Json& operator[](size_t i) const override;

public:
	JsonArray(const Json::array& value) : Value(value) {}
	JsonArray(Json::array&& value) : Value(move(value)) {}
	JsonArray() : Value() {}

	operator Json()
	{
		return m_value;
	}
	JsonArray& operator<<(Json const& value) override
	{
		m_value.push_back(value);
		return *this;
	}
};

class JsonObject final : public Value<Json::OBJECT, Json::object> {
	const Json::object& object_items() const override { return m_value; }
	const Json& operator[](const string& key) const override;
public:
	JsonObject(const Json::object& value) : Value(value) {}
	JsonObject(Json::object&& value) : Value(move(value)) {}
	JsonObject() : Value() {}

	operator Json()
	{
		return m_value;
	}
	JsonObject& insert(std::string const& key, Json const& value) override
	{
		m_value[key] = value;
		return *this;
	}
};

class JsonNull final : public Value<Json::NUL, NullStruct> {
public:
	JsonNull() : Value() {}
};

/* * * * * * * * * * * * * * * * * * * *
* Static globals - static-init-safe
*/
struct Statics {
	const std::shared_ptr<JsonValue> null = make_shared<JsonNull>();
	const std::shared_ptr<JsonValue> t = make_shared<JsonBoolean>(true);
	const std::shared_ptr<JsonValue> f = make_shared<JsonBoolean>(false);
	const string empty_string;
	const vector<Json> empty_vector;
	const map<string, Json> empty_map;
	Statics() {}
};

static const Statics& statics() {
	static const Statics s{};
	return s;
}

static const Json& static_null() {
	// This has to be separate, not in Statics, because Json() accesses statics().null.
	static const Json json_null;
	return json_null;
}

/* * * * * * * * * * * * * * * * * * * *
* Constructors
*/

inline Json::Json() noexcept : m_ptr(statics().null) {}
inline Json::Json(std::nullptr_t) noexcept : m_ptr(statics().null) {}
inline Json::Json(double value) : m_ptr(make_shared<JsonDouble>(value)) {}
inline Json::Json(int value) : m_ptr(make_shared<JsonInt>(value)) {}
inline Json::Json(bool value) : m_ptr(value ? statics().t : statics().f) {}
inline Json::Json(const string& value) : m_ptr(make_shared<JsonString>(value)) {}
inline Json::Json(string&& value) : m_ptr(make_shared<JsonString>(move(value))) {}
inline Json::Json(const char* value) : m_ptr(make_shared<JsonString>(value)) {}
inline Json::Json(const Json::array& values) : m_ptr(make_shared<JsonArray>(values)) {}
inline Json::Json(Json::array&& values) : m_ptr(make_shared<JsonArray>(move(values))) {}
inline Json::Json(const Json::object& values) : m_ptr(make_shared<JsonObject>(values)) {}
inline Json::Json(Json::object&& values) : m_ptr(make_shared<JsonObject>(move(values))) {}

/* * * * * * * * * * * * * * * * * * * *
* Accessors
*/

inline Json::Type Json::type()                           const { return m_ptr->type(); }
inline double Json::number_value()                       const { return m_ptr->number_value(); }
inline double Json::to_double()							 const { return number_value(); }
inline int Json::int_value()                             const { return m_ptr->int_value(); }
inline int Json::to_int()								 const { return int_value(); }
inline bool Json::bool_value()                           const { return m_ptr->bool_value(); }
inline bool Json::to_bool()								const {	return bool_value(); }
inline const string& Json::string_value()               const { return m_ptr->string_value(); }
inline const std::string& Json::to_string()				const {	return string_value(); }
inline const vector<Json>& Json::array_items()          const { return m_ptr->array_items(); }
inline const vector<Json>& Json::to_array()				const { return this->array_items(); }
inline const map<string, Json>& Json::object_items()    const { return m_ptr->object_items(); }
inline const map<string, Json>& Json::to_object()		const {	return object_items(); }
inline const Json& Json::operator[] (size_t i)          const { return (*m_ptr)[i]; }
inline const Json& Json::operator[] (const string& key) const { return (*m_ptr)[key]; }
inline const Json& Json::value(const std::string& key)  const { return this->operator[](key); }

inline double                    JsonValue::number_value()              const { return 0; }
inline int                       JsonValue::int_value()                 const { return 0; }
inline bool                      JsonValue::bool_value()                const { return false; }
inline const string& JsonValue::string_value()              const { return statics().empty_string; }
inline const vector<Json>& JsonValue::array_items()               const { return statics().empty_vector; }
inline const map<string, Json>& JsonValue::object_items()              const { return statics().empty_map; }
inline const Json& JsonValue::operator[] (size_t)         const { return static_null(); }
inline const Json& JsonValue::operator[] (const string&) const { return static_null(); }
inline JsonValue& JsonValue::operator<<(Json const& val) { return *this; }
inline JsonValue& JsonValue::insert(std::string const& key, Json const& val) { return *this; }

inline const Json& JsonObject::operator[] (const string& key) const {
	auto iter = m_value.find(key);
	return (iter == m_value.end()) ? static_null() : iter->second;
}
inline const Json& JsonArray::operator[] (size_t i) const {
	if (i >= m_value.size()) return static_null();
	else return m_value[i];
}

/* * * * * * * * * * * * * * * * * * * *
* Comparison
*/

inline bool Json::operator== (const Json& other) const {
	if (m_ptr == other.m_ptr)
		return true;
	if (m_ptr->type() != other.m_ptr->type())
		return false;

	return m_ptr->equals(other.m_ptr.get());
}

inline bool Json::operator< (const Json& other) const {
	if (m_ptr == other.m_ptr)
		return false;
	if (m_ptr->type() != other.m_ptr->type())
		return m_ptr->type() < other.m_ptr->type();

	return m_ptr->less(other.m_ptr.get());
}

/* * * * * * * * * * * * * * * * * * * *
* Parsing
*/

/* esc(c)
*
* Format char c suitable for printing in an error message.
*/
static inline string esc(char c) {
	char buf[12];
	if (static_cast<uint8_t>(c) >= 0x20 && static_cast<uint8_t>(c) <= 0x7f) {
		snprintf(buf, sizeof buf, "'%c' (%d)", c, c);
	}
	else {
		snprintf(buf, sizeof buf, "(%d)", c);
	}
	return string(buf);
}

static inline bool in_range(long x, long lower, long upper) {
	return (x >= lower && x <= upper);
}

namespace {
	/* JsonParser
	*
	* Object that tracks all state of an in-progress parse.
	*/
	struct JsonParser final {

		/* State
		*/
		const string& str;
		size_t i;
		string& err;
		bool failed;
		const JsonParse strategy;

		/* fail(msg, err_ret = Json())
		*
		* Mark this parse as failed.
		*/
		Json fail(string&& msg) {
			return fail(move(msg), Json());
		}

		template <typename T>
		T fail(string&& msg, const T err_ret) {
			if (!failed)
				err = std::move(msg);
			failed = true;
			return err_ret;
		}

		/* consume_whitespace()
		*
		* Advance until the current character is non-whitespace.
		*/
		void consume_whitespace() {
			while (str[i] == ' ' || str[i] == '\r' || str[i] == '\n' || str[i] == '\t')
				i++;
		}

		/* consume_comment()
		*
		* Advance comments (c-style inline and multiline).
		*/
		bool consume_comment() {
			bool comment_found = false;
			if (str[i] == '/') {
				i++;
				if (i == str.size())
					return fail("unexpected end of input after start of comment", false);
				if (str[i] == '/') { // inline comment
					i++;
					// advance until next line, or end of input
					while (i < str.size() && str[i] != '\n') {
						i++;
					}
					comment_found = true;
				}
				else if (str[i] == '*') { // multiline comment
					i++;
					if (i > str.size() - 2)
						return fail("unexpected end of input inside multi-line comment", false);
					// advance until closing tokens
					while (!(str[i] == '*' && str[i + 1] == '/')) {
						i++;
						if (i > str.size() - 2)
							return fail(
								"unexpected end of input inside multi-line comment", false);
					}
					i += 2;
					comment_found = true;
				}
				else
					return fail("malformed comment", false);
			}
			return comment_found;
		}

		/* consume_garbage()
		*
		* Advance until the current character is non-whitespace and non-comment.
		*/
		void consume_garbage() {
			consume_whitespace();
			if (strategy == JsonParse::COMMENTS) {
				bool comment_found = false;
				do {
					comment_found = consume_comment();
					if (failed) return;
					consume_whitespace();
				} while (comment_found);
			}
		}

		/* get_next_token()
		*
		* Return the next non-whitespace character. If the end of the input is reached,
		* flag an error and return 0.
		*/
		char get_next_token() {
			consume_garbage();
			if (failed) return static_cast<char>(0);
			if (i == str.size())
				return fail("unexpected end of input", static_cast<char>(0));

			return str[i++];
		}

		/* encode_utf8(pt, out)
		*
		* Encode pt as UTF-8 and add it to out.
		*/
		void encode_utf8(long pt, string& out) {
			if (pt < 0)
				return;

			if (pt < 0x80) {
				out += static_cast<char>(pt);
			}
			else if (pt < 0x800) {
				out += static_cast<char>((pt >> 6) | 0xC0);
				out += static_cast<char>((pt & 0x3F) | 0x80);
			}
			else if (pt < 0x10000) {
				out += static_cast<char>((pt >> 12) | 0xE0);
				out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
				out += static_cast<char>((pt & 0x3F) | 0x80);
			}
			else {
				out += static_cast<char>((pt >> 18) | 0xF0);
				out += static_cast<char>(((pt >> 12) & 0x3F) | 0x80);
				out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
				out += static_cast<char>((pt & 0x3F) | 0x80);
			}
		}

		/* parse_string()
		*
		* Parse a string, starting at the current position.
		*/
		string parse_string() {
			string out;
			long last_escaped_codepoint = -1;
			while (true) {
				if (i == str.size())
					return fail("unexpected end of input in string", "");

				char ch = str[i++];

				if (ch == '"') {
					encode_utf8(last_escaped_codepoint, out);
					return out;
				}

				if (in_range(ch, 0, 0x1f))
					return fail("unescaped " + esc(ch) + " in string", "");

				// The usual case: non-escaped characters
				if (ch != '\\') {
					encode_utf8(last_escaped_codepoint, out);
					last_escaped_codepoint = -1;
					out += ch;
					continue;
				}

				// Handle escapes
				if (i == str.size())
					return fail("unexpected end of input in string", "");

				ch = str[i++];

				if (ch == 'u') {
					// Extract 4-byte escape sequence
					string esc = str.substr(i, 4);
					// Explicitly check length of the substring. The following loop
					// relies on std::string returning the terminating NUL when
					// accessing str[length]. Checking here reduces brittleness.
					if (esc.length() < 4) {
						return fail("bad \\u escape: " + esc, "");
					}
					for (size_t j = 0; j < 4; j++) {
						if (!in_range(esc[j], 'a', 'f') && !in_range(esc[j], 'A', 'F')
							&& !in_range(esc[j], '0', '9'))
							return fail("bad \\u escape: " + esc, "");
					}

					long codepoint = strtol(esc.data(), nullptr, 16);

					// JSON specifies that characters outside the BMP shall be encoded as a pair
					// of 4-hex-digit \u escapes encoding their surrogate pair components. Check
					// whether we're in the middle of such a beast: the previous codepoint was an
					// escaped lead (high) surrogate, and this is a trail (low) surrogate.
					if (in_range(last_escaped_codepoint, 0xD800, 0xDBFF)
						&& in_range(codepoint, 0xDC00, 0xDFFF)) {
						// Reassemble the two surrogate pairs into one astral-plane character, per
						// the UTF-16 algorithm.
						encode_utf8((((last_escaped_codepoint - 0xD800) << 10)
							| (codepoint - 0xDC00)) + 0x10000, out);
						last_escaped_codepoint = -1;
					}
					else {
						encode_utf8(last_escaped_codepoint, out);
						last_escaped_codepoint = codepoint;
					}

					i += 4;
					continue;
				}

				encode_utf8(last_escaped_codepoint, out);
				last_escaped_codepoint = -1;

				if (ch == 'b') {
					out += '\b';
				}
				else if (ch == 'f') {
					out += '\f';
				}
				else if (ch == 'n') {
					out += '\n';
				}
				else if (ch == 'r') {
					out += '\r';
				}
				else if (ch == 't') {
					out += '\t';
				}
				else if (ch == '"' || ch == '\\' || ch == '/') {
					out += ch;
				}
				else {
					return fail("invalid escape character " + esc(ch), "");
				}
			}
		}

		/* parse_number()
		*
		* Parse a double.
		*/
		Json parse_number() {
			size_t start_pos = i;

			if (str[i] == '-')
				i++;

			// Integer part
			if (str[i] == '0') {
				i++;
				if (in_range(str[i], '0', '9'))
					return fail("leading 0s not permitted in numbers");
			}
			else if (in_range(str[i], '1', '9')) {
				i++;
				while (in_range(str[i], '0', '9'))
					i++;
			}
			else {
				return fail("invalid " + esc(str[i]) + " in number");
			}

			if (str[i] != '.' && str[i] != 'e' && str[i] != 'E'
				&& (i - start_pos) <= static_cast<size_t>(std::numeric_limits<int>::digits10)) {
				return std::atoi(str.c_str() + start_pos);
			}

			// Decimal part
			if (str[i] == '.') {
				i++;
				if (!in_range(str[i], '0', '9'))
					return fail("at least one digit required in fractional part");

				while (in_range(str[i], '0', '9'))
					i++;
			}

			// Exponent part
			if (str[i] == 'e' || str[i] == 'E') {
				i++;

				if (str[i] == '+' || str[i] == '-')
					i++;

				if (!in_range(str[i], '0', '9'))
					return fail("at least one digit required in exponent");

				while (in_range(str[i], '0', '9'))
					i++;
			}

			return std::strtod(str.c_str() + start_pos, nullptr);
		}

		/* expect(str, res)
		*
		* Expect that 'str' starts at the character that was just read. If it does, advance
		* the input and return res. If not, flag an error.
		*/
		Json expect(const string& expected, Json res) {
			assert(i != 0);
			i--;
			if (str.compare(i, expected.length(), expected) == 0) {
				i += expected.length();
				return res;
			}
			else {
				return fail("parse error: expected " + expected + ", got " + str.substr(i, expected.length()));
			}
		}

		/* parse_json()
		*
		* Parse a JSON object.
		*/
		Json parse_json(int depth) {
			if (depth > max_depth) {
				return fail("exceeded maximum nesting depth");
			}

			char ch = get_next_token();
			if (failed)
				return Json();

			if (ch == '-' || (ch >= '0' && ch <= '9')) {
				i--;
				return parse_number();
			}

			if (ch == 't')
				return expect("true", true);

			if (ch == 'f')
				return expect("false", false);

			if (ch == 'n')
				return expect("null", Json());

			if (ch == '"')
				return parse_string();

			if (ch == '{') {
				map<string, Json> data;
				ch = get_next_token();
				if (ch == '}')
					return data;

				while (1) {
					if (ch != '"')
						return fail("expected '\"' in object, got " + esc(ch));

					string key = parse_string();
					if (failed)
						return Json();

					ch = get_next_token();
					if (ch != ':')
						return fail("expected ':' in object, got " + esc(ch));

					data[std::move(key)] = parse_json(depth + 1);
					if (failed)
						return Json();

					ch = get_next_token();
					if (ch == '}')
						break;
					if (ch != ',')
						return fail("expected ',' in object, got " + esc(ch));

					ch = get_next_token();
				}
				return data;
			}

			if (ch == '[') {
				vector<Json> data;
				ch = get_next_token();
				if (ch == ']')
					return data;

				while (1) {
					i--;
					data.push_back(parse_json(depth + 1));
					if (failed)
						return Json();

					ch = get_next_token();
					if (ch == ']')
						break;
					if (ch != ',')
						return fail("expected ',' in list, got " + esc(ch));

					ch = get_next_token();
					(void)ch;
				}
				return data;
			}

			return fail("expected value, got " + esc(ch));
		}
	};
}//namespace {

inline Json Json::parse(const string& in, string& err, JsonParse strategy) {
	JsonParser parser{ in, 0, err, false, strategy };
	Json result = parser.parse_json(0);

	// Check for any trailing garbage
	parser.consume_garbage();
	if (parser.failed)
		return Json();
	if (parser.i != in.size())
		return parser.fail("unexpected trailing " + esc(in[parser.i]));

	return result;
}

// Documented in json11.hpp
inline vector<Json> Json::parse_multi(const string& in,
	std::string::size_type& parser_stop_pos,
	string& err,
	JsonParse strategy) {
	JsonParser parser{ in, 0, err, false, strategy };
	parser_stop_pos = 0;
	vector<Json> json_vec;
	while (parser.i != in.size() && !parser.failed) {
		json_vec.push_back(parser.parse_json(0));
		if (parser.failed)
			break;

		// Check for another object
		parser.consume_garbage();
		if (parser.failed)
			break;
		parser_stop_pos = parser.i;
	}
	return json_vec;
}

/* * * * * * * * * * * * * * * * * * * *
* Shape-checking
*/

inline bool Json::has_shape(const shape& types, string& err) const {
	if (!is_object()) {
		err = "expected JSON object, got " + dump();
		return false;
	}

	for (auto& item : types) {
		if ((*this)[item.first].type() != item.second) {
			err = "bad type for " + item.first + " in " + dump();
			return false;
		}
	}

	return true;
}

_ROLA_END_

#pragma endregion

#pragma region interfaces

#include <fstream>
#include <sstream>

_ROLA_BEGIN_

class Easy_json
{
public:
	static std::shared_ptr<Json> from_json(std::string const& json)
	{
		std::string err;
		auto p = Json::parse(json, err);
		if (err != "")
			return std::shared_ptr<Json>();
		return std::shared_ptr<Json>(new Json(std::move(p)));
	}

	static std::shared_ptr<Json> from_file(std::string const& file)
	{
		std::ifstream inf(file, std::ios::ios_base::in | std::ios::basic_ios::binary);
		if (!inf)
			return std::shared_ptr<Json>();
		std::ostringstream oss;
		oss << inf.rdbuf();
		return from_json(oss.str());
	}

	static std::string to_json(rola::JsonObject const& json)
	{
		std::string out;
		json.dump(out);
		return std::move(out);
	}

	static bool to_file(std::string const& file, rola::JsonObject const& json)
	{
		std::ofstream outf(file, std::ios::ios_base::out
			| std::ios::ios_base::trunc
			| std::ios::basic_ios::binary);
		if (!outf)
			return false;

		std::string out = to_json(json);
		outf << out;
		outf.flush();
		outf.close();
		return true;
	}
};

_ROLA_END_

#pragma endregion

#endif // _ROCO_PARSE_JSON11_HPP_