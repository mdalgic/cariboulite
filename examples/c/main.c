#include <stdio.h>
#include <cariboulite.h>

static cariboulite_lib_version_st version = {0};
static uint32_t serial_number = 0;

int main ()
{
	cariboulite_init(false, cariboulite_log_level_none);
    cariboulite_get_lib_version(&version);
    
    printf("Version: %02d.%02d.%02d\n", version.major_version, version.minor_version, version.revision);
    
    serial_number = cariboulite_get_sn();
    
    printf("Serial Number: %08X\n", serial_number);
    
    cariboulite_close();
	return 0;
}