#ifndef MODELS_H
#define MODELS_H

#include "common.h"

typedef struct Patient {
    int id;
    char name[NAME_LEN];
    char gender[16];
    char birth[DATE_LEN];
    char phone[PHONE_LEN];
    char insurance[SMALL_LEN];
    int archived;
    struct Patient *next;
} Patient;

typedef struct Doctor {
    int id;
    char name[NAME_LEN];
    char dept[SMALL_LEN];
    char title[SMALL_LEN];
    int archived;
    struct Doctor *next;
} Doctor;

typedef struct Registration {
    int id;
    int patientId;
    int doctorId;
    char dept[SMALL_LEN];
    char date[DATE_LEN];
    char type[SMALL_LEN];
    char status[SMALL_LEN];
    struct Registration *next;
} Registration;

typedef struct Visit {
    int id;
    int regId;
    char diagnosis[LONG_LEN];
    char examItems[LONG_LEN];
    char prescription[LONG_LEN];
    struct Visit *next;
} Visit;

typedef struct Exam {
    int id;
    int patientId;
    int doctorId;
    char code[SMALL_LEN];
    char itemName[NAME_LEN];
    char execTime[DATE_LEN];
    double fee;
    char result[LONG_LEN];
    struct Exam *next;
} Exam;

typedef struct Ward {
    int id;
    char wardType[SMALL_LEN];
    char dept[SMALL_LEN];
    int bedCount;
    int occupiedBeds;
    int maintenanceBeds;
    struct Ward *next;
} Ward;

typedef struct Inpatient {
    int id;
    int patientId;
    int wardId;
    int bedNo;
    char admitDate[DATE_LEN];
    char expectedDischarge[DATE_LEN];
    double totalCost;
    struct Inpatient *next;
} Inpatient;

typedef struct Drug {
    int id;
    char genericName[NAME_LEN];
    char brandName[NAME_LEN];
    char alias[NAME_LEN];
    char type[SMALL_LEN];
    char dept[SMALL_LEN];
    double price;
    int stock;
    struct Drug *next;
} Drug;

typedef struct DrugLog {
    int id;
    int drugId;
    char operation[SMALL_LEN];
    int quantity;
    char operatorName[NAME_LEN];
    char date[DATE_LEN];
    struct DrugLog *next;
} DrugLog;

typedef struct Database {
    Patient *patients;
    Doctor *doctors;
    Registration *registrations;
    Visit *visits;
    Exam *exams;
    Ward *wards;
    Inpatient *inpatients;
    Drug *drugs;
    DrugLog *drugLogs;
} Database;

void init_database(Database *db);
void free_database(Database *db);

#endif
