#ifndef METRICS_HPP
#define METRICS_HPP

struct Metrics {
    long long count = 0;

    // helpers opcionales (solo para legibilidad)
    void assign(long long n = 1) { count += n; }
    void compare(long long n = 1) { count += n; }
    void access(long long n = 1) { count += n; }
    void arithmetic(long long n = 1) { count += n; }
    void call(long long n = 1) { count += n; }
};

#endif
