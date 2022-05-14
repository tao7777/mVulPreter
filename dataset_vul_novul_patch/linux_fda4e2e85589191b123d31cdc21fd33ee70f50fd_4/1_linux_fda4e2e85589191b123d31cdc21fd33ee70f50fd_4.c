static int vapic_enter(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;
	struct page *page;
	if (!apic || !apic->vapic_addr)
		return 0;
	page = gfn_to_page(vcpu->kvm, apic->vapic_addr >> PAGE_SHIFT);
	if (is_error_page(page))
		return -EFAULT;
	vcpu->arch.apic->vapic_page = page;
	return 0;
}
