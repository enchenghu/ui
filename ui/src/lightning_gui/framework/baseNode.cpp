#include "baseNode.h"

BaseNode::BaseNode(int taskNum, int slotNum)
{
    nodeTaskMap.clear();
    for(int i = 0; i < taskNum; i++)
    {
        nodeTaskMap.insert(taskMapUnit(i, \
        taskUnit(new Task_(), std::make_shared<MsgQueuesUnit>(MsgQueues(slotNum), MsgQueues(slotNum)))));
    }
}
void BaseNode::process(int what, void* msg)
{
    printf("Base node process do nothing\n");
}


void BaseNode::TaskFunc(void* arg)
{
    if(arg){
        BaseNode* ptr = (BaseNode*)arg;
        ptr->handleLoop();
    }
}
void BaseNode::startTask(int task_id)
{
    //printf("Base node start_task do nothing\n");
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        vx_task_set_default_create_params(&taskPara);
        taskPara.app_var = this;
        taskPara.task_mode = 0;
        taskPara.task_main = TaskFunc;
        vx_task_create(nodeTaskMap[task_id].first, &taskPara);       
    }
}

int BaseNode::handleLoop()
{
    printf("Base node start_task do nothing\n");
}

void BaseNode::deinit()
{
    printf("Base node deinit do nothing\n");
}

template <class T>
int BaseNode::initTask(int task_id, int slot_id, int buf_num)
{
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        for(int i = 0; i < buf_num; i++){
            iter->second.second->free.at(slot_id).put(std::make_shared<T>());
        }        
    }
    return 0;
}

int BaseNode::getMsg(MsgPtr_ & msg, int task_id, int slot_id)
{
    printf("Base node init do nothing\n");    
    return 0;
}

int BaseNode::releaseMsg(MsgPtr_ msg, int task_id, int slot_id)
{
    printf("Base node init do nothing\n");    
    return 0;
}

template <class T>
int BaseNode::dispatchMsg(T & msg, int task_id, int slot_id)
{
    //printf("Base node init do nothing\n");   
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        MsgPtr_ ptr = nullptr;
        if(!iter->second.second->free.at(slot_id).get(ptr)){
            T * pptr = (T *)ptr.get();
            *pptr = msg;
            iter->second.second->done.at(slot_id).put(ptr);   
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

void BaseNode::taskLoop()
{
    while(true){

    }
}

void* BaseNode::trampoline(void* p)
{

}