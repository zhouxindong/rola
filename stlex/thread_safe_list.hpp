#ifndef ROLA_STLEX_THREAD_SAFE_LIST_HPP
#define ROLA_STLEX_THREAD_SAFE_LIST_HPP

#include <memory>
#include <mutex>

namespace rola
{
    template<typename T>
    class Thread_safe_list
    {
        struct Node
        {
            std::mutex m;
            std::shared_ptr<T> data;
            std::unique_ptr<Node> next;

            Node() :
                next()
            {}

            Node(T const& value) :
                data(std::make_shared<T>(value))
            {}
        };

        Node head;

    public:
        Thread_safe_list()
        {}

        ~Thread_safe_list()
        {
            remove_if([](T const&) {return true; });
        }

        Thread_safe_list(Thread_safe_list const& other) = delete;
        Thread_safe_list& operator=(Thread_safe_list const& other) = delete;

        void push_front(T const& value)
        {
            std::unique_ptr<Node> new_node(new Node(value));
            std::lock_guard<std::mutex> lk(head.m);
            new_node->next = std::move(head.next);
            head.next = std::move(new_node);
        }

        template<typename Function>
        void for_each(Function f)
        {
            Node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (Node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                f(*next->data);
                current = next;
                lk = std::move(next_lk);
            }
        }

        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            Node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (Node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                if (p(*next->data))
                {
                    return next->data;
                }
                current = next;
                lk = std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }

        template<typename Predicate>
        void remove_if(Predicate p)
        {
            Node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (Node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                if (p(*next->data))
                {
                    std::unique_ptr<Node> old_next = std::move(current->next);
                    current->next = std::move(next->next);
                    next_lk.unlock();
                }
                else
                {
                    lk.unlock();
                    current = next;
                    lk = std::move(next_lk);
                }
            }
        }
    };
} // namespace rola

#endif // !ROLA_STLEX_THREAD_SAFE_LIST_HPP
