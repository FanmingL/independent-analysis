MP3Encoder.out  /* input COFF file */
-a /* create ASCII image */
-image /* Create a memory image (no discontinuities) */
-memwidth 8 /* Width of ROM/Flash memory */
-map MP3Encoder_hex.map /* create a hex map file */
-boot /* create a boot table for all initialized sects*/
-bootorg 0x90000400 /* address of the boot/copy?table */
-bootsection .boot_load 0x90000000 /* section containing our asm boot routine */
ROMS
{
FLASH: org = 0x90000000, len = 0x0040000, romwidth = 8, files = {MP3Encoder.hex}
}