#ifndef POLYM_QUEUE_HPP
#define POLYM_QUEUE_HPP

#include <memory>
#include "Msg.hpp"

namespace PolyM {

/** Msg ID for timeout message */
const int MSG_TIMEOUT = -1;
const int MSG_EMPTY = 1;
const int MSG_DEFAULT = 0;

/**
 * Queue is a thread-safe message queue.
 * It supports one-way messaging and request-response pattern.
 */
class Queue {
   public:
    Queue();

    ~Queue();

    /**
     * Put Msg to the end of the queue.
     *
     * @param msg Msg to put to the queue.
     */
    void put(Msg&& msg);

    /**
     * Get message from the head of the queue.
     * Blocks until at least one message is available in the queue, or until
     * timeout happens. If get() returns due to timeout, the returned Msg will
     * have Msg ID MSG_TIMEOUT.
     *
     * @param timeoutMillis How many ms to wait for message until timeout
     * happens. = 0:  return immediately, if queue is empty, return MSG_EMPTY <
     * 0:  wait indefinitely.
     */
    std::unique_ptr<Msg> get(int timeoutMillis = -1);

    /**
     * Make a request.
     * Call will block until response is given with respondTo().
     *
     * @param msg Request message. Is put to the queue so it can be retrieved
     * from it with get().
     */
    std::unique_ptr<Msg> request(Msg&& msg);

    /**
     * Respond to a request previously made with request().
     *
     * @param reqUid Msg UID of the request message.
     * @param responseMsg Response message. The requester will receive it as the
     * return value of request().
     */
    void respondTo(MsgUID reqUid, Msg&& responseMsg);

    int size();
    bool empty();
    void setQueueSize(unsigned int size);

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace PolyM

#endif
