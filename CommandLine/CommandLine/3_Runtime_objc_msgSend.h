#pragma mark  ------------------------OC中对象的创建
//在我们使用过程中,一般我们使用alloc方法进行创建对象
NSObject *obj = [NSObject alloc];

//经过转化之后,其底层代码如下
NSObject *obj = ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc"));
//精简一下
//分为三部分
//1、objc_msgSend() : 方法的底层实际调用,绝大部分的OC方法调用在底层都需要通过objc_msgSend进行调用
//2、objc_getClass() : 方法的调用对象
//3、sel_registerName : 方法的名字
//这里暂时只针对 alloc 方法进行分析
NSObject *obj = objc_msgSend(objc_getClass("NSObject"), sel_registerName("alloc"));


//alloc方法如下
+ (id)alloc {
    return _objc_rootAlloc(self);
}

//_objc_rootAlloc()方法如下
id _objc_rootAlloc(Class cls)
{
    return callAlloc(cls, false/*checkNil*/, true/*allocWithZone*/);
}

//callAlloc()方法如下
static ALWAYS_INLINE id callAlloc(Class cls, bool checkNil, bool allocWithZone=false)
{
#if __OBJC2__
    //一种高效的if-else判断,这里表示执行else的可能性更大,即更有可能进入下一步而不是return
    //这里只是进行一些数据判断,从传入的值可知,checkNil为false,同时cls不为空,因此不会执行return
    if (slowpath(checkNil && !cls)) return nil;
    //这里表示执行if的可能性更大
    //判断是否有重写allocWithZone方法,这里表示重写了allocWithZone方法的可能性更大
    if (fastpath(!cls->ISA()->hasCustomAWZ())) {
        //调用对应的方法进行处理
        return _objc_rootAllocWithZone(cls, nil);
    }
#endif
    //这里判断传入的allocWithZone值,从传入的参数可知,其值为true
    if (allocWithZone) {
        return ((id(*)(id, SEL, struct _NSZone *))objc_msgSend)(cls, @selector(allocWithZone:), nil);
    }
    return ((id(*)(id, SEL))objc_msgSend)(cls, @selector(alloc));
}
/*
 通过以上简单的分析,其最终结果会流向_objc_rootAllocWithZone()、allocWithZone:、alloc三个方法,那么对这三个方法进行分析
 */
//_objc_rootAllocWithZone()
id _objc_rootAllocWithZone(Class cls, malloc_zone_t *zone __unused)
{
    // allocWithZone under __OBJC2__ ignores the zone parameter
    return _class_createInstanceFromZone(cls, 0, nil,
                                         OBJECT_CONSTRUCT_CALL_BADALLOC);
}

//allocWithZone:
+ (id)allocWithZone:(struct _NSZone *)zone {
    return _objc_rootAllocWithZone(self, (malloc_zone_t *)zone);
}
//alloc
//该方法在上面的方法中已有描述

//通过对上面三个方法可以看出,最终都会导向_class_createInstanceFromZone()方法,该方法如下
//其过程主要为 :
//1、计算对象大小,这里会有内对对齐处理
//2、分配空间,按指定的空间,或者由系统进行分配
//3、构建isa,对每个对象构建isa成员值
//4、返回对象
static ALWAYS_INLINE id
_class_createInstanceFromZone(Class cls, size_t extraBytes, void *zone,
                              int construct_flags = OBJECT_CONSTRUCT_NONE,
                              bool cxxConstruct = true,
                              size_t *outAllocatedSize = nil)
{
    ASSERT(cls->isRealized());
    
    //判断是否有c++的构造器
    bool hasCxxCtor = cxxConstruct && cls->hasCxxCtor();
    //判断是否有c++的析构器
    bool hasCxxDtor = cls->hasCxxDtor();
    //是否支持isa_t类型
    bool fast = cls->canAllocNonpointer();
    size_t size;
    //获取当前对象经过字节对齐之后的字节大小
    size = cls->instanceSize(extraBytes);
    if (outAllocatedSize) *outAllocatedSize = size;
    //开始开行内存申请分配,如果有指定的zone内存空间,那么向这块内存空间申请,否则由系统进行分配,一般来说,我们都是让系统进行分配的.
    //在一般情况下,我们都没有指定zone,也就是说,这里我们总是执行else代码块
    id obj;
    if (zone) {
        obj = (id)malloc_zone_calloc((malloc_zone_t *)zone, 1, size);
    } else {
        //内部会进行内存对齐,对齐大小为16,也就是说分配的空间总是16的倍数
        obj = (id)calloc(1, size);
    }
    //如果创建失败
    if (slowpath(!obj)) {
        if (construct_flags & OBJECT_CONSTRUCT_CALL_BADALLOC) {
            return _objc_callBadAllocHandler(cls);
        }
        return nil;
    }
    //这里是去创建isa指针
    if (!zone && fast) {
        //该方法内部为 initIsa(cls, true, hasCxxDtor); 因此实际上和else中的方法是一样的,只是参数不一样而已
        obj->initInstanceIsa(cls, hasCxxDtor);
    } else {
        obj->initIsa(cls);
    }
    //如果没有c++析构器,那么返回创建好的对象
    if (fastpath(!hasCxxCtor)) {
        return obj;
    }
    //处理构造器,暂不研究
    construct_flags |= OBJECT_CONSTRUCT_FREE_ONFAILURE;
    return object_cxxConstructFromClass(obj, cls, construct_flags);
}
#pragma mark  ------------------------补充 1 :  fastpath 、slowpath
//表示执行bool(x)为1的可能性更大,即执行if的可能性更大
#define fastpath(x) (__builtin_expect(bool(x), 1))
//表示执行bool(x)为0的可能性更大,即执行else的可能性更大
#define slowpath(x) (__builtin_expect(bool(x), 0))

#pragma mark  ------------------------补充 2 : cls->ISA()->hasCustomAWZ()

//功能 : 判断cls的(元)类对象是否有重写的allocWithZone()方法
//简单的分析如下 :
//首先,cls->ISA()得到的是一个Class类型的数据,也就是,通过cls的isa获取得到对应的(元)类对象
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
//从而可以知道,cls->ISA()->hasCustomAWZ()实际上就是对cls的(元)类对象调用hasCustomAWZ()方法,其方法如下,
bool hasCustomAWZ() const {
    return !cache.getBit(FAST_CACHE_HAS_DEFAULT_AWZ);
}
//FAST_CACHE_HAS_DEFAULT_AWZ的定义,实际上其值就是 1后面有14个0
#define FAST_CACHE_HAS_DEFAULT_AWZ    (1<<14)
//cache是一个cache_t结构体类型数据,其中的getBit方法如下
//该方法意思就是判断当前(元)类对象的_flags中第15位(从后向前数)是否为1
bool getBit(uint16_t flags) const {
    return _flags & flags;
}
#pragma mark  ------------------------instanceSize() 对象的最小字节数
//获取一个对象的大小,从中可以看到,如果大小小于16个字节,那么返回16个字节,从中可以的中,一个对象的最小字节大小为16个字节
size_t instanceSize(size_t extraBytes) const {
    if (fastpath(cache.hasFastInstanceSize(extraBytes))) {
        return cache.fastInstanceSize(extraBytes);
    }
    //注意这里有一个内存对齐的处理,具体描述在下面
    size_t size = alignedInstanceSize() + extraBytes;
    // CF requires all objects be at least 16 bytes.
    if (size < 16) size = 16;
    return size;
}
//内存对齐处理,从下面的方法可以得知,字节对齐后其值总是为8的倍数
uint32_t alignedInstanceSize() const {
    return word_align(unalignedInstanceSize());
}
static inline uint32_t word_align(uint32_t x) {
    return (x + WORD_MASK) & ~WORD_MASK;
}
define WORD_MASK 7UL

#pragma mark  ------------------------总结
/*
 1、在OC中,调用+alloc方法之后,底层会进行一系列的方法调用,主要路径为
 a、_objc_rootAlloc()
 b、callAlloc()
 c、_class_createInstanceFromZone
 2、在_class_createInstanceFromZone方法中,主要的流程是
 a、计算对象大小,这里会有内对对齐处理
 b、分配空间,按指定的空间,或者由系统进行分配
 c、构建isa,对每个对象构建isa成员值
 d、返回对象
 3、对象的大小,在OC层面,分配的内存大小为最小16字节,不足补齐,在64位系统中,会在这个基础上进行8字节对齐,也就是大小总是8的倍数,而在最后的calloc方法中,会再次进行一次16字节内存对齐,也就是我们最终的对象大小总是16字节的倍数
 */




