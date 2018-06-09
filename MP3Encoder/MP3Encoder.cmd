/*************************************************************************/
/* MP3Encoder.cmd 							 */
/*************************************************************************/


SECTIONS
{
	.tables >  SDRAM

}

SECTIONS
{

	.boot_load :> BOOT_SEC
	$BRID > IRAM

}

-l C:\Users\erdou\workspace_v5_5\MP3Encoder\lib\DEC6713BSL.lib
-l C:\Users\erdou\workspace_v5_5\MP3Encoder\lib\dec6713_edma_aic23_l67.l67
-l C:\Users\erdou\workspace_v5_5\MP3Encoder\lib\c6x1x_edma_mcasp.l67

SECTIONS {
   .vers > IRAM /* version information */
}

