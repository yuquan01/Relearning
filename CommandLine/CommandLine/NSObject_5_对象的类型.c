#pragma mark  ------------------------类的类型
/*
在OC中,类主要分为三类
1、实例对象 : instance对象
2、类对象 : class对象
3、元类对象 : meta-class对象
*/
#pragma mark  ------------------------实例对象 : instance对象
//创建方式如下,创建了两个实例对象,obj1、obj2,他们是两个不同的实例对象,有不同的内存空间
NSObject *obj1 = [[NSObject alloc] init];
NSObject *obj2 = [[NSObject alloc] init];

#pragma mark  ------------------------类对象 : class对象
//创建方式如下
Class class1 = [obj1 class];
Class class2 = [NSObject class];
Class class4 = object_getClass(obj1);
//对各个方法进行分析
//1、[obj1 class] : 从方法调用可以看出,其内部是调用了object_getClass()方法
- (Class)class {
    return object_getClass(self);
}
//2、[NSObject class] : 该方法直接返回了自身
+ (Class)class {
    return self;
}
//3、object_getClass(obj1) : 从该方法可以得知,获取一个对象的class时,是直接返回的他的isa所指向的对象类型
Class object_getClass(id obj)
{
    if (obj) return obj->getIsa();
    else return Nil;
}
//通过上面的三个方法可以知道,class对象实际上就是获取实例对象的isa指向的对象,或者就是类对象本身
//很明显可以看出,如果是使用+class方式的化,那么返回的永远都是同一个类对象,那么如果是使用object_getClass()形式呢?
//obj->getIsa()的实现方式,分为两种形式,一种是tagged_pointers类型,一种是普通对象类型,这里只对普通对象类型分析,关系tagged_pointer类型在后面进行分析
inline Class objc_object::getIsa()
{
    return ISA();
}
//在之前的分析可以知道,isa是一个isa_t联合体类型的数据,内部有两个成员,因此获取isa也有两种可能
//一种是未经过优化的isa,其值就是cls,也就是直接返回当前对象的cls值
//一种是经过优化的isa,其返回的值是 isa.bits & ISA_MASK 之后的值,从这里可以看出,isa中存储的地址并不是对象的真正地址,而是经过处理之后的地址,真正的对象地址是isa & ISA_MASK运算之后的地址
#if SUPPORT_NONPOINTER_ISA
inline Class objc_object::ISA()
{
    ASSERT(!isTaggedPointer());
#if SUPPORT_INDEXED_ISA
    if (isa.nonpointer) {
        uintptr_t slot = isa.indexcls;
        return classForIndex((unsigned)slot);
    }
    return (Class)isa.bits;
#else
    return (Class)(isa.bits & ISA_MASK);
#endif
}
inline Class objc_object::ISA()
{
    ASSERT(!isTaggedPointer());
    return isa.cls;
}
#if SUPPORT_TAGGED_POINTERS
//从上面的方法流程可以看出,获取类对象时,会分为两种,
//一种是返回类对象本身,而通过对对象的结构认识可以知道,其返回的就是类对象本身的isa
//一种是返回实例对象的isa指针指向的对象地址,而在之前的认知知道,一个实例对象的isa指针指向了类对象
//综上两点,一个类对象的class类型是一致的,换而言之,一个类的类对象只有一份,不管由什么方法得到,其值相同

#pragma mark  ------------------------元类对象 : meta-class对象
//创建方式如下
Class metaClass1 = object_getClass([NSObject class]);
Class metaClass2 = objc_getMetaClass("NSObject");
//对各个方法进行分析
//1、object_getClass() : 本方法在上面已经有介绍
//2、objc_getMetaClass(),主要通过objc_getClass()进行处理
Class objc_getMetaClass(const char *aClassName)
{
    Class cls;
    
    if (!aClassName) return Nil;
    
    cls = objc_getClass (aClassName);
    if (!cls)
    {
        _objc_inform ("class `%s' not linked into application", aClassName);
        return Nil;
    }
    //这里很关键,他是返回通过反射得到的对象的isa指针
    return cls->ISA();
}
Class objc_getClass(const char *aClassName)
{
    if (!aClassName) return Nil;
    //这个内部通过aClassName反射得到一个对象,具体分析这里跳过
    return look_up_class(aClassName, NO, YES);
}
//通过对上面的方法可以看出,获取一个元类对象有两种方法,一种是获取类对象的isa指针,一种是通过传入类名通过反射得到的对象,在获取其isa指针
//综上可以知道,获取一个元类对象,实际上就是获取一个类对象的isa指针,即,一个类对象的isa指针指向了这个类对象的元类对象
//而一个元类对象的isa指针指向哪里呢?在OC中,一个元类对象的isa指针指向根元类对象,而跟元类对象的isa指针指向自身,这里的代码暂时没有找到具体是如何做的

#pragma mark  ------------------------总结
/*
 1、对象分为三种
    a、实例对象 : instance对象
    b、类对象 : class对象
    c、元类对象 : meta-class对象
 2、对象的获取
    a、实例对象 : 通过alloc、new等形式创建
    b、类对象 : 有多种方法,但其底层都是返回一个实例对象的isa指针,或者是返回类对象本身的isa指针
    c、元类对象: 底层是返回一个类对象的isa指针
 3、从上面可以得出结论 :
    实例对象的isa指针指向类对象,类对象的isa指针指向元类对象,元类对象的isa指针指向跟元类对象,跟元类对象的isa指针指向自身,从而形成一个闭环
 */


