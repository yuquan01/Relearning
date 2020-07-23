#pragma mark  转化命令
//目录下执行该命令,将oc文件转化为cpp文件,方便查看底层实现
xcrun -sdk iphoneos clang -arch arm64 -rewrite-objc main.m -o main.cpp

#pragma mark  结构体与联合体

#pragma mark  位域
/*
 位域 : 指定数据所占用的内存大小,一块内存空间可能不会被完全使用,而有些数据只需要小部分空间就可以表示,因此使用位域的形式来减少内存空间开销
 */
