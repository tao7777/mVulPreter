static v8::Handle<v8::Value> fooCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.Float64Array.foo");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     Float64Array* imp = V8Float64Array::toNative(args.Holder());
     EXCEPTION_BLOCK(Float32Array*, array, V8Float32Array::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8Float32Array::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     return toV8(imp->foo(array), args.GetIsolate());
}
