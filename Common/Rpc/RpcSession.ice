#pragma once

module Rpc
{
	interface Session
	{
		void destroy();
		void refresh();
	};
};
