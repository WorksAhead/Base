#include "ErrorMessage.h"

const char* errorMessage(Rpc::ErrorCode ec)
{
	switch (ec)
	{
	case Rpc::ec_success:
		return "Success";
	case Rpc::ec_username_does_not_exist:
		return "Username does not exist";
	case Rpc::ec_username_or_password_incorrect:
		return "Username or password incorrect";
	case Rpc::ec_engine_version_is_locked:
		return "Engine version is locked";
	case Rpc::ec_engine_version_already_exists:
		return "Engine version already exists";
	case Rpc::ec_engine_version_does_not_exist:
		return "Engine version does not exist";
	case Rpc::ec_engine_version_is_removed:
		return "Engine version is removed";
	case Rpc::ec_page_does_not_exist:
		return "Page does not exist";
	case Rpc::ec_category_does_not_exist:
		return "Category does not exist";
	case Rpc::ec_file_io_error:
		return "File IO error";
	case Rpc::ec_file_data_error:
		return "File data error";
	case Rpc::ec_out_of_range:
		return "Out of range";
	case Rpc::ec_access_denied:
		return "Access denied";
	case Rpc::ec_invalid_operation:
		return "Invalid operation";
	case Rpc::ec_server_busy:
		return "Server Busy";
	case Rpc::ec_incomplete_form:
		return "Incomplete form";
	case Rpc::ec_incomplete_content:
		return "Incomplete content";
	case Rpc::ec_parent_does_not_exist:
		return "Parent does not exist";
	case Rpc::ec_content_does_not_exist:
		return "Content does not exists";
	case Rpc::ec_operation_failed:
		return "Operation Failed";
	case Rpc::ec_incomplete_engine_version:
		return "Incomplete Engine version";
	case Rpc::ec_extra_does_not_exist:
		return "Extra does not exist";
	case Rpc::ec_incomplete_extra:
		return "Incomplete Extra";
	default:
		return "";
	}
}

