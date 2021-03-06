#pragma once

#include <emdl/message/Request.h>

namespace emdl
{
	namespace message
	{
		//! C-STORE-RQ message.
		class EMDL_API CStoreRequest : public Request
		{
		public:
			//! Create a store request with given Message ID, affected SOP class, priority, and data set.
			CStoreRequest(Value::Integer messageId, SOP_Class affectedSopClass,
						  const Value::String& affectedSopInstanceUid,
						  Value::Integer priority, const DataSet& dataSet,
						  const Value::String& moveOriginatorAet = "",
						  Value::Integer moveOriginatorMessageId = -1);

			//! Create a C-STORE-RQ from a generic Message.
			CStoreRequest(const Message& message);

			MandatoryField<SOP_Class> affectedSopClass;
			MandatoryField<Value::String> affectedSopInstanceUid;
			MandatoryField<Value::Integer> priority;

			OptionalField<Value::String> moveOriginatorAet;
			OptionalField<Value::Integer> moveOriginatorMessageId;

		private:
			CStoreRequest();
		};
	}
}
