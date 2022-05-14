 PPB_URLLoader_Impl::~PPB_URLLoader_Impl() {
  // There is a path whereby the destructor for the loader_ member can
  // invoke InstanceWasDeleted() upon this PPB_URLLoader_Impl, thereby
  // re-entering the scoped_ptr destructor with the same scoped_ptr object
  // via loader_.reset(). Be sure that loader_ is first NULL then destroy
  // the scoped_ptr. See http://crbug.com/159429.
  scoped_ptr<WebKit::WebURLLoader> for_destruction_only(loader_.release());
 }
