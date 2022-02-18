#ifndef MY_COMPLEX_H
#define MY_COMPLEX_H
struct complex {
    double a;
    double b;    
};
complex toComplex(double a, double b) {
    complex zr = {a, b};
    return zr;
}
double dot(complex z1, complex z2) {
    return (z1.a * z2.a + z1.b * z2.b);
}
double det(complex z1, complex z2) {
    return (z1.a * z2.b - z1.b * z2.a);
}
complex conjugate(complex z) {
    complex zr = {z.a, -z.b};
    return zr;
}
complex operator*(double c, complex z) {
    complex zr = {c*z.a, c*z.b};
    return zr;
}
complex operator*(complex z1, complex z2) { 
    complex zr = {
        dot(conjugate(z1), z2),
        det(conjugate(z1), z2)
    };
    return zr;
}
complex operator+(complex z1, complex z2) {
    complex z3 = {z1.a + z2.a, z1.b + z2.b};
    return z3;
}


complex operator*(complex z, double c) { return c*z; }
complex operator/(complex z, double c) { return z*(1/c); }
complex recip(complex z) { return conjugate(z)/dot(z,z); }
complex operator/(double c, complex z) { return c*recip(z); }
complex operator/(complex z1, complex z2) { return z1*recip(z2); }
complex operator-(complex z) { return -1*z; }
complex operator-(complex z1, complex z2) { return z1 + (-z2); }
    

double EuclidDist(complex z1, complex z2) {
    complex diff = z2 - z1;
    return dot(diff,diff);
}
#endif