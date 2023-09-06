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
#include "bst_msg_queue.h"

typedef enum {
	TASK_DEFAULT = 0, 
	TASK_FFT_ADC_DATA_RECV, 
	TASK_FFT_ADC_DATA_PARSE, 
	TASK_POINTCLOUD_DATA_RECV, 
	TASK_POINTCLOUD_DATA_PARSE, 
	TASK_MOTOR_DATA_RECV, 
	TASK_SYSTEM_DATA_RECV,
	//TASK_LAST = BST_MAX_TASK_NUM - 1
}LIGHTNING_TASK_ID;
using MsgPtr_ = std::shared_ptr<void>;
using MsgQueue = SafeQueue<MsgPtr_>;//bstMsgQueue<std::shared_ptr<void>>;
//using MsgQueue = bstMsgQueue<void *>;
using MsgQueues  = std::vector<MsgQueue>;
struct MsgQueuesUnit{
  MsgQueuesUnit(MsgQueues &&a, MsgQueues &&b): free(std::forward<MsgQueues>(a)), done(std::forward<MsgQueues>(b)){}
  MsgQueues free;
  MsgQueues done;
};
//using MsgQueuesUnit = std::pair<MsgQueues, MsgQueues>;
using TaskQueuesUnit = std::pair<LIGHTNING_TASK_ID, std::shared_ptr<MsgQueuesUnit>>;
using TaskQueuesMap  = std::map<LIGHTNING_TASK_ID, std::shared_ptr<MsgQueuesUnit>>;
using Task_ = vx_task;
using TaskPtr_ = std::unique_ptr<Task_>;
using TaskPara_ = vx_task_create_params_t;
using msgQueueSharePtr = std::shared_ptr<MsgQueuesUnit>;
using taskUnit = std::pair<TaskPtr_, msgQueueSharePtr>;
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
		virtual void startTask(void);
		virtual int releaseMsg(MsgPtr_ msg, int task_id, int slot_i = 0);
		virtual int getMsg(MsgPtr_ & msg, int task_id, int slot_i = 0);
		virtual int dispatchMsg(MsgPtr_ msg, int task_id, int slot_i = 0);
        virtual void process(int what, void* msg);
		//template <class T>
		virtual int initTask(MsgPtr_);
        //std::vector<TaskSharePtr_> getTask();

	private:
		TaskPara_ taskPara;
        std::vector<msgQueueSharePtr> msgQueues;
        taskMap_ nodeTaskMap;
};


