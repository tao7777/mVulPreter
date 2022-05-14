static void kvm_unpin_pages(struct kvm *kvm, pfn_t pfn, unsigned long npages)
{
	unsigned long i;
	for (i = 0; i < npages; ++i)
		kvm_release_pfn_clean(pfn + i);
}
