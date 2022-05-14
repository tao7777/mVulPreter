void StackDumpSignalHandler(int signal, siginfo_t* info, void* void_context) {

  in_signal_handler = 1;

  if (BeingDebugged())
    BreakDebugger();

  PrintToStderr("Received signal ");
  char buf[1024] = { 0 };
  internal::itoa_r(signal, buf, sizeof(buf), 10, 0);
  PrintToStderr(buf);
  if (signal == SIGBUS) {
    if (info->si_code == BUS_ADRALN)
      PrintToStderr(" BUS_ADRALN ");
    else if (info->si_code == BUS_ADRERR)
      PrintToStderr(" BUS_ADRERR ");
    else if (info->si_code == BUS_OBJERR)
      PrintToStderr(" BUS_OBJERR ");
    else
      PrintToStderr(" <unknown> ");
  } else if (signal == SIGFPE) {
    if (info->si_code == FPE_FLTDIV)
      PrintToStderr(" FPE_FLTDIV ");
    else if (info->si_code == FPE_FLTINV)
      PrintToStderr(" FPE_FLTINV ");
    else if (info->si_code == FPE_FLTOVF)
      PrintToStderr(" FPE_FLTOVF ");
    else if (info->si_code == FPE_FLTRES)
      PrintToStderr(" FPE_FLTRES ");
    else if (info->si_code == FPE_FLTSUB)
      PrintToStderr(" FPE_FLTSUB ");
    else if (info->si_code == FPE_FLTUND)
      PrintToStderr(" FPE_FLTUND ");
    else if (info->si_code == FPE_INTDIV)
      PrintToStderr(" FPE_INTDIV ");
    else if (info->si_code == FPE_INTOVF)
      PrintToStderr(" FPE_INTOVF ");
    else
      PrintToStderr(" <unknown> ");
  } else if (signal == SIGILL) {
    if (info->si_code == ILL_BADSTK)
      PrintToStderr(" ILL_BADSTK ");
    else if (info->si_code == ILL_COPROC)
      PrintToStderr(" ILL_COPROC ");
    else if (info->si_code == ILL_ILLOPN)
      PrintToStderr(" ILL_ILLOPN ");
    else if (info->si_code == ILL_ILLADR)
      PrintToStderr(" ILL_ILLADR ");
    else if (info->si_code == ILL_ILLTRP)
      PrintToStderr(" ILL_ILLTRP ");
    else if (info->si_code == ILL_PRVOPC)
      PrintToStderr(" ILL_PRVOPC ");
    else if (info->si_code == ILL_PRVREG)
      PrintToStderr(" ILL_PRVREG ");
    else
      PrintToStderr(" <unknown> ");
  } else if (signal == SIGSEGV) {
    if (info->si_code == SEGV_MAPERR)
      PrintToStderr(" SEGV_MAPERR ");
    else if (info->si_code == SEGV_ACCERR)
      PrintToStderr(" SEGV_ACCERR ");
    else
      PrintToStderr(" <unknown> ");
  }
  if (signal == SIGBUS || signal == SIGFPE ||
      signal == SIGILL || signal == SIGSEGV) {
    internal::itoa_r(reinterpret_cast<intptr_t>(info->si_addr),
                     buf, sizeof(buf), 16, 12);
    PrintToStderr(buf);
  }
  PrintToStderr("\n");

  debug::StackTrace().Print();

#if defined(OS_LINUX)
#if ARCH_CPU_X86_FAMILY
  ucontext_t* context = reinterpret_cast<ucontext_t*>(void_context);
  const struct {
    const char* label;
    greg_t value;
  } registers[] = {
#if ARCH_CPU_32_BITS
    { "  gs: ", context->uc_mcontext.gregs[REG_GS] },
    { "  fs: ", context->uc_mcontext.gregs[REG_FS] },
    { "  es: ", context->uc_mcontext.gregs[REG_ES] },
    { "  ds: ", context->uc_mcontext.gregs[REG_DS] },
    { " edi: ", context->uc_mcontext.gregs[REG_EDI] },
    { " esi: ", context->uc_mcontext.gregs[REG_ESI] },
    { " ebp: ", context->uc_mcontext.gregs[REG_EBP] },
    { " esp: ", context->uc_mcontext.gregs[REG_ESP] },
    { " ebx: ", context->uc_mcontext.gregs[REG_EBX] },
    { " edx: ", context->uc_mcontext.gregs[REG_EDX] },
    { " ecx: ", context->uc_mcontext.gregs[REG_ECX] },
    { " eax: ", context->uc_mcontext.gregs[REG_EAX] },
    { " trp: ", context->uc_mcontext.gregs[REG_TRAPNO] },
    { " err: ", context->uc_mcontext.gregs[REG_ERR] },
    { "  ip: ", context->uc_mcontext.gregs[REG_EIP] },
    { "  cs: ", context->uc_mcontext.gregs[REG_CS] },
    { " efl: ", context->uc_mcontext.gregs[REG_EFL] },
    { " usp: ", context->uc_mcontext.gregs[REG_UESP] },
    { "  ss: ", context->uc_mcontext.gregs[REG_SS] },
#elif ARCH_CPU_64_BITS
    { "  r8: ", context->uc_mcontext.gregs[REG_R8] },
    { "  r9: ", context->uc_mcontext.gregs[REG_R9] },
    { " r10: ", context->uc_mcontext.gregs[REG_R10] },
    { " r11: ", context->uc_mcontext.gregs[REG_R11] },
    { " r12: ", context->uc_mcontext.gregs[REG_R12] },
    { " r13: ", context->uc_mcontext.gregs[REG_R13] },
    { " r14: ", context->uc_mcontext.gregs[REG_R14] },
    { " r15: ", context->uc_mcontext.gregs[REG_R15] },
    { "  di: ", context->uc_mcontext.gregs[REG_RDI] },
    { "  si: ", context->uc_mcontext.gregs[REG_RSI] },
    { "  bp: ", context->uc_mcontext.gregs[REG_RBP] },
    { "  bx: ", context->uc_mcontext.gregs[REG_RBX] },
    { "  dx: ", context->uc_mcontext.gregs[REG_RDX] },
    { "  ax: ", context->uc_mcontext.gregs[REG_RAX] },
    { "  cx: ", context->uc_mcontext.gregs[REG_RCX] },
    { "  sp: ", context->uc_mcontext.gregs[REG_RSP] },
    { "  ip: ", context->uc_mcontext.gregs[REG_RIP] },
    { " efl: ", context->uc_mcontext.gregs[REG_EFL] },
    { " cgf: ", context->uc_mcontext.gregs[REG_CSGSFS] },
    { " erf: ", context->uc_mcontext.gregs[REG_ERR] },
    { " trp: ", context->uc_mcontext.gregs[REG_TRAPNO] },
    { " msk: ", context->uc_mcontext.gregs[REG_OLDMASK] },
    { " cr2: ", context->uc_mcontext.gregs[REG_CR2] },
#endif
  };

#if ARCH_CPU_32_BITS
  const int kRegisterPadding = 8;
#elif ARCH_CPU_64_BITS
   const int kRegisterPadding = 16;
 #endif
 
  for (size_t i = 0; i < arraysize(registers); i++) {
     PrintToStderr(registers[i].label);
     internal::itoa_r(registers[i].value, buf, sizeof(buf),
                      16, kRegisterPadding);
    PrintToStderr(buf);

    if ((i + 1) % 4 == 0)
      PrintToStderr("\n");
  }
  PrintToStderr("\n");
#endif
#elif defined(OS_MACOSX)
#if ARCH_CPU_X86_FAMILY && ARCH_CPU_32_BITS
  ucontext_t* context = reinterpret_cast<ucontext_t*>(void_context);
  size_t len;

  len = static_cast<size_t>(
      snprintf(buf, sizeof(buf),
               "ax: %x, bx: %x, cx: %x, dx: %x\n",
               context->uc_mcontext->__ss.__eax,
               context->uc_mcontext->__ss.__ebx,
               context->uc_mcontext->__ss.__ecx,
               context->uc_mcontext->__ss.__edx));
  write(STDERR_FILENO, buf, std::min(len, sizeof(buf) - 1));

  len = static_cast<size_t>(
      snprintf(buf, sizeof(buf),
               "di: %x, si: %x, bp: %x, sp: %x, ss: %x, flags: %x\n",
               context->uc_mcontext->__ss.__edi,
               context->uc_mcontext->__ss.__esi,
               context->uc_mcontext->__ss.__ebp,
               context->uc_mcontext->__ss.__esp,
               context->uc_mcontext->__ss.__ss,
               context->uc_mcontext->__ss.__eflags));
  write(STDERR_FILENO, buf, std::min(len, sizeof(buf) - 1));

  len = static_cast<size_t>(
      snprintf(buf, sizeof(buf),
               "ip: %x, cs: %x, ds: %x, es: %x, fs: %x, gs: %x\n",
               context->uc_mcontext->__ss.__eip,
               context->uc_mcontext->__ss.__cs,
               context->uc_mcontext->__ss.__ds,
               context->uc_mcontext->__ss.__es,
               context->uc_mcontext->__ss.__fs,
               context->uc_mcontext->__ss.__gs));
  write(STDERR_FILENO, buf, std::min(len, sizeof(buf) - 1));
#endif  // ARCH_CPU_32_BITS
#endif  // defined(OS_MACOSX)
  _exit(1);
}
