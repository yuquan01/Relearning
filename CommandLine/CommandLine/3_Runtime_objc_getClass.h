#pragma mark  ------------------------objc_getClass()
//objc_getClass()方法的定义,其返回一个对象,参数是一个字符串
//其内部是通过一个字符串反射成一个Class对象
Class objc_getClass(const char *aClassName)
{
    if (!aClassName) return Nil;
    
    // NO unconnected, YES class handler
    return look_up_class(aClassName, NO, YES);
}
//look_up_class()方法的定义,返回一个对象
Class look_up_class(const char *name,
                    bool includeUnconnected __attribute__((unused)),
                    bool includeClassHandler __attribute__((unused)))
{
    if (!name) return nil;
    
    Class result;
    bool unrealized;
    {
        runtimeLock.lock();
        //方法一
        result = getClassExceptSomeSwift(name);
        unrealized = result  &&  !result->isRealized();
        if (unrealized) {
            //方法二
            result = realizeClassMaybeSwiftAndUnlock(result, runtimeLock);
            // runtimeLock is now unlocked
        } else {
            runtimeLock.unlock();
        }
    }
    
    if (!result) {
        // Ask Swift about its un-instantiated classes.
        
        // We use thread-local storage to prevent infinite recursion
        // if the hook function provokes another lookup of the same name
        // (for example, if the hook calls objc_allocateClassPair)
        
        auto *tls = _objc_fetch_pthread_data(true);
        
        // Stop if this thread is already looking up this name.
        for (unsigned i = 0; i < tls->classNameLookupsUsed; i++) {
            if (0 == strcmp(name, tls->classNameLookups[i])) {
                return nil;
            }
        }
        
        // Save this lookup in tls.
        if (tls->classNameLookupsUsed == tls->classNameLookupsAllocated) {
            tls->classNameLookupsAllocated =
            (tls->classNameLookupsAllocated * 2 ?: 1);
            size_t size = tls->classNameLookupsAllocated *
            sizeof(tls->classNameLookups[0]);
            tls->classNameLookups = (const char **)
            realloc(tls->classNameLookups, size);
        }
        tls->classNameLookups[tls->classNameLookupsUsed++] = name;
        
        // Call the hook.
        Class swiftcls = nil;
        if (GetClassHook.get()(name, &swiftcls)) {
            ASSERT(swiftcls->isRealized());
            result = swiftcls;
        }
        
        // Erase the name from tls.
        unsigned slot = --tls->classNameLookupsUsed;
        ASSERT(slot >= 0  &&  slot < tls->classNameLookupsAllocated);
        ASSERT(name == tls->classNameLookups[slot]);
        tls->classNameLookups[slot] = nil;
    }
    
    return result;
}

static Class getClassExceptSomeSwift(const char *name)
{
    runtimeLock.assertLocked();
    
    // Try name as-is
    Class result = getClass_impl(name);
    if (result) return result;
    
    // Try Swift-mangled equivalent of the given name.
    if (char *swName = copySwiftV1MangledName(name)) {
        result = getClass_impl(swName);
        free(swName);
        return result;
    }
    
    return nil;
}

static Class
realizeClassMaybeSwiftMaybeRelock(Class cls, mutex_t& lock, bool leaveLocked)
{
    lock.assertLocked();
    
    if (!cls->isSwiftStable_ButAllowLegacyForNow()) {
        // Non-Swift class. Realize it now with the lock still held.
        // fixme wrong in the future for objc subclasses of swift classes
        realizeClassWithoutSwift(cls, nil);
        if (!leaveLocked) lock.unlock();
    } else {
        // Swift class. We need to drop locks and call the Swift
        // runtime to initialize it.
        lock.unlock();
        cls = realizeSwiftClass(cls);
        ASSERT(cls->isRealized());    // callback must have provoked realization
        if (leaveLocked) lock.lock();
    }
    
    return cls;
}
