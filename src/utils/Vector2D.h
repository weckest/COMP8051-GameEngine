//
// Created by Weckest on 2026-01-28.
//

#ifndef INC_8051TUTORIAL_VECTOR2D_H
#define INC_8051TUTORIAL_VECTOR2D_H

class Vector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vector2D() {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2D(float x, float y) : x(x), y(y) {}

    Vector2D& normalize();

    //member operator function
    //Vector2D has to be on the left and float on the right
    Vector2D operator*(float scalar) const;

    Vector2D operator/(float scalar) const;

    Vector2D operator+(const Vector2D& vector) const;

    Vector2D operator-(const Vector2D& vector) const;

    Vector2D& operator+=(const Vector2D& vec);

    Vector2D& operator-=(const Vector2D& vec);

    Vector2D& operator*=(float scalar);

    Vector2D& operator/=(float scalar);

    bool operator==(const Vector2D& vector) const;

    bool operator!=(const Vector2D& vector) const;

    Vector2D operator-() const;


    //Vector2D is on the left and float is on the left
    //friends makes it a non member function
    friend Vector2D operator*(float scalar, Vector2D& vec);


};

#endif //INC_8051TUTORIAL_VECTOR2D_H