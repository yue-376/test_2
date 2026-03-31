#include "report.h"

static double inpatient_total_cost(HospitalDB *db, int patientId) {
    double s = 0;
    Inpatient *p = db->inpatients;
    while (p) { if (p->patientId == patientId) s += p->cost; p = p->next; }
    return s;
}

void print_patient_full_record(HospitalDB *db, int patientId) {
    Patient *p = find_patient(db, patientId);
    if (!p) { printf("未找到该患者。\n"); return; }
    printf("\n===== 患者视角 =====\n");
    printf("病历号:%d 姓名:%s 性别:%s 出生:%s 电话:%s 医保:%s\n", p->id, p->name, p->gender, p->birth, p->phone, p->insurance);

    printf("\n[挂号记录]\n");
    Registration *r = db->registrations;
    while (r) {
        if (r->patientId == patientId) {
            Department *d = find_department(db, r->deptId);
            Doctor *doc = find_doctor(db, r->doctorId);
            printf("挂号号:%d 日期:%s 科室:%s 医生:%s 类型:%s 状态:%s\n",
                   r->id, r->regDate, d ? d->name : "未知", doc ? doc->name : "未知", r->regType, r->status);
        }
        r = r->next;
    }

    printf("\n[检查记录]\n");
    Exam *e = db->exams;
    while (e) {
        if (e->patientId == patientId) {
            printf("检查号:%d 编码:%s 名称:%s 时间:%s 费用:%.2f 结果:%s\n",
                   e->id, e->code, e->name, e->execTime, e->fee, e->result);
        }
        e = e->next;
    }

    printf("\n[住院记录]\n");
    Inpatient *in = db->inpatients;
    while (in) {
        if (in->patientId == patientId) {
            printf("住院号:%d 病房:%d 床位:%d 入院:%s 预计/实际出院:%s 费用:%.2f\n",
                   in->id, in->wardId, in->bedNo, in->admitDate, in->dischargePlan, in->cost);
        }
        in = in->next;
    }
    printf("住院费用合计: %.2f\n", inpatient_total_cost(db, patientId));
}

void print_doctor_stats(HospitalDB *db, int doctorId) {
    Doctor *d = find_doctor(db, doctorId);
    int regCount = 0, examCount = 0, dispCount = 0;
    Registration *r = db->registrations;
    Exam *e = db->exams;
    Dispense *ds = db->dispenses;
    if (!d) { printf("未找到该医生。\n"); return; }

    while (r) { if (r->doctorId == doctorId) regCount++; r = r->next; }
    while (e) { if (e->doctorId == doctorId) examCount++; e = e->next; }
    while (ds) { if (ds->doctorId == doctorId) dispCount++; ds = ds->next; }

    printf("\n===== 医护视角 =====\n");
    printf("工号:%d 姓名:%s 科室ID:%d 职称:%s\n", d->id, d->name, d->deptId, d->title);
    printf("接诊患者数量: %d\n", regCount);
    printf("执行检查数量: %d\n", examCount);
    printf("处方发药统计: %d\n", dispCount);
    printf("说明: 日/周/月接诊量可按日期字段继续细分，这里给出总量版，便于课程设计演示。\n");
}

void print_management_report(HospitalDB *db) {
    double revenue = 0;
    int deptIncome[16] = {0};
    double deptRevenue[16] = {0};
    int lowStock = 0;
    Inpatient *in = db->inpatients;
    Exam *e = db->exams;
    Registration *r = db->registrations;
    Drug *drug = db->drugs;
    Ward *w = db->wards;

    while (in) { revenue += in->cost; in = in->next; }
    while (e) { revenue += e->fee; e = e->next; }
    while (r) { deptIncome[r->deptId]++; deptRevenue[r->deptId] += (strcmp(r->regType, "专家") == 0 ? 30 : 10); revenue += (strcmp(r->regType, "专家") == 0 ? 30 : 10); r = r->next; }

    printf("\n===== 管理视角 =====\n");
    printf("全院营收估算(挂号+检查+住院): %.2f\n", revenue);

    printf("\n[科室收入排名(粗略版)]\n");
    for (int i = 1; i <= 5; ++i) {
        Department *d = find_department(db, i);
        printf("科室:%s  挂号笔数:%d  挂号收入:%.2f\n", d ? d->name : "未知", deptIncome[i], deptRevenue[i]);
    }

    printf("\n[药品库存盘点]\n");
    while (drug) {
        if (drug->stock < 30) lowStock++;
        printf("药品:%s 库存:%d\n", drug->genericName, drug->stock);
        drug = drug->next;
    }
    printf("低库存药品数量: %d\n", lowStock);

    printf("\n[床位使用率]\n");
    while (w) {
        printf("病房ID:%d 类型:%s 使用率:%.2f%% (%d/%d)\n", w->id, w->type,
               w->totalBeds ? 100.0 * w->occupiedBeds / w->totalBeds : 0.0,
               w->occupiedBeds, w->totalBeds);
        w = w->next;
    }
}

void report_menu(HospitalDB *db) {
    int ch;
    while (1) {
        printf("\n===== 报表与统计查询 =====\n");
        printf("1. 患者视角查询\n2. 医护视角查询\n3. 管理视角查询\n0. 返回上一级\n");
        ch = read_int("请选择: ", 0, 3);
        switch (ch) {
            case 1: {
                int patientId = read_int("输入患者病历号: ", 1, 9999999);
                print_patient_full_record(db, patientId);
                printf("\n按回车继续..."); getchar();
                break;
            }
            case 2: {
                int doctorId = read_int("输入医生工号: ", 1, 9999999);
                print_doctor_stats(db, doctorId);
                printf("\n按回车继续..."); getchar();
                break;
            }
            case 3:
                print_management_report(db);
                printf("\n按回车继续..."); getchar();
                break;
            case 0: return;
        }
    }
}
