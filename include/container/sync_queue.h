// =====================================================================================
//  Copyright (C) 2024 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  sync_queue.h
//  作    者:  Jiaxing Shao, 980853650@qq.com
//  创建时间:  2024-05-24 17:28:07
//  描    述:
// =====================================================================================

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

namespace kgr {
    namespace container {

        /**
         * 线程安全的同步队列
         */
        template <typename T>
        class SynchronizedQueue
        {
            typedef std::deque<T> self_type;

        public:
            SynchronizedQueue(){};
            virtual ~SynchronizedQueue(){};

        public:
            /**
             * 放入队列
             * @param data 入队列数据
             */
            void push_back(const T &data)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push_back(data);
                m_cond.notify_one();
            }

            /**
             * 出队列, 阻塞读取
             * @param result 数据
             * @return 读取结果
             */
            bool pop_front(T &result)
            {
                return pop_front(result, -1);
            }

            /**
             * 出队列
             * @param result 数据
             * @param waitMs 等待毫秒数(-1为阻塞)
             * @return 读取结果
             */
            bool pop_front(T &result, int waitMs)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_queue.empty()) {
                    if (waitMs > 0) {
                        m_cond.wait_for(lock, std::chrono::milliseconds(waitMs));
                    } else if (waitMs == 0) {
                        return false;
                    } else {
                        m_cond.wait(lock);
                    }
                }

                if (m_queue.empty()) {
                    return false;
                }
                result = m_queue.front();
                m_queue.pop_front();
                return true;
            }

            /**
             * 清空队列
             */
            void clear()
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                while (!m_queue.empty()) {
                    m_queue.pop_front();
                }
            }

            /**
             * 所有出队操作退出阻塞状态
             */
            void loopbreak()
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_cond.notify_all();
            }

            /**
             * 获取队列大小
             * @return 队列大小
             */
            unsigned int size() const
            {
                return static_cast<unsigned int>(m_queue.size());
            }

            /**
             * 队列是否为空
             * @return true/false
             */
            bool empty() const
            {
                return m_queue.empty();
            }

        private:
            std::deque<T>           m_queue;
            mutable std::mutex      m_mutex;
            std::condition_variable m_cond;
        };
    } // namespace container
} // namespace kgr