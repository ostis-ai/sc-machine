#include "sc_config.h"
#include "sc_types.h"
#include "sc_element.h"


//! sc-iterator types
typedef enum
{
  sc_iterator3_f_a_a = 0, // output arcs
  sc_iterator3_a_a_f,     // input arcs
  sc_iterator3_f_a_f      // arcs between two nodes
} sc_iterator_type;

/*! Iterator parameter
 */
struct _sc_iterator_param
{
  gboolean is_type;
  union
  {
    sc_uri uri;
    sc_type type;
  };
};

/*! Structure to store iterator information
 */
struct _sc_iterator
{
  sc_iterator_type type; // iterator type (search template)
  guint params_count; // number of parameters
  sc_iterator_param *params; // pointer to parameters array
  sc_uri *results;
#if USE_PARALLEL_SEARCH
  guint time_stamp;
#endif
};

/*! Create new sc-iterator-3
 * @param type Iterator type (search template)
 * @param p1 First iterator parameter
 * @param p2 Second iterator parameter
 * @param p3 Third iterator parameter
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a sc-iterator-3, then return 0 
 */
sc_iterator* sc_iterator3_new(sc_iterator_type type, sc_iterator_param p1, sc_iterator_param p2, sc_iterator_param p3);

/*! Destroy iterator and free allocated memory
 * @param it Pointer to sc-iterator that need to be destroyed
 */
void sc_iterator_free(sc_iterator *it);

/*! Go to next iterator result
 * @param it Pointer to iterator that we need to go next result
 * @return Return TRUE, if iterator moved to new results; otherwise return FALSE.
 * example: while(sc_iterator_next(it)) { <your code> }
 */
gboolean sc_iterator_next(sc_iterator *it);

/*! Get iterator value
 * @param it Pointer to iterator for getting value
 * @param vid Value id (can't be more that 3 for sc-iterator3)
 * @return Return sc-uri of search result value
 */
sc_uri sc_iterator_value(sc_iterator *it, guint vid);
