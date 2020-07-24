#pragma mark  ------------------------总结
/*
在之前的学习中,主要是
1、分析了NSObject的底层结构,这里了解了objc_object、obje_class两种结构体,其中,NSObject在底层就是一个objc_class结构体,其内部成员主要有isa、superclass、cache、bits
2、分析了objc_object中唯一的成员isa,其结构是一个isa_t的联合体,内部有两个成员cls、bits,cls是一个class类型,bits使用位域存储数据
3、分析了NSObject的创建流程,主要是通过alloc->_objc_rootAlloc()->callalloc()->_objc_rootAllocWithZone()->_class_createInstanceFromZone(),最终的一个细节流程主要是,先进行内存大小的计算,这里涉及到了内存对齐的处理,然后进行内存空间的申请与分配,这里再次进行了内存对齐处理,然后进行了isa指针的初始化,期间也有一些C++构造器的处理,最终返回一个对象
4、分析了isa的创建过程,其过程相对简单,主要是对bits位域的数据处理,特别是第4位开始对象地址字段
5、分析了对象的类型,了解了实例对象、类对象、元类对象的创建方法、创建流程,从而认识到了isa指针的作用
6、分析了objc_class中第一个成员superclass,由于没有找到源码,因此写了些自己的理解
7、总结了isa和superclass的作用
*/

#pragma mark  ------------------------补充 1 : Class 和 id
typedef struct objc_class *Class;
typedef struct objc_object *id;
/*
从定义可以看出,Class是一个objc_class类型的指针,id是一个objc_object类型的指针,因此可以得出
1、Class是一个对象的指针,而id仅仅是指向了一个isa地址,因此,id可以表示任何首地址为isa的对象,包括不仅限于objc_class对象
*/



