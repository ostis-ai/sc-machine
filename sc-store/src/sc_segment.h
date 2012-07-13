#ifndef _sc_segment_h_
#define _sc_segment_h_

#include "sc_types.h"
#include "sc_defines.h"
#include "sc_element.h"
#include <glib.h>

/*! Structure for segment storing
 */
//typedef struct _sc_array sc_array;
struct _sc_segment
{
  sc_element elements[SEGMENT_SIZE];
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  guint empty_slot_buff[SEGMENT_EMPTY_BUFFER_SIZE]; // works like a stack
  guint empty_slot_buff_head;
  gboolean have_empty_slots;
#else
  guint empty_slot; // index empty slot in segment
#endif
  sc_addr_seg num; // number of this segment in memory
};

/*! Create new segment with specified size.
 * @param num Number of created intance in sc-memory
 */
sc_segment* sc_segment_new(sc_addr_seg num);

void sc_segment_free(sc_segment *segment);

/*! Check if there are any empty slots in segment
 *
 * @param segment Pointer to segment for checking.
 * @return TRUE if there are any empty slots, else - return FALSE.
 */
gboolean sc_segment_have_empty_slot(sc_segment *segment);

/*! Append element into segment at first empty position.
 * @param segment Pointer to segment, that will be contains element
 * @param element Pointer to sc-element data (will be just copied)
 * @param offset Pointer that used to return offset in segment for appended element
 * @return Return pointer to created sc-element data. If element wasn't append into segment, then return 0.
 */
sc_element* sc_segment_append_element(sc_segment *segment,
				      sc_element *element,
				      guint16 *offset);

/*! Get sc-element pointer by id
 * @param seg Pointer to segment where we need to get element
 * @param id sc-element id in segment
 * @return Pointer to sc-element with specified id
 */
sc_element* sc_segment_get_element(sc_segment *seg, guint id);

/*! Remove element from specified segment
 */
void sc_segment_remove_element(sc_segment *segment,
			       guint el_id);

//! Update information in segment about first empty slot
void sc_segment_update_empty_slot(sc_segment *segment);

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
void sc_segment_update_empty_slot_buffer(sc_segment *segment);
#else
void sc_segment_update_empty_slot_value(sc_segment *segment);
#endif


#endif
