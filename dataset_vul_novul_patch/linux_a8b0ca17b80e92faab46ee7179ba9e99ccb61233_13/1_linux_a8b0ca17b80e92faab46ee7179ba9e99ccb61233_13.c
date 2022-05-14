static int cop1Emulate(struct pt_regs *xcp, struct mips_fpu_struct *ctx,
		       void *__user *fault_addr)
{
	mips_instruction ir;
	unsigned long emulpc, contpc;
	unsigned int cond;

	if (!access_ok(VERIFY_READ, xcp->cp0_epc, sizeof(mips_instruction))) {
		MIPS_FPU_EMU_INC_STATS(errors);
		*fault_addr = (mips_instruction __user *)xcp->cp0_epc;
		return SIGBUS;
	}
	if (__get_user(ir, (mips_instruction __user *) xcp->cp0_epc)) {
		MIPS_FPU_EMU_INC_STATS(errors);
		*fault_addr = (mips_instruction __user *)xcp->cp0_epc;
		return SIGSEGV;
	}

	/* XXX NEC Vr54xx bug workaround */
	if ((xcp->cp0_cause & CAUSEF_BD) && !isBranchInstr(&ir))
		xcp->cp0_cause &= ~CAUSEF_BD;

	if (xcp->cp0_cause & CAUSEF_BD) {
		/*
		 * The instruction to be emulated is in a branch delay slot
		 * which means that we have to  emulate the branch instruction
		 * BEFORE we do the cop1 instruction.
		 *
		 * This branch could be a COP1 branch, but in that case we
		 * would have had a trap for that instruction, and would not
		 * come through this route.
		 *
		 * Linux MIPS branch emulator operates on context, updating the
		 * cp0_epc.
		 */
		emulpc = xcp->cp0_epc + 4;	/* Snapshot emulation target */

		if (__compute_return_epc(xcp)) {
#ifdef CP1DBG
			printk("failed to emulate branch at %p\n",
				(void *) (xcp->cp0_epc));
#endif
			return SIGILL;
		}
		if (!access_ok(VERIFY_READ, emulpc, sizeof(mips_instruction))) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = (mips_instruction __user *)emulpc;
			return SIGBUS;
		}
		if (__get_user(ir, (mips_instruction __user *) emulpc)) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = (mips_instruction __user *)emulpc;
			return SIGSEGV;
		}
		/* __compute_return_epc() will have updated cp0_epc */
		contpc = xcp->cp0_epc;
		/* In order not to confuse ptrace() et al, tweak context */
		xcp->cp0_epc = emulpc - 4;
	} else {
		emulpc = xcp->cp0_epc;
		contpc = xcp->cp0_epc + 4;
 	}
 
       emul:
	perf_sw_event(PERF_COUNT_SW_EMULATION_FAULTS,
			1, 0, xcp, 0);
 	MIPS_FPU_EMU_INC_STATS(emulated);
 	switch (MIPSInst_OPCODE(ir)) {
 	case ldc1_op:{
		u64 __user *va = (u64 __user *) (xcp->regs[MIPSInst_RS(ir)] +
			MIPSInst_SIMM(ir));
		u64 val;

		MIPS_FPU_EMU_INC_STATS(loads);

		if (!access_ok(VERIFY_READ, va, sizeof(u64))) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGBUS;
		}
		if (__get_user(val, va)) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGSEGV;
		}
		DITOREG(val, MIPSInst_RT(ir));
		break;
	}

	case sdc1_op:{
		u64 __user *va = (u64 __user *) (xcp->regs[MIPSInst_RS(ir)] +
			MIPSInst_SIMM(ir));
		u64 val;

		MIPS_FPU_EMU_INC_STATS(stores);
		DIFROMREG(val, MIPSInst_RT(ir));
		if (!access_ok(VERIFY_WRITE, va, sizeof(u64))) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGBUS;
		}
		if (__put_user(val, va)) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGSEGV;
		}
		break;
	}

	case lwc1_op:{
		u32 __user *va = (u32 __user *) (xcp->regs[MIPSInst_RS(ir)] +
			MIPSInst_SIMM(ir));
		u32 val;

		MIPS_FPU_EMU_INC_STATS(loads);
		if (!access_ok(VERIFY_READ, va, sizeof(u32))) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGBUS;
		}
		if (__get_user(val, va)) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGSEGV;
		}
		SITOREG(val, MIPSInst_RT(ir));
		break;
	}

	case swc1_op:{
		u32 __user *va = (u32 __user *) (xcp->regs[MIPSInst_RS(ir)] +
			MIPSInst_SIMM(ir));
		u32 val;

		MIPS_FPU_EMU_INC_STATS(stores);
		SIFROMREG(val, MIPSInst_RT(ir));
		if (!access_ok(VERIFY_WRITE, va, sizeof(u32))) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGBUS;
		}
		if (__put_user(val, va)) {
			MIPS_FPU_EMU_INC_STATS(errors);
			*fault_addr = va;
			return SIGSEGV;
		}
		break;
	}

	case cop1_op:
		switch (MIPSInst_RS(ir)) {

#if defined(__mips64)
		case dmfc_op:
			/* copregister fs -> gpr[rt] */
			if (MIPSInst_RT(ir) != 0) {
				DIFROMREG(xcp->regs[MIPSInst_RT(ir)],
					MIPSInst_RD(ir));
			}
			break;

		case dmtc_op:
			/* copregister fs <- rt */
			DITOREG(xcp->regs[MIPSInst_RT(ir)], MIPSInst_RD(ir));
			break;
#endif

		case mfc_op:
			/* copregister rd -> gpr[rt] */
			if (MIPSInst_RT(ir) != 0) {
				SIFROMREG(xcp->regs[MIPSInst_RT(ir)],
					MIPSInst_RD(ir));
			}
			break;

		case mtc_op:
			/* copregister rd <- rt */
			SITOREG(xcp->regs[MIPSInst_RT(ir)], MIPSInst_RD(ir));
			break;

		case cfc_op:{
			/* cop control register rd -> gpr[rt] */
			u32 value;

			if (MIPSInst_RD(ir) == FPCREG_CSR) {
				value = ctx->fcr31;
				value = (value & ~FPU_CSR_RM) |
					mips_rm[modeindex(value)];
#ifdef CSRTRACE
				printk("%p gpr[%d]<-csr=%08x\n",
					(void *) (xcp->cp0_epc),
					MIPSInst_RT(ir), value);
#endif
			}
			else if (MIPSInst_RD(ir) == FPCREG_RID)
				value = 0;
			else
				value = 0;
			if (MIPSInst_RT(ir))
				xcp->regs[MIPSInst_RT(ir)] = value;
			break;
		}

		case ctc_op:{
			/* copregister rd <- rt */
			u32 value;

			if (MIPSInst_RT(ir) == 0)
				value = 0;
			else
				value = xcp->regs[MIPSInst_RT(ir)];

			/* we only have one writable control reg
			 */
			if (MIPSInst_RD(ir) == FPCREG_CSR) {
#ifdef CSRTRACE
				printk("%p gpr[%d]->csr=%08x\n",
					(void *) (xcp->cp0_epc),
					MIPSInst_RT(ir), value);
#endif

				/*
				 * Don't write reserved bits,
				 * and convert to ieee library modes
				 */
				ctx->fcr31 = (value &
						~(FPU_CSR_RSVD | FPU_CSR_RM)) |
						ieee_rm[modeindex(value)];
			}
			if ((ctx->fcr31 >> 5) & ctx->fcr31 & FPU_CSR_ALL_E) {
				return SIGFPE;
			}
			break;
		}

		case bc_op:{
			int likely = 0;

			if (xcp->cp0_cause & CAUSEF_BD)
				return SIGILL;

#if __mips >= 4
			cond = ctx->fcr31 & fpucondbit[MIPSInst_RT(ir) >> 2];
#else
			cond = ctx->fcr31 & FPU_CSR_COND;
#endif
			switch (MIPSInst_RT(ir) & 3) {
			case bcfl_op:
				likely = 1;
			case bcf_op:
				cond = !cond;
				break;
			case bctl_op:
				likely = 1;
			case bct_op:
				break;
			default:
				/* thats an illegal instruction */
				return SIGILL;
			}

			xcp->cp0_cause |= CAUSEF_BD;
			if (cond) {
				/* branch taken: emulate dslot
				 * instruction
				 */
				xcp->cp0_epc += 4;
				contpc = (xcp->cp0_epc +
					(MIPSInst_SIMM(ir) << 2));

				if (!access_ok(VERIFY_READ, xcp->cp0_epc,
					       sizeof(mips_instruction))) {
					MIPS_FPU_EMU_INC_STATS(errors);
					*fault_addr = (mips_instruction __user *)xcp->cp0_epc;
					return SIGBUS;
				}
				if (__get_user(ir,
				    (mips_instruction __user *) xcp->cp0_epc)) {
					MIPS_FPU_EMU_INC_STATS(errors);
					*fault_addr = (mips_instruction __user *)xcp->cp0_epc;
					return SIGSEGV;
				}

				switch (MIPSInst_OPCODE(ir)) {
				case lwc1_op:
				case swc1_op:
#if (__mips >= 2 || defined(__mips64))
				case ldc1_op:
				case sdc1_op:
#endif
				case cop1_op:
#if __mips >= 4 && __mips != 32
				case cop1x_op:
#endif
					/* its one of ours */
					goto emul;
#if __mips >= 4
				case spec_op:
					if (MIPSInst_FUNC(ir) == movc_op)
						goto emul;
					break;
#endif
				}

				/*
				 * Single step the non-cp1
				 * instruction in the dslot
				 */
				return mips_dsemul(xcp, ir, contpc);
			}
			else {
				/* branch not taken */
				if (likely) {
					/*
					 * branch likely nullifies
					 * dslot if not taken
					 */
					xcp->cp0_epc += 4;
					contpc += 4;
					/*
					 * else continue & execute
					 * dslot as normal insn
					 */
				}
			}
			break;
		}

		default:
			if (!(MIPSInst_RS(ir) & 0x10))
				return SIGILL;
			{
				int sig;

				/* a real fpu computation instruction */
				if ((sig = fpu_emu(xcp, ctx, ir)))
					return sig;
			}
		}
		break;

#if __mips >= 4 && __mips != 32
	case cop1x_op:{
		int sig = fpux_emu(xcp, ctx, ir, fault_addr);
		if (sig)
			return sig;
		break;
	}
#endif

#if __mips >= 4
	case spec_op:
		if (MIPSInst_FUNC(ir) != movc_op)
			return SIGILL;
		cond = fpucondbit[MIPSInst_RT(ir) >> 2];
		if (((ctx->fcr31 & cond) != 0) == ((MIPSInst_RT(ir) & 1) != 0))
			xcp->regs[MIPSInst_RD(ir)] =
				xcp->regs[MIPSInst_RS(ir)];
		break;
#endif

	default:
		return SIGILL;
	}

	/* we did it !! */
	xcp->cp0_epc = contpc;
	xcp->cp0_cause &= ~CAUSEF_BD;

	return 0;
}
