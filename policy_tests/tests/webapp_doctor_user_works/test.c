#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "database.h"
#include "auth.h"
#include "user.h"
#include "medical.h"
#include "test.h"
#include "test_status.h"

void TestDoctorAddRecord(void)
{
  user_t *user;
  user_t *doctor_user;

  user = UserCreate("patient_user", "password123", "Pat", "Ient", "123 Main St.");
  if(user == NULL) {
    t_printf("Failed to create user\n");
    return;
  }                                                                         

  doctor_user = UserCreate("doctor_user", "password123", "Pat", "Ient", "123 Main St.");
  if(doctor_user == NULL) {
    t_printf("Failed to create user\n");
    return;
  }

  MedicalSetPatient(user);
  MedicalSetDoctor(doctor_user);

  AuthSetCurrentUserType(doctor_user);
  
  // should not fail, since doctor is active
  MedicalAddRecord(doctor_user, user, "Fractured Authentication", "The doctor is not logged in");
}

int test_main()
{
  test_positive();
  test_begin();

  TestDoctorAddRecord();

  return test_done();
}
