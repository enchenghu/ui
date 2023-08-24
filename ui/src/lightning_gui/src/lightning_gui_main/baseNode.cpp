#include "baseNode.h"

BaseNode::BaseNode(int taskNum, int queueNum)
{
    nodeTaskMap.clear();
    for(int i = 0; i < taskNum; i++)
    {
        nodeTaskMap.insert(taskMapUnit(i, \
        taskUnit(std::make_unique<Task_>(), std::make_shared<MsgQueuesUnit>(MsgQueues(queueNum), MsgQueues(queueNum)))));
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

int BaseNode::getMsg()
{
    printf("Base node init do nothing\n");    
    return 0;
}

int BaseNode::releaseMsg()
{
    printf("Base node init do nothing\n");    
    return 0;
}

int BaseNode::dispatchMsg()
{
    printf("Base node init do nothing\n");    
    return 0;
}