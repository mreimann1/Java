// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// listmap::print()
//
template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t,mapped_t,less_t>::print() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));

   // Base Case: list is empty
   if (empty()) return;

   for (auto it=begin(); it!=end(); ++it) {
      cout << it->first << " = " << it->second << endl;
   }
   return;
}

//
// listmap::print_by_val()
//
template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t,mapped_t,less_t>::print_by_val(mapped_t val) {
   DEBUGF ('l', reinterpret_cast<const void*> (this));

   // Base Case: list is empty
   if (empty()) return;

   for (auto it=begin(); it!=end(); ++it) {
      if (it->second == val)
         cout << it->first << " = " << it->second << endl;
   }
   return;
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);

   node* new_node;

   // Base case: List empty
   if (empty()) {
      // Create a new node with prev=anchor, next=anchor->prev, value=pair
      //                                     anchor->prev bc circle
      new_node = new node (anchor(), anchor()->prev, pair);

      anchor()->prev = new_node;
      anchor()->next = new_node;
      return iterator(new_node);
   }
   
   // Loop through items and insert lexographically

   for (auto it = begin(); it!=end(); ++it) {
      // Determine if new node is less than current node
      bool is_less = less(pair.first, it->first);
      bool is_greater = less(it->first, pair.first);
      bool is_equal = not is_less and not is_greater;
      if (is_less) {
         // insert the node before this node
         new_node = new node (it.where, it.where->prev, pair);

         it.where->prev->next = new_node;
         it.where->prev = new_node;

         return iterator(new_node);
      }
      if (is_equal) {
         // Change this nodes mapped value
         it->second = pair.second;
         return it; 
      }
      // else continue
   }

   // insert the node at end
   new_node = new node(anchor(), anchor()->prev, pair);
   anchor()->prev->next = new_node;
   anchor()->prev = new_node;

   return iterator(new_node);
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);

   // Base Case: empty list
   if (empty()) return end();

   for(auto it=begin(); it!=end(); ++it)
      if(it->first==that) return it;

   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);

   // Base Case: position points to the anchor
   if (position == end()) return position;

   // Set the items neighbors to point past to each other
   position.where->prev->next = position.where->next;
   position.where->next->prev = position.where->prev;
   auto result = iterator(position.where->next);

   // Set the items pointers to null
   position.where->prev = NULL;
   position.where->next = NULL;

   free(position.where);

   return result;
}


