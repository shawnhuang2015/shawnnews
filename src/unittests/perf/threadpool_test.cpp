/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-CSE
 * threadpool_test.cpp: /gdbms/src/unittests/perf/threadpool_test.cpp
 *
 *  Created on: Oct 7, 2013
 *      Author: like
 ******************************************************************************/
#include <gutil/gthreadpool.hpp>
#include <gtest/gtest.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NUM_THREADS 2000

/******************************************************/
/* Testing performance of pthread: no reuse           */
/******************************************************/
void *BusyWork(void *t) {
  pthread_exit(reinterpret_cast<void *>(t));
}

TEST(THREADPOOL_PERF, Pthread_) {
  pthread_t thread[NUM_THREADS];
  pthread_attr_t attr;
  int rc;
  uint64_t t;
  void *status;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (t = 0; t < NUM_THREADS; t++) {
    rc = pthread_create(&thread[t], &attr, BusyWork, reinterpret_cast<void *>(t));
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
  for (t = 0; t < NUM_THREADS; t++) {
    rc = pthread_join(thread[t], &status);
    if (rc) {
      exit(-1);
    }
  }
}


/******************************************************/
/* Testing performance of threadpool 2.0: reuse       */
/******************************************************/
void *BusyWork3(void *t, uint32_t &current) {
  current++;
  return 0;
}

void *BusyWork4(void *t, uint32_t &current) {
  current++;
  current++;
  return 0;
}

TEST(THREADPOOL_PERF, Threadpool) {
  /* Since we can reuse threads here, we don't need to
   * create/delete/create 2000 threads.
   * We only create 250 threads and reuse them
   * in 8 calls (4 runs, each run 2 calls, and
   * each call 250 threads)
   */
  gutil::GThreadPool pool(NUM_THREADS/16);

  for (int xx = 0; xx < 4; xx++) {
    /* 1st calling of function BusyWork3 that +1 */
    uint32_t current = 0;
    for (size_t i = 0; i < NUM_THREADS/8; i++) {
      pool.run_task(boost::bind(BusyWork3, this, boost::ref(current)));
    }
    pool.wait();
    //EXPECT_EQ(static_cast<int>(current), 250);

    /* 2nd calling of another function BusyWork4 that +2 */
    current = 0;
    for (size_t i = 0; i < NUM_THREADS/8; i++) {
      pool.run_task(boost::bind(BusyWork4, this, boost::ref(current)));
    }
    pool.wait();
    //EXPECT_EQ(static_cast<int>(current), 500);
  }
}
