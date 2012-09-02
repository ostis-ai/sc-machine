#include "sc_element.h"
#include "sc_defines.h"
#include "sc_types.h"
#include "sc_segment.h"

void sc_element_set_type(sc_element *element,
                         sc_type type)
{
    g_assert(element != 0);
    element->type = type;
}



