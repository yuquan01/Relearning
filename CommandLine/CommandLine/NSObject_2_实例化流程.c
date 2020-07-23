//在OC中,我们创建一个实例对象
NSObject *obj = [NSObject alloc];
//alloc方法的定义为
+ (id)alloc {
    return _objc_rootAlloc(self);
}
id _objc_rootAlloc(Class cls)
{
    return callAlloc(cls, false/*checkNil*/, true/*allocWithZone*/);
}
/*
 可以看出,底层都是通过一个callAlloc()的方法进行创建的,而callAlloc()的方法如下
 */
//补充
//表示执行x为1的可能性更大
#define fastpath(x) (__builtin_expect(bool(x), 1))
//表示执行x为0的可能性更大
#define slowpath(x) (__builtin_expect(bool(x), 0))

static ALWAYS_INLINE id callAlloc(Class cls, bool checkNil, bool allocWithZone=false)
{
#if __OBJC2__
    //一种高效率的if-else判断方式,具体在上面有描述
    //这里表示 checkNil == true 且 !cls == true 的可能性更小,
    //从调用该方法的入口可以看出,checkNil的值为true,而 cls 表示的是当前类,因此这个方法更多的是判断cls对象为空的可能性更小
    //当其中有一个为
    if (slowpath(checkNil && !cls)) return nil;
    //
    if (fastpath(!cls->ISA()->hasCustomAWZ())) {
        return _objc_rootAllocWithZone(cls, nil);
    }
#endif
    
    // No shortcuts available.
    if (allocWithZone) {
        return ((id(*)(id, SEL, struct _NSZone *))objc_msgSend)(cls, @selector(allocWithZone:), nil);
    }
    return ((id(*)(id, SEL))objc_msgSend)(cls, @selector(alloc));
}

/*
 1、
 */
