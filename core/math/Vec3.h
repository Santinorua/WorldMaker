#pragma once
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>


namespace WorldMaker
{
	struct Vec2;
	struct Vec4;
	struct Vec3
	{
	    double x;
	    double y;
	    double z;

	    // Constructor
	    Vec3(double xP = 0.0f, double yP = 0.0f, double zP = 0.0f);
	    Vec3(const glm::vec3& other);

	    Vec3(const Vec2& other);
	    Vec3(const Vec4& other);


	    // Basic operators
	    Vec3 operator+(Vec3 other) const;
	    Vec3 operator-(Vec3 other) const;
	    Vec3 operator*(Vec3 other) const;
	    Vec3 operator*(double number) const;
	    Vec3 operator/(Vec3 other) const;
	    Vec3 operator/(double number) const;

		// Vector operations
	    double Magnitude() const;
	    Vec3 Normalized() const;
	    Vec3 To(Vec3 target) const;
	    double DotProduct(Vec3 other) const;
		Vec3 CrossProduct(Vec3 other) const;
		static Vec3 GetDirectionFromEuler(Vec3 eulerAngles);


	    // Common directions
	    static Vec3 Right();
	    static Vec3 Left();
	    static Vec3 Up();
	    static Vec3 Down();
	    static Vec3 Forward();
		static Vec3 Backward();
	    static Vec3 Zero();
	    static Vec3 One();

		// Compound assignment operators
	    Vec3& operator=(Vec3 other);
	    Vec3& operator+=(Vec3 other);
	    Vec3& operator-=(Vec3 other);
	    Vec3& operator*=(Vec3 other);
	    Vec3& operator/=(Vec3 other);
	    Vec3& operator*=(double number);
	    operator glm::vec4() {
	        return glm::vec4(x, y, z, 0);
	    }
	    operator glm::vec3() {
	        return glm::vec3(x, y, z);
	    }
	    operator glm::vec2() {
	        return glm::vec2(x, y);
	    }
	    // Output
	    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
	};
}
