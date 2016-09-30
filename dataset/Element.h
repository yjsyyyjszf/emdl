#pragma once

#include "Value.h"

#include "odil/Tag.h"
#include "odil/VR.h"

namespace
{
	// Returns true if T is a Value after removing const, volatile and references
	template <class T>
	constexpr bool is_element = typename std::is_same<emds::Element, std::remove_cv_t<std::remove_reference_t<T>>>::value;
}

namespace emds
{

//! Element of a DICOM data set
class Element
{
public:

	//! Value Rrepresentation of the element
	odil::VR vr = odil::VR::INVALID;

	//! Default constructor
	Element() = default;

	//! Copy an existing element 
	Element(const Element&) = default;

	//! Move an existing element
	Element(Element&&) = default;

	//! Create an element by creating, copying or moving a value
	template <class T, std::enable_if_t<!is_element<T>, bool> = true>
	Element(T&& value, const odil::VR& vr = odil::VR::INVALID)
		: m_value(std::forward<T>(value))
		, vr(vr)
	{}

	// Assignement operators (as we declared custom constructors)
	Element& operator=(const Element&) = default;
	Element& operator=(Element&&) = default;

	//! Test whether the element is empty
	bool empty() const;

	//! Return the number of items in the value
	std::size_t size() const;

	// Return the value
	Value& value();
	const Value& value() const;

	// Test the type of the value
	bool isInt() const;
	bool isReal() const;
	bool isString() const;
	bool isDataSet() const;
	bool isBinary() const;

	// Accessors
	Value::Integers& asInt();
	const Value::Integers& asInt() const;

	Value::Reals& asReal();
	const Value::Reals& asReal() const;

	Value::Strings& asString();
	const Value::Strings& asString() const;

	Value::DataSets& asDataSet();
	const Value::DataSets& asDataSet() const;

	Value::Binaries& asBinary();
	const Value::Binaries& asBinary() const;

	// Apply a visitor of elements
	template <class Visitor>
	typename Visitor::result_type applyVisitor(const Visitor& visitor) const
	{ return boost::apply_visitor(visitor, m_value.value()); }

	template <class Visitor>
	typename Visitor::result_type applyVisitor(Visitor& visitor)
	{ return boost::apply_visitor(visitor, m_value.value()); }

private:
	Value m_value;
};

}

