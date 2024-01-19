#include "baseNode.h"

BaseNode::BaseNode():inputNode(nullptr)
{
    msgQueues.clear();
    nodeTaskMap.clear();
}

void BaseNode::initNode(int slotNum)
{
    nodeTaskMap.clear();
    for(int i = 0; i < MAX_TASK_NUM; i++)
    {
        nodeTaskMap.insert(taskMapUnit(i, \
        taskUnit(new Task_(), std::make_shared<MsgQueuesUnit>(MsgQueues(slotNum), MsgQueues(slotNum)))));
    }
    initTaskQueue();
}

void BaseNode::deinit()
{

}

void BaseNode::initTaskQueue()
{
    /*Inherited node should use initTaskQueue(std::vector<MsgPtr_> buff_addrs, int task_id = 0, int slot_id = 0) 
    to init task queues*/
    printf("Base node initTaskQueue do nothing\n");
}

void BaseNode::TaskFunc(void* arg, int task_id)
{
    if(arg){
        BaseNode* ptr = (BaseNode*)arg;
        switch (task_id)
        {
        case 0:
            ptr->handleLoopTask0();
            break;
        case 1:
            ptr->handleLoopTask1();
            break;
        case 2:
            ptr->handleLoopTask2();
            break;
        
        default:
            break;
        }
    }
}
void BaseNode::startTask(int task_id)
{
    LOGI("task%d begin........", task_id);
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        vx_task_set_default_create_params(&taskPara);
        taskPara.app_var = this;
        taskPara.task_mode = 1;
        taskPara.task_multi = TaskFunc;
        taskPara.fishnet_index = task_id;
        vx_task_create(nodeTaskMap[task_id].first, &taskPara);       
    }else{
        LOGE("Invaild task ID");
    }
}

void BaseNode::handleLoopTask0()
{
    LOGI("Base node handleLoopTask0 do nothing");
}

void BaseNode::handleLoopTask1()
{
    LOGI("Base node handleLoopTask1 do nothing");
}

void BaseNode::handleLoopTask2()
{
    LOGI("Base node handleLoopTask2 do nothing");
}



int BaseNode::initTaskQueue(std::vector<std::shared_ptr<void>> buff_addrs,  int task_id, int slot_id)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        for(auto& e : buff_addrs){
            iter->second.second->free.at(slot_id).put(e);
        }        
    }
    return 0;
}

BaseNode::MsgPtr_ BaseNode::getFreeMsg(int task_id, int slot_id)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        MsgPtr_ ptr = nullptr;
        if(!iter->second.second->free.at(slot_id).get(ptr))
        {
            return ptr;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return nullptr;
}

BaseNode::MsgPtr_ BaseNode::getDoneMsg(int task_id, int slot_id)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        MsgPtr_ ptr = nullptr;
        if(!iter->second.second->done.at(slot_id).get(ptr))
        {
            return ptr;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return nullptr;
}

int BaseNode::releaseMsg(MsgPtr_& msg, int task_id, int slot_id)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        iter->second.second->free.at(slot_id).put(msg); 
    } else {
        return -1;
    }
    return 0; 
}

int BaseNode::dispatchMsg(MsgPtr_& msg, int task_id, int slot_id)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        iter->second.second->done.at(slot_id).put(msg); 
    } else {
        return -1;
    }
    return 0;
}