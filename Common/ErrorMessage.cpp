#include "ErrorMessage.h"

const char* errorMessage(Rpc::ErrorCode ec)
{
	switch (ec)
	{
	case Rpc::ec_success:
		return "Success";
	case Rpc::username_already_exists:
		return "Username already exists";
	case Rpc::username_or_password_incorrect:
		return "Username or password incorrect";
	default:
		return "";
	}
}

