#pragma once

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <SafeQueue.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include "vx_mutex.h"
#include "vx_queue.h"
#include "vx_task.h"
#include  <map>
#include "logutil.h"
#include "bst_msg_queue.h"


#define MAX_QUEUE_BUFF 4
#define MAX_TASK_NUM 3
//template<class T>
class BaseNode
{
public:
	using MsgPtr_ = std::shared_ptr<void>;
	using MsgQueue = SafeQueue<MsgPtr_>;//bstMsgQueue<std::shared_ptr<void>>;
	using MsgQueues  = std::vector<MsgQueue>;
	using Task_ = vx_task;
	using TaskPtr_ = std::unique_ptr<Task_>;
	using TaskPara_ = vx_task_create_params_t;
	struct MsgQueuesUnit{
		MsgQueuesUnit(MsgQueues &&a, MsgQueues &&b): free(std::forward<MsgQueues>(a)), done(std::forward<MsgQueues>(b)){}
		MsgQueues free;
		MsgQueues done;
	};
	using msgQueueSharePtr = std::shared_ptr<MsgQueuesUnit>;
	using taskUnit = std::pair<Task_ *, msgQueueSharePtr>;
	using taskMap_ = std::map<int, taskUnit>;
	using taskMapUnit = std::pair<int, taskUnit>;
	using taskBuffAddrs = std::vector<std::vector<std::shared_ptr<void>>>;
	using taskBuffAddrsMap = std::map<int, taskBuffAddrs>;
	using taskBuffAddrsMapUnit = std::pair<int, taskBuffAddrs>;	
	BaseNode();
	virtual ~BaseNode(){};
public:	
	MsgPtr_ getDoneMsg(int task_id = 0, int slot_id = 0);
	int releaseMsg(MsgPtr_& msg, int task_id = 0, int slot_id = 0);

private:
	TaskPara_ taskPara;
	std::vector<msgQueueSharePtr> msgQueues;
	taskMap_ nodeTaskMap;
	static void TaskFunc(void* arg, int task_id);
	
protected:
	BaseNode* inputNode;
	MsgPtr_ getFreeMsg(int task_id = 0, int slot_id = 0);
	int dispatchMsg(MsgPtr_&, int task_id = 0, int slot_id = 0);
	int initTaskQueue(std::vector<MsgPtr_> buff_addrs, int task_id = 0, int slot_id = 0);
	virtual void initTaskQueue(void);	
	virtual void deinit(void);	
	virtual void handleLoopTask0();
	virtual void handleLoopTask1();
	virtual void handleLoopTask2();
	void initNode(int slotNum = 1);	
	void startTask(int task_id);

};


