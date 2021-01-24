#ifndef ROLA_STLEX_THREAD_SAFE_LOOKUP_TABLE_HPP
#define ROLA_STLEX_THREAD_SAFE_LOOKUP_TABLE_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <list>
#include <utility>

namespace rola
{
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class Thread_safe_lookup_table
    {
    private:
        class Bucket_type
        {
        private:
            typedef std::pair<Key, Value> bucket_value;
            typedef std::list<bucket_value> bucket_data;
            typedef typename bucket_data::iterator bucket_iterator;

            bucket_data data;
            std::mutex mutex;

            bucket_iterator find_entry_for(Key const& key) const
            {
                return std::find_if(data.begin(), data.end(),
                    [&](bucket_value const& item)
                    {return item.first == key; });
            }
        public:
            Value value_for(Key const& key, Value const& default_value) const
            {
                std::lock_guard<std::mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                return (found_entry == data.end()) ?
                    default_value : found_entry->second;
            }

            void add_or_update_mapping(Key const& key, Value const& value)
            {
                std::lock_guard<std::mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry == data.end())
                {
                    data.push_back(bucket_value(key, value));
                }
                else
                {
                    found_entry->second = value;
                }
            }

            void remove_mapping(Key const& key)
            {
                std::lock_guard<std::mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry != data.end())
                {
                    data.erase(found_entry);
                }
            }
        };

        std::vector<std::unique_ptr<Bucket_type>> buckets;
        Hash hasher;

        Bucket_type& get_bucket(Key const& key) const
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }

    public:
        typedef Key key_type;
        typedef Value mapped_type;
        typedef Hash hash_type;

        Thread_safe_lookup_table(
            unsigned num_buckets = 19, Hash const& hasher_ = Hash()) :
            buckets(num_buckets), hasher(hasher_)
        {
            for (unsigned i = 0; i < num_buckets; ++i)
            {
                buckets[i].reset(new Bucket_type);
            }
        }

        Thread_safe_lookup_table(Thread_safe_lookup_table const& other) = delete;
        Thread_safe_lookup_table& operator=(
            Thread_safe_lookup_table const& other) = delete;

        Value value_for(Key const& key,
            Value const& default_value = Value()) const
        {
            return get_bucket(key).value_for(key, default_value);
        }

        void add_or_update_mapping(Key const& key, Value const& value)
        {
            get_bucket(key).add_or_update_mapping(key, value);
        }

        void remove_mapping(Key const& key)
        {
            get_bucket(key).remove_mapping(key);
        }
    };
} // namespace rola

#endif // !ROLA_STLEX_THREAD_SAFE_LOOKUP_TABLE_HPP
