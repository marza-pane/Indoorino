/*
 * common/list.h
 *
 *  Created on: Dec 22, 2020
 *      Author: n00b
 * 
 */

#pragma once

#ifndef SOURCE_LIST_H_
#define SOURCE_LIST_H_

#include "utils.h"

inline void throw_list_indexerror(const char * name, iSize_t index, iSize_t size)
{
    warning_mem("list:%s: index %u out of range %u", name, index, size);
}

namespace utils
{

    template <typename OBJ> class List
    {
    private:
        
    #if defined(ARDUINO)
        class Node
        {
        public:

            Node()  { next = nullptr; }
            ~Node() { next = nullptr; }

            OBJ         item;
            Node    *   next;
        };
        
        Node    *   head=nullptr;    
        iSize_t     _count = 0;
    #else
        std::vector<OBJ> _list;
    #endif

    protected:
        iSize_t zero_index(int index)
        {
            if (index < 0)  { return count() + index; }
            else            { return index; }
        }
        bool    validate_index(iSize_t index)
        {
            if (index >= count())
            {
                warning_mem("list: index %u out of range 0:%u", index, MAX_INTEGER_M_(count() - 1, 0));
                return false;
            }
            return true;
        }

    public:
        List            ()  {};
        virtual ~List   ()  { clear(); }

        // Adds an item at the back of List
        bool        append      (const OBJ& item)
        {
        #if defined(ARDUINO)
            if (!utils::board::can_alloc(sizeof(OBJ)))
            {
                return false;
            }
            
            Node * node = new Node();
            
            if (node == nullptr)
            {
                error_mem("list:append: failed allocating node [%u]", _count);
                return false;
            }

            node->item=item;
            if (head == nullptr)    { head = node; }
            else                    
            {
                Node* iter=head;
                while (iter->next != nullptr)
                {
                    iter = iter->next;
                }
                iter->next = node;
            }
            _count++;
        #else
            _list.push_back(std::move(item));

        #endif
            return true;
        }
        
        // Adds an item at the top of List
        bool        push        (const OBJ& item)
        {
        #if defined(ARDUINO)
            if (!utils::board::can_alloc(sizeof(OBJ)))
            {
                return false;
            }

            Node * node = new Node();
            
            if (node == nullptr)
            {
                error_mem("list:push: failed allocating node [%u]", _count);
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
        #else
            _list.insert(_list.begin(), std::move(item));
        #endif
            return true;
        }

        /* TODO: implement negative indices */
        
        // Remove item from list
        bool        rem         (int index=0)
        {
            iSize_t i = zero_index(index);
            
            if (validate_index(i))
            {
                this->pop(i);
                return true;
            }
            return false;
        }
                
        // Remove item from list and returns it
        OBJ         pop         (int index=0)
        {
            iSize_t i = zero_index(index);
            
            if (validate_index(i))
            {
            #if defined(ARDUINO)
                Node * node=head;
                OBJ item;
                if (i == 0 || _count == 1 || head->next == nullptr)
                {
                    head=node->next;
                    item=node->item;
                    _count--;
                    delete node;
                }
                else
                {
                    
                    for (iSize_t n=0; n<i - 1; n++)
                    {
                        node=node->next;
                    }
                    Node * target = node->next->next;
                    item = node->next->item;
                    delete node->next;
                    node->next=target;
                    _count--;
                }

            #else
                auto item = std::move(_list[i]);
                _list.erase((_list.begin() + i));
            #endif
                return item;
            }
            else { return OBJ(); }        
        }
                
        OBJ         operator[]  (int index)
        {
            iSize_t i = zero_index(index);
            
            if (validate_index(i))
            {
            #if defined(ARDUINO)
                Node * node=head;
                for (iSize_t n=0; n<i; n++)
                {
                    node = node->next;
                }
                return node->item;
            #else
                return _list[i];
            #endif
            }
            else { return OBJ(); }
        }
                
        // Returns true if List has no items
        bool        is_empty        (void)
        {
            #if defined(ARDUINO)
                return ((_count == 0) || (head == nullptr));
            #else
                return _list.empty();
            #endif
        }

        // Returns number of items in List
        iSize_t     count           (void)
        {
        #if defined(ARDUINO)
            return (_count);
        #else
            return iSize_t(_list.size());
        #endif
        }

        // Clears List
        void        clear       (void)
        {
            debug_mem("Clearing List [size = %u]", count());
        #if defined(ARDUINO)
            _count=0;
            for (Node* node = head; node != nullptr; node = head)
            {
                head = node->next;
                delete node;
            }
        #else
            _list.clear();
        #endif
        }
        
    };
    
    
    /* This class must be used with pointers */
    template <typename OBJ> class ObjectList : public List<OBJ*>
    {
    public:
         ObjectList() {}
        
        // Clears ObjectList
        void        clear       (void)
        {
            iSize_t size = List<OBJ*>::count();
            debug_mem("Clearing ObjectList");
            for (iSize_t i=0; i<size; i++)
            {
                OBJ * p = List<OBJ*>::pop();
                if (p != nullptr) { delete(p); }
            }
            List<OBJ*>::clear();
        }
        
        // Adds an item at the back of List
        bool        append      (OBJ * pointer)
        {
            return List<OBJ*>::append(pointer);
        }
        
        // Adds an item at the top of List
        bool        push        (OBJ * pointer)
        {
            return List<OBJ*>::push(pointer);
        }
        
        bool        rem         (int index=0)
        {
            iSize_t i = List<OBJ*>::zero_index(index);

            if ( !( List<OBJ*>::validate_index(i) ) ) { return false; }
            
            OBJ * p = this->pop(i);
            
            if (p != nullptr) { delete p; return true; }
            return false;
        }
        
        OBJ     *   pop         (int index=0)
        {
            iSize_t i = List<OBJ*>::zero_index(index);
            if (!List<OBJ*>::validate_index(i)) { return nullptr; }
            return List<OBJ*>::pop(i);            
            /* WARNING remember to delete pointer after usage */
        }
        
        OBJ     *   operator[]  (int index)
        {
            iSize_t i = List<OBJ*>::zero_index(index);
            if (!List<OBJ*>::validate_index(i)) { return nullptr; }
            return List<OBJ*>::operator[](i);

        }

    };
    
    
    
    
}   /* namespace::utils */

#endif /* SOURCE_LIST_H_ */

