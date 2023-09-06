#include "baseNode.h"

BaseNode::BaseNode(int taskNum, int slotNum)
{
    nodeTaskMap.clear();
    for(int i = 0; i < taskNum; i++)
    {
        nodeTaskMap.insert(taskMapUnit(i, \
        taskUnit(std::make_unique<Task_>(), std::make_shared<MsgQueuesUnit>(MsgQueues(slotNum), MsgQueues(slotNum)))));
    }
}
void BaseNode::process(int what, void* msg)
{
    printf("Base node process do nothing\n");
}


void BaseNode::startTask()
{
    printf("Base node start_task do nothing\n");
}

void BaseNode::deinit()
{
    printf("Base node deinit do nothing\n");
}

//template <class T>
int BaseNode::initTask(MsgPtr_ msg)
{
    for(auto &t : nodeTaskMap){
        for(int i = 0; i < MAX_QUEUE_BUFF; i++){
            for(auto &e : t.second.second->free) e.put(msg);
        }        
    }
    return 0;
}

int BaseNode::getMsg(MsgPtr_ & msg, int task_id, int slot_i)
{
    printf("Base node init do nothing\n");    
    return 0;
}

int BaseNode::releaseMsg(MsgPtr_ msg, int task_id, int slot_i)
{
    printf("Base node init do nothing\n");    
    return 0;
}

int BaseNode::dispatchMsg(MsgPtr_ msg, int task_id, int slot_i)
{
    //printf("Base node init do nothing\n");   
    auto iter =  nodeTaskMap.find(task_id);
    if(iter != nodeTaskMap.end()){
        MsgPtr_ ptr = nullptr;
        if(!iter->second.second->free.at(slot_i).get(ptr)){
            //*ptr = *msg;
            iter->second.second->done.at(slot_i).put(ptr);   
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}