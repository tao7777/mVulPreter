static void vapic_exit(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	int idx;
	if (!apic || !apic->vapic_addr)
		return;
	idx = srcu_read_lock(&vcpu->kvm->srcu);
	kvm_release_page_dirty(apic->vapic_page);
	mark_page_dirty(vcpu->kvm, apic->vapic_addr >> PAGE_SHIFT);
	srcu_read_unlock(&vcpu->kvm->srcu, idx);
}
