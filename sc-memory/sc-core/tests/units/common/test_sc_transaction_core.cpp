#include <gtest/gtest.h>
#include <pthread.h>
#include <sc-memory/test/sc_test.hpp>

extern "C" {
#include <sc-store/sc-transaction/sc_transaction_manager.h>
}

struct TransactionThreadData {
    transaction_manager* manager;
    int thread_id;
};

void* transaction_thread(void* arg) {
    auto* data = static_cast<TransactionThreadData*>(arg);
    transaction_manager* manager = data->manager;

    sc_element_version version = {};
    log_version_change(manager, &version);

    if (data->thread_id % 2 == 0) {
        commit_transaction(manager);
    } else {
        rollback_transaction(manager);
    }

    return nullptr;
}

TEST_F(ScMemoryTest, MultiThreadedTransactionTest) {
    transaction_manager manager;
    initialize_transaction_manager(&manager);

    const int NUM_THREADS = 10;
    pthread_t threads[NUM_THREADS];
    TransactionThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].manager = &manager;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], nullptr, transaction_thread, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    EXPECT_EQ(manager.current_transaction, nullptr);

    EXPECT_NE(manager.log_head, nullptr);
}

TEST_F(ScMemoryTest, MultiThreadedTransactionLogTest) {
    transaction_manager manager;
    initialize_transaction_manager(&manager);

    const int NUM_THREADS = 5;
    pthread_t threads[NUM_THREADS];
    TransactionThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].manager = &manager;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], nullptr, transaction_thread, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    EXPECT_NE(manager.log_head, nullptr);
}

TEST_F(ScMemoryTest, TransactionManagerTests) {
    transaction_manager manager;
    initialize_transaction_manager(&manager);

    const int NUM_THREADS = 20;
    pthread_t threads[NUM_THREADS];
    TransactionThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].manager = &manager;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], nullptr, transaction_thread, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    EXPECT_EQ(manager.current_transaction, nullptr);

    EXPECT_NE(manager.log_head, nullptr);
}
