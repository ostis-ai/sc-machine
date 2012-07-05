#include "sc_iterator.h"

sc_iterator* sc_iterator3_new(sc_iterator_type type, sc_iterator_param p1, sc_iterator_param p2, sc_iterator_param p3)
{
  // check types
  if (type > sc_iterator3_f_a_f) return (sc_iterator*)0;

  // check params with template
  switch (type)
  {
  case sc_iterator3_f_a_a:
    if (p1.is_type || !p2.is_type || !p3.is_type) return (sc_iterator*)0;
    break;
  case sc_iterator3_a_a_f:
    if (!p1.is_type || !p2.is_type || p3.is_type) return (sc_iterator*)0;
    break;
  case sc_iterator3_f_a_f:
    if (p1.is_type || !p2.is_type || p3.is_type) return (sc_iterator*)0;
    break;
  };

  sc_iterator *it = g_new0(sc_iterator, 1);

  it->params_count = 3;
  it->params = g_new0(sc_iterator_param, 3);
  //  it->results = g_new0(sc_id, 3);

  return it;
}

void sc_iterator_free(sc_iterator *it)
{
  g_free(it->params);
  if (it->results != 0)
    g_free(it->results);

  g_free(it);
}

gboolean sc_iterator_param_compare(sc_element *el, sc_addr addr, sc_iterator_param param)
{
  g_assert(el != 0);

  if (param.is_type)
    return el->type & param.type;
  else
    return uri_eq(addr, param.addr);

  return FALSE;
}

gboolean sc_iterator_next(sc_iterator *it)
{
  g_assert(it != 0);
  if (it->results == 0)
  {
    
  }

  return FALSE;
}

sc_addr sc_iterator_value(sc_iterator *it, guint vid)
{
  g_assert(it != 0);
  g_assert(vid < it->params_count);
  g_assert(it->results != 0);

  return it->results[vid];
}
