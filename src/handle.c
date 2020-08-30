#include <bfd.h>
#include <stdbool.h>
#include <stdio.h>

#include "general.h"

bfd *raw_handle(char *file) {

	static int initialized = false;

	//initialize libbfd's  internal data structures
	if (initialized) {
		bfd_init();
		initialized = true;
	}

	//open the binary 
	bfd *handle = bfd_openr(file, NULL);
	if (handle == NULL)
		error("Opening binary failed\n");
	
	return handle;
}

bfd *validated_handle(bfd *handle) {
	
	/* validate if file is executable, 
	 * relocatable object or shared library
	 */
	if (bfd_check_format(handle, bfd_object) == false)
		error("File must be executable");
	bfd_set_error(bfd_error_no_error);

	//validate binary format
	if (bfd_get_flavour(handle) == bfd_target_unknown_flavour)
		error("Binary format not recognized");

	return handle;
}

