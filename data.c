#include "data.h"

static void append_patient(Database *db, Patient *node) { node->next = NULL; if (!db->patients) db->patients = node; else { Patient *p = db->patients; while (p->next) p = p->next; p->next = node; } }
static void append_doctor(Database *db, Doctor *node) { node->next = NULL; if (!db->doctors) db->doctors = node; else { Doctor *p = db->doctors; while (p->next) p = p->next; p->next = node; } }
static void append_reg(Database *db, Registration *node) { node->next = NULL; if (!db->registrations) db->registrations = node; else { Registration *p = db->registrations; while (p->next) p = p->next; p->next = node; } }
static void append_visit(Database *db, Visit *node) { node->next = NULL; if (!db->visits) db->visits = node; else { Visit *p = db->visits; while (p->next) p = p->next; p->next = node; } }
static void append_exam(Database *db, Exam *node) { node->next = NULL; if (!db->exams) db->exams = node; else { Exam *p = db->exams; while (p->next) p = p->next; p->next = node; } }
static void append_ward(Database *db, Ward *node) { node->next = NULL; if (!db->wards) db->wards = node; else { Ward *p = db->wards; while (p->next) p = p->next; p->next = node; } }
static void append_inpatient(Database *db, Inpatient *node) { node->next = NULL; if (!db->inpatients) db->inpatients = node; else { Inpatient *p = db->inpatients; while (p->next) p = p->next; p->next = node; } }
static void append_drug(Database *db, Drug *node) { node->next = NULL; if (!db->drugs) db->drugs = node; else { Drug *p = db->drugs; while (p->next) p = p->next; p->next = node; } }
static void append_druglog(Database *db, DrugLog *node) { node->next = NULL; if (!db->drugLogs) db->drugLogs = node; else { DrugLog *p = db->drugLogs; while (p->next) p = p->next; p->next = node; } }

Patient *find_patient(Database *db, int id) { Patient *p = db->patients; while (p) { if (p->id == id) return p; p = p->next; } return NULL; }
Doctor *find_doctor(Database *db, int id) { Doctor *p = db->doctors; while (p) { if (p->id == id) return p; p = p->next; } return NULL; }
Registration *find_registration(Database *db, int id) { Registration *p = db->registrations; while (p) { if (p->id == id) return p; p = p->next; } return NULL; }
Ward *find_ward(Database *db, int id) { Ward *p = db->wards; while (p) { if (p->id == id) return p; p = p->next; } return NULL; }
Drug *find_drug(Database *db, int id) { Drug *p = db->drugs; while (p) { if (p->id == id) return p; p = p->next; } return NULL; }

#define NEXT_ID_FUNC(type, field, name) int name(Database *db) { int max = 0; type *p = db->field; while (p) { if (p->id > max) max = p->id; p = p->next; } return max + 1; }
NEXT_ID_FUNC(Patient, patients, next_patient_id)
NEXT_ID_FUNC(Doctor, doctors, next_doctor_id)
NEXT_ID_FUNC(Registration, registrations, next_registration_id)
NEXT_ID_FUNC(Visit, visits, next_visit_id)
NEXT_ID_FUNC(Exam, exams, next_exam_id)
NEXT_ID_FUNC(Ward, wards, next_ward_id)
NEXT_ID_FUNC(Inpatient, inpatients, next_inpatient_id)
NEXT_ID_FUNC(Drug, drugs, next_drug_id)
NEXT_ID_FUNC(DrugLog, drugLogs, next_druglog_id)

static int file_exists(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp) { fclose(fp); return 1; }
    return 0;
}

static void path_join(char *out, size_t size, const char *dir, const char *name) {
    snprintf(out, size, "%s/%s", dir, name);
}

static void load_patients(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Patient *p = (Patient*)malloc(sizeof(Patient));
        if (sscanf(line, "%d|%63[^|]|%15[^|]|%15[^|]|%31[^|]|%31[^|]|%d", &p->id, p->name, p->gender, p->birth, p->phone, p->insurance, &p->archived) == 7) append_patient(db, p); else free(p);
    }
    fclose(fp);
}
static void load_doctors(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Doctor *p = (Doctor*)malloc(sizeof(Doctor));
        if (sscanf(line, "%d|%63[^|]|%31[^|]|%31[^|]|%d", &p->id, p->name, p->dept, p->title, &p->archived) == 5) append_doctor(db, p); else free(p);
    }
    fclose(fp);
}
static void load_regs(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Registration *p = (Registration*)malloc(sizeof(Registration));
        if (sscanf(line, "%d|%d|%d|%31[^|]|%15[^|]|%31[^|]|%31[^\n]", &p->id, &p->patientId, &p->doctorId, p->dept, p->date, p->type, p->status) == 7) append_reg(db, p); else free(p);
    }
    fclose(fp);
}
static void load_visits(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Visit *p = (Visit*)malloc(sizeof(Visit));
        if (sscanf(line, "%d|%d|%255[^|]|%255[^|]|%255[^\n]", &p->id, &p->regId, p->diagnosis, p->examItems, p->prescription) == 5) append_visit(db, p); else free(p);
    }
    fclose(fp);
}
static void load_exams(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Exam *p = (Exam*)malloc(sizeof(Exam));
        if (sscanf(line, "%d|%d|%d|%31[^|]|%63[^|]|%15[^|]|%lf|%255[^\n]", &p->id, &p->patientId, &p->doctorId, p->code, p->itemName, p->execTime, &p->fee, p->result) == 8) append_exam(db, p); else free(p);
    }
    fclose(fp);
}
static void load_wards(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Ward *p = (Ward*)malloc(sizeof(Ward));
        if (sscanf(line, "%d|%31[^|]|%31[^|]|%d|%d|%d", &p->id, p->wardType, p->dept, &p->bedCount, &p->occupiedBeds, &p->maintenanceBeds) == 6) append_ward(db, p); else free(p);
    }
    fclose(fp);
}
static void load_inpatients(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Inpatient *p = (Inpatient*)malloc(sizeof(Inpatient));
        if (sscanf(line, "%d|%d|%d|%d|%15[^|]|%15[^|]|%lf", &p->id, &p->patientId, &p->wardId, &p->bedNo, p->admitDate, p->expectedDischarge, &p->totalCost) == 7) append_inpatient(db, p); else free(p);
    }
    fclose(fp);
}
static void load_drugs(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        Drug *p = (Drug*)malloc(sizeof(Drug));
        if (sscanf(line, "%d|%63[^|]|%63[^|]|%63[^|]|%31[^|]|%31[^|]|%lf|%d", &p->id, p->genericName, p->brandName, p->alias, p->type, p->dept, &p->price, &p->stock) == 8) append_drug(db, p); else free(p);
    }
    fclose(fp);
}
static void load_druglogs(Database *db, const char *path) {
    FILE *fp = fopen(path, "r"); char line[MAX_LINE];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        DrugLog *p = (DrugLog*)malloc(sizeof(DrugLog));
        if (sscanf(line, "%d|%d|%31[^|]|%d|%63[^|]|%15[^\n]", &p->id, &p->drugId, p->operation, &p->quantity, p->operatorName, p->date) == 6) append_druglog(db, p); else free(p);
    }
    fclose(fp);
}

static void save_patients(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Patient *p; if (!fp) return; for (p = db->patients; p; p = p->next) fprintf(fp, "%d|%s|%s|%s|%s|%s|%d\n", p->id, p->name, p->gender, p->birth, p->phone, p->insurance, p->archived); fclose(fp);} 
static void save_doctors(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Doctor *p; if (!fp) return; for (p = db->doctors; p; p = p->next) fprintf(fp, "%d|%s|%s|%s|%d\n", p->id, p->name, p->dept, p->title, p->archived); fclose(fp);} 
static void save_regs(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Registration *p; if (!fp) return; for (p = db->registrations; p; p = p->next) fprintf(fp, "%d|%d|%d|%s|%s|%s|%s\n", p->id, p->patientId, p->doctorId, p->dept, p->date, p->type, p->status); fclose(fp);} 
static void save_visits(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Visit *p; if (!fp) return; for (p = db->visits; p; p = p->next) fprintf(fp, "%d|%d|%s|%s|%s\n", p->id, p->regId, p->diagnosis, p->examItems, p->prescription); fclose(fp);} 
static void save_exams(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Exam *p; if (!fp) return; for (p = db->exams; p; p = p->next) fprintf(fp, "%d|%d|%d|%s|%s|%s|%.2f|%s\n", p->id, p->patientId, p->doctorId, p->code, p->itemName, p->execTime, p->fee, p->result); fclose(fp);} 
static void save_wards(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Ward *p; if (!fp) return; for (p = db->wards; p; p = p->next) fprintf(fp, "%d|%s|%s|%d|%d|%d\n", p->id, p->wardType, p->dept, p->bedCount, p->occupiedBeds, p->maintenanceBeds); fclose(fp);} 
static void save_inpatients(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Inpatient *p; if (!fp) return; for (p = db->inpatients; p; p = p->next) fprintf(fp, "%d|%d|%d|%d|%s|%s|%.2f\n", p->id, p->patientId, p->wardId, p->bedNo, p->admitDate, p->expectedDischarge, p->totalCost); fclose(fp);} 
static void save_drugs(Database *db, const char *path) { FILE *fp = fopen(path, "w"); Drug *p; if (!fp) return; for (p = db->drugs; p; p = p->next) fprintf(fp, "%d|%s|%s|%s|%s|%s|%.2f|%d\n", p->id, p->genericName, p->brandName, p->alias, p->type, p->dept, p->price, p->stock); fclose(fp);} 
static void save_druglogs(Database *db, const char *path) { FILE *fp = fopen(path, "w"); DrugLog *p; if (!fp) return; for (p = db->drugLogs; p; p = p->next) fprintf(fp, "%d|%d|%s|%d|%s|%s\n", p->id, p->drugId, p->operation, p->quantity, p->operatorName, p->date); fclose(fp);} 

int load_all(Database *db, const char *dir) {
    char path[256];
    path_join(path, sizeof(path), dir, "patients.txt"); if (file_exists(path)) load_patients(db, path);
    path_join(path, sizeof(path), dir, "doctors.txt"); if (file_exists(path)) load_doctors(db, path);
    path_join(path, sizeof(path), dir, "registrations.txt"); if (file_exists(path)) load_regs(db, path);
    path_join(path, sizeof(path), dir, "visits.txt"); if (file_exists(path)) load_visits(db, path);
    path_join(path, sizeof(path), dir, "exams.txt"); if (file_exists(path)) load_exams(db, path);
    path_join(path, sizeof(path), dir, "wards.txt"); if (file_exists(path)) load_wards(db, path);
    path_join(path, sizeof(path), dir, "inpatients.txt"); if (file_exists(path)) load_inpatients(db, path);
    path_join(path, sizeof(path), dir, "drugs.txt"); if (file_exists(path)) load_drugs(db, path);
    path_join(path, sizeof(path), dir, "druglogs.txt"); if (file_exists(path)) load_druglogs(db, path);
    return 1;
}

int save_all(Database *db, const char *dir) {
    char path[256];
    path_join(path, sizeof(path), dir, "patients.txt"); save_patients(db, path);
    path_join(path, sizeof(path), dir, "doctors.txt"); save_doctors(db, path);
    path_join(path, sizeof(path), dir, "registrations.txt"); save_regs(db, path);
    path_join(path, sizeof(path), dir, "visits.txt"); save_visits(db, path);
    path_join(path, sizeof(path), dir, "exams.txt"); save_exams(db, path);
    path_join(path, sizeof(path), dir, "wards.txt"); save_wards(db, path);
    path_join(path, sizeof(path), dir, "inpatients.txt"); save_inpatients(db, path);
    path_join(path, sizeof(path), dir, "drugs.txt"); save_drugs(db, path);
    path_join(path, sizeof(path), dir, "druglogs.txt"); save_druglogs(db, path);
    return 1;
}

void ensure_seed_data(Database *db) {
    if (!db->patients) {
        int i;
        const char *genders[] = {"男", "女"};
        const char *ins[] = {"城镇医保", "新农合", "自费"};
        for (i = 1; i <= 130; ++i) {
            Patient *p = (Patient*)malloc(sizeof(Patient));
            p->id = i;
            snprintf(p->name, sizeof(p->name), "患者%03d", i <= 5 ? 1 : i);
            strcpy(p->gender, genders[i % 2]);
            snprintf(p->birth, sizeof(p->birth), "199%d-0%d-%02d", i % 10, (i % 9) + 1, (i % 28) + 1);
            snprintf(p->phone, sizeof(p->phone), "1380000%04d", i);
            strcpy(p->insurance, ins[i % 3]);
            p->archived = 0;
            append_patient(db, p);
        }
    }
    if (!db->doctors) {
        const char *depts[] = {"内科", "外科", "儿科", "骨科", "眼科"};
        const char *titles[] = {"住院医师", "主治医师", "副主任医师", "主任医师"};
        int i;
        for (i = 1; i <= 20; ++i) {
            Doctor *d = (Doctor*)malloc(sizeof(Doctor));
            d->id = i;
            snprintf(d->name, sizeof(d->name), "医生%02d", i <= 2 ? 1 : i);
            strcpy(d->dept, depts[(i - 1) % 5]);
            strcpy(d->title, titles[(i - 1) % 4]);
            d->archived = 0;
            append_doctor(db, d);
        }
    }
    if (!db->wards) {
        const char *types[] = {"普通病房", "监护病房", "康复病房"};
        const char *depts[] = {"内科", "外科", "骨科"};
        int i;
        for (i = 1; i <= 3; ++i) {
            Ward *w = (Ward*)malloc(sizeof(Ward));
            w->id = i;
            strcpy(w->wardType, types[i - 1]);
            strcpy(w->dept, depts[i - 1]);
            w->bedCount = 20;
            w->occupiedBeds = (i == 1 ? 12 : (i == 2 ? 10 : 8));
            w->maintenanceBeds = 1;
            append_ward(db, w);
        }
    }
    if (!db->inpatients) {
        int i;
        for (i = 1; i <= 30; ++i) {
            Inpatient *ip = (Inpatient*)malloc(sizeof(Inpatient));
            ip->id = i;
            ip->patientId = i;
            ip->wardId = (i % 3) + 1;
            ip->bedNo = (i % 18) + 1;
            snprintf(ip->admitDate, sizeof(ip->admitDate), "2026-03-%02d", (i % 28) + 1);
            snprintf(ip->expectedDischarge, sizeof(ip->expectedDischarge), "2026-04-%02d", (i % 28) + 1);
            ip->totalCost = 1000 + i * 86.5;
            append_inpatient(db, ip);
        }
    }
    if (!db->drugs) {
        const char *types[] = {"抗生素", "止痛药", "降压药", "消化药", "眼科药"};
        const char *depts[] = {"内科", "外科", "儿科", "骨科", "眼科"};
        int i;
        for (i = 1; i <= 20; ++i) {
            Drug *dr = (Drug*)malloc(sizeof(Drug));
            dr->id = i;
            snprintf(dr->genericName, sizeof(dr->genericName), "通用名%02d", i);
            snprintf(dr->brandName, sizeof(dr->brandName), "商品名%02d", i);
            snprintf(dr->alias, sizeof(dr->alias), "别名%02d", i);
            strcpy(dr->type, types[(i - 1) % 5]);
            strcpy(dr->dept, depts[(i - 1) % 5]);
            dr->price = 10.0 + i * 1.5;
            dr->stock = 80 + i * 3;
            append_drug(db, dr);
        }
    }
    if (!db->registrations) {
        int i;
        const char *depts[] = {"内科", "外科", "儿科", "骨科", "眼科"};
        for (i = 1; i <= 30; ++i) {
            Registration *r = (Registration*)malloc(sizeof(Registration));
            r->id = i;
            r->patientId = i;
            r->doctorId = (i % 20) + 1;
            strcpy(r->dept, depts[(i - 1) % 5]);
            snprintf(r->date, sizeof(r->date), "2026-03-%02d", (i % 28) + 1);
            strcpy(r->type, (i % 5 == 0) ? "专家" : "普通");
            strcpy(r->status, (i % 4 == 0) ? "未就诊" : "已就诊");
            append_reg(db, r);
        }
    }
    if (!db->visits) {
        int i;
        for (i = 1; i <= 20; ++i) {
            Visit *v = (Visit*)malloc(sizeof(Visit));
            v->id = i;
            v->regId = i;
            snprintf(v->diagnosis, sizeof(v->diagnosis), "诊断结果%d", i);
            snprintf(v->examItems, sizeof(v->examItems), "血常规/影像项目%d", i);
            snprintf(v->prescription, sizeof(v->prescription), "处方药品%d", (i % 20) + 1);
            append_visit(db, v);
        }
    }
    if (!db->exams) {
        int i;
        for (i = 1; i <= 20; ++i) {
            Exam *e = (Exam*)malloc(sizeof(Exam));
            e->id = i;
            e->patientId = i;
            e->doctorId = (i % 20) + 1;
            snprintf(e->code, sizeof(e->code), "EX%03d", i);
            snprintf(e->itemName, sizeof(e->itemName), "检查项目%02d", i);
            snprintf(e->execTime, sizeof(e->execTime), "2026-03-%02d", (i % 28) + 1);
            e->fee = 50 + i * 10;
            snprintf(e->result, sizeof(e->result), "结果正常%d", i);
            append_exam(db, e);
        }
    }
    if (!db->drugLogs) {
        int i;
        for (i = 1; i <= 30; ++i) {
            DrugLog *l = (DrugLog*)malloc(sizeof(DrugLog));
            l->id = i;
            l->drugId = (i % 20) + 1;
            strcpy(l->operation, (i % 2) ? "入库" : "出库");
            l->quantity = 5 + i;
            snprintf(l->operatorName, sizeof(l->operatorName), "药房管理员%02d", (i % 3) + 1);
            snprintf(l->date, sizeof(l->date), "2026-03-%02d", (i % 28) + 1);
            append_druglog(db, l);
        }
    }
}
