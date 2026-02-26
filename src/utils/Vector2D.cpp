//
// Created by Weckest on 2026-01-28.
//

#include "Vector2D.h"

#include <cmath>

Vector2D &Vector2D::normalize() {
    float length = std::sqrt(x * x + y * y);

    if (length > 0) {
        this->x /= length;
        this->y /= length;
    }

    return *this;
}

Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D (x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D (x / scalar, y / scalar);
}

Vector2D Vector2D::operator-(const Vector2D& vector) const {
    return Vector2D (x - vector.x, y - vector.y);
}

Vector2D Vector2D::operator+(const Vector2D& vector) const {
    return Vector2D (x + vector.x, y + vector.y);
}

Vector2D& Vector2D::operator+=(const Vector2D &vec) {
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D &vec) {
    this->x -= vec.x;
    this->y -= vec.y;
    return *this;
}

Vector2D &Vector2D::operator*=(float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}

Vector2D &Vector2D::operator/=(float scalar) {
    this->x /= scalar;
    this->y /= scalar;
    return *this;
}

bool Vector2D::operator==(const Vector2D &vector) const {
    return (x == vector.x && y == vector.y);
}

bool Vector2D::operator!=(const Vector2D &vector) const {
    return (x != vector.x || y != vector.y);
}

Vector2D Vector2D::operator-() const {
    return Vector2D (-x, -y);
}

Vector2D operator*(float scalar, Vector2D& vec) {
    return Vector2D (vec.x * scalar, vec.y * scalar);
}
