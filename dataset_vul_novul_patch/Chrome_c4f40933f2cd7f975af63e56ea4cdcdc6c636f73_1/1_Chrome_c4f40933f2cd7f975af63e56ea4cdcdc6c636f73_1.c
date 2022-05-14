 Accelerator GetAccelerator(KeyboardCode code, int mask) {
  return Accelerator(code, mask & (1 << 0), mask & (1 << 1), mask & (1 << 2));
 }
