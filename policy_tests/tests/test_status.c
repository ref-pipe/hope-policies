#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "test_status.h"
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include "test.h"
#include "isp_utils.h"

#define log printf

/*
 * Test status functions
 */

static bool test_status_passing = false;
static bool test_status_positive = false;
static bool test_status_negative = false;

#define US_TO_SEC 1000000

static uint32_t test_start_time = 0;
static uint32_t test_start_interval = 0;

// Identify positive case (test will end)
void test_positive(){
  uint32_t cycle_hi, cycle_lo;
  test_status_positive = true;
  printf("MSG: Positive test.\n");

  isp_get_cycle_count(&cycle_hi, &cycle_lo);
  printf("Start time: %uus\n", isp_get_time_usec());
  printf("Start cycles: 0x%x%08x\n", cycle_hi, cycle_lo);
}

// Identify negative case (test will never end)
void test_negative(){
  test_status_negative = true;
  printf("MSG: Negative test.\n");
}

// Set passing status at start of test
void test_begin(){
  printf("MSG: Begin test.\n");
  test_status_passing = true;
}

// Start a timer for benchmark tests
void test_start_timer(){
  test_start_time = isp_get_time_usec();
  test_start_interval = test_start_time;
}

// Print the current time interval
void test_print_time_interval(){
  uint32_t current_time = isp_get_time_usec();
  printf("Current interval: %u\n", (current_time - test_start_interval));
  test_start_interval = isp_get_time_usec();
}

// Print the total time elapsed since test_start_timer() call
void test_print_total_time(){
  uint32_t current_time = isp_get_time_usec();
  printf("Total time: %u\n", (current_time - test_start_time));
}

// Set passing status
void test_pass(){
  test_status_passing = true;
}

// Set failing status
void test_fail(){
  test_status_passing = false;
}

// print the test status for a positive test case
int test_done(){
  uint32_t cycle_hi, cycle_lo;
  if(test_status_passing && test_status_positive && !test_status_negative){
  printf("PASS: test passed.\n");

  isp_get_cycle_count(&cycle_hi, &cycle_lo);
  printf("End time: %uus\n", isp_get_time_usec());
  printf("End cycles: 0x%x%08x\n", cycle_hi, cycle_lo);

  printf("MSG: End test.\n");
  isp_test_device_pass();
  return 0;
  }
  else if(test_status_positive && test_status_negative) {
    printf("FAIL: error in test, can't be both positive and negative test.\n");
    printf("MSG: End test.\n");
    isp_test_device_fail();
    return 1;
  }
  else {
    printf("FAIL: test failed.\n");
    printf("MSG: End test.\n");
    isp_test_device_fail();
    return 1;
  }
}


// print the test status
void test_error(const char *fmt, ...){
  va_list args;

  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);

  test_fail();
}

