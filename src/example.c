#include "escdf_group.h"

void *main()
{
    group_file_setup(FILE_ROOT, NULL);
    handle_r = escdf_open(FILE_ROOT, NULL);

    escdf_group_specs_register(&group_specs);

    group_system  = escdf_group_create(system_specs.group_id, handle_r, "system");

    escdf_close(handle_r);
}
