# KAKAOS-MINI-LINUX
这个project的主要想法是将KAKAOS（从commit id 67426c8开始）重写成一个mini-linux，准确点来说是把linux上的一些框架流程在KAKAOS上实现一遍
为了简化调试，这个项目准备只在stm32f103上实现并验证，这样可以精简KAKAOS的工程

# 主要想做的工作如下
## 1. 实现调度组
## 2. 实现调度实体
## 3. VFS && proc文件系统 && sysfs文件系统
## 4. 像linux一样多种调度类
## 5. 解析dtb？
## 6. printk打印级别
## 7. 用户态和内核态
## 8. 就绪队列
