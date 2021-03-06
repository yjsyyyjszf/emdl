#include <emdl/association/AssociationParameters.h>
#include <emdl/association/UserIdentity.h>

#include <emdl/pdu/AAssociateAC.h>
#include <emdl/pdu/AAssociateRQ.h>
#include <emdl/pdu/items/PresentationContextAC.h>
#include <emdl/pdu/items/PresentationContextRQ.h>
#include <emdl/pdu/subitems/AsynchronousOperationsWindow.h>
#include <emdl/pdu/subitems/ImplementationClassUID.h>
#include <emdl/pdu/subitems/ImplementationVersionName.h>
#include <emdl/pdu/subitems/MaximumLength.h>
#include <emdl/pdu/subitems/RoleSelection.h>
#include <emdl/pdu/subitems/SOPClassCommonExtendedNegotiation.h>
#include <emdl/pdu/subitems/SOPClassExtendedNegotiation.h>
#include <emdl/pdu/subitems/UserIdentityAC.h>
#include <emdl/pdu/subitems/UserIdentityRQ.h>

namespace emdl
{
	std::string defaultImplementationClassUid = "1.2.250.1.119.1.1.1.1.1.1.36";
	std::string defaultImplementationVersionName = "EMDL 1.0";

	AssociationParameters::AssociationParameters(const pdu::AAssociateRQ& pdu)
	{
		calledAeTitle = pdu.calledAeTitle.get();
		callingAeTitle = pdu.callingAeTitle.get();

		const auto userInformation = pdu.userInformation();

		// Precompute roles
		std::map<SOP_Class, std::pair<bool, bool>> rolesMap;
		for (const auto& role : userInformation->subItems.get<pdu::RoleSelection>())
			rolesMap[role->sopClass()] = std::make_pair(role->scuRoleSupport(), role->scpRoleSupport());

		// Presentation contexts
		const auto contextsPdus = pdu.presentationContexts();
		presentationContexts.reserve(contextsPdus.size());
		for (const auto& pdu : contextsPdus)
		{
			const auto sopClass = pdu->abstractSyntax();
			const auto it = rolesMap.find(sopClass);
			presentationContexts.emplace_back(
				pdu->presentationContextId.get(),
				sopClass, pdu->transferSyntaxes(),
				(it != rolesMap.end()) ? it->second.first : true,
				(it != rolesMap.end()) ? it->second.second : false,
				it != rolesMap.end());
		}

		// Maximum length
		const auto maxLenPdus = userInformation->subItems.get<pdu::MaximumLength>();
		if (!maxLenPdus.empty())
			maximumLength = maxLenPdus.front()->maximumLength.get();

		// Implementation class UID
		const auto implClassUid = userInformation->subItems.get<pdu::ImplementationClassUID>();
		if (!implClassUid.empty())
			implementationClassUid = implClassUid.front()->implementationClassUid.get();

		// Maximum number of operations performed/invoked
		const auto asyncWindowPdus = userInformation->subItems.get<pdu::AsynchronousOperationsWindow>();
		if (!asyncWindowPdus.empty())
		{
			maximumNumberOperationsInvoked = asyncWindowPdus.front()->maximumNumberOperationsInvoked.get();
			maximumNumberOperationsPerformed = asyncWindowPdus.front()->maximumNumberOperationsPerformed.get();
		}

		// Implementation version name
		const auto implVersionName = userInformation->subItems.get<pdu::ImplementationVersionName>();
		if (!implVersionName.empty())
			implementationVersionName = implVersionName.front()->implementationVersionName.get();

		// Extended negociation
		sopClassExtendedNegotiation = userInformation->subItems.get<pdu::SOPClassExtendedNegotiation>();
		sopClassCommonExtendedNegotiation = userInformation->subItems.get<pdu::SOPClassCommonExtendedNegotiation>();

		// User identity
		const auto userIdPdus = userInformation->subItems.get<pdu::UserIdentityRQ>();
		if (!userIdPdus.empty())
		{
			const auto& userId = *userIdPdus.front();
			switch (userId.type())
			{
			case 1:
				userIdentity = {UserIdentity::Type::Username, userId.primaryField(), ""};
				break;
			case 2:
				userIdentity = {UserIdentity::Type::UsernameAndPassword, userId.primaryField(), userId.secondaryField()};
				break;
			case 3:
				userIdentity = {UserIdentity::Type::Kerberos, userId.primaryField(), ""};
				break;
			case 4:
				userIdentity = {UserIdentity::Type::SAML, userId.primaryField(), ""};
				break;
			}
			userIdentity.positiveResponseRequested = userId.positiveResponseRequested();
		}
	}

	AssociationParameters::AssociationParameters(const pdu::AAssociateAC& pdu, const AssociationParameters& request)
	{
		// Calling and Called AE titles are not meaningful in A-ASSOCIATE-AC
		calledAeTitle = pdu.calledAeTitle.get();
		callingAeTitle = pdu.callingAeTitle.get();

		const auto userInformation = pdu.userInformation();

		// Request presentation contexts
		const auto& requestContexts = request.presentationContexts;
		std::map<uint8_t, PresentationContext> requestContextsMap;
		for (const auto& context : requestContexts)
			requestContextsMap.emplace(context.id, context);

		// Precompute roles
		std::map<SOP_Class, std::pair<bool, bool>> rolesMap;
		for (const auto& role : userInformation->subItems.get<pdu::RoleSelection>())
			rolesMap[role->sopClass()] = std::make_pair(role->scuRoleSupport(), role->scpRoleSupport());

		// Presentation contexts
		const auto contextsPdus = pdu.presentationContexts();
		presentationContexts.reserve(contextsPdus.size());
		for (const auto& pdu : contextsPdus)
		{
			const auto& requestContext = requestContextsMap.at(pdu->presentationContextId.get());
			const auto sopClass = requestContext.abstractSyntax;
			const auto it = rolesMap.find(sopClass);

			presentationContexts.emplace_back(
				pdu->presentationContextId.get(),
				sopClass,
				std::vector<TransferSyntax>{pdu->transferSyntax()},
				(it != rolesMap.end()) ? it->second.first : requestContext.scuRoleSupport,
				(it != rolesMap.end()) ? it->second.second : requestContext.scpRoleSupport,
				it != rolesMap.end(),
				static_cast<PresentationContext::Result>(pdu->result.get()));
		}

		// Maximum length
		const auto maxLenPdus = userInformation->subItems.get<pdu::MaximumLength>();
		if (!maxLenPdus.empty())
			maximumLength = maxLenPdus.front()->maximumLength.get();

		// Implementation class UID
		const auto implClassUid = userInformation->subItems.get<pdu::ImplementationClassUID>();
		if (!implClassUid.empty())
			implementationClassUid = implClassUid.front()->implementationClassUid.get();

		// Maximum number of operations performed/invoked
		const auto asyncWindowPdus = userInformation->subItems.get<pdu::AsynchronousOperationsWindow>();
		if (!asyncWindowPdus.empty())
		{
			maximumNumberOperationsInvoked = asyncWindowPdus.front()->maximumNumberOperationsInvoked.get();
			maximumNumberOperationsPerformed = asyncWindowPdus.front()->maximumNumberOperationsPerformed.get();
		}

		// Implementation version name
		const auto implVersionName = userInformation->subItems.get<pdu::ImplementationVersionName>();
		if (!implVersionName.empty())
			implementationVersionName = implVersionName.front()->implementationVersionName.get();

		// Extended negociation
		sopClassExtendedNegotiation = userInformation->subItems.get<pdu::SOPClassExtendedNegotiation>();
		// No SOPClassCommonExtendedNegotiation in AC

		// User identity
		const auto userIdPdus = userInformation->subItems.get<pdu::UserIdentityAC>();
		if (!userIdPdus.empty())
		{
			const auto& userId = *userIdPdus.front();
			switch (request.userIdentity.type)
			{
			case UserIdentity::Type::Kerberos:
				userIdentity = {UserIdentity::Type::Kerberos, userId.serverResponse(), ""};
				break;
			case UserIdentity::Type::SAML:
				userIdentity = {UserIdentity::Type::SAML, userId.serverResponse(), ""};
				break;
			}
		}
	}

	std::shared_ptr<pdu::AAssociateRQ> toAAssociateRQ(const AssociationParameters& params)
	{
		auto pdu = std::make_shared<pdu::AAssociateRQ>();
		pdu->protocolVersion.set(1);
		pdu->calledAeTitle.set(params.calledAeTitle);
		pdu->callingAeTitle.set(params.callingAeTitle);
		pdu->setApplicationContext("1.2.840.10008.3.1.1.1");

		// Presentation contexts
		std::vector<std::shared_ptr<pdu::PresentationContextRQ>> contexts;
		contexts.reserve(params.presentationContexts.size());
		for (const auto& context : params.presentationContexts)
			contexts.push_back(std::make_shared<pdu::PresentationContextRQ>(context.id,
																			context.abstractSyntax,
																			context.transferSyntaxes));
		pdu->setPresentationContexts(contexts);

		// User information sub items, sorted by their type
		auto userInformation = std::make_shared<pdu::UserInformation>();
		userInformation->subItems.add(std::make_shared<pdu::MaximumLength>(params.maximumLength));

		userInformation->subItems.add(std::make_shared<pdu::ImplementationClassUID>(!params.implementationClassUid.empty()
																						? params.implementationClassUid
																						: defaultImplementationClassUid));

		if (params.maximumNumberOperationsInvoked != 1 || params.maximumNumberOperationsPerformed != 1)
			userInformation->subItems.add(std::make_shared<pdu::AsynchronousOperationsWindow>(
				params.maximumNumberOperationsInvoked,
				params.maximumNumberOperationsPerformed));

		std::vector<std::shared_ptr<pdu::RoleSelection>> roles;
		for (const auto& context : params.presentationContexts)
			roles.push_back(std::make_shared<pdu::RoleSelection>(context.abstractSyntax,
																 context.scuRoleSupport,
																 context.scpRoleSupport));
		userInformation->subItems.add(roles);

		userInformation->subItems.add(std::make_shared<pdu::ImplementationVersionName>(!params.implementationVersionName.empty()
																						   ? params.implementationVersionName
																						   : defaultImplementationVersionName));

		userInformation->subItems.add(params.sopClassExtendedNegotiation);
		userInformation->subItems.add(params.sopClassCommonExtendedNegotiation);

		if (params.userIdentity.type != UserIdentity::Type::None)
		{
			auto userIdentity = std::make_shared<pdu::UserIdentityRQ>();
			userIdentity->setType(static_cast<uint8_t>(params.userIdentity.type));
			userIdentity->setPrimaryField(params.userIdentity.primaryField);
			userIdentity->setSecondaryField(params.userIdentity.secondaryField);
			userIdentity->setPositiveResponseRequested(params.userIdentity.positiveResponseRequested);

			userInformation->subItems.add(userIdentity);
		}

		pdu->setUserInformation(userInformation);

		return pdu;
	}

	std::shared_ptr<pdu::AAssociateAC> toAAssociateAC(const AssociationParameters& params)
	{
		auto pdu = std::make_shared<pdu::AAssociateAC>();
		pdu->protocolVersion.set(1);
		pdu->calledAeTitle.set(params.calledAeTitle);
		pdu->callingAeTitle.set(params.callingAeTitle);
		pdu->setApplicationContext("1.2.840.10008.3.1.1.1");

		// Presentation contexts
		std::vector<std::shared_ptr<pdu::PresentationContextAC>> contexts;
		contexts.reserve(params.presentationContexts.size());
		for (const auto& context : params.presentationContexts)
			contexts.push_back(std::make_shared<pdu::PresentationContextAC>(context.id,
																			context.transferSyntaxes.front(),
																			static_cast<uint8_t>(context.result)));

		pdu->setPresentationContexts(contexts);

		// User information sub items, sorted by their type
		auto userInformation = std::make_shared<pdu::UserInformation>();
		userInformation->subItems.add(std::make_shared<pdu::MaximumLength>(params.maximumLength));

		userInformation->subItems.add(std::make_shared<pdu::ImplementationClassUID>(!params.implementationClassUid.empty()
																						? params.implementationClassUid
																						: defaultImplementationClassUid));

		if (params.maximumNumberOperationsInvoked != 1 || params.maximumNumberOperationsPerformed != 1)
			userInformation->subItems.add(std::make_shared<pdu::AsynchronousOperationsWindow>(
				params.maximumNumberOperationsInvoked,
				params.maximumNumberOperationsPerformed));

		std::vector<std::shared_ptr<pdu::RoleSelection>> roles;
		for (const auto& context : params.presentationContexts)
		{
			if (context.roleSelectionPresent)
			{
				roles.push_back(std::make_shared<pdu::RoleSelection>(context.abstractSyntax,
																	 context.scuRoleSupport,
																	 context.scpRoleSupport));
			}
		}
		userInformation->subItems.add(roles);

		userInformation->subItems.add(std::make_shared<pdu::ImplementationVersionName>(!params.implementationVersionName.empty()
																						   ? params.implementationVersionName
																						   : defaultImplementationVersionName));

		userInformation->subItems.add(params.sopClassExtendedNegotiation);
		// No SOPClassCommonExtendedNegotiation in AC

		if (!params.userIdentity.primaryField.empty())
			userInformation->subItems.add(std::make_shared<pdu::UserIdentityAC>(params.userIdentity.primaryField));

		pdu->setUserInformation(userInformation);

		return pdu;
	}
}
