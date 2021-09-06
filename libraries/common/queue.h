/*
 * common/queue.h
 *
 *  Created on: Dec 21, 2020
 *      Author: n00b
 * 
 * Thanks to Einar Arnason
 * email: einsiarna@gmail.com
 * https://github.com/EinarArnason/ArduinoQueue
 * 
 */

#pragma once

#ifndef SOURCE_QUEUE_H_
#define SOURCE_QUEUE_H_

#if defined (__linux__)
    #include <deque>
    #include <mutex>
    #include <condition_variable>
#endif

#include "utils.h"

template<typename OBJ>
struct is_pointer { static const bool value = false; };

template<typename OBJ>
struct is_pointer<OBJ*> { static const bool value = true; };

/*
 * use upper struct to test if OBJ is a pointer with:
 * if (!is_pointer<OBJ>::value) { error_os("ObjectQueue: OBJ is not a pointer!"); }
 */

inline void throw_queue_error(const char * name)
{
    error_mem("queue:%s: empty queue", name);
}

namespace utils
{
    template <typename OBJ> class Queue
    {

    protected:
        
        #if defined(ARDUINO)
            class Node
            {
            public:

                Node()  {};
                ~Node() { next = nullptr; }

                OBJ         item;
                Node    *   next=nullptr;
            };

            Node    *   head=nullptr;
            Node    *   tail=nullptr;
            
            iSize_t     _count = 0;

        #else
        std::mutex              _queue_mutex;
        std::mutex              _block_mutex;
        std::deque<OBJ>         _queue;
        std::condition_variable _cvblock;
        #endif

    public:

        Queue()                   = default;
        Queue(const Queue<OBJ>&)  = delete;   
        virtual ~Queue() { clear(); _cvblock.notify_one(); };

        
    public:
        
        OBJ         front           (void)
        {
            if (is_empty())
            {
                throw_queue_error("front");
//                 if (is_pointer<OBJ>::value) return nullptr;
                return OBJ();
            }
            #if defined(ARDUINO)
                OBJ item = head->item;
                return item;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                return _queue.front();
            #endif
        }

        OBJ         back            (void)
        {
            if (is_empty())
            {
                throw_queue_error("back");
//                 if (is_pointer<OBJ>::value) return nullptr;
                return OBJ();
            }
            #if defined(ARDUINO)
                OBJ item = tail->item;
                return item;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                return _queue.back();
            #endif
        }
			
        // Removes and returns item from front of Queue
        OBJ         pop_front       (void)
        {
            if (is_empty())
            {
                throw_queue_error("pop_front");
                return OBJ();
            }

            #if defined(ARDUINO)

                Node * node = head;
                head = node->next;
                OBJ item = node->item;
                delete node;
                node = nullptr;

                if (head == nullptr) { tail = nullptr; }

                _count--;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                auto item = std::move(_queue.front());
                _queue.pop_front();
            #endif
            return item;
        }

        OBJ         pop_back        (void)
        {
            if (is_empty())
            { 
                throw_queue_error("pop_back");
                return OBJ();
            }
            #if defined(ARDUINO)
                
                Node * iter = head;
                Node * node = tail;
                OBJ item = node->item;
                
                if (node != iter)
                {
                    while (iter->next != node)
                    {
                        if (iter->next == nullptr)
                        {
                            error_mem("queue:pop_back: could not find pointer before last!");
                            return OBJ();
                        }
                        
                        iter = iter->next;
                    }
                    tail = iter;
                }
                else
                {
                    head = nullptr;
                    tail = nullptr;
                }
                delete node;
                _count--;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                auto item = std::move(_queue.back());
                _queue.pop_back();
            #endif
            return item;
        }
        
        bool        push_front      (const OBJ& item)
        {
            #if defined(ARDUINO)
                if (!utils::board::can_alloc(sizeof(OBJ)))
                {
                    return false;
                }

                Node * node = new Node();
                
                if (node == nullptr)
                {
                    error_mem("queue:push_front: failed allocating node [%u]", _count);
                    return false;
                }
                
                node->item=item;
                
                if (head == nullptr)    { head = node; }
                else
                {
                    node->next = head;
                    head = node;
                }
                _count++;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                _queue.emplace_front(std::move(item));

                std::unique_lock<std::mutex> ul(_block_mutex);
                _cvblock.notify_one();
            #endif
            return true;
        }     
        
        bool        push_back       (const OBJ& item)
        {
            #if defined(ARDUINO)
                if (!utils::board::can_alloc(sizeof(OBJ)))
                {
                    return false;
                }
                
                Node* node = new Node();
                
                if (node == nullptr)
                {
                    error_mem("queue:push_back: failed allocating node [%u]", _count);
                    return false;
                }

                node->item=item;
                if (head == nullptr)    { head = node; }
                else                    { tail->next = node; }

                tail = node;
                _count++;
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                _queue.emplace_back(std::move(item));

                std::unique_lock<std::mutex> ul(_block_mutex);
                _cvblock.notify_one();
            #endif
            return true;
        }
        
        
        bool        is_empty        (void)
        {
            #if defined(ARDUINO)
                return ((_count == 0) || (head == nullptr));
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                return _queue.empty();
            #endif
        }

        iSize_t     count           (void)
        {
            #if defined(ARDUINO)
                return (_count);
            #elif defined(__linux__)
                #if defined (INDOORINO_SERVER)
                std::scoped_lock lock(_queue_mutex);
                #endif
                return iSize_t(_queue.size());
            #endif
        }

        void        clear           (void)
        {
            debug_mem("Clearing Queue [size = %u]", count());
        #if defined(ARDUINO)
            _count=0;
            for (Node* node = head; node != nullptr; node = head)
            {
                head = node->next;
                delete node;
            }
        #elif defined(__linux__)
            #if defined (INDOORINO_SERVER)
            std::scoped_lock lock(_queue_mutex);
            #endif
            _queue.clear();
        #endif
        }
        
        
        void        wait()
        {
            #if defined(__linux__)
            while (is_empty())
            {
                std::unique_lock<std::mutex> ul(_block_mutex);
                _cvblock.wait(ul);
            }
            #endif
        }

    };

    
    /* This class must be used with pointers */
    template <typename OBJ> class ObjectQueue : public Queue<OBJ*>
    {
    public:
        ObjectQueue() {}
        
        OBJ     *   front           (void)
        {
            if (this->is_empty()) { return nullptr; }
            return Queue<OBJ*>::front();
        }

        OBJ     *   back            (void)
        {
            if (this->is_empty()) { return nullptr; }
            return Queue<OBJ*>::back();
        }
		
        OBJ     *   pop_front       (void)
        {
            if (this->is_empty()) { return nullptr; }
            return Queue<OBJ*>::pop_front();
            /* WARNING remember to delete pointer after usage */
        }

        OBJ     *   pop_back        (void)
        {
            if (this->is_empty()) { return nullptr; }
            return Queue<OBJ*>::pop_back();
            /* WARNING remember to delete pointer after usage */
        }
		
        bool        push_front      (OBJ* pointer)
        {
            return Queue<OBJ*>::push_front(pointer);
        }     
        
        bool        push_back       (OBJ* pointer)
        {
            return Queue<OBJ*>::push_back(pointer);
        }

        void        clear           (void)
        {
            iSize_t size = Queue<OBJ*>::count();
            debug_mem("Clearing ObjectQueue");
            for (iSize_t i=0; i<size; i++)
            {
                OBJ * p = Queue<OBJ*>::pop_front();
                if (p != nullptr) { delete(p); }
            }
            Queue<OBJ*>::clear();
        }
        
    };

}


#endif /* SOURCE_QUEUE_H_ */












//
//     ArduinoQueue(unsigned int maxItems = (unsigned int)(-1) ,unsigned int maxMemory = (unsigned int)(-1))
//     {
//         this->head = nullptr;
//         this->tail = nullptr;
//         this->count = 0;
//         this->maxMemory = maxMemory;
//         this->maxItems = maxMemory / sizeof(Node);
// 
//         if (maxItems != 0 && this->maxItems > maxItems) { this->maxItems = maxItems; }
//     }
// 
//     ~ArduinoQueue()
//     {
//         for (Node* node = head; node != nullptr; node = head)
//         {
//             head = node->next;
//             delete node;
//         }
//     }
// 
//   /*
//     Push an item to the queue.
//     Returns false if memory is
//     full, or true if the item
//     was added to queue.
//   */
//     bool enqueue(OBJ item)
//     {
//         if (count == maxItems)
//         { 
//             error_mem("queue:enqueue: too many items [%u]", maxItems);
//             return false;
//         }
// 
//         Node* node = new Node;
//         if (node == nullptr)
//         {
//             error_mem("queue:enqueue: failed allocating new node", maxItems);
//             return false;
//         }
// 
//         node->item = item;
// 
//         if (head == nullptr)
//         {
//             head = node;
//             tail = node;
//             count++;
//             return true;
//         }
// 
//         tail->next = node;
//         tail = node;
//         count++;
// 
//         return true;
//     }
// 
//     /*  Pop the front of the queue. */
//     OBJ dequeue()
//     {
//         if ((count == 0) || (head == nullptr))
//         { 
//             error_mem("queue:dequeue: not items to pop", maxItems);
//             return nullptr;
//         }
// 
//         Node* node = head;
//         head = node->next;
//         OBJ item = node->item;
//         delete node;
//         node = nullptr;
// 
//         if (head == nullptr) {
//         tail = nullptr;
//         }
// 
//         count--;
//         return item;
//     }
// 
//     /* Returns true if the queue is empty, false otherwise. */
//     bool isEmpty() { return head == nullptr; }
// 
//     /* Returns true if the queue is full, false otherwise. */
//     bool isFull() { return count == maxItems; }
// 
//     /* Returns the number of items currently in the queue. */
//     unsigned int itemCount() { return count; }
// 
//     /* Returns the size of the queue item in bytes. */
//     unsigned int itemSize() { return sizeof(Node); }
// 
//     /* Returns the size of the queue (maximum number of items) */
//     unsigned int maxQueueSize() { return maxItems; }
// 
//     /* Returns the size of the queue (maximum size in bytes) */
//     unsigned int maxMemorySize() { return maxMemory; }
// 
//   /* Get the item in the front of the queue. */
//     OBJ getHead()
//     {
//         if ((count == 0) || (head == nullptr))
//         {
//             error_mem("queue:enqueue failed allocating new node", maxItems);
//             return OBJ();
//         }
// 
//         OBJ item = head->item;
//         return item;
//     }
// 
//   /* Get the item in the back of the queue. */
//     OBJ getTail()
//     {
//         if ((count == 0) || (head == nullptr))
//         {
//             return OBJ();
//         }
// 
//     OBJ item = tail->item;
//     return item;
//     }
// 
//     OBJ* getHeadPtr()
//     {
//         if ((count == 0) || (head == nullptr)) {
//             return nullptr;
//         }
// 
//         return &(head->item);
//     }
// 
//     OBJ* getTailPtr()
//     {
//         if ((count == 0) || (head == nullptr)) {
//             return nullptr;
//         }
// 
//         return &(tail->item);
//     }
// 
// };






























/*
	MMO Client/Server Framework using ASIO
	"Happy Birthday Mrs Javidx9!" - javidx9

	Videos:
	Part #1: https://youtu.be/2hNdkYInj4g
	Part #2: https://youtu.be/UbjxGvrDrbw

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2020 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, �OneLoneCoder 2019, 2020


#pragma once

#include "net_common.h"

namespace olc
{
	namespace net
	{
		template<typename OBJ>
		class tsqueue
		{
		public:
			tsqueue() = default;
			tsqueue(const tsqueue<OBJ>&) = delete;
			virtual ~tsqueue() { clear(); }

		public:
			// Returns and maintains item at front of Queue
			const OBJ& front()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.front();
			}

			// Returns and maintains item at back of Queue
			const OBJ& back()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.back();
			}

			// Removes and returns item from front of Queue
			OBJ pop_front()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}

			// Removes and returns item from back of Queue
			OBJ pop_back()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.back());
				deqQueue.pop_back();
				return t;
			}

			// Adds an item to back of Queue
			void push_back(const OBJ& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_back(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Adds an item to front of Queue
			void push_front(const OBJ& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_front(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Returns true if Queue has no items
			bool empty()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.empty();
			}

			// Returns number of items in Queue
			size_t count()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.size();
			}

			// Clears Queue
			void clear()
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.clear();
			}

			void wait()
			{
				while (empty())
				{
					std::unique_lock<std::mutex> ul(muxBlocking);
					cvBlocking.wait(ul);
				}
			}

		protected:
			std::mutex muxQueue;
			std::deque<OBJ> deqQueue;
			std::condition_variable cvBlocking;
			std::mutex muxBlocking;
		};
	}
}
*/
