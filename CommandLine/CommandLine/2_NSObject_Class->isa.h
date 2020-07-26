#pragma mark  ------------------------初识
//在objc_object结构体中可以看到,其内部有一个isa_t类型的isa成员变量
struct objc_object {
    isa_t isa;
}

#pragma mark  ------------------------isa
//isa_t的定义如下代码所示,其类型是一个union联合体,其内部有两个成员
//cls : Class类型,即objc_class指针类型
//bits : uintptr_t类型,即unsigned long类型,长度为8个字节
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

//从其类型不难得出,当isa_t值为cls时,表示isa是一个objc_class的指针,也就是指向一个对象

#pragma mark  ------------------------isa->bits
//其类型为unsigned long,长度为8个字节,即64位,而表示一个地址不需要用到这么大的空间,为了充分使用这块内存空间,使用了位域的形式来存储数据
//其具体的表示如下
//需要说明的是,在不同的架构平台上,位域的占用大小、位置也是也是有一定差异性的,以下是在arm64平台下的定义
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

#pragma mark  ------------------------补充union联合体的定义
/*
 union联合体的定义 :
 联合体表示的是所有成员共用一块内存,这块内存同时间只能表示其中一个成员的值
 */
#pragma mark  ------------------------struct与union联合体的共同点和区别
/*
 struct与union联合体的共同点和区别
 共同点 : 都可以包含基本数据类型,如int、char等
 不同点 : 1、struct成员在内部有自己的内存空间,一个struct的大小为成员大小之和的总值经过内存对齐之后的大小,在ios中总是16的倍数,同时也会是结构体中最长的那个成员的倍数
 但是union中的成员在内部是共享一个内存空间的,其长度总是等于最长的那个成员的长度
 2、对struct成员进行赋值,并不会对其他成员造成影响,但是对union中的成员进行赋值,会影响到其他成员,因为union是内存覆盖,同时间只能表示一个成员的值.
 */
#pragma mark  ------------------------位域
/*
 位域 : 指定数据所占用的内存大小,一块内存空间可能不会被完全使用,而有些数据只需要小部分空间就可以表示,因此使用位域的形式来减少内存空间开销
 */
#pragma mark  ------------------------总结
/*
 1、isa出现在objc_object结构体中,从之前的分析可以知道,所有的类都存在一个isa成员
 2、isa实际上是一个isa_t的类型数据,本质上是一个unsigned long类型,其长度为8字节
 3、isa_t类型是一个联合体,其内部有2个成员,分别是cls、bits
 4、cls是class类型,指向一个objc_class类型数据,即指向一个对象
 5、bits使用位域存储数据,在不同的空间内定义了不同用途的数据
 */
