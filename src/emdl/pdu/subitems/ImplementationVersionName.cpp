#include <emdl/pdu/subitems/ImplementationVersionName.h>
#include <emdl/pdu/items/ItemFactory.h>

namespace emdl
{
	namespace pdu
	{
		ImplementationVersionName::ImplementationVersionName()
			: implementationVersionName(initField("Implementation-version-name"), length, 0)
		{
			itemType.set(ItemType::ImplementationVersionName);
		}

		ImplementationVersionName::ImplementationVersionName(const std::string& name)
			: ImplementationVersionName()
		{
			implementationVersionName.set(name);
		}

		ImplementationVersionName::ImplementationVersionName(std::istream& in)
			: ImplementationVersionName()
		{
			read(in);
		}

		RegisterItem<ImplementationVersionName> implementationVersionNameSubItem{ItemType::ImplementationVersionName};
	}
}
