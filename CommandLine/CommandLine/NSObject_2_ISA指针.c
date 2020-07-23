#pragma mark  ------------------------isa
//在objc_object结构体中可以看到,其内部有一个isa_t类型的isa成员变量

#pragma mark  ------------------------isa的底层实现
//isa_t的定义如下代码所示,其类型是一个union联合体,其内部有两个成员
//cls : Class类型,即objc_class指针类型
//bits : uintptr_t类型,即unsigned long类型,长度为8个字节

/*
 union联合体的定义 :
 联合体表示的是所有成员共用一块内存,这块内存同时间只能表示其中一个成员的值
 */
/*
 struct与union联合体的共同点和区别
 共同点 : 都可以包含基本数据类型,如int、char等
 不同点 : 1、struct成员在内部有自己的内存空间,一个struct的大小为成员大小之和的总值经过内存对齐之后的大小,在ios中总是16的倍数,同时也会是结构体中最长的那个成员的倍数
 但是union中的成员在内部是共享一个内存空间的,其长度总是等于最长的那个成员的长度
 2、对struct成员进行赋值,并不会对其他成员造成影响,但是对union中的成员进行赋值,会影响到其他成员,因为union是内存覆盖,同时间只能表示一个成员的值.
 */
typedef unsigned long           uintptr_t;

union isa_t {
    isa_t() { }
    isa_t(uintptr_t value) : bits(value) { }
    
    Class cls;
    uintptr_t bits;
#if defined(ISA_BITFIELD)
    struct {
        ISA_BITFIELD;//其定义在下面
    };
#endif
};
#pragma mark  ------------------------isa->cls

//从其类型不难得出,当isa_t值为cls时,表示isa是一个objc_class的指针

#pragma mark  ------------------------isa->bits
//其类型为unsigned long,长度为8个字节,即64位,而表示一个地址不需要用到这么大的空间,为了充分使用这块内存空间,使用了位域的形式来存储数据
//其具体的表示如下
//需要说明的是,在不同的架构平台上,位域的占用大小、位置也是也是有一定差异性的,以下是在arm64平台下的定义

/*
 位域 : 指定数据所占用的内存大小,一块内存空间可能不会被完全使用,而有些数据只需要小部分空间就可以表示,因此使用位域的形式来减少内存空间开销
 */
# if __arm64__
//一些辅助数据的定义
#   define ISA_MASK        0x0000000ffffffff8ULL
#   define ISA_MAGIC_MASK  0x000003f000000001ULL
#   define ISA_MAGIC_VALUE 0x000001a000000001ULL
//这部分开始才是位域的具体定义部分
#   define ISA_BITFIELD                                                      \
uintptr_t nonpointer        : 1;                                       \
uintptr_t has_assoc         : 1;                                       \
uintptr_t has_cxx_dtor      : 1;                                       \
uintptr_t shiftcls          : 33; /*MACH_VM_MAX_ADDRESS 0x1000000000*/ \
uintptr_t magic             : 6;                                       \
uintptr_t weakly_referenced : 1;                                       \
uintptr_t deallocating      : 1;                                       \
uintptr_t has_sidetable_rc  : 1;                                       \
uintptr_t extra_rc          : 19
//位域定义部分结束
#   define RC_ONE   (1ULL<<45)
#   define RC_HALF  (1ULL<<18)
/*
 位域内容解释
 uintptr_t nonpointer        : 1;
 表示是否开启指针优化,
 0 : 不优化,访问isa指针时,直接返回cls
 1 : 优化,表示isa指针内容还包含一些其他内容
 
 uintptr_t has_assoc         : 1;
 表示是否有关联对象
 
 uintptr_t has_cxx_dtor      : 1;
 表示是否有c++析构器
 0 : 没有析构器,更快的释放对象
 1 : 有析构器,需要对析构器做一些处理
 
 uintptr_t shiftcls          : 33;
 表示类地址,其值实际上是 isa & ISA_MASK 得到的值

 uintptr_t magic             : 6;
 用于判断当前对象是否时真的对象,还是没有初始化的内容空间
 
 uintptr_t weakly_referenced : 1;
 用于判断是否被指向或者曾经被指向一个弱引用变量,如果没有指向,则释放更快速

 uintptr_t deallocating      : 1;
 表示当前对象是否正在释放

 uintptr_t has_sidetable_rc  : 1;
 判断当前对象引用计数是否有进位,
 0 : 不实用side_table存储引用计数
 1 : 使用了side_table存储引用计数
 
 uintptr_t extra_rc          : 19
 表示当前对象的引用计数值,值为额外的引用计数,所以对象的真实引用计数为 extra_rc + 1
 当能存储下引用计数时, has_sidetable_rc 为0,否则其值为1
 在64为机器中,其能存储的最大长度为 2^19 - 1 + 1的大小
 */

#pragma mark  ------------------------isa的作用

#pragma mark  ------------------------isa的创建


/*
 创建一个类对象isa
 cls : 类对象
 hasCxxDtor : 是否有c++析构器
 */
objc_object::initInstanceIsa(Class cls, bool hasCxxDtor)
{
    ASSERT(!cls->instancesRequireRawIsa());
    ASSERT(hasCxxDtor == cls->hasCxxDtor());
    
    initIsa(cls, true, hasCxxDtor);
}
/*
 创建一个元类对象isa
 cls : 元类对象
 */
objc_object::initClassIsa(Class cls)
{
    if (DisableNonpointerIsa  ||  cls->instancesRequireRawIsa()) {
        initIsa(cls, false/*not nonpointer*/, false);
    } else {
        initIsa(cls, true/*nonpointer*/, false);
    }
}

inline void
objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor)
{
    ASSERT(!isTaggedPointer());
    
    if (!nonpointer) {
        isa = isa_t((uintptr_t)cls);
    } else {
        ASSERT(!DisableNonpointerIsa);
        ASSERT(!cls->instancesRequireRawIsa());
        
        isa_t newisa(0);
        
#if SUPPORT_INDEXED_ISA
        ASSERT(cls->classArrayIndex() > 0);
        newisa.bits = ISA_INDEX_MAGIC_VALUE;
        // isa.magic is part of ISA_MAGIC_VALUE
        // isa.nonpointer is part of ISA_MAGIC_VALUE
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.indexcls = (uintptr_t)cls->classArrayIndex();
#else
        newisa.bits = ISA_MAGIC_VALUE;
        // isa.magic is part of ISA_MAGIC_VALUE
        // isa.nonpointer is part of ISA_MAGIC_VALUE
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.shiftcls = (uintptr_t)cls >> 3;
#endif
        
        // This write must be performed in a single store in some cases
        // (for example when realizing a class because other threads
        // may simultaneously try to use the class).
        // fixme use atomics here to guarantee single-store and to
        // guarantee memory order w.r.t. the class index table
        // ...but not too atomic because we don't want to hurt instantiation
        isa = newisa;
    }
}

#parm mark



