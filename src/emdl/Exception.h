#pragma once

#include <emdl/emdl_api.h>

#include <exception>
#include <string>

#include <fmt/format.h>

#ifndef LOG_POSITION
#define EMDLLOG_STR_H(x) #x
#define EMDLLOG_STR_HELPER(x) EMDLLOG_STR_H(x)
#define LOG_POSITION ("[" __FILE__ " line " EMDLLOG_STR_HELPER(__LINE__) "] ")
#endif

namespace emdl
{

	//! Base class for EMDS exceptions, allowing use of std::string
	class EMDL_API Exception : public std::exception
	{
	public:
		Exception(const std::string& message = ""); //!< Message string constructor.

		template <class ...Args> 
		Exception(const std::string& str, Args&&... args)
			: m_message(fmt::format(str, std::forward<Args>(args)...))
		{}

		const char* what() const noexcept override; //!< Return the reason for the exception.

	protected:
		std::string m_message; //!< brief Message of the exception.
	};

}
