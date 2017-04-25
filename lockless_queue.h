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

    void produce(const T &data);

    bool isEmpty();

    node<T>* consume_all();
private:
    std::atomic<node<T>*> head_;
};