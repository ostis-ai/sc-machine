#ifndef _sc_common_utils_h_
#define _sc_common_utils_h_

#include "sc_common_prerequest.h"
#include <glib.h>

#define resolve_keynode(__ctx, __keynode) \
    if (sc_helper_resolve_system_identifier(__ctx, str_##__keynode, &__keynode) == SC_FALSE) \
    {\
        g_warning("Can't find element with system identifier: %s", str_##__keynode); \
        __keynode = sc_memory_node_new(__ctx, 0); \
        if (sc_helper_set_system_identifier(__ctx, __keynode, str_##__keynode, strlen(str_##__keynode)) != SC_RESULT_OK) \
            return SC_RESULT_ERROR; \
        g_message("Created element with system identifier: %s", str_##__keynode); \
    }else \
        g_message("Found keynode: %s (%d, %d)", str_##__keynode, __keynode.seg, __keynode.offset);

#endif
