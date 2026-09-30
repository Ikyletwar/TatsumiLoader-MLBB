#define __SYSCALL_LL_E(x) (x)
#define __SYSCALL_LL_O(x) (x)

// ARM64 stealth syscalls 0–6 args
static inline long __syscall0(long n) {
    long ret;
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0");
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall1(long n, long a) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall2(long n, long a, long b) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x1 __asm__("x1") = b;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x1), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall3(long n, long a, long b, long c) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x1 __asm__("x1") = b;
    register long x2 __asm__("x2") = c;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x1), "r"(x2), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall4(long n, long a, long b, long c, long d) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x1 __asm__("x1") = b;
    register long x2 __asm__("x2") = c;
    register long x3 __asm__("x3") = d;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall5(long n, long a, long b, long c, long d, long e) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x1 __asm__("x1") = b;
    register long x2 __asm__("x2") = c;
    register long x3 __asm__("x3") = d;
    register long x4 __asm__("x4") = e;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

static inline long __syscall6(long n, long a, long b, long c, long d, long e, long f) {
    long ret;
    register long x0 __asm__("x0") = a;
    register long x1 __asm__("x1") = b;
    register long x2 __asm__("x2") = c;
    register long x3 __asm__("x3") = d;
    register long x4 __asm__("x4") = e;
    register long x5 __asm__("x5") = f;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0\n\t"
        "mov %0, x0"
        : "=r"(ret)
        : "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
        : "memory","cc"
    );
    return ret;
}

#define VDSO_USEFUL
#define VDSO_CGT_SYM "__kernel_clock_gettime"
#define VDSO_CGT_VER "LINUX_2.6.39"