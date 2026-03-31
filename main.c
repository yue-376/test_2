#include "menu.h"

int main(void) {
    Database db;
    const char *dataDir = "../data";
    init_database(&db);
    load_all(&db, dataDir);
    ensure_seed_data(&db);
    save_all(&db, dataDir);
    main_menu(&db, dataDir);
    free_database(&db);
    return 0;
}
