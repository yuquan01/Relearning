#pragma mark  ------------------------创建的入口
//创建NSObject对象的时候,在_class_createInstanceFromZone方法中,会对分配好空间之后的obj进行isa创建操作,如下代码所示
if (!zone && fast) {
    obj->initInstanceIsa(cls, hasCxxDtor);
} else {
    obj->initIsa(cls);
}
#pragma mark  ------------------------isa的创建流程
//initInstanceIsa()方法,可以看出,其内部也是调用initIsa()方法
inline void
objc_object::initInstanceIsa(Class cls, bool hasCxxDtor)
{
    ASSERT(!cls->instancesRequireRawIsa());
    ASSERT(hasCxxDtor == cls->hasCxxDtor());
    
    initIsa(cls, true, hasCxxDtor);
}
//initIsa()方法,内部也是调用initIsa()方法
inline void
objc_object::initIsa(Class cls)
{
    initIsa(cls, false, false);
}

//initIsa 方法的实现
inline void
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor)
{
    ASSERT(!isTaggedPointer());
    //是否时经过优化的isa指针,如果不是,那么直接返回cls对象
    if (!nonpointer) {
        isa = isa_t((uintptr_t)cls);
    } else {
        ASSERT(!DisableNonpointerIsa);
        ASSERT(!cls->instancesRequireRawIsa());
        //对经过优化的isa指针进行初始化
        isa_t newisa(0);
        //在ios设备中,会执行else代码块
#if SUPPORT_INDEXED_ISA
        ASSERT(cls->classArrayIndex() > 0);
        //#   define ISA_INDEX_MAGIC_VALUE 0x001C0001
        newisa.bits = ISA_INDEX_MAGIC_VALUE;
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.indexcls = (uintptr_t)cls->classArrayIndex();
#else
        //#   define ISA_MAGIC_VALUE 0x000001a000000001ULL
        newisa.bits = ISA_MAGIC_VALUE;
        newisa.has_cxx_dtor = hasCxxDtor;
        //这里可以看出,他将cls的shiftcls值赋值给了shiftcls
        newisa.shiftcls = (uintptr_t)cls >> 3;
#endif
        isa = newisa;
    }
}
//从上面的方法可以看出,这里主要是做了cls的绑定,即当前实例对象的cls/isa->shiftcls值为类对象的Class,也就是实例对象的isa指针指向类对象


#pragma mark  ------------------------总结
/*
 1、在创建NSObject对象的时候,会进行一次isa的创建
 2、isa的创建相对简单,主要内容就是对cls/isa->shiftcls进行绑定,其值指向该对象类型,即实例对象的cls/isa->shiftcls值为类对象的Class,也就是实例对象的isa指针指向类对象
 */
