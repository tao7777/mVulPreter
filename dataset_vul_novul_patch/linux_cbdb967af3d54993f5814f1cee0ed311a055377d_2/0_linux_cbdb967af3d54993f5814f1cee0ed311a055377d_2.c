static void update_db_bp_intercept(struct kvm_vcpu *vcpu)
static void update_bp_intercept(struct kvm_vcpu *vcpu)
 {
 	struct vcpu_svm *svm = to_svm(vcpu);
 
 	clr_exception_intercept(svm, BP_VECTOR);
 
 	if (vcpu->guest_debug & KVM_GUESTDBG_ENABLE) {
 		if (vcpu->guest_debug & KVM_GUESTDBG_USE_SW_BP)
 			set_exception_intercept(svm, BP_VECTOR);
 	} else
		vcpu->guest_debug = 0;
}
