#include "NSetResponse.h"

#include <odil/registry.h>

namespace emdl
{

namespace message
{

	NSetResponse::NSetResponse()
		: affectedSopClass(initField(odil::registry::AffectedSOPClassUID))
		, affectedSopInstance(initField(odil::registry::AffectedSOPInstanceUID))
	{
		commandField.set(Command::N_SET_RSP);
	}

	NSetResponse::NSetResponse(Value::Integer msgIdResponse, Value::Integer statusVal)
		: NSetResponse()
	{
		messageIdBeingRespondedTo.set(msgIdResponse);
		status.set(statusVal);
	}

	NSetResponse::NSetResponse(const Message& message)
		: NSetResponse()
	{
		copyFromMessage(message, Command::N_SET_RSP, DataSetRequirement::Optional);
	}

}

}
