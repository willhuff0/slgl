#pragma once

#include <vector>

#define OBSERVER_PATTERN(name, ...) \
template<typename... TData> \
class T##name##Subject { \
public: \
    class Observer { \
    public: \
        virtual ~Observer() = default; \
        virtual void on##name##Changed(const TData&... data) = 0; \
    }; \
\
    void Add##name##Observer(Observer* observer) { \
        observers.push_back(observer); \
    } \
    void Remove##name##Observer(Observer* observer) { \
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end()); \
    } \
\
protected: \
    void notify##name##Changed(const TData&... data) { \
        for(Observer* observer : observers) { \
            observer->on##name##Changed(data...); \
        } \
    } \
\
private: \
    std::vector<Observer*> observers; \
}; \
using name##Subject = T##name##Subject<__VA_ARGS__>;