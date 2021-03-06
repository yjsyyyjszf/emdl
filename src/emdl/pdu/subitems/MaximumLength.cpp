#include <emdl/pdu/subitems/MaximumLength.h>
#include <emdl/pdu/items/ItemFactory.h>

namespace emdl
{
	namespace pdu
	{
		MaximumLength::MaximumLength()
			: maximumLength(initField("Maximum-length-received", 0))
		{
			itemType.set(ItemType::MaximumLength);
		}

		MaximumLength::MaximumLength(uint32_t maxLen)
			: MaximumLength()
		{
			maximumLength.set(maxLen);
		}

		MaximumLength::MaximumLength(std::istream& in)
			: MaximumLength()
		{
			read(in);
		}

		RegisterItem<MaximumLength> maximumLengthSubItem{ItemType::MaximumLength};
	}
}
