#include <emdl/message/CStoreRequest.h>
#include <emdl/registry.h>

namespace emdl
{
	namespace message
	{
		CStoreRequest::CStoreRequest()
			: affectedSopClass(initField(registry::AffectedSOPClassUID))
			, affectedSopInstanceUid(initField(registry::AffectedSOPInstanceUID))
			, priority(initField(registry::Priority))
			, moveOriginatorAet(initField(registry::MoveOriginatorApplicationEntityTitle))
			, moveOriginatorMessageId(initField(registry::MoveOriginatorMessageID))
		{
			commandField.set(Command::C_STORE_RQ);
		}

		CStoreRequest::CStoreRequest(Value::Integer msgId, SOP_Class sopClass, const Value::String& sopInstance,
									 Value::Integer prio, const DataSet& dataSet, const Value::String& originAet, Value::Integer originMsgId)
			: CStoreRequest()
		{
			messageId.set(msgId);
			affectedSopClass.set(sopClass);
			affectedSopInstanceUid.set(sopInstance);
			priority.set(prio);

			if (!originAet.empty())
				moveOriginatorAet.set(originAet);
			if (originMsgId >= 0)
				moveOriginatorMessageId.set(originMsgId);

			setDataSet(dataSet);
		}

		CStoreRequest::CStoreRequest(const Message& message)
			: CStoreRequest()
		{
			copyFromMessage(message, Command::C_STORE_RQ, DataSetRequirement::Mandatory);
		}
	}
}
