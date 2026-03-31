#ifndef REPORT_H
#define REPORT_H

#include "data.h"

void report_menu(HospitalDB *db);
void print_patient_full_record(HospitalDB *db, int patientId);
void print_doctor_stats(HospitalDB *db, int doctorId);
void print_management_report(HospitalDB *db);

#endif
