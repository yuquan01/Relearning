#pragma mark  ------------------------isKindOfClass
//方法实现,可以看出,isKindOfClass()方法是判断当前对象是否是指定对象类型,并且进行父类对象循环判断,即当前对象是否是指定对象的派生对象
+ (BOOL)isKindOfClass:(Class)cls {
    for (Class tcls = self->ISA(); tcls; tcls = tcls->superclass) {
        if (tcls == cls) return YES;
    }
    return NO;
}

- (BOOL)isKindOfClass:(Class)cls {
    for (Class tcls = [self class]; tcls; tcls = tcls->superclass) {
        if (tcls == cls) return YES;
    }
    return NO;
}

#pragma mark  ------------------------isMemberOfClass
//方法实现,可以看出,isMemberOfClass()方法是判断当前对象的isa指针是否指向指定对象
+ (BOOL)isMemberOfClass:(Class)cls {
    return self->ISA() == cls;
}

- (BOOL)isMemberOfClass:(Class)cls {
    return [self class] == cls;
}

#pragma mark  ------------------------例子
//NSObject
//SuperObject : NSObject
//SubObject : SuperObject

NSObject *obj = [[NSObject alloc] init];
SuperObject *superObj = [[SuperObject alloc] init];
SubObject *subObj = [[SubObject alloc] init];

[obj isKindOfClass:[NSObject class]];     //YES
[superObj isKindOfClass:[NSObject class]];//YES
[subObj isKindOfClass:[NSObject class]];  //YES

[superObj isKindOfClass:[SuperObject class]];//YES
[subObj isKindOfClass:[SubObject class]];//YES

[obj isMemberOfClass:[NSObject class]];     //YES
[superObj isMemberOfClass:[NSObject class]];//NO
[subObj isMemberOfClass:[NSObject class]];  //NO

[superObj isMemberOfClass:[SuperObject class]];//YES
[subObj isMemberOfClass:[SubObject class]];//YES

#pragma mark  ------------------------总结
/*
 1、isKindOfClass()方法用于判断当前对象是否是指定对象的派生对象
 2、isMemberOfClass()方法用于判断当前对象是否和指定对象一致
 3、从中可以看出,如果一个对象和指定对象其isMemberOfClass()结果为YES,那么其isKindOfClass()结果必为YES,反之则可能不成立.
 */


