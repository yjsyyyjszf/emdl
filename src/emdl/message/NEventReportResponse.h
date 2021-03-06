#pragma once

#include <emdl/message/Response.h>

namespace emdl
{
	namespace message
	{
		//! N-EVENT-REPORT-RSP message.
		class EMDL_API NEventReportResponse : public Response
		{
		public:
			//! Create an event report response with given Message ID and status.
			NEventReportResponse(Value::Integer messageIdBeingRespondedTo, Value::Integer status);

			//! Create a N-EVENT-REPORT-RSP from a generic Message.
			NEventReportResponse(const Message& message);

			OptionalField<SOP_Class> affectedSopClass;
			OptionalField<Value::String> affectedSopInstance;
			OptionalField<Value::Integer> eventTypeId;

		private:
			NEventReportResponse();
		};
	}
}
