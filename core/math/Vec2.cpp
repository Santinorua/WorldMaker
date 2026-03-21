#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace WorldMaker
{
	Vec2::Vec2(double xP, double yP) : x(xP), y(yP) {}
	Vec2::Vec2(const glm::vec2& other) : Vec2(other.x, other.y) {}

	Vec2::Vec2(const Vec3& other) : Vec2(other.x, other.y) {}
	Vec2::Vec2(const Vec4& other) : Vec2(other.x, other.y) {}


	Vec2 Vec2::operator+(Vec2 other) const {
	    return Vec2(x + other.x, y + other.y);
	}

	Vec2 Vec2::operator-(Vec2 other) const {
	    return Vec2(x - other.x, y - other.y);
	}

	Vec2 Vec2::operator*(Vec2 other) const {
	    return Vec2(x * other.x, y * other.y);
	}

	Vec2 Vec2::operator*(double number) const {
	    return Vec2(x * number, y * number);
	}

	Vec2 Vec2::operator/(Vec2 other) const {
	    return Vec2(x / other.x, y / other.y);
	}

	Vec2 Vec2::operator/(double number) const {
	    return Vec2(x / number, y / number);
	}

	double Vec2::Magnitude() const {
	    return std::sqrt(x * x + y * y);
	}

	Vec2 Vec2::Normalized() const {
	    double mag = Magnitude();
	    return (mag != 0) ? Vec2(x / mag, y / mag) : Zero();
	}

	Vec2 Vec2::To(Vec2 target) const {
	    return target - *this;
	}

	double Vec2::DotProduct(Vec2 other) const {
	    Vec2 a = Normalized();
	    Vec2 b = other.Normalized();
	    return static_cast<double>(a.x * b.x + a.y * b.y);
	}

	Vec2 Vec2::Right() { return Vec2(1, 0); }
	Vec2 Vec2::Left() { return Vec2(-1, 0); }
	Vec2 Vec2::Up() { return Vec2(0, 1); }
	Vec2 Vec2::Down() { return Vec2(0, -1); }
	Vec2 Vec2::Zero() { return Vec2(0, 0); }
	Vec2 Vec2::One() { return Vec2(1, 1); }

	Vec2& Vec2::operator=(Vec2 other) {
	    x = other.x;
	    y = other.y;
	    return *this;
	}

	Vec2& Vec2::operator+=(Vec2 other) {
	    x += other.x;
	    y += other.y;
	    return *this;
	}

	Vec2& Vec2::operator-=(Vec2 other) {
	    x -= other.x;
	    y -= other.y;
	    return *this;
	}

	Vec2& Vec2::operator*=(Vec2 other) {
	    x *= other.x;
	    y *= other.y;
	    return *this;
	}

	Vec2& Vec2::operator/=(Vec2 other) {
	    x /= other.x;
	    y /= other.y;
	    return *this;
	}

	Vec2& Vec2::operator*=(double number) {
	    x *= number;
	    y *= number;
	    return *this;
	}

	std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	    os << "(" << v.x << ", " << v.y << ")";
	    return os;
	}
}
