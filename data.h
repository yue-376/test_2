#ifndef DATA_H
#define DATA_H

#include "common.h"

typedef struct Patient {
    int id;
    char name[NAME_LEN];
    char gender[SMALL_LEN];
    char birth[DATE_LEN];
    char phone[PHONE_LEN];
    char insurance[SMALL_LEN];
    int archived;
    struct Patient *next;
} Patient;

typedef struct Department {
    int id;
    char name[SMALL_LEN];
    char wardType[SMALL_LEN];
    struct Department *next;
} Department;

typedef struct Doctor {
    int id;
    char name[NAME_LEN];
    int deptId;
    char title[SMALL_LEN];
    int archived;
    struct Doctor *next;
} Doctor;

typedef struct Ward {
    int id;
    char type[SMALL_LEN];
    int deptId;
    int totalBeds;
    int occupiedBeds;
    struct Ward *next;
} Ward;

typedef struct Bed {
    int wardId;
    int bedNo;
    char status[SMALL_LEN];
    int patientId;
    struct Bed *next;
} Bed;

typedef struct Drug {
    int id;
    char genericName[NAME_LEN];
    char brandName[NAME_LEN];
    char alias[NAME_LEN];
    char category[SMALL_LEN];
    int deptId;
    double price;
    int stock;
    struct Drug *next;
} Drug;

typedef struct Registration {
    int id;
    int patientId;
    int deptId;
    int doctorId;
    char regDate[DATE_LEN];
    char regType[SMALL_LEN];
    char status[SMALL_LEN];
    struct Registration *next;
} Registration;

typedef struct Visit {
    int id;
    int regId;
    char diagnosis[TEXT_LEN];
    char exams[TEXT_LEN];
    char prescription[TEXT_LEN];
    struct Visit *next;
} Visit;

typedef struct Exam {
    int id;
    int patientId;
    int doctorId;
    char code[SMALL_LEN];
    char name[NAME_LEN];
    char execTime[DATE_LEN];
    double fee;
    char result[TEXT_LEN];
    struct Exam *next;
} Exam;

typedef struct Inpatient {
    int id;
    int patientId;
    int wardId;
    int bedNo;
    char admitDate[DATE_LEN];
    char dischargePlan[DATE_LEN];
    double cost;
    struct Inpatient *next;
} Inpatient;

typedef struct StockLog {
    int id;
    int drugId;
    char opType[SMALL_LEN];
    int qty;
    char operatorName[NAME_LEN];
    char opDate[DATE_LEN];
    struct StockLog *next;
} StockLog;

typedef struct Dispense {
    int id;
    int patientId;
    int doctorId;
    int drugId;
    int qty;
    char dispDate[DATE_LEN];
    struct Dispense *next;
} Dispense;

typedef struct HospitalDB {
    Patient *patients;
    Department *departments;
    Doctor *doctors;
    Ward *wards;
    Bed *beds;
    Drug *drugs;
    Registration *registrations;
    Visit *visits;
    Exam *exams;
    Inpatient *inpatients;
    StockLog *stockLogs;
    Dispense *dispenses;
} HospitalDB;

void init_db(HospitalDB *db);
void free_db(HospitalDB *db);

Patient *find_patient(HospitalDB *db, int id);
Department *find_department(HospitalDB *db, int id);
Doctor *find_doctor(HospitalDB *db, int id);
Ward *find_ward(HospitalDB *db, int id);
Bed *find_bed(HospitalDB *db, int wardId, int bedNo);
Drug *find_drug(HospitalDB *db, int id);
Registration *find_registration(HospitalDB *db, int id);

int next_patient_id(HospitalDB *db);
int next_doctor_id(HospitalDB *db);
int next_drug_id(HospitalDB *db);
int next_registration_id(HospitalDB *db);
int next_visit_id(HospitalDB *db);
int next_exam_id(HospitalDB *db);
int next_inpatient_id(HospitalDB *db);
int next_stocklog_id(HospitalDB *db);
int next_dispense_id(HospitalDB *db);

void append_patient(HospitalDB *db, Patient *node);
void append_department(HospitalDB *db, Department *node);
void append_doctor(HospitalDB *db, Doctor *node);
void append_ward(HospitalDB *db, Ward *node);
void append_bed(HospitalDB *db, Bed *node);
void append_drug(HospitalDB *db, Drug *node);
void append_registration(HospitalDB *db, Registration *node);
void append_visit(HospitalDB *db, Visit *node);
void append_exam(HospitalDB *db, Exam *node);
void append_inpatient(HospitalDB *db, Inpatient *node);
void append_stocklog(HospitalDB *db, StockLog *node);
void append_dispense(HospitalDB *db, Dispense *node);

int load_all(HospitalDB *db, const char *dir);
int save_all(HospitalDB *db, const char *dir);
void generate_sample_data(HospitalDB *db);
void ensure_sample_files(HospitalDB *db, const char *dir);

#endif
