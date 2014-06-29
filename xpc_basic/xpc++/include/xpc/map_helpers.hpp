#if ! defined XPC_MAP_HELPERS_HPP
#define XPC_MAP_HELPERS_HPP

/******************************************************************************
 * map_helpers.hpp
 *------------------------------------------------------------------------*//**
 *
 * \file          map_helpers.hpp
 * \library       xpc++
 * \author        Chris Ahlstrom
 * \updates       2010-09-27 to 2013-07-19
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::map_helpers class.
 *
 *    This class provides helper functions for std::map containers.
 *
 *    We've tended to create whole wrapper classes to make our containers
 *    easier to use [e.g by providing iterator types], but this module
 *    provides a way  to avoid creating such a class.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* XPC_REVISION_DECL                   */
#include <map>                         /* std::map                            */
XPC_REVISION_DECL(map_helpers)

namespace xpc
{

/******************************************************************************
 * insert(container, value)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated insert of a value into
 *    a container.
 *
 *    The container is an std::map<KEY, VALUE> object.
 *
 * Template parameters:
 *
 *    -  KEY.
 *       The key class for the container.
 *    -  VALUE.
 *       This object must provide a key() member function that returns a KEY
 *       object.
 *
 * \param container
 *    The std::map into which the value will be inserted.
 *
 * \param value
 *    The value to be inserted.  It must provide a key() function that
 *    returns a KEY object.
 *
 * \return
 *    Return's true if the insertion succeeded, meaning something was
 *    actually inserted.  Usually, if insertion fails, it means the object
 *    was already present in the container.  If the size was 0, then there's
 *    something screwy going on.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
insert
(
   std::map<KEY, VALUE> & container,
   const VALUE & value
)
{
   KEY key = value.key();
   std::pair<std::map<KEY, VALUE>::iterator, bool> result =
      container.insert(std::make_pair(key, value));

   return result.second;
}

/******************************************************************************
 * insert(container, key, value)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated insert of a value into
 *    a container.
 *
 *    See the other insert() function.  For this version, the VALUE object
 *    does not need to provide a key() function.
 *
 * Template parameters:
 *
 *    -  KEY.
 *       The key class for the container.
 *    -  VALUE.
 *       This object must provide a key() member function that returns a KEY
 *       object.
 *
 * \param container
 *    The std::map into which the value will be inserted.
 *
 * \param value
 *    The value to be inserted.  It must provide a key() function that
 *    returns a KEY object.
 *
 * \return
 *    Return's true if the insertion succeeded, meaning something was
 *    actually inserted.  Usually, if insertion fails, it means the object
 *    was already present in the container.  If the size was 0, then there's
 *    something screwy going on.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
insert
(
   std::map<KEY, VALUE> & container,
   const KEY & key,
   const VALUE & value
)
{
   std::pair<std::map<KEY, VALUE>::iterator, bool> result =
      container.insert(std::make_pair(key, value));

   return result.second;
}

/******************************************************************************
 * erase(container, value)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated erasure of a value
 *    from a container.
 *
 *    See the insert() function for more information.
 *
 * \param container
 *    The std::map from which the value having the given key will be erased.
 *
 * \param value
 *    The value to be erased.  It must provide a key() function that
 *    returns a KEY object.
 *
 * \return
 *    Return's true if the erasure succeeded, meaning something was actually
 *    erased.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
erase
(
   std::map<KEY, VALUE> & container,
   const VALUE & value
)
{
   KEY key = value.key();
   std::size_type s = container.size();
   std::size_type s2 = container.erase(key);
   return s2 == s - 1;
}

/******************************************************************************
 * erase(container, key)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated erasure of a value
 *    from a container.
 *
 *    See the insert() function for more information.
 *
 * \param container
 *    The std::map from which the value having the given key will be erased.
 *
 * \param key
 *    The key to be erased.
 *
 * \return
 *    Return's true if the erasure succeeded, meaning something was actually
 *    erased.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
erase
(
   std::map<KEY, VALUE> & container,
   const KEY & key
)
{
   std::size_type s = container.size();
   std::size_type s2 = container.erase(key);
   return s2 == s - 1;
}

/******************************************************************************
 * replace(container, value)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated replacement of a value
 *    in a container.
 *
 *    This function first does an erase(), than does an insert().
 *
 * \note
 *    We could have called the insert(container, value) function, but
 *    calling the insert(container, key, value) function saves a call to the
 *    key() function.
 *
 * \param container
 *    The std::map in which the value having the given key will be replaced.
 *
 * \param value
 *    The value to be erased.  It must provide a key() function that
 *    returns a KEY object.
 *
 * \return
 *    Return's true if the erasure succeeded, meaning something was actually
 *    erased.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
replace
(
   std::map<KEY, VALUE> & container,
   const VALUE & value
)
{
   KEY key = value.key();
   (void) container.erase(key);
   return insert(container, key, value);
}

/******************************************************************************
 * replace(container, key, value)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a free function to perform a validated replacement of a value
 *    in a container.
 *
 *    This function first does an erase(), than does an insert().
 *
 * \note
 *    We could have called the insert(container, value) function, but
 *    calling the insert(container, key, value) function saves a call to the
 *    key() function.
 *
 * \param container
 *    The std::map in which the value having the given key will be replaced.
 *
 * \param key
 *    The key to be erased and then replaced.
 *
 * \param value
 *    The value to be replaced.  It need not provide a key() function.
 *
 * \return
 *    Return's true if the replacement succeeded, meaning something was
 *    inserted.  This provides no guarantee that the object was initially
 *    erased -- it simply may not have been present in the container.
 *
 *//*-------------------------------------------------------------------------*/

template <typename KEY, class VALUE>
bool
replace
(
   std::map<KEY, VALUE> & container,
   const KEY & key,
   const VALUE & value
)
{
   (void) container.erase(key);
   return insert(container, key, value);
}

}              // namespace xpc

#endif         // XPC_MAP_HELPERS_HPP

/******************************************************************************
 * map_helpers.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
