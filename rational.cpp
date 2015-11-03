#include "rational.h"

#include <QString>
#include <QRegExp>
#include <QDebug>

//Rational::Rational(Rational &r) {
//    Rational(r.numerator, r.denominator);
//}
//Rational::Rational(Rational *r) {
//    Rational(r->numerator, r->denominator);
//}
Rational::Rational() : numerator(0), denominator(1) {
}
Rational::Rational(const Rational &r) {
    numerator = r.numerator;
    denominator = r.denominator;
}

Rational::Rational(QString expression) {
    QRegExp reg("([\\+\\-\\d]+)(/([\\+\\-\\d]+))?");
    if (expression.indexOf(reg) < 0) throw QString("Expression not valid. Should be xx/xx");
    long numerator = reg.cap(1).toLong();
    long denominator = (reg.matchedLength() >= 3) ? reg.cap(3).toLong() : 1;
    Rational r(numerator, denominator);
    this->numerator = r.numerator;
    this->denominator = r.denominator;
}
Rational::Rational(long numerator, long denominator)
{
    if (denominator == 0) throw QString("Denominator must be a non-zero number !");
    long gcd = Rational::gcd(numerator, denominator);
    this->numerator = numerator / gcd;
    this->denominator = denominator / gcd;
}

Rational::operator double() const {
    return (double)numerator / (double)denominator;
}
Rational::operator float() const {
    return (float)numerator / (float)denominator;
}
Rational::operator int() const {
    return (int)numerator / (int)denominator;
}
QString Rational::toString() const {
    return QString("%1/%2").arg(numerator).arg(denominator);
}

Rational& operator +(Rational &r1, Rational &r2) {
    long gcd = Rational::gcd(r1.denominator, r2.denominator);
    long m1 = r2.denominator / gcd;
    long m2 = r1.denominator / gcd;
    Rational *r = new Rational(r1.numerator * m1 + r2.numerator * m2, r1.denominator * m1);
    return *r;
}
Rational& operator -(Rational &r1, Rational &r2) {
    long gcd = Rational::gcd(r1.denominator, r2.denominator);
    long m1 = r2.denominator / gcd;
    long m2 = r1.denominator / gcd;
    Rational *r = new Rational(r1.numerator * m1 - r2.numerator * m2, r1.denominator * m1);
    return *r;
}
Rational& operator *(Rational &r1, Rational &r2) {
    long gcd = Rational::gcd(Rational::gcd(r1.numerator, r2.denominator), Rational::gcd(r1.denominator, r2.numerator));
    Rational *r = new Rational(r1.numerator * (r2.numerator / gcd), r1.denominator * (r2.denominator / gcd));
    return *r;
}
Rational& operator /(Rational &r1, Rational &r2) {
    Rational rr2(r2.denominator, r2.numerator);
    return r1 * rr2;
}

//Rational Rational::operator +(Rational r1, Rational r2) { return r1 + r2; }
//Rational Rational::operator -(Rational r1, Rational r2) { return r1 + r2; }
//Rational Rational::operator *(Rational r1, Rational r2) { return r1 + r2; }
//Rational Rational::operator /(Rational r1, Rational r2) { return r1 + r2; }

//Rational& operator =(Rational &r) {
//    Rational(r.numerator, r.denominator);
//    return *this;
//}
Rational& operator -(Rational &r) {
    Rational *rr = new Rational(-r.numerator, r.denominator);
    return *rr;
}
Rational& operator +=(Rational &r1, Rational &r2) {
    Rational r = r1 + r2;
    r1.numerator = r.numerator;
    r1.denominator = r.denominator;
    return r1;
}
Rational& operator -=(Rational &r1, Rational &r2) {
    Rational r = r1 - r2;
    r1.numerator = r.numerator;
    r1.denominator = r.denominator;
    return r1;
}
Rational& operator *=(Rational &r1, Rational &r2) {
    Rational r = r1 * r2;
    r1.numerator = r.numerator;
    r1.denominator = r.denominator;
    return r1;
}
Rational& operator /=(Rational &r1, Rational &r2) {
    Rational r = r1 / r2;
    r1.numerator = r.numerator;
    r1.denominator = r.denominator;
    return r1;
}
