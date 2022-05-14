 int kvm_set_msr(struct kvm_vcpu *vcpu, struct msr_data *msr)
 {
 	return kvm_x86_ops->set_msr(vcpu, msr);
 }
