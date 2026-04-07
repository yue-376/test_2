#include "menu.h"

static int count_patients(Database *db) { int c=0; Patient *p=db->patients; while(p){c++;p=p->next;} return c; }
static int count_doctors(Database *db) { int c=0; Doctor *p=db->doctors; while(p){c++;p=p->next;} return c; }
static int count_regs(Database *db) { int c=0; Registration *p=db->registrations; while(p){c++;p=p->next;} return c; }
static int count_inpatients(Database *db) { int c=0; Inpatient *p=db->inpatients; while(p){c++;p=p->next;} return c; }
static int count_drugs(Database *db) { int c=0; Drug *p=db->drugs; while(p){c++;p=p->next;} return c; }
static int read_line_or_back(const char *prompt, char *buf, int size) {
    read_line(prompt, buf, size);
    if (strcmp(buf, "0") == 0) {
        return 0;
    }
    return 1;
}

static int read_int_or_back(const char *prompt, int min, int max, int *out) {
    char line[64];
    char *end;
    long value;
    while (1) {
        read_line(prompt, line, sizeof(line));
        if (strcmp(line, "0") == 0) return 0;
        value = strtol(line, &end, 10);
        if (*line != '\0' && *end == '\0' && value >= min && value <= max) {
            *out = (int)value;
            return 1;
        }
        printf("输入无效，请输入 %d ~ %d 的整数，或输入0返回上一步。\n", min, max);
    }
}

static void list_patients(Database *db) {
    Patient *p = db->patients;
    printf("\n%-6s %-12s %-6s %-12s %-14s %-10s\n", "病历号", "姓名", "性别", "出生日期", "联系方式", "医保");
    while (p) {
        printf("%-6d %-12s %-6s %-12s %-14s %-10s\n", p->id, p->name, p->gender, p->birth, p->phone, p->insurance);
        p = p->next;
    }
}


static int patient_has_related_records(Database *db, int patientId) {
    Registration *r;
    Exam *e;
    Inpatient *ip;
    for (r = db->registrations; r; r = r->next) if (r->patientId == patientId) return 1;
    for (e = db->exams; e; e = e->next) if (e->patientId == patientId) return 1;
    for (ip = db->inpatients; ip; ip = ip->next) if (ip->patientId == patientId) return 1;
    return 0;
}

static void delete_patient(Database *db, const char *dataDir) {
    int id = read_int("要删除的患者病历号(输入0返回): ", 0, 1000000);
    Patient *prev = NULL;
    Patient *cur = db->patients;
    char confirm[16];
    if (id == 0) { printf("已返回上一步。\n"); return; }

    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }

    if (!cur) {
        printf("患者不存在。\n");
        return;
    }

    if (patient_has_related_records(db, id)) {
        printf("删除失败：该患者存在挂号/检查/住院关联记录，请先处理关联数据。\n");
        return;
    }

    printf("确认删除患者[%d] %s ? (y/n): ", cur->id, cur->name);
    read_line(NULL, confirm, sizeof(confirm));
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) {
        printf("已取消删除。\n");
        return;
    }

    if (prev) prev->next = cur->next;
    else db->patients = cur->next;
    free(cur);
    save_all(db, dataDir);
    printf("删除成功。\n");
}

static void add_patient(Database *db, const char *dataDir) {
    Patient *p = (Patient*)malloc(sizeof(Patient));
    int step = 0;
    p->id = next_patient_id(db);
    while (step < 5) {
        int ok = 0;
        if (step == 0) ok = read_line_or_back("姓名(输入0返回上一步): ", p->name, sizeof(p->name));
        else if (step == 1) ok = read_line_or_back("性别(输入0返回上一步): ", p->gender, sizeof(p->gender));
        else if (step == 2) ok = read_line_or_back("出生日期(YYYY-MM-DD，输入0返回上一步): ", p->birth, sizeof(p->birth));
        else if (step == 3) ok = read_line_or_back("联系方式(输入0返回上一步): ", p->phone, sizeof(p->phone));
        else ok = read_line_or_back("医保类型(输入0返回上一步): ", p->insurance, sizeof(p->insurance));

        if (ok) {
            step++;
        } else if (step == 0) {
            printf("已返回上一步。\n");
            free(p);
            return;
        } else {
            printf("已返回上一项输入。\n");
            step--;
        }
    }
    p->archived = 0;
    p->next = NULL;
    if (!db->patients) db->patients = p; else { Patient *q = db->patients; while (q->next) q = q->next; q->next = p; }
    save_all(db, dataDir);
    printf("添加成功，病历号=%d\n", p->id);
}

static int count_patient_regs_same_day_dept(Database *db, int patientId, const char *date, const char *dept) {
    int cnt = 0; Registration *r = db->registrations;
    while (r) { if (r->patientId == patientId && strcmp(r->date, date) == 0 && strcmp(r->dept, dept) == 0) cnt++; r = r->next; }
    return cnt;
}
static int count_patient_regs_same_day(Database *db, int patientId, const char *date) {
    int cnt = 0; Registration *r = db->registrations;
    while (r) { if (r->patientId == patientId && strcmp(r->date, date) == 0) cnt++; r = r->next; }
    return cnt;
}

static void add_registration(Database *db, const char *dataDir) {
    Registration *r;
    int patientId = 0, doctorId = 0;
    int step = 0;
    char dept[SMALL_LEN], date[DATE_LEN], type[SMALL_LEN];
    while (step < 5) {
        int ok = 0;
        if (step == 0) {
            ok = read_int_or_back("患者病历号(输入0返回上一步): ", 1, 1000000, &patientId);
            if (ok && !find_patient(db, patientId)) { printf("患者不存在。\n"); ok = 0; }
        } else if (step == 1) {
            ok = read_int_or_back("医生工号(输入0返回上一步): ", 1, 1000000, &doctorId);
            if (ok && !find_doctor(db, doctorId)) { printf("医生不存在。\n"); ok = 0; }
        } else if (step == 2) ok = read_line_or_back("科室(输入0返回上一步): ", dept, sizeof(dept));
        else if (step == 3) ok = read_line_or_back("挂号日期(YYYY-MM-DD，输入0返回上一步): ", date, sizeof(date));
        else ok = read_line_or_back("挂号类型(普通/专家，输入0返回上一步): ", type, sizeof(type));

        if (ok) step++;
        else if (step == 0) { printf("已返回上一步。\n"); return; }
        else { printf("已返回上一项输入。\n"); step--; }
    }
    if (count_patient_regs_same_day_dept(db, patientId, date, dept) >= 1) { printf("同一患者同一天同一科室最多挂号1次。\n"); return; }
    if (count_patient_regs_same_day(db, patientId, date) >= 3) { printf("同一患者同一天最多挂号3次。\n"); return; }
    r = (Registration*)malloc(sizeof(Registration));
    r->id = next_registration_id(db);
    r->patientId = patientId;
    r->doctorId = doctorId;
    strcpy(r->dept, dept);
    strcpy(r->date, date);
    strcpy(r->type, type);
    strcpy(r->status, "未就诊");
    r->next = NULL;
    if (!db->registrations) db->registrations = r; else { Registration *q = db->registrations; while (q->next) q = q->next; q->next = r; }
    save_all(db, dataDir);
    printf("挂号成功，挂号编号=%d\n", r->id);
}

static int registration_has_visit(Database *db, int regId) {
    Visit *v = db->visits;
    while (v) {
        if (v->regId == regId) return 1;
        v = v->next;
    }
    return 0;
}

static void delete_registration(Database *db, const char *dataDir) {
    int id = read_int("要删除的挂号编号(输入0返回): ", 0, 1000000);
    Registration *prev = NULL;
    Registration *cur = db->registrations;
    char confirm[16];
    if (id == 0) { printf("已返回上一步。\n"); return; }
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) { printf("挂号记录不存在。\n"); return; }
    if (registration_has_visit(db, id)) {
        printf("删除失败：该挂号记录已有关联看诊记录，请先删除看诊记录。\n");
        return;
    }
    printf("确认删除挂号[%d] 患者%d %s ? (y/n): ", cur->id, cur->patientId, cur->date);
    read_line(NULL, confirm, sizeof(confirm));
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) { printf("已取消删除。\n"); return; }
    if (prev) prev->next = cur->next; else db->registrations = cur->next;
    free(cur);
    save_all(db, dataDir);
    printf("删除成功。\n");
}

static void add_visit(Database *db, const char *dataDir) {
    int regId = 0;
    int step = 0;
    char diagnosis[TEXT_LEN], examItems[TEXT_LEN], prescription[TEXT_LEN];
    Registration *r = find_registration(db, regId);
    Visit *v;
    while (step < 4) {
        int ok = 0;
        if (step == 0) {
            ok = read_int_or_back("挂号编号(输入0返回上一步): ", 1, 1000000, &regId);
            if (ok) {
                r = find_registration(db, regId);
                if (!r) { printf("挂号记录不存在。\n"); ok = 0; }
            }
        } else if (step == 1) ok = read_line_or_back("诊断结果(输入0返回上一步): ", diagnosis, sizeof(diagnosis));
        else if (step == 2) ok = read_line_or_back("检查项目(输入0返回上一步): ", examItems, sizeof(examItems));
        else ok = read_line_or_back("处方信息(输入0返回上一步): ", prescription, sizeof(prescription));

        if (ok) step++;
        else if (step == 0) { printf("已返回上一步。\n"); return; }
        else { printf("已返回上一项输入。\n"); step--; }
    }
    v = (Visit*)malloc(sizeof(Visit));
    v->id = next_visit_id(db);
    v->regId = regId;
    strcpy(v->diagnosis, diagnosis);
    strcpy(v->examItems, examItems);
    strcpy(v->prescription, prescription);
    v->next = NULL;
    if (!db->visits) db->visits = v; else { Visit *q = db->visits; while (q->next) q = q->next; q->next = v; }
    strcpy(r->status, "已就诊");
    save_all(db, dataDir);
    printf("看诊记录已添加。\n");
}

static void delete_visit(Database *db, const char *dataDir) {
    int id = read_int("要删除的看诊编号(输入0返回): ", 0, 1000000);
    Visit *prev = NULL;
    Visit *cur = db->visits;
    char confirm[16];
    if (id == 0) { printf("已返回上一步。\n"); return; }
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) { printf("看诊记录不存在。\n"); return; }
    printf("确认删除看诊[%d] (挂号%d) ? (y/n): ", cur->id, cur->regId);
    read_line(NULL, confirm, sizeof(confirm));
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) { printf("已取消删除。\n"); return; }
    if (prev) prev->next = cur->next; else db->visits = cur->next;
    free(cur);
    save_all(db, dataDir);
    printf("删除成功。\n");
}

static void add_exam(Database *db, const char *dataDir) {
    Exam *e = (Exam*)malloc(sizeof(Exam));
    int step = 0;
    char feeBuf[64];
    e->id = next_exam_id(db);
    while (step < 7) {
        int ok = 0;
        if (step == 0) {
            ok = read_int_or_back("患者病历号(输入0返回上一步): ", 1, 1000000, &e->patientId);
            if (ok && !find_patient(db, e->patientId)) { printf("患者不存在。\n"); ok = 0; }
        } else if (step == 1) {
            ok = read_int_or_back("医生工号(输入0返回上一步): ", 1, 1000000, &e->doctorId);
            if (ok && !find_doctor(db, e->doctorId)) { printf("医生不存在。\n"); ok = 0; }
        } else if (step == 2) ok = read_line_or_back("检查编码(输入0返回上一步): ", e->code, sizeof(e->code));
        else if (step == 3) ok = read_line_or_back("检查项目名称(输入0返回上一步): ", e->itemName, sizeof(e->itemName));
        else if (step == 4) ok = read_line_or_back("执行时间(输入0返回上一步): ", e->execTime, sizeof(e->execTime));
        else if (step == 5) {
            ok = read_line_or_back("检查费用(输入0返回上一步): ", feeBuf, sizeof(feeBuf));
            if (ok) e->fee = atof(feeBuf);
        } else ok = read_line_or_back("检查结果(输入0返回上一步): ", e->result, sizeof(e->result));

        if (ok) step++;
        else if (step == 0) { printf("已返回上一步。\n"); free(e); return; }
        else { printf("已返回上一项输入。\n"); step--; }
    }
    e->next = NULL;
    if (!db->exams) db->exams = e; else { Exam *q = db->exams; while (q->next) q = q->next; q->next = e; }
    save_all(db, dataDir);
    printf("检查记录已添加。\n");
}

static void delete_exam(Database *db, const char *dataDir) {
    int id = read_int("要删除的检查编号(输入0返回): ", 0, 1000000);
    Exam *prev = NULL;
    Exam *cur = db->exams;
    char confirm[16];
    if (id == 0) { printf("已返回上一步。\n"); return; }
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) { printf("检查记录不存在。\n"); return; }
    printf("确认删除检查[%d] %s ? (y/n): ", cur->id, cur->itemName);
    read_line(NULL, confirm, sizeof(confirm));
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) { printf("已取消删除。\n"); return; }
    if (prev) prev->next = cur->next; else db->exams = cur->next;
    free(cur);
    save_all(db, dataDir);
    printf("删除成功。\n");
}

static void add_inpatient(Database *db, const char *dataDir) {
    Inpatient *ip = (Inpatient*)malloc(sizeof(Inpatient));
    Ward *w;
    int step = 0;
    char buf[64];
    ip->id = next_inpatient_id(db);
    while (step < 6) {
        int ok = 0;
        if (step == 0) {
            ok = read_int_or_back("患者病历号(输入0返回上一步): ", 1, 1000000, &ip->patientId);
            if (ok && !find_patient(db, ip->patientId)) { printf("患者不存在。\n"); ok = 0; }
        } else if (step == 1) ok = read_int_or_back("病房编号(输入0返回上一步): ", 1, 1000000, &ip->wardId);
        else if (step == 2) ok = read_int_or_back("床位号(输入0返回上一步): ", 1, 1000, &ip->bedNo);
        else if (step == 3) ok = read_line_or_back("入院时间(输入0返回上一步): ", ip->admitDate, sizeof(ip->admitDate));
        else if (step == 4) ok = read_line_or_back("预计出院时间(输入0返回上一步): ", ip->expectedDischarge, sizeof(ip->expectedDischarge));
        else {
            ok = read_line_or_back("预估住院费用(输入0返回上一步): ", buf, sizeof(buf));
            if (ok) ip->totalCost = atof(buf);
        }

        if (ok) step++;
        else if (step == 0) { printf("已返回上一步。\n"); free(ip); return; }
        else { printf("已返回上一项输入。\n"); step--; }
    }
    w = find_ward(db, ip->wardId);
    if (!w) { printf("病房不存在。\n"); free(ip); return; }
    if (w->occupiedBeds + w->maintenanceBeds >= w->bedCount) { printf("病房没有空闲床位。\n"); free(ip); return; }
    w->occupiedBeds++;
    ip->next = NULL;
    if (!db->inpatients) db->inpatients = ip; else { Inpatient *q = db->inpatients; while (q->next) q = q->next; q->next = ip; }
    save_all(db, dataDir);
    printf("住院登记成功，编号=%d\n", ip->id);
}

static void delete_inpatient(Database *db, const char *dataDir) {
    int id = read_int("要删除的住院编号(输入0返回): ", 0, 1000000);
    Inpatient *prev = NULL;
    Inpatient *cur = db->inpatients;
    Ward *w;
    char confirm[16];
    if (id == 0) { printf("已返回上一步。\n"); return; }
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) { printf("住院记录不存在。\n"); return; }
    printf("确认删除住院[%d] 患者%d ? (y/n): ", cur->id, cur->patientId);
    read_line(NULL, confirm, sizeof(confirm));
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) { printf("已取消删除。\n"); return; }
    w = find_ward(db, cur->wardId);
    if (w && w->occupiedBeds > 0) w->occupiedBeds--;
    if (prev) prev->next = cur->next; else db->inpatients = cur->next;
    free(cur);
    save_all(db, dataDir);
    printf("删除成功。\n");
}

static void registration_management_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n--- 挂号管理 ---\n");
        printf("1. 新增挂号记录\n");
        printf("2. 删除挂号记录\n");
        printf("0. 返回上级菜单\n");
        choice = read_int("请选择: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) add_registration(db, dataDir);
        else delete_registration(db, dataDir);
        pause_and_wait();
    }
}

static void visit_management_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n--- 看诊管理 ---\n");
        printf("1. 新增看诊记录\n");
        printf("2. 删除看诊记录\n");
        printf("0. 返回上级菜单\n");
        choice = read_int("请选择: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) add_visit(db, dataDir);
        else delete_visit(db, dataDir);
        pause_and_wait();
    }
}

static void exam_management_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n--- 检查管理 ---\n");
        printf("1. 新增检查记录\n");
        printf("2. 删除检查记录\n");
        printf("0. 返回上级菜单\n");
        choice = read_int("请选择: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) add_exam(db, dataDir);
        else delete_exam(db, dataDir);
        pause_and_wait();
    }
}

static void inpatient_management_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n--- 住院管理 ---\n");
        printf("1. 新增住院记录\n");
        printf("2. 删除住院记录\n");
        printf("0. 返回上级菜单\n");
        choice = read_int("请选择: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) add_inpatient(db, dataDir);
        else delete_inpatient(db, dataDir);
        pause_and_wait();
    }
}

static void patient_management_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n--- 患者管理 ---\n");
        printf("1. 查看患者列表\n");
        printf("2. 新增患者\n");
        printf("3. 删除患者\n");
        printf("0. 返回上级菜单\n");
        choice = read_int("请选择: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) list_patients(db);
        else if (choice == 2) add_patient(db, dataDir);
        else delete_patient(db, dataDir);
        pause_and_wait();
    }
}

static void drug_inout(Database *db, const char *dataDir) {
    int drugId = read_int("药品编号(输入0返回): ", 0, 1000000);
    Drug *d = find_drug(db, drugId);
    DrugLog *l;
    char op[SMALL_LEN];
    int qty;
    if (drugId == 0) { printf("已返回上一步。\n"); return; }
    if (!d) { printf("药品不存在。\n"); return; }
    if (!read_line_or_back("操作类型(入库/出库，输入0返回): ", op, sizeof(op))) return;
    qty = read_int("数量(输入0返回): ", 0, 1000000);
    if (qty == 0) { printf("已返回上一步。\n"); return; }
    if (strcmp(op, "出库") == 0 && d->stock < qty) { printf("库存不足。\n"); return; }
    l = (DrugLog*)malloc(sizeof(DrugLog));
    l->id = next_druglog_id(db);
    l->drugId = drugId;
    strcpy(l->operation, op);
    l->quantity = qty;
    if (!read_line_or_back("操作人(输入0返回): ", l->operatorName, sizeof(l->operatorName))) { free(l); return; }
    if (!read_line_or_back("日期(输入0返回): ", l->date, sizeof(l->date))) { free(l); return; }
    if (strcmp(op, "出库") == 0) d->stock -= qty; else d->stock += qty;
    l->next = NULL;
    if (!db->drugLogs) db->drugLogs = l; else { DrugLog *q = db->drugLogs; while (q->next) q = q->next; q->next = l; }
    save_all(db, dataDir);
    printf("操作成功，当前库存=%d\n", d->stock);
}

static void patient_report(Database *db) {
    int pid = read_int("输入患者病历号(输入0返回): ", 0, 1000000);
    Patient *p = find_patient(db, pid);
    Registration *r;
    Exam *e;
    Inpatient *ip;
    if (pid == 0) { printf("已返回上一步。\n"); return; }
    if (!p) { printf("患者不存在。\n"); return; }
    printf("\n患者: %s(%d) %s %s %s %s\n", p->name, p->id, p->gender, p->birth, p->phone, p->insurance);
    printf("挂号记录:\n");
    for (r = db->registrations; r; r = r->next) if (r->patientId == pid) printf("  [%d] %s %s 医生%d %s %s\n", r->id, r->date, r->dept, r->doctorId, r->type, r->status);
    printf("检查记录:\n");
    for (e = db->exams; e; e = e->next) if (e->patientId == pid) printf("  [%d] %s %s %.2f %s\n", e->id, e->code, e->itemName, e->fee, e->result);
    printf("住院记录:\n");
    for (ip = db->inpatients; ip; ip = ip->next) if (ip->patientId == pid) printf("  [%d] 病房%d 床位%d %s ~ %s 费用%.2f\n", ip->id, ip->wardId, ip->bedNo, ip->admitDate, ip->expectedDischarge, ip->totalCost);
}

static void doctor_report(Database *db) {
    int did = read_int("输入医生工号(输入0返回): ", 0, 1000000);
    Doctor *d = find_doctor(db, did);
    Registration *r; int count = 0;
    if (did == 0) { printf("已返回上一步。\n"); return; }
    if (!d) { printf("医生不存在。\n"); return; }
    printf("\n医生: %s(%d) %s %s\n", d->name, d->id, d->dept, d->title);
    for (r = db->registrations; r; r = r->next) {
        if (r->doctorId == did) {
            printf("  挂号[%d] 患者%d %s %s %s\n", r->id, r->patientId, r->date, r->type, r->status);
            count++;
        }
    }
    printf("总接诊/挂号关联数量: %d\n", count);
}

static void management_report(Database *db) {
    Ward *w; Drug *d; Inpatient *ip; Exam *e;
    double wardRate, inpatientIncome = 0, examIncome = 0;
    int totalBeds = 0, usedBeds = 0;
    for (w = db->wards; w; w = w->next) { totalBeds += w->bedCount; usedBeds += w->occupiedBeds; }
    for (ip = db->inpatients; ip; ip = ip->next) inpatientIncome += ip->totalCost;
    for (e = db->exams; e; e = e->next) examIncome += e->fee;
    wardRate = totalBeds ? (usedBeds * 100.0 / totalBeds) : 0;
    printf("\n=== 管理视角报表 ===\n");
    printf("患者总数: %d\n", count_patients(db));
    printf("医生总数: %d\n", count_doctors(db));
    printf("挂号记录总数: %d\n", count_regs(db));
    printf("住院人数: %d\n", count_inpatients(db));
    printf("药品种类: %d\n", count_drugs(db));
    printf("床位利用率: %.2f%%\n", wardRate);
    printf("住院费用汇总: %.2f\n", inpatientIncome);
    printf("检查费用汇总: %.2f\n", examIncome);
    printf("药品库存盘点(全部):\n");
    for (d = db->drugs; d; d = d->next) printf("  药品[%d] %s/%s 库存=%d 单价=%.2f 科室=%s\n", d->id, d->genericName, d->brandName, d->stock, d->price, d->dept);
}

void main_menu(Database *db, const char *dataDir) {
    int choice;
    while (1) {
        printf("\n==============================\n");
        printf("  医疗综合管理系统 HIS\n");
        printf("==============================\n");
        printf("1. 患者管理\n");
        printf("2. 挂号管理\n");
        printf("3. 看诊管理\n");
        printf("4. 检查管理\n");
        printf("5. 住院管理\n");
        printf("6. 药品出入库\n");
        printf("7. 患者视角查询\n");
        printf("8. 医护视角查询\n");
        printf("9. 管理视角报表\n");
        printf("0. 保存并退出\n");
        choice = read_int("请选择: ", 0, 9);
        switch (choice) {
            case 1: patient_management_menu(db, dataDir); break;
            case 2: registration_management_menu(db, dataDir); break;
            case 3: visit_management_menu(db, dataDir); break;
            case 4: exam_management_menu(db, dataDir); break;
            case 5: inpatient_management_menu(db, dataDir); break;
            case 6: drug_inout(db, dataDir); pause_and_wait(); break;
            case 7: patient_report(db); pause_and_wait(); break;
            case 8: doctor_report(db); pause_and_wait(); break;
            case 9: management_report(db); pause_and_wait(); break;
            case 0: save_all(db, dataDir); printf("数据已保存。\n"); return;
        }
    }
}
