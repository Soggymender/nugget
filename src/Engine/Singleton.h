#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class NSingleton {
public:

    static T& Instance() {
        static T m_instance; // created on first use
        return m_instance;
    }

    // No copying
    NSingleton(const NSingleton&) = delete;
    NSingleton& operator=(const NSingleton&) = delete;

protected:
  
    NSingleton() = default;
   ~NSingleton() = default;
};

#endif