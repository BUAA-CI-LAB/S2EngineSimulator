#ifndef __BASE_MESSAGE_QUEUE_HPP
#define __BASE_MESSAGE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <memory>
#include <condition_variable>

typedef struct task_tag {
    int data;
    task_tag( int i ) : data(i) { }
} Task, *PTask;

class MessageQueue {
private:
    std::mutex m_queueMutex;
    std::condition_variable m_cond;
    std::queue<PTask> m_queue;

public:
     MessageQueue(){}
    ~MessageQueue(){
        if ( !m_queue.empty() ){
            PTask pRtn = m_queue.front();
            delete pRtn;
        }
    }

    void PushTask( PTask pTask ){
        std::unique_lock<std::mutex> lock( m_queueMutex );
        m_queue.push( pTask );
        m_cond.notify_one();
    }

    PTask PopTask() {
        PTask pRtn = NULL;
        std::unique_lock<std::mutex> lock( m_queueMutex );
        while ( m_queue.empty() )
        {
            m_cond.wait_for( lock, std::chrono::seconds(1) );
        }

        if ( !m_queue.empty() )
        {
            pRtn = m_queue.front();
            if ( pRtn->data != 0 )
                m_queue.pop();
        }

        return pRtn;
    }
};

void thread_fun( MessageQueue *arguments ) {
    while ( true ) {
        PTask data = arguments->PopTask();
        if (data != NULL) {
            printf( "Thread is: %d\n", std::this_thread::get_id() );
            printf("   %d\n", data->data );
            if ( 0 == data->data ) //Thread end.
                break;
            else
                delete data;
        }
    }
    return;
}

#endif // __BASE_MESSAGE_QUEUE_HPP
