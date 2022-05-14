ProcessIdToFilterMap* GetProcessIdToFilterMap() {
  static base::NoDestructor<ProcessIdToFilterMap> instance;
  return instance.get();
}
