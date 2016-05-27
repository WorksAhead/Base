#ifndef ASYNCTASK_HEADER_
#define ASYNCTASK_HEADER_

#include <string>

class ASyncTask {
public:
	enum State {
		state_idle,
		state_running,
		state_cancelled,
		state_finished,
		state_failed,
	};

public:
	virtual ~ASyncTask()
	{
	}

	virtual void start() = 0;
	virtual void cancel() = 0;

	virtual int state() = 0;
	virtual int progress() = 0;
	virtual std::string information() = 0;
};

void publishAsyncTask(ASyncTask*);

#endif // ASYNCTASK_HEADER_

