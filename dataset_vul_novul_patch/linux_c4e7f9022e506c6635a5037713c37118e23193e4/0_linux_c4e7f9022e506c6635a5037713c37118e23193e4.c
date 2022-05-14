 static int kvm_vm_ioctl_assign_device(struct kvm *kvm,
 				      struct kvm_assigned_pci_dev *assigned_dev)
 {
 	int r = 0, idx;
 	struct kvm_assigned_dev_kernel *match;
 	struct pci_dev *dev;
	u8 header_type;
 
 	if (!(assigned_dev->flags & KVM_DEV_ASSIGN_ENABLE_IOMMU))
 		return -EINVAL;

	mutex_lock(&kvm->lock);
	idx = srcu_read_lock(&kvm->srcu);

	match = kvm_find_assigned_dev(&kvm->arch.assigned_dev_head,
				      assigned_dev->assigned_dev_id);
	if (match) {
		/* device already assigned */
		r = -EEXIST;
		goto out;
	}

	match = kzalloc(sizeof(struct kvm_assigned_dev_kernel), GFP_KERNEL);
	if (match == NULL) {
		printk(KERN_INFO "%s: Couldn't allocate memory\n",
		       __func__);
		r = -ENOMEM;
		goto out;
	}
	dev = pci_get_domain_bus_and_slot(assigned_dev->segnr,
				   assigned_dev->busnr,
				   assigned_dev->devfn);
	if (!dev) {
		printk(KERN_INFO "%s: host device not found\n", __func__);
 		r = -EINVAL;
 		goto out_free;
 	}

	/* Don't allow bridges to be assigned */
	pci_read_config_byte(dev, PCI_HEADER_TYPE, &header_type);
	if ((header_type & PCI_HEADER_TYPE) != PCI_HEADER_TYPE_NORMAL) {
		r = -EPERM;
		goto out_put;
	}

	r = probe_sysfs_permissions(dev);
	if (r)
		goto out_put;

 	if (pci_enable_device(dev)) {
 		printk(KERN_INFO "%s: Could not enable PCI device\n", __func__);
 		r = -EBUSY;
		goto out_put;
	}
	r = pci_request_regions(dev, "kvm_assigned_device");
	if (r) {
		printk(KERN_INFO "%s: Could not get access to device regions\n",
		       __func__);
		goto out_disable;
	}

	pci_reset_function(dev);
	pci_save_state(dev);
	match->pci_saved_state = pci_store_saved_state(dev);
	if (!match->pci_saved_state)
		printk(KERN_DEBUG "%s: Couldn't store %s saved state\n",
		       __func__, dev_name(&dev->dev));
	match->assigned_dev_id = assigned_dev->assigned_dev_id;
	match->host_segnr = assigned_dev->segnr;
	match->host_busnr = assigned_dev->busnr;
	match->host_devfn = assigned_dev->devfn;
	match->flags = assigned_dev->flags;
	match->dev = dev;
	spin_lock_init(&match->intx_lock);
	match->irq_source_id = -1;
	match->kvm = kvm;
	match->ack_notifier.irq_acked = kvm_assigned_dev_ack_irq;

	list_add(&match->list, &kvm->arch.assigned_dev_head);

	if (!kvm->arch.iommu_domain) {
		r = kvm_iommu_map_guest(kvm);
		if (r)
			goto out_list_del;
	}
	r = kvm_assign_device(kvm, match);
	if (r)
		goto out_list_del;

out:
	srcu_read_unlock(&kvm->srcu, idx);
	mutex_unlock(&kvm->lock);
	return r;
out_list_del:
	if (pci_load_and_free_saved_state(dev, &match->pci_saved_state))
		printk(KERN_INFO "%s: Couldn't reload %s saved state\n",
		       __func__, dev_name(&dev->dev));
	list_del(&match->list);
	pci_release_regions(dev);
out_disable:
	pci_disable_device(dev);
out_put:
	pci_dev_put(dev);
out_free:
	kfree(match);
	srcu_read_unlock(&kvm->srcu, idx);
	mutex_unlock(&kvm->lock);
	return r;
}
