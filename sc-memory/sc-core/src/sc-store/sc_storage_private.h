/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_storage_private_h_
#define _sc_storage_private_h_

#include "sc-store/sc-base/sc_monitor_table.h"

#include "sc-store/sc-event/sc_event_private.h"

#include "sc-store/sc_storage_dump_manager.h"


#include "sc-store/sc-base/sc_monitor_table_private.h"

#define SC_L1_CACHE_CAPACITY_FACTOR 0.5 // 50% от max_loaded_segments
#define SC_L2_CACHE_CAPACITY_FACTOR 1.0 // 100% от max_loaded_segments
#define SC_BUFFER_MAX_OPERATIONS 1000   // Размер буфера операций

typedef enum
{
  SC_OP_NODE_NEW,
  SC_OP_ARC_NEW,
  SC_OP_LINK_NEW,
  SC_OP_SET_LINK_CONTENT,
  SC_OP_ERASE_ELEMENT
} sc_operation_type;

typedef struct
{
  sc_operation_type type;
  sc_addr addr;           // Для узлов, ссылок, дуг
  sc_type type_mask;      // Для узлов и дуг
  sc_addr begin, end;     // Для дуг
  sc_char * content;      // Для содержимого ссылок (копия для логирования)
  sc_uint64 content_size; // Размер содержимого
} sc_operation;


typedef struct
{
  sc_operation * operations;
  sc_uint32 size;
  sc_uint32 capacity;
  sc_monitor monitor;
} sc_segment_buffer;


// Структура узла LRU-кэша
typedef struct _sc_lru_node
{
  sc_addr_seg segment_num; // Номер сегмента
  sc_segment * segment;    // Указатель на сегмент
  struct _sc_lru_node * prev;
  struct _sc_lru_node * next;
} sc_lru_node;

// Структура L1-кэша (DRAM)
typedef struct _sc_l1_cache
{
  sc_hash_table * segments; // Хэш-таблица: sc_addr_seg -> sc_lru_node
  sc_lru_node * head;       // Самый недавно использованный
  sc_lru_node * tail;       // Наименее недавно использованный
  sc_uint32 size;           // Текущий размер
  sc_uint32 capacity;       // Максимальный размер
  sc_monitor monitor;       // Монитор для синхронизации
} sc_l1_cache;

// Структура L2-кэша (SSD/NVMe)
typedef struct _sc_l2_cache
{
  sc_hash_table * segments; // Хэш-таблица: sc_addr_seg -> offset
  sc_char * cache_path;     // Путь к файлу кэша
  sc_uint32 size;           // Текущий размер
  sc_uint32 capacity;       // Максимальный размер
  sc_monitor monitor;       // Монитор для синхронизации
} sc_l2_cache;




struct _sc_storage
{
  sc_segment ** segments;
  sc_addr_seg segments_count;
  sc_addr_seg max_segments_count;
  sc_addr_seg last_not_engaged_segment_num;
  sc_addr_seg last_released_segment_num;
  sc_monitor segments_monitor;
  sc_monitor_table addr_monitors_table;
  sc_hash_table * processes_segments_table;
  sc_monitor processes_monitor;
  sc_storage_dump_manager * dump_manager;
  sc_event_emission_manager * events_emission_manager;
  sc_event_subscription_manager * events_subscription_manager;
  sc_segment_buffer segment_buffer;
  sc_l1_cache * l1_cache; // L1-кэш (DRAM)
  sc_l2_cache * l2_cache; // L2-кэш (SSD/NVMe)
};

struct _sc_storage * sc_storage_get();

sc_event_emission_manager * sc_storage_get_event_emission_manager();

sc_event_subscription_manager * sc_storage_get_event_subscription_manager();

sc_element * sc_storage_allocate_new_element(sc_memory_context const * ctx, sc_addr * addr);

sc_result sc_storage_get_element_by_addr(sc_addr addr, sc_element ** el);

sc_result sc_storage_free_element(sc_addr addr);

// Объявления функций кэширования
void _sc_l1_cache_init(sc_l1_cache * cache, sc_uint32 capacity);
void _sc_l1_cache_destroy(sc_l1_cache * cache);
void _sc_l1_cache_add(sc_l1_cache * cache, sc_addr_seg seg_num, sc_segment * segment);
sc_segment * _sc_l1_cache_get(sc_l1_cache * cache, sc_addr_seg seg_num);
void _sc_l2_cache_init(sc_l2_cache * cache, sc_uint32 capacity, sc_char * path);
void _sc_l2_cache_destroy(sc_l2_cache * cache);
sc_result _sc_l2_cache_add(sc_l2_cache * cache, sc_addr_seg seg_num, sc_segment * segment);
sc_segment * _sc_l2_cache_get(sc_l2_cache * cache, sc_addr_seg seg_num);

#endif
