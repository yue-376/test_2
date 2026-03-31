#include "models.h"

static void free_patients(Patient *head) { while (head) { Patient *t = head; head = head->next; free(t);} }
static void free_doctors(Doctor *head) { while (head) { Doctor *t = head; head = head->next; free(t);} }
static void free_regs(Registration *head) { while (head) { Registration *t = head; head = head->next; free(t);} }
static void free_visits(Visit *head) { while (head) { Visit *t = head; head = head->next; free(t);} }
static void free_exams(Exam *head) { while (head) { Exam *t = head; head = head->next; free(t);} }
static void free_wards(Ward *head) { while (head) { Ward *t = head; head = head->next; free(t);} }
static void free_inpatients(Inpatient *head) { while (head) { Inpatient *t = head; head = head->next; free(t);} }
static void free_drugs(Drug *head) { while (head) { Drug *t = head; head = head->next; free(t);} }
static void free_druglogs(DrugLog *head) { while (head) { DrugLog *t = head; head = head->next; free(t);} }

void init_database(Database *db) {
    db->patients = NULL;
    db->doctors = NULL;
    db->registrations = NULL;
    db->visits = NULL;
    db->exams = NULL;
    db->wards = NULL;
    db->inpatients = NULL;
    db->drugs = NULL;
    db->drugLogs = NULL;
}

void free_database(Database *db) {
    free_patients(db->patients);
    free_doctors(db->doctors);
    free_regs(db->registrations);
    free_visits(db->visits);
    free_exams(db->exams);
    free_wards(db->wards);
    free_inpatients(db->inpatients);
    free_drugs(db->drugs);
    free_druglogs(db->drugLogs);
    init_database(db);
}
