static int handle_vmread(struct kvm_vcpu *vcpu)
{
	unsigned long field;
	u64 field_value;
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	u32 vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	gva_t gva = 0;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (!nested_vmx_check_vmcs12(vcpu))
		return kvm_skip_emulated_instruction(vcpu);

	/* Decode instruction info and find the field to read */
	field = kvm_register_readl(vcpu, (((vmx_instruction_info) >> 28) & 0xf));
	/* Read the field, zero-extended to a u64 field_value */
	if (vmcs12_read_any(vcpu, field, &field_value) < 0) {
		nested_vmx_failValid(vcpu, VMXERR_UNSUPPORTED_VMCS_COMPONENT);
		return kvm_skip_emulated_instruction(vcpu);
	}
	/*
	 * Now copy part of this value to register or memory, as requested.
	 * Note that the number of bits actually copied is 32 or 64 depending
	 * on the guest's mode (32 or 64 bit), not on the given field's length.
	 */
	if (vmx_instruction_info & (1u << 10)) {
		kvm_register_writel(vcpu, (((vmx_instruction_info) >> 3) & 0xf),
			field_value);
	} else {
 		if (get_vmx_mem_address(vcpu, exit_qualification,
 				vmx_instruction_info, true, &gva))
 			return 1;
		/* _system ok, as hardware has verified cpl=0 */
 		kvm_write_guest_virt_system(&vcpu->arch.emulate_ctxt, gva,
 			     &field_value, (is_long_mode(vcpu) ? 8 : 4), NULL);
 	}

	nested_vmx_succeed(vcpu);
	return kvm_skip_emulated_instruction(vcpu);
}
