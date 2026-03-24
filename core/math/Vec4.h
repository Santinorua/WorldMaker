#pragma once

#include <cmath>
#include <iostream>
#include "glm/glm.hpp"

namespace WorldMaker
{
	struct Vec2;
	struct Vec3;
	struct Vec4
	{
	    double x;
	    double y;
	    double z;
	    double w;

	    // Constructor
	    Vec4(double xP = 0.0f, double yP = 0.0f, double zP = 0.0f, double wP = 0.0f);
		Vec4(const glm::vec4& other);

	    Vec4(const Vec2& other);
	    Vec4(const Vec3& other);

	    // Basic operators
	    Vec4 operator+(Vec4 other) const;
	    Vec4 operator-(Vec4 other) const;
	    Vec4 operator*(Vec4 other) const;
	    Vec4 operator*(double number) const;
	    Vec4 operator/(Vec4 other) const;
	    Vec4 operator/(double number) const;

	    // Vector operations
	    double Magnitude() const;
	    Vec4 Normalized() const;
	    Vec4 To(Vec4 target) const;
	    double DotProduct(Vec4 other) const;

	    // Common directions
	    static Vec4 Right();     // (1, 0, 0, 0)
	    static Vec4 Left();      // (-1, 0, 0, 0)
	    static Vec4 Up();        // (0, 1, 0, 0)
	    static Vec4 Down();      // (0, -1, 0, 0)
	    static Vec4 Forward();   // (0, 0, 1, 0)
	    static Vec4 Backward();  // (0, 0, -1, 0)
	    static Vec4 In();        // (0, 0, 0, 1)
	    static Vec4 Out();       // (0, 0, 0, -1)
	    static Vec4 Zero();      // (0, 0, 0, 0)
	    static Vec4 One();       // (1, 1, 1, 1)

	    // Compound assignment operators
	    Vec4& operator=(Vec4 other);
	    Vec4& operator+=(Vec4 other);
	    Vec4& operator-=(Vec4 other);
	    Vec4& operator*=(Vec4 other);
	    Vec4& operator/=(Vec4 other);
	    Vec4& operator*=(double number);
		operator glm::vec4() {
			return glm::vec4(x, y, z, w);
		}
	    operator glm::vec3() {
	        return glm::vec3(x, y, z);
	    }
	    operator glm::vec2() {
	        return glm::vec2(x, y);
	    }
	    // Output
	    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
	};
}
