#pragma mark  ------------------------OC
//在OC中创建一个对象方式如下 :
NSObject *obj = [[NSObject alloc] init];

#pragma mark  ------------------------NSObject的创建过程
//之前创建对象的OC代码经过转化之后,其底层代码如下
NSObject *obj = ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"));
//精简一下
NSObject *obj = objc_msgSend(objc_msgSend(objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"));
//将其拆分为alloc、init两部分
//alloc
NSObject *allocObj = objc_msgSend(objc_getClass("NSObject"), sel_registerName("alloc"));
//init
NSObject *initObj = objc_msgSend(allocObj, sel_registerName("init"));

//从中可以看出,我们调用一个方法,其底层实际上是通过objc_msgSend()方法进行方法调用,该方法有两个参数

#pragma mark  ------------------------objc_getClass()
//objc_getClass()方法,返回一个对象,参数是一个字符串,也就是说,其内部通过一个字符串得到了一个对象
Class objc_getClass(const char *aClassName)
{
    if (!aClassName) return Nil;
    
    // NO unconnected, YES class handler
    return look_up_class(aClassName, NO, YES);
}

#pragma mark  ------------------------sel_registerName()
//sel_registerName()方法,返回一个SEL类型的数据
SEL sel_registerName(const char *name) {
    return __sel_registerName(name, 1, 1);     // YES lock, YES copy
}

#pragma mark  ------------------------总结
/*
 1、OC对象的创建,在底层转化成为了objc_msgSend()方法进行方法转发,
 2、objc_msgSend()方法有两个参数,
    a、参数一 : 一个Class类型,也就是一个对象
    b、参数二 : 一个SEL类型数据
 */






