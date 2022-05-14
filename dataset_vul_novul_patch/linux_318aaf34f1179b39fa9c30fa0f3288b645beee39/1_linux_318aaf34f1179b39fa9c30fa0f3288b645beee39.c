static void sas_eh_defer_cmd(struct scsi_cmnd *cmd)
{
	struct domain_device *dev = cmd_to_domain_dev(cmd);
	struct sas_ha_struct *ha = dev->port->ha;
	struct sas_task *task = TO_SAS_TASK(cmd);
	if (!dev_is_sata(dev)) {
		sas_eh_finish_cmd(cmd);
		return;
	}
	/* report the timeout to libata */
	sas_end_task(cmd, task);
	list_move_tail(&cmd->eh_entry, &ha->eh_ata_q);
}
