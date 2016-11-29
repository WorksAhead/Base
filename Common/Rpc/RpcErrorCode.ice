#pragma once

module Rpc
{
	enum ErrorCode {
		ec_success = 0,
		ec_username_does_not_exist,
		ec_username_or_password_incorrect,
		ec_engine_version_is_locked,
		ec_engine_version_already_exists,
		ec_engine_version_does_not_exist,
		ec_engine_version_is_removed,
		ec_page_does_not_exist,
		ec_category_does_not_exist,
		ec_file_io_error,
		ec_file_data_error,
		ec_out_of_range,
		ec_access_denied,
		ec_invalid_operation,
		ec_server_busy,
		ec_incomplete_form,
		ec_incomplete_content,
		ec_parent_does_not_exist,
		ec_content_does_not_exist,
		ec_operation_failed,
		ec_incomplete_engine_version,
		ec_extra_does_not_exist,
		ec_incomplete_extra,
		ec_incomplete_client_version,
		ec_client_version_is_locked,
		ec_client_version_already_exists,
		ec_client_version_does_not_exist,
		ec_client_version_is_removed,
		ec_client_version_too_low,
	};
};

