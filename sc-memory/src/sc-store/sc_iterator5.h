#ifndef SC_ITERATOR5_H
#define SC_ITERATOR5_H
#include "system.h"

//! sc-iterator5 types
typedef enum
{
    sc_iterator5_f_a_a_a_f = 0,
    sc_iterator5_a_a_f_a_f,
    sc_iterator5_f_a_f_a_f,
    sc_iterator5_f_a_f_a_a,
    sc_iterator5_f_a_a_a_a,
    sc_iterator5_a_a_f_a_a
} sc_iterator5_type;

/*! Structure to store iterator information
 */
struct _sc_iterator5
{
    sc_iterator5_type type; // iterator type (search template)
    sc_iterator_param params[5]; // parameters array
    sc_addr results[5]; // results array (same size as params)
    sc_iterator3* it_main; //iterator for main cycle
    sc_iterator3* it_attr; //iterator for attribute cycle
};

typedef struct _sc_iterator5 sc_iterator5;

sc_iterator5* sc_iterator5_a_a_f_a_f_new(sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5);
sc_iterator5* sc_iterator5_f_a_a_a_f_new(sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_addr p5);
sc_iterator5* sc_iterator5_f_a_f_a_f_new(sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5);
sc_iterator5* sc_iterator5_f_a_f_a_a_new(sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5);
sc_iterator5* sc_iterator5_f_a_a_a_a_new(sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_type p5);
sc_iterator5* sc_iterator5_a_a_f_a_a_new(sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5);
sc_bool sc_iterator5_a_a_f_a_f_next(sc_iterator5 *it);
sc_bool sc_iterator5_f_a_a_a_f_next(sc_iterator5 *it);
sc_bool sc_iterator5_f_a_f_a_f_next(sc_iterator5 *it);
sc_bool sc_iterator5_f_a_f_a_a_next(sc_iterator5 *it);
sc_bool sc_iterator5_f_a_a_a_a_next(sc_iterator5 *it);
sc_bool sc_iterator5_a_a_f_a_a_next(sc_iterator5 *it);
void sc_iterator5_free(sc_iterator5 *it);

#endif // SC_ITERATOR5_H
