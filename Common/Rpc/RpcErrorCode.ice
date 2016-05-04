#pragma once

module Rpc
{
	enum ErrorCode {
		ec_success = 0,
		username_already_exists,
		username_or_password_incorrect,
	};
};

