#pragma once

#include <emdl/TransferSyntaxes.h>

#include <odil/Tag.h>
#include <odil/VR.h>

namespace emds
{

	class SparseDataSet;

	odil::VR asVR(const odil::Tag& tag);
	odil::VR findVR(const odil::Tag& tag, const SparseDataSet& dataSet);

} // namespace emds
