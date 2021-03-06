#pragma once

#include <emdl/message/Response.h>

namespace emdl
{
	namespace message
	{
		//! C-ECHO-RSP message.
		class EMDL_API CEchoResponse : public Response
		{
		public:
			//! Create an echo response with given Message ID and affected SOP class.
			CEchoResponse(Value::Integer messageIdBeingRespondedTo, Value::Integer status, SOP_Class affectedSopClass);

			//! Create a C-ECHO-RSP from a generic Message.
			CEchoResponse(const Message& message);

			MandatoryField<SOP_Class> affectedSopClass;

		private:
			CEchoResponse();
		};
	}
}
