#include <atomic>

template<typename T>
class lockless_queue
{
public:
    template<typename DataType>
    struct node
    {
        node(const DataType& data)
          : data(data), next(nullptr) {}
        DataType data;
        node* next;
    };

    lockless_queue()
      : head_(nullptr) {}

    void produce(const T &data)
    {
        node<T>* new_node = new node<T>(data);
        // put the current value of head into new_node->next
        new_node->next = head_.load(std::memory_order_relaxed);
        // now make new_node the new head, but if the head
        // is no longer what's stored in new_node->next
        // (some other thread must have inserted a node just now)
        // then put that new head into new_node->next and try again
        while(!std::atomic_compare_exchange_weak_explicit(
            &head_,
            &new_node->next,
            new_node,
            std::memory_order_release,
            std::memory_order_relaxed)) {}
    }

    bool isEmpty() {
        return head_ == NULL;
    }

    node<T>* consume_all()
    {
        // Reset queue and return head atomically
        return head_.exchange(nullptr, std::memory_order_consume);
    }
private:
    std::atomic<node<T>*> head_;
};