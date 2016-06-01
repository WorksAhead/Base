#pragma once

module Rpc
{
	enum ErrorCode {
		ec_success = 0,
		ec_username_already_exists,
		ec_username_or_password_incorrect,
		ec_engine_version_is_locked,
		ec_engine_version_already_exists,
		ec_engine_version_does_not_exist,
		ec_engine_version_is_removed,
		ec_file_io_error,
		ec_file_data_error,
		ec_access_denied,
	};
};

