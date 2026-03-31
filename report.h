#ifndef REPORT_H
#define REPORT_H

#include "models.h"

void report_menu(Database *db);
void print_patient_full_record(Database *db, int patientId);
void print_doctor_stats(Database *db, int doctorId);
void print_management_report(Database *db);

#endif
