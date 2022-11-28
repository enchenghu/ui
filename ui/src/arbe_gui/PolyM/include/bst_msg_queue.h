
#ifndef BST_MSG_QUEUE_H
#define BST_MSG_QUEUE_H
#include <string>
#include "Msg.hpp"
#include "Queue.hpp"

template <typename dataType>
class bstMsgQueue {
   public:
    bstMsgQueue(std::string sQueueName = "bst_queue",
                unsigned int maxObject = 1)
        : m_message(0), m_queueName(sQueueName), m_maxObject(maxObject)
    {
        m_queue.setQueueSize(maxObject);
    }

    ~bstMsgQueue() {}

    enum queue_get_type_t {GET_TIME_OUT = -1, GET_OK = 0};

    void setParam(std::string queueName, unsigned int maxObject)
    {
        m_queueName = queueName;
        m_maxObject = maxObject;
        m_queue.setQueueSize(maxObject);
    }

    void put(dataType& Data)
    {
        m_queue.put(PolyM::DataMsg<dataType>(m_message++, Data));
    }

    queue_get_type_t get(dataType& data, int timeoutMillis = 0)
    {
        int64_t wait_us = 3000;
        auto Message = m_queue.get(wait_us);
        if(Message.get()->getMsgId() < 0) 
        {
            std::cout << m_queueName << " is empty! get element timeout!!" << std::endl;
            return GET_TIME_OUT;
        }
        auto& dMessage = dynamic_cast<PolyM::DataMsg<dataType>&>(*Message);
        data = std::move(dMessage.getPayload());
        return GET_OK;
    }

    int size() { return m_queue.size(); }

    bool empty() { return m_queue.empty(); }

   private:
    PolyM::Queue m_queue;
    long long m_message;
    std::string m_queueName;
    int m_maxObject;
};

#endif
