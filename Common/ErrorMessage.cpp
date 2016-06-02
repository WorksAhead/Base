#include "ErrorMessage.h"

const char* errorMessage(Rpc::ErrorCode ec)
{
	switch (ec)
	{
	case Rpc::ec_success:
		return "Success";
	case Rpc::ec_username_already_exists:
		return "Username already exists";
	case Rpc::ec_username_or_password_incorrect:
		return "Username or password incorrect";
	case Rpc::ec_engine_version_is_locked:
		return "Engine version is locked";
	case Rpc::ec_engine_version_already_exists:
		return "Engine version already exists";
	case Rpc::ec_file_io_error:
		return "File IO error";
	case Rpc::ec_file_data_error:
		return "File data error";
	case Rpc::ec_out_of_range:
		return "Out of range";
	case Rpc::ec_access_denied:
		return "Access denied";
	default:
		return "";
	}
}

