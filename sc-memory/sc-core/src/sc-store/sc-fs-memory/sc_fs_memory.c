/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/


#define _GNU_SOURCE // Необходимо для O_DIRECT

#include "sc_fs_memory.h"
#include "sc_fs_memory_builder.h"

#include "sc_file_system.h"
#include "sc_dictionary_fs_memory_private.h"

#include "sc-store/sc_segment.h"
#include "sc-store/sc_storage_private.h"

#include "sc_io.h"

sc_fs_memory_manager * manager;

sc_fs_memory_status sc_fs_memory_initialize_ext(sc_memory_params const * params)
{
 manager = sc_fs_memory_build();
 manager->version = params->version;
 manager->path = params->storage;

 if (manager->path == null_ptr)
 {
   sc_fs_memory_error("Empty repo path to initialize memory");
   return SC_FS_MEMORY_NO;
 }

 static sc_char const * segments_postfix = "segments" SC_FS_EXT;
 sc_fs_concat_path(manager->path, segments_postfix, &manager->segments_path);

 if (manager->initialize(&manager->fs_memory, params) != SC_FS_MEMORY_OK)
   return SC_FS_MEMORY_NO;

 // clear repository if it needs
 if (params->clear == SC_TRUE)
 {
   sc_fs_memory_info("Clear sc-memory segments");
   if (sc_fs_remove_file(manager->segments_path) == SC_FALSE)
     sc_fs_memory_info("Can't remove segments file: %s", manager->segments_path);
 }

 return SC_FS_MEMORY_OK;
}

sc_fs_memory_status sc_fs_memory_initialize(sc_char const * path, sc_bool clear)
{
 sc_memory_params * params = _sc_dictionary_fs_memory_get_default_params(path, clear);
 sc_fs_memory_status const status = sc_fs_memory_initialize_ext(params);
 sc_mem_free(params);
 return status;
}

sc_fs_memory_status sc_fs_memory_shutdown()
{
 sc_fs_memory_status const result = manager->shutdown(manager->fs_memory);
 sc_mem_free(manager->segments_path);
 sc_mem_free(manager);
 return result;
}

sc_fs_memory_status sc_fs_memory_link_string(
   sc_addr_hash const link_hash,
   sc_char const * string,
   sc_uint32 const string_size)
{
 return manager->link_string(manager->fs_memory, link_hash, string, string_size, SC_TRUE);
}

sc_fs_memory_status sc_fs_memory_link_string_ext(
   sc_addr_hash const link_hash,
   sc_char const * string,
   sc_uint32 const string_size,
   sc_bool is_searchable_string)
{
 return manager->link_string(manager->fs_memory, link_hash, string, string_size, is_searchable_string);
}

sc_fs_memory_status sc_fs_memory_get_string_by_link_hash(
   sc_addr_hash const link_hash,
   sc_char ** string,
   sc_uint32 * string_size)
{
 sc_uint64 size;
 sc_fs_memory_status result = manager->get_string_by_link_hash(manager->fs_memory, link_hash, string, &size);
 *string_size = size;
 return result;
}

sc_fs_memory_status sc_fs_memory_get_link_hashes_by_string(
   sc_char const * string,
   sc_uint32 const string_size,
   sc_link_handler * link_handler)
{
 return manager->get_link_hashes_by_string(manager->fs_memory, string, string_size, link_handler);
}

sc_fs_memory_status sc_fs_memory_get_link_hashes_by_substring(
   sc_char const * substring,
   sc_uint32 const substring_size,
   sc_uint32 const max_length_to_search_as_prefix,
   sc_link_handler * link_handler)
{
 return manager->get_link_hashes_by_substring(
     manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, link_handler);
}

sc_fs_memory_status sc_fs_memory_get_strings_by_substring(
   sc_char const * substring,
   sc_uint32 const substring_size,
   sc_uint32 const max_length_to_search_as_prefix,
   sc_link_handler * link_handler)
{
 return manager->get_strings_by_substring(
     manager->fs_memory, substring, substring_size, max_length_to_search_as_prefix, link_handler);
}

sc_fs_memory_status sc_fs_memory_unlink_string(sc_addr_hash link_hash)
{
 return manager->unlink_string(manager->fs_memory, link_hash);
}




// Асинхронная запись сегмента
void write_segment_async(int fd, sc_segment* segment, off_t offset) {
  io_context_t ctx = 0;
  struct iocb cb;
  struct iocb* cbs[1];
  struct io_event events[1];

  if (io_setup(1, &ctx) < 0) {
    sc_fs_memory_error("io_setup failed");
    return;
  }

  io_prep_pwrite(&cb, fd, segment, sizeof(sc_segment), offset);
  cbs[0] = &cb;

  if (io_submit(ctx, 1, cbs) != 1) {
    sc_fs_memory_error("io_submit failed");
    io_destroy(ctx);
    return;
  }

  if (io_getevents(ctx, 1, 1, events, NULL) != 1) {
    sc_fs_memory_error("io_getevents failed");
    io_destroy(ctx);
    return;
  }

  io_destroy(ctx);
}


// Функция для выполнения записи в потоке
void* thread_write(void* arg) {
  thread_args* args = (thread_args*)arg;
  write_segment_async(args->fd, args->segment, args->offset);
  free(args);
  return NULL;
}

// Параллельная запись сегментов
void write_segments_parallel(int fd, sc_segment* segments, sc_addr_seg num_segments) {
  pthread_t threads[NUM_THREADS];
  sc_addr_seg segments_per_thread = num_segments / NUM_THREADS;
  sc_addr_seg remaining_segments = num_segments % NUM_THREADS;
  sc_addr_seg segment_idx = 0;

  for (sc_uint32 i = 0; i < NUM_THREADS; i++) {
    sc_addr_seg count = segments_per_thread + (i < remaining_segments ? 1 : 0);
    if (count == 0) break;

    for (sc_addr_seg j = 0; j < count; j++) {
      thread_args* args = malloc(sizeof(thread_args));
      args->fd = fd;
      args->segment = &segments[segment_idx];
      args->offset = segment_idx * sizeof(sc_segment);

      if (pthread_create(&threads[i], NULL, thread_write, args) != 0) {
        sc_fs_memory_error("pthread_create failed");
        free(args);
        continue;
      }
      segment_idx++;
    }
  }

  for (sc_uint32 i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}



sc_fs_memory_status _sc_fs_memory_load_sc_memory_segments(sc_storage * storage) {
  if (sc_fs_is_file(manager->segments_path) == SC_FALSE) {
    storage->segments_count = 0;
    sc_fs_memory_info("There are no sc-memory segments in %s", manager->segments_path);
    return SC_FS_MEMORY_OK;
  }

  int fd = open(manager->segments_path, O_RDONLY);
  if (fd < 0) {
    sc_fs_memory_error("Failed to open segments file: %s", manager->segments_path);
    return SC_FS_MEMORY_READ_ERROR;
  }

  off_t file_size = lseek(fd, 0, SEEK_END);
  if (file_size < sizeof(sc_fs_memory_header) + 3 * sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Segments file is too small");
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  lseek(fd, 0, SEEK_SET);

  sc_fs_memory_header* header = aligned_alloc(PAGE_SIZE, sizeof(sc_fs_memory_header));
  if (!header) {
    sc_fs_memory_error("Failed to allocate memory for header");
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }

  if (read(fd, header, sizeof(sc_fs_memory_header)) != sizeof(sc_fs_memory_header)) {
    sc_fs_memory_error("Failed to read header");
    free(header);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  manager->header = *header;
  storage->segments_count = manager->header.size;
  free(header);

  sc_addr_seg* segments_count = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  sc_addr_seg* last_not_engaged = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  sc_addr_seg* last_released = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  if (!segments_count || !last_not_engaged || !last_released) {
    sc_fs_memory_error("Failed to allocate memory for metadata");
    free(segments_count);
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }

  if (read(fd, segments_count, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to read segments_count");
    free(segments_count);
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  storage->segments_count = *segments_count;
  free(segments_count);

  if (read(fd, last_not_engaged, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to read last_not_engaged_segment_num");
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  storage->last_not_engaged_segment_num = *last_not_engaged;
  free(last_not_engaged);

  if (read(fd, last_released, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to read last_released_segment_num");
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  storage->last_released_segment_num = *last_released;
  free(last_released);

  sc_version read_version;
  sc_version_from_int(manager->header.version, &read_version);
  if (sc_version_compare(&manager->version, &read_version) == -1) {
    sc_char* version = sc_version_string_new(&read_version);
    sc_fs_memory_error("Read sc-memory segments has incompatible version %s", version);
    sc_version_string_free(version);
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }

  storage->segments = malloc(storage->segments_count * sizeof(sc_segment*));
  if (!storage->segments) {
    sc_fs_memory_error("Failed to allocate memory for segments array");
    close(fd);
    return SC_FS_MEMORY_READ_ERROR;
  }
  for (sc_addr_seg i = 0; i < storage->segments_count; ++i)
    storage->segments[i] = null_ptr;

  // Загрузка L2-кэша
  sc_char * l2_cache_path = sc_mem_new(sc_char, strlen(manager->path) + 20);
  sprintf(l2_cache_path, "%s/cache_segments.dat", manager->path);
  int cache_fd = open(l2_cache_path, O_RDONLY);
  if (cache_fd >= 0) {
    off_t cache_size = lseek(cache_fd, 0, SEEK_END);
    lseek(cache_fd, 0, SEEK_SET);
    while (lseek(cache_fd, 0, SEEK_CUR) < cache_size) {
      struct { sc_addr_seg num; off_t offset; } entry;
      if (read(cache_fd, &entry, sizeof(entry)) != sizeof(entry)) {
        sc_fs_memory_warning("Failed to read L2 cache entry");
        break;
      }
      sc_segment* seg = aligned_alloc(PAGE_SIZE, sizeof(sc_segment));
      if (!seg) {
        sc_fs_memory_warning("Failed to allocate segment for L2 cache");
        continue;
      }
      if (pread(cache_fd, seg, sizeof(sc_segment), entry.offset) != sizeof(sc_segment)) {
        sc_fs_memory_warning("Failed to read segment from L2 cache");
        free(seg);
        continue;
      }
      if (entry.num <= storage->segments_count && storage->segments[entry.num - 1] == null_ptr) {
        storage->segments[entry.num - 1] = seg;
        _sc_l1_cache_add(storage->l1_cache, entry.num, seg);
      } else {
        free(seg);
      }
    }
    close(cache_fd);
  }
  sc_mem_free(l2_cache_path);

  // Загрузка оставшихся сегментов из основного хранилища
  for (sc_addr_seg i = 0; i < storage->segments_count; ++i) {
    if (storage->segments[i] != null_ptr)
      continue;

    sc_segment* seg = aligned_alloc(PAGE_SIZE, sizeof(sc_segment));
    if (!seg) {
      sc_fs_memory_error("Failed to allocate memory for segment %d", i);
      for (sc_addr_seg j = 0; j < storage->segments_count; ++j) {
        if (storage->segments[j])
          free(storage->segments[j]);
      }
      free(storage->segments);
      close(fd);
      return SC_FS_MEMORY_READ_ERROR;
    }
    if (read(fd, seg, sizeof(sc_segment)) != sizeof(sc_segment)) {
      sc_fs_memory_error("Failed to read segment %d", i);
      free(seg);
      for (sc_addr_seg j = 0; j < storage->segments_count; ++j) {
        if (storage->segments[j])
          free(storage->segments[j]);
      }
      free(storage->segments);
      close(fd);
      return SC_FS_MEMORY_READ_ERROR;
    }
    storage->segments[i] = seg;
    _sc_l1_cache_add(storage->l1_cache, i + 1, seg);
  }

  close(fd);
  sc_message("\tLoaded segments count: %d", storage->segments_count);
  sc_message("\tSc-segments size: %ld", storage->segments_count * sizeof(sc_segment));
  sc_message("\tLast not engaged segment num: %d", storage->last_not_engaged_segment_num);
  sc_message("\tLast released segment num: %d", storage->last_released_segment_num);
  sc_fs_memory_info("Sc-memory segments loaded");
  return SC_FS_MEMORY_OK;
}



sc_fs_memory_status sc_fs_memory_load(sc_storage * storage)
{
 if (_sc_fs_memory_load_sc_memory_segments(storage) != SC_FS_MEMORY_OK)
   return SC_FS_MEMORY_READ_ERROR;
 if (manager->load(manager->fs_memory) != SC_FS_MEMORY_OK)
   return SC_FS_MEMORY_READ_ERROR;

 return SC_FS_MEMORY_OK;
}



sc_fs_memory_status _sc_fs_memory_save_sc_memory_segments(sc_storage * storage) {
  sc_fs_memory_info("Save sc-memory segments");

  int fd = open(manager->segments_path, O_RDWR | O_CREAT, 0666);
  if (fd < 0) {
    sc_fs_memory_error("Failed to open segments file: %s", manager->segments_path);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  sc_fs_memory_header* header = aligned_alloc(PAGE_SIZE, sizeof(sc_fs_memory_header));
  if (!header) {
    sc_fs_memory_error("Failed to allocate memory for header");
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }
  header->size = storage->segments_count;
  header->version = sc_version_to_int(&manager->version);
  header->timestamp = g_get_real_time();

  if (write(fd, header, sizeof(sc_fs_memory_header)) != sizeof(sc_fs_memory_header)) {
    sc_fs_memory_error("Failed to write header");
    free(header);
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }
  free(header);

  sc_addr_seg* segments_count = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  sc_addr_seg* last_not_engaged = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  sc_addr_seg* last_released = aligned_alloc(PAGE_SIZE, sizeof(sc_addr_seg));
  if (!segments_count || !last_not_engaged || !last_released) {
    sc_fs_memory_error("Failed to allocate memory for metadata");
    free(segments_count);
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  *segments_count = storage->segments_count;
  if (write(fd, segments_count, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to write segments_count");
    free(segments_count);
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }
  free(segments_count);

  *last_not_engaged = storage->last_not_engaged_segment_num;
  if (write(fd, last_not_engaged, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to write last_not_engaged_segment_num");
    free(last_not_engaged);
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }
  free(last_not_engaged);

  *last_released = storage->last_released_segment_num;
  if (write(fd, last_released, sizeof(sc_addr_seg)) != sizeof(sc_addr_seg)) {
    sc_fs_memory_error("Failed to write last_released_segment_num");
    free(last_released);
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }
  free(last_released);

  sc_segment* segments = aligned_alloc(PAGE_SIZE, storage->segments_count * sizeof(sc_segment));
  if (!segments) {
    sc_fs_memory_error("Failed to allocate memory for segments");
    close(fd);
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  for (sc_addr_seg idx = 0; idx < storage->segments_count; ++idx) {
    sc_segment * segment = storage->segments[idx];
    if (segment == null_ptr) {
      segment = _sc_l1_cache_get(storage->l1_cache, idx + 1);
      if (!segment)
        segment = _sc_l2_cache_get(storage->l2_cache, idx + 1);
    }
    if (segment == null_ptr) {
      sc_fs_memory_error("Error: segment %d is null", idx);
      free(segments);
      close(fd);
      return SC_FS_MEMORY_WRITE_ERROR;
    }
    sc_monitor_acquire_read(&segment->monitor);
    memcpy(&segments[idx], segment, sizeof(sc_segment));
    sc_monitor_release_read(&segment->monitor);
  }

  write_segments_parallel(fd, segments, storage->segments_count);

  free(segments);
  close(fd);

  sc_message("\tSaved segments count: %d", storage->segments_count);
  sc_message("\tSc-segments size: %ld", storage->segments_count * sizeof(sc_segment));
  sc_message("\tLast not engaged segment num: %d", storage->last_not_engaged_segment_num);
  sc_message("\tLast released segment num: %d", storage->last_released_segment_num);
  sc_fs_memory_info("Sc-memory segments saved");
  return SC_FS_MEMORY_OK;
}


sc_fs_memory_status sc_fs_memory_save(sc_storage * storage)
{
 if (manager->path == null_ptr)
 {
   sc_fs_memory_error("Repo path is empty to save memory");
   return SC_FS_MEMORY_NO;
 }

 if (_sc_fs_memory_save_sc_memory_segments(storage) != SC_FS_MEMORY_OK)
   return SC_FS_MEMORY_WRITE_ERROR;
 if (manager->save(manager->fs_memory) != SC_FS_MEMORY_OK)
   return SC_FS_MEMORY_WRITE_ERROR;

 return SC_FS_MEMORY_OK;
}
