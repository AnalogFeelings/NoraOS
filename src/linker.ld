/* We want the symbol _start to be our entry point */
ENTRY(KiSystemStartup)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS {
	text PT_LOAD FLAGS((1 << 0) | (1 << 2)); /* Execute + Read */
	rodata PT_LOAD FLAGS((1 << 2));			 /* Read only */
	data PT_LOAD FLAGS((1 << 1) | (1 << 2)); /* Write + Read */
}

SECTIONS {
	/* We wanna be placed in the topmost 2GiB of the address space, for
	 * optimisations */
	/* and because that is what the limine spec mandates. */
	/* Any address in this region will do, but often 0xffffffff80000000 is
	 * chosen as */
	/* that is the beginning of the region. */
	. = 0xffffffff80000000;

	.text : {*(.text.text.*)}
		: text

			  /* Move to the next memory page for .rodata */
			  . += CONSTANT(MAXPAGESIZE);

			  .rodata : {*(.rodata.rodata.*)} : rodata

			  /* Move to the next memory page for .data */
			  . += CONSTANT(MAXPAGESIZE);

	.data : {*(.data.data.*)} : data

									.bss : {*(COMMON) * (.bss.bss.*)} : data
}
