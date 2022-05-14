void NativeStackSamplerWin::RecordStackSample(
     StackSamplingProfiler::Sample* sample) {
   DCHECK(current_modules_);
 
  if (!stack_copy_buffer_)
    return;

   const int max_stack_size = 64;
   const void* instruction_pointers[max_stack_size] = {0};
   HMODULE module_handles[max_stack_size] = {0};
 
   int stack_depth = SuspendThreadAndRecordStack(thread_handle_.Get(),
                                                thread_stack_base_address_,
                                                stack_copy_buffer_.get(),
                                                kStackCopyBufferSize,
                                                 max_stack_size,
                                                 instruction_pointers);
   FindModuleHandlesForAddresses(instruction_pointers, module_handles,
                                stack_depth);
  CopyToSample(instruction_pointers, module_handles, stack_depth, sample,
               current_modules_);
  FreeModuleHandles(stack_depth, module_handles);
}
