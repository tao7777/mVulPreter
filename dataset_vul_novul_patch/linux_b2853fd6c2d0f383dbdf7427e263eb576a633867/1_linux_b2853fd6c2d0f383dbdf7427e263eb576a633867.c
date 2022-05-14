int ib_update_cm_av(struct ib_cm_id *id, const u8 *smac, const u8 *alt_smac)
{
	struct cm_id_private *cm_id_priv;
	cm_id_priv = container_of(id, struct cm_id_private, id);
	if (smac != NULL)
		memcpy(cm_id_priv->av.smac, smac, sizeof(cm_id_priv->av.smac));
	if (alt_smac != NULL)
		memcpy(cm_id_priv->alt_av.smac, alt_smac,
		       sizeof(cm_id_priv->alt_av.smac));
	return 0;
}
