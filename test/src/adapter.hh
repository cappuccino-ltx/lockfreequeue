#pragma once

template<typename Q>
struct LFreeQueueAdapter{
    static void enqueue(Q& q, int v){
        while(!q.try_put(v)) {
        }
    }
    static bool dequeue(Q& q, int& v){
        return q.try_get(v);
    }
};

template<typename Q>
struct ConcurrentQueueAdapter{
    static void enqueue(Q& q, int v){
        while(!q.try_enqueue(v)) {
        }
    }
    static bool dequeue(Q& q, int& v){
        return q.try_dequeue(v);
    }
};