#include <emdl/message/Message.h>

namespace emdl
{
	namespace message
	{
		BaseField::BaseField(const BaseInitField& init)
			: m_tag(init.tag)
			, m_dataSet(init.dataSet)
		{
			init.registerField(*this);
		}

		Tag BaseField::tag() const
		{
			return m_tag;
		}

		void BaseField::BaseInitField::registerField(BaseField& field) const
		{
			owner.addField(field);
		}
	}
}
