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


#define MAX_QUEUE_BUFF 4
//template<class T>
class BaseNode
{
	public:
		BaseNode(int taskNum = 1, int slotNum = 1);
		virtual ~BaseNode(){};
	public:	
		virtual void deinit(void);	
		virtual void startTask(int task_id);
		virtual int handleLoop();
		virtual int releaseMsg(MsgPtr_ msg, int task_id = 0, int slot_id = 0);
		virtual int getMsg(MsgPtr_ & msg, int task_id = 0, int slot_id = 0);
		template <class T>
		int dispatchMsg(T & , int task_id = 0, int slot_id = 0);
        virtual void process(int what, void* msg);
		template <class T>
		int initTask(int task_id = 0, int slot_id = 0, int buf_num = 4);
    	static void* trampoline(void* p);
		void taskLoop();

        //std::vector<TaskSharePtr_> getTask();

	private:
		TaskPara_ taskPara;
        std::vector<msgQueueSharePtr> msgQueues;
        taskMap_ nodeTaskMap;
	protected:
  		static void TaskFunc(void* arg);

};


