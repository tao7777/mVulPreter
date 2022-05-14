static int command_read(struct pci_dev *dev, int offset, u16 *value, void *data)
/* Bits guests are allowed to control in permissive mode. */
#define PCI_COMMAND_GUEST (PCI_COMMAND_MASTER|PCI_COMMAND_SPECIAL| \
			   PCI_COMMAND_INVALIDATE|PCI_COMMAND_VGA_PALETTE| \
			   PCI_COMMAND_WAIT|PCI_COMMAND_FAST_BACK)

static void *command_init(struct pci_dev *dev, int offset)
 {
	struct pci_cmd_info *cmd = kmalloc(sizeof(*cmd), GFP_KERNEL);
	int err;

	if (!cmd)
		return ERR_PTR(-ENOMEM);

	err = pci_read_config_word(dev, PCI_COMMAND, &cmd->val);
	if (err) {
		kfree(cmd);
		return ERR_PTR(err);
 	}
 
	return cmd;
}

static int command_read(struct pci_dev *dev, int offset, u16 *value, void *data)
{
	int ret = pci_read_config_word(dev, offset, value);
	const struct pci_cmd_info *cmd = data;

	*value &= PCI_COMMAND_GUEST;
	*value |= cmd->val & ~PCI_COMMAND_GUEST;

 	return ret;
 }
