#pragma once

#include "BaseWriter.h"
#include "Element.h"

#include <ostream>

namespace emdl
{

//! Write DICOM objects to a stream
class ElementWriter : public BaseWriter
{
public:
	ElementWriter(std::ostream& stream, bool explicitTS, ItemEncoding itemEncoding = ItemEncoding::ExplicitLength);

	//! Write an element (VR, VL and value)
	void writeElement(const Element& element);	

private:
	class WriterVisitor : public BaseWriter
	{
	public:
		using result_type = void;

		WriterVisitor(std::ostream& stream, odil::VR vr, bool explicitTS, ItemEncoding itemEncoding);

		void operator()(const boost::blank& value) const {}
		void operator()(const emdl::Value::Integers& value) const;
		void operator()(const emdl::Value::Reals&    value) const;
		void operator()(const emdl::Value::Strings&  value) const;
		void operator()(const emdl::Value::DataSets& value) const;
		void operator()(const emdl::Value::Binaries& value) const;

	private:
		void writeEncapsulatedPixelData(const emdl::Value::Binaries& value) const;

		const odil::VR m_vr;
	};
};

}
