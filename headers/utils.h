#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <string>
#include <vector>

enum class LOG_LEVEL{ DEBUG, WARNING, ERROR, INFO};
void logger(const std::string& msg, LOG_LEVEL level = LOG_LEVEL::DEBUG){
    if(level == LOG_LEVEL::DEBUG){
        std::cout << "DEBUG::" << msg << "\n";
    }
    else if(level == LOG_LEVEL::WARNING){
        std::cout << "WARNING::" << msg << "\n";
    }
    else if(level == LOG_LEVEL::ERROR){
        std::cout << "ERROR::" << msg << "\n";
    }
    else if(level == LOG_LEVEL::INFO){
        std::cout << "INFO::" << msg << "\n";
    }

}

template<typename K, typename V>
class MinHeap{
    private:
        
        std::vector<std::pair<K, V>> data;
        int size;
        std::unordered_map<int, int> m;

        void heapify_up(int index){
            int par = (index-1)/2;
            int curr = index;
            std::pair<K, V> ori = data[index];
            while(data[par].second > ori.second && curr != 0){
            
                data[curr] = data[par];
                m[data[par].first] = curr;
                curr = par;
                par = (curr-1)/2;
            
            }
            m[ori.first] = curr;
            data[curr] = ori;
        }
        void swap(int a, int b){
            std::pair<K, V> t = data[a];
            data[a] = data[b];
            data[b] = t;
        }

        void heapify_down(int index){
            int l = index*2+1, r=index*2+2;
            int smal = index;
            if(l < size && data[l].second < data[smal].second)
                smal = l;
            if(r < size && data[r].second < data[smal].second)
                smal = r;
            if(index != smal){
                m[data[index].first] = smal;
                m[data[smal].first] = index;
                swap(index, smal);
                heapify_down(smal);
            }
        }
    public:
        MinHeap():size(0), data(10){
        }

        void insert(K k, V v){
            if(data.size() > size)
                data[size] = {k, v};
            else data.push_back({k, v});
            heapify_up(size++);
        }
        std::pair<K, V> getMin(){
            if(size == 0){
                std::cout << "HEAP EMPTY\n";
                return std::pair<K, V>();
            }
            return data[0];
        }
        void pop(){
            if(size == 0) return;
            swap(0, size-1);
            size--;
            if(size > 0)
                heapify_down(0);
        }

        void decrease_key(int key, int val){
            if(m.count(key) == 0) return;
            data[m[key]].second = val;
            heapify_up(m[key]);
        }
        V get_val(int key){
            if(m.count(key) == 0){
                std::cout << "KEY NOT FOUND\n";
                return V();
            }
            return data[m[key]].second;
        }
        bool empty(){
            return size==0;
        }

};

#endif