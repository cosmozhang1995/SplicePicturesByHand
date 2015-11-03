#ifndef RETIONAL_H
#define RETIONAL_H

class QString;

class Rational
{
public:
//    Rational(Rational &r);
//    Rational(Rational *r);
    Rational();
    Rational(const Rational &r);
    Rational(QString expression);
    Rational(long numerator, long denominator = 1);
    operator double() const;
    operator float() const;
    operator int() const;
    QString toString() const;
    friend Rational& operator +(Rational &r1, Rational &r2);
    friend Rational& operator -(Rational &r1, Rational &r2);
    friend Rational& operator *(Rational &r1, Rational &r2);
    friend Rational& operator /(Rational &r1, Rational &r2);
//    Rational& operator =(Rational &r);
    friend Rational& operator -(Rational &r);
    friend Rational& operator +=(Rational &r1, Rational &r2);
    friend Rational& operator -=(Rational &r1, Rational &r2);
    friend Rational& operator *=(Rational &r1, Rational &r2);
    friend Rational& operator /=(Rational &r1, Rational &r2);

    static long gcd(long a, long b) {
        while (a != 0) {
            b = b % a;
            if (b == 0) {
                return a;
            }
            a = a % b;
            if (a == 0) {
                return b;
            }
        }
        return 1;
    }
    static long lcm(long a, long b) {
        return (a * b) / Rational::gcd(a,b);
    }
private:
    long numerator;
    long denominator;
};

#endif // RETIONAL_H
