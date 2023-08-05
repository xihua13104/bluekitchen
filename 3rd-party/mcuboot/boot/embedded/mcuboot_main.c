
#include "bootutil/bootutil.h"
#include "bootutil/bootutil_log.h"
#include "bootutil/image.h"
#include "mcuboot_config/mcuboot_logging.h"
#include "port/boot_startup_port.h"

#ifdef MCUBOOT_USE_MBED_TLS
#define CRYPTO_HEAP_SIZE (1024 * 16)
static unsigned char crypto_heap[CRYPTO_HEAP_SIZE];
#endif

int main_boot( void )
{
	struct boot_rsp rsp;
	fih_int fih_rc = FIH_FAILURE;

	BOOT_LOG_INF("Starting bootloader...");
#ifdef MCUBOOT_USE_MBED_TLS
    mbedtls_memory_buffer_alloc_init(crypto_heap, sizeof(crypto_heap));
#else
    /* Tinycrypt does not require a heap.
     * However, if MCUBoot loader is used, this needs implementation. */
#endif
	FIH_CALL(boot_go, fih_rc, &rsp);
	if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
		BOOT_LOG_ERR("Unable to find bootable image.");
		FIH_PANIC;
	}
	boot_port_startup(&rsp);

	BOOT_LOG_ERR("Returned after image startup. Should never get here.");
	while (1);
}
