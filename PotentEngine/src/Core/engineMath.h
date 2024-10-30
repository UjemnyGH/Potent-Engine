/**
* File origins from github.com/UjemnyGH/ProjectFix
*/

#pragma once
#ifndef _POTENT_ENGINE_ALGEBRA_MATH_
#define _POTENT_ENGINE_ALGEBRA_MATH_

#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace potent {

    typedef float real;

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#ifndef M_180_BY_PI
#define M_180_BY_PI 57.29577951471995
#endif

#ifndef M_PI_BY_180
#define M_PI_BY_180 0.017453292519444445
#endif

    real ToDegrees(real radians) { return radians * M_180_BY_PI; }
    real ToRadians(real degrees) { return degrees * M_PI_BY_180; }

    /**
     * @brief Improved reverse square root from quake
     *
     * @param x
     * @return float
     */
    float RSqrt32(float x) {
        float y = x * 0.5f;
        long i = *reinterpret_cast<long*>(&x);
        i = 0x5F375A86 - (i >> 1);
        float r = *reinterpret_cast<float*>(&i);
        r *= (1.5f - r * r * y);
        r *= (1.5f - r * r * y);

        return r;
    }

    /**
     * @brief Improved reverese square root from quake in 64-bit version
     *
     * @param x
     * @return long double
     */
    long double RSqrt64(long double x) {
        long double y = x * 0.5f;
        long long i = *reinterpret_cast<long long*>(&x);
        i = 0x5FE6EB50C7B537A9 - (i >> 1);
        long double r = *reinterpret_cast<long double*>(&i);
        r *= (1.5 - r * r * y);
        r *= (1.5 - r * r * y);

        return r;
    }

    float Sqrt32(float x) {
        return x * RSqrt32(x);
    }

    long double Sqrt64(long double x) {
        return x * RSqrt64(x);
    }

    template<typename T>
    class Vector {
    public:
        T x, y, z, w;

        Vector<T>() { x = y = z = w = static_cast<T>(0); }
        Vector<T>(T init) { x = y = z = w = init; }
        Vector<T>(T _x, T _y, T _z = static_cast<T>(0), T _w = static_cast<T>(0)) : x(_x), y(_y), z(_z), w(_w) {}

        inline Vector<T> operator+(T v) { return Vector<T>(this->x + v, this->y + v, this->z + v, this->w + v); }
        inline Vector<T> operator-(T v) { return Vector<T>(this->x - v, this->y - v, this->z - v, this->w - v); }
        inline Vector<T> operator*(T v) { return Vector<T>(this->x * v, this->y * v, this->z * v, this->w * v); }
        inline Vector<T> operator/(T v) { return Vector<T>(this->x / v, this->y / v, this->z / v, this->w / v); }

        inline Vector<T> operator+(Vector<T> v) { return Vector<T>(this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w); }
        inline Vector<T> operator-(Vector<T> v) { return Vector<T>(this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w); }
        inline Vector<T> operator*(Vector<T> v) { return Vector<T>(this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w); }
        inline Vector<T> operator/(Vector<T> v) { return Vector<T>(this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w); }

        inline Vector<T> operator+=(T v) { return (*this = this->operator+(v)); }
        inline Vector<T> operator-=(T v) { return (*this = this->operator-(v)); }
        inline Vector<T> operator*=(T v) { return (*this = this->operator*(v)); }
        inline Vector<T> operator/=(T v) { return (*this = this->operator/(v)); }

        inline Vector<T> operator+=(Vector<T> v) { return (*this = this->operator+(v)); }
        inline Vector<T> operator-=(Vector<T> v) { return (*this = this->operator-(v)); }
        inline Vector<T> operator*=(Vector<T> v) { return (*this = this->operator*(v)); }
        inline Vector<T> operator/=(Vector<T> v) { return (*this = this->operator/(v)); }

        inline bool operator==(Vector<T> v) { return x == v.x && y == v.y && z == v.z && w == v.w; }
        inline bool operator!=(Vector<T> v) { return x != v.x || y != v.y || z != v.z || w != v.w; }
        inline bool operator>(Vector<T> v) { return x > v.x || y > v.y || z > v.z; }
        inline bool operator>=(Vector<T> v) { return x >= v.x || y >= v.y || z >= v.z; }
        inline bool operator<(Vector<T> v) { return x < v.x || y < v.y || z < v.z; }
        inline bool operator<=(Vector<T> v) { return x <= v.x || y <= v.y || z <= v.z; }

        /**
         * @brief Scalar (dot) product of vector
         *
         * @param v
         * @return T
         */
        inline T Dot(Vector<T> const& v) { return x * v.x + y * v.y + z * v.z + w * v.w; }

        /**
         * @brief Scalar (dot) product of vector
         *
         * @param v
         * @return T
         */
        inline static T Dot(Vector<T> v, Vector<T> v2) { return v.x * v2.x + v.y * v2.y + v.z * v2.z + v.w * v2.w; }

        /**
         * @brief Cross product of vector
         *
         * @param v2
         * @return Vector<T>
         */
        inline Vector<T> Cross(Vector<T> const& v2) { return Vector<T>(y * v2.z - z * v2.y, z * v2.x - x * v2.z, x * v2.y - y * v2.x, 0.0f); }

        /**
         * @brief Cross product of vector
         *
         * @param v2
         * @return Vector<T>
         */
        inline static Vector<T> Cross(Vector<T> v, Vector<T> v2) { return Vector<T>(v.y * v2.z - v.z * v2.y, v.z * v2.x - v.x * v2.z, v.x * v2.y - v.y * v2.x, 0.0f); }

        /**
         * @brief Length of vector
         *
         * @return T
         */
        inline T Length() { return Sqrt32(Dot(*this)); }

        /**
         * @brief Length of vector
         *
         * @param v
         * @return T
         */
        inline static T Length(Vector<T> v) { return Sqrt32(Vector<T>::Dot(v, v)); }

        /**
         * @brief Distance to another vector
         *
         * @param v2
         * @return T
         */
        inline T Distance(Vector<T> const& v2) { return Vector<T>::Length(*this - v2); }

        /**
         * @brief Distance to another vector
         *
         * @param v2
         * @return T
         */
        inline static T Distance(Vector<T> v, Vector<T> v2) { return Vector<T>::Length(v - v2); }

        /**
         * @brief Normalized vector
         *
         * @return Vector<T>
         */
        inline Vector<T> Normalize() { return *this * (static_cast<T>(1) / Length()); }

        /**
         * @brief Normalized vector
         *
         * @return Vector<T>
         */
        inline static Vector<T> Normalize(Vector<T> v) { return v * (static_cast<T>(1) / Vector<T>::Length(v)); }

        /**
         * @brief Converting vector to degrees
         *
         * @return Vector<T>
         */
        inline Vector<T> ToDeg() { return *this * (180.0f / M_PI); }

        /**
         * @brief Converting vector to degrees
         *
         * @return Vector<T>
         */
        inline static Vector<T> ToDeg(Vector<T> v) { return v * (180.0f / M_PI); }

        /**
         * @brief Converting vector to radians
         *
         * @return Vector<T>
         */
        inline Vector<T> ToRad() { return *this * (M_PI / 180.0f); }

        /**
         * @brief Converting vector to radians
         *
         * @return Vector<T>
         */
        inline static Vector<T> ToRad(Vector<T> v) { return v * (M_PI / 180.0f); }

        /**
         * @brief Give plane normal
         *
         * @param pv1
         * @param pv2
         * @param pv3
         * @return Vector<T>
         */
        inline static Vector<T> PlaneNormal(Vector<T> pv1, Vector<T> pv2, Vector<T> pv3) {
            return (pv2 - pv1).Cross(pv3 - pv1).Normalize();
        }

        inline Vector<T> Abs() { return Vector<T>(std::abs(this->x), std::abs(this->y), std::abs(this->z), std::abs(this->w)); }
        inline static Vector<T> Abs(Vector<T> v) { return Vector<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w)); }

        inline Vector<T> Negate() { return Vector<T>(-x, -y, -z, -w); }
        inline static Vector<T> Negate(Vector<T> v) { return Vector<T>(-(v.x), -(v.y), -(v.z), -(v.w)); }

        inline Vector<T> Clamp(T const& min, T const& max) { return Vector<T>(std::clamp(x, min, max), std::clamp(y, min, max), std::clamp(z, min, max), std::clamp(w, min, max)); }
        inline static Vector<T> Clamp(Vector<T> v, T min, T max) { return Vector<T>(std::clamp(v.x, min, max), std::clamp(v.y, min, max), std::clamp(v.z, min, max), std::clamp(v.w, min, max)); }
    };

    typedef Vector<float> FVec;
    typedef Vector<double> DVec;
    typedef Vector<real> RVec;

    template<typename T>
    class Matrix3 {
    public:
        T m[9];

        Matrix3<T>() { Fill(static_cast<T>(0)); }
        Matrix3<T>(T init) { Fill(init); }
        Matrix3<T>(Matrix3<T> const& v) { std::memcpy(m, v.m, 9 * sizeof(T)); }

        void Fill(T value) { std::fill(m, m + 9, value); }

        static Matrix3<T> Identity() {
            Matrix3<T> mat;

            mat.m[0] = static_cast<T>(1);
            mat.m[4] = static_cast<T>(1);
            mat.m[8] = static_cast<T>(1);

            return mat;
        }

        Matrix3<T> operator+(Matrix3<T> const& t) {
            Matrix3<T> result;

            for (int i = 0; i < 9; i++) {
                result.m[i] = m[i] + t.m[i];
            }

            return result;
        }

        Matrix3<T> operator-(Matrix3<T> const& t) {
            Matrix3<T> result;

            for (int i = 0; i < 9; i++) {
                result.m[i] = m[i] - t.m[i];
            }

            return result;
        }

        Matrix3<T> operator*(Matrix3<T> const& t) {
            Matrix3<T> result;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    result.m[i * 3 + j] = m[i * 3] * t.m[j * 3] + m[i * 3 + 1] * t.m[j * 3 + 1] + m[i * 3 + 2] * t.m[j * 3 + 2];
                }
            }

            return result;
        }

        Matrix3<T> operator*(T const& v) {
            Matrix3<T> mat;

            for (int i = 0; i < 9; i++) {
                mat.m[i] = m[i] * v;
            }

            return mat;
        }

        Matrix3<T> operator/(T const& v) {
            Matrix3<T> mat;

            for (int i = 0; i < 9; i++) {
                mat.m[i] = m[i] / v;
            }

            return mat;
        }

        Matrix3<T> operator+=(Matrix3<T> const& t) { return (*this = operator+(t)); }
        Matrix3<T> operator-=(Matrix3<T> const& t) { return (*this = operator-(t)); }
        Matrix3<T> operator*=(Matrix3<T> const& t) { return (*this = operator*(t)); }

        Matrix3<T> Inverse() {
            Matrix3<T> mat;

            /**
                    | 0 1 2 |
                    | 3 4 5 |
                    | 6 7 8 |
            */

            mat.m[0] = m[4] * m[8] - m[5] * m[7];
            mat.m[1] = m[3] * m[8] - m[5] * m[6];
            mat.m[2] = m[3] * m[7] - m[4] * m[6];
            mat.m[3] = m[1] * m[8] - m[2] * m[7];
            mat.m[4] = m[0] * m[8] - m[2] * m[6];
            mat.m[5] = m[0] * m[7] - m[1] * m[6];
            mat.m[6] = m[1] * m[5] - m[2] * m[4];
            mat.m[7] = m[0] * m[5] - m[2] * m[3];
            mat.m[8] = m[0] * m[4] - m[1] * m[3];

            T det = m[0] * mat.m[0] + m[1] * mat.m[3] + m[2] * mat.m[6];

            return mat / det;
        }
    };

    template<class T>
    class Matrix4 {
    public:
        T m[16];

        Matrix4<T>() { Fill(static_cast<T>(0)); }
        Matrix4<T>(T v) { Fill(v); }

        /**
         * @brief Fill entire matrix with value
         *
         * @param v
         */
        void Fill(T v) {
            for (int i = 0; i < 16; i++) {
                m[i] = v;
            }
        }

        /**
         * @brief Return all zero matrix
         *
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> Zero() {
            Matrix4<T> mat;

            mat.Fill(static_cast<T>(0));

            return mat;
        }

        /**
         * @brief Return all one matrix
         *
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> One() {
            Matrix4<T> mat;

            mat.Fill(static_cast<T>(1));

            return mat;
        }

        /**
         * @brief Return identity matrix
         *
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> Identity() {
            Matrix4<T> mat;

            mat.m[0] = static_cast<T>(1); mat.m[1] = static_cast<T>(0); mat.m[2] = static_cast<T>(0); mat.m[3] = static_cast<T>(0);
            mat.m[4] = static_cast<T>(0); mat.m[5] = static_cast<T>(1); mat.m[6] = static_cast<T>(0); mat.m[7] = static_cast<T>(0);
            mat.m[8] = static_cast<T>(0); mat.m[9] = static_cast<T>(0); mat.m[10] = static_cast<T>(1); mat.m[11] = static_cast<T>(0);
            mat.m[12] = static_cast<T>(0); mat.m[13] = static_cast<T>(0); mat.m[14] = static_cast<T>(0); mat.m[15] = static_cast<T>(1);

            return mat;
        }

        /**
         * @brief Return rotation in x axis matrix
         *
         * @param angle in radians
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> RotateX(T angle) {
            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[5] = std::cos(angle);
            mat.m[6] = -std::sin(angle);
            mat.m[9] = std::sin(angle);
            mat.m[10] = std::cos(angle);

            return mat;
        }

        /**
         * @brief Return rotation in y axis matrix
         *
         * @param angle in radians
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> RotateY(T angle) {
            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[0] = std::cos(angle);
            mat.m[2] = std::sin(angle);
            mat.m[8] = -std::sin(angle);
            mat.m[10] = std::cos(angle);

            return mat;
        }

        /**
         * @brief Return rotation in z axis matrix
         *
         * @param angle in radians
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> RotateZ(T angle) {
            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[0] = std::cos(angle);
            mat.m[1] = -std::sin(angle);
            mat.m[4] = std::sin(angle);
            mat.m[5] = std::cos(angle);

            return mat;
        }

        /**
         * @brief Return translation matrix
         *
         * @param pos
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> Translate(Vector<T> pos) {
            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[3] = pos.x;
            mat.m[7] = pos.y;
            mat.m[11] = pos.z;

            return mat;
        }

        /**
         * @brief Return scaling matrix
         *
         * @param scl
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> Scale(Vector<T> scl) {
            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[0] = scl.x;
            mat.m[5] = scl.y;
            mat.m[10] = scl.z;

            return mat;
        }

        /**
         * @brief Get X Axis (Column)
         *
         * @return Vector<T>
         */
        Vector<T> GetXAxis() { return Vector<T>(m[0], m[4], m[8], m[12]); }

        /**
         * @brief Get Y Axis (Column)
         *
         * @return Vector<T>
         */
        Vector<T> GetYAxis() { return Vector<T>(m[1], m[5], m[9], m[13]); }

        /**
         * @brief Get Z Axis (Column)
         *
         * @return Vector<T>
         */
        Vector<T> GetZAxis() { return Vector<T>(m[2], m[6], m[10], m[14]); }

        /**
         * @brief Get W Axis (Column)
         *
         * @return Vector<T>
         */
        Vector<T> GetWAxis() { return Vector<T>(m[3], m[7], m[11], m[15]); }

        /**
         * @brief Get 1 Row
         *
         * @return Vector<T>
         */
        Vector<T> GetRow1() { return Vector<T>(m[0], m[1], m[2], m[3]); }

        /**
         * @brief Get 2 Row
         *
         * @return Vector<T>
         */
        Vector<T> GetRow2() { return Vector<T>(m[4], m[5], m[6], m[7]); }

        /**
         * @brief Get 3 Row
         *
         * @return Vector<T>
         */
        Vector<T> GetRow3() { return Vector<T>(m[8], m[9], m[10], m[11]); }

        /**
         * @brief Get 4 Row
         *
         * @return Vector<T>
         */
        Vector<T> GetRow4() { return Vector<T>(m[12], m[13], m[14], m[15]); }

        /**
         * @brief Get Translation Axis
         *
         * @return Vector<T>
         */
        Vector<T> GetTranslation() { return GetWAxis(); }

        /**
         * @brief Get the Scale Axis
         *
         * @return Vector<T>
         */
        Vector<T> GetScale() { return Vector<T>(m[0], m[5], m[10], m[15]); }

        /**
         * @brief Get transposed matrix
         *
         * @return constexpr Matrix4<T>
         */
        Matrix4<T> Transpose() {
            Matrix4<T> mat;

            mat.m[0] = m[0]; mat.m[1] = m[4]; mat.m[2] = m[8]; mat.m[3] = m[12];
            mat.m[4] = m[1]; mat.m[5] = m[5]; mat.m[6] = m[9]; mat.m[7] = m[13];
            mat.m[8] = m[2]; mat.m[9] = m[6]; mat.m[10] = m[10]; mat.m[11] = m[14];
            mat.m[12] = m[3]; mat.m[13] = m[7]; mat.m[14] = m[11]; mat.m[15] = m[15];

            return mat;
        }

        Matrix4<T> operator+(Matrix4<T> mt) {
            Matrix4<T> mat;

            for (int i = 0; i < 16; i++) {
                mat.m[i] = m[i] + mt.m[i];
            }

            return mat;
        }

        Matrix4<T> operator-(Matrix4<T> mt) {
            Matrix4<T> mat;

            for (int i = 0; i < 16; i++) {
                mat.m[i] = m[i] - mt.m[i];
            }

            return mat;
        }

        Matrix4<T> operator+=(Matrix4<T> mt) { return ((*this) = operator+(mt)); }

        Matrix4<T> operator-=(Matrix4<T> mt) { return ((*this) = operator-(mt)); }

        Matrix4<T> operator*=(T v) {
            for (int i = 0; i < 16; i++) {
                m[i] *= v;
            }

            return *this;
        }

        Matrix4<T> operator/=(T v) {
            return operator*=(static_cast<T>(1) / v);
        }

        Matrix4<T> operator*(Matrix4<T> mt) {
            Matrix4<T> mat;

            mat.m[0] = GetXAxis().Dot(mt.GetRow1());
            mat.m[1] = GetYAxis().Dot(mt.GetRow1());
            mat.m[2] = GetZAxis().Dot(mt.GetRow1());
            mat.m[3] = GetWAxis().Dot(mt.GetRow1());

            mat.m[4] = GetXAxis().Dot(mt.GetRow2());
            mat.m[5] = GetYAxis().Dot(mt.GetRow2());
            mat.m[6] = GetZAxis().Dot(mt.GetRow2());
            mat.m[7] = GetWAxis().Dot(mt.GetRow2());

            mat.m[8] = GetXAxis().Dot(mt.GetRow3());
            mat.m[9] = GetYAxis().Dot(mt.GetRow3());
            mat.m[10] = GetZAxis().Dot(mt.GetRow3());
            mat.m[11] = GetWAxis().Dot(mt.GetRow3());

            mat.m[12] = GetXAxis().Dot(mt.GetRow4());
            mat.m[13] = GetYAxis().Dot(mt.GetRow4());
            mat.m[14] = GetZAxis().Dot(mt.GetRow4());
            mat.m[15] = GetWAxis().Dot(mt.GetRow4());

            return mat;
        }

        Matrix4<T> operator*=(Matrix4<T> mt) { return ((*this) = operator*(mt)); }
        Vector<T> operator*(Vector<T> v) { return Vector<T>(GetRow1().Dot(v), GetRow2().Dot(v), GetRow3().Dot(v), GetRow4().Dot(v)); }

        /**
         * @brief Convert matrix to point
         *
         * @param v
         * @return Vector<T>
         */
        Vector<T> ToPoint(Vector<T> v) {
            Vector<T> p(m[0] * v.x + m[1] * v.y + m[2] * v.z, m[4] * v.x + m[5] * v.y + m[6] * v.z, m[8] * v.x + m[9] * v.y + m[10] * v.z, 1.0f);

            T w = m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15];

            return p / w;
        }

        Vector<T> VectorizePoint(Vector<T> v) {
            return Vector<T>(GetRow1().Dot(v), GetRow2().Dot(v), GetRow3().Dot(v), GetRow4().Dot(v));
        }

        Vector<T> RightMultiply(Vector<T> v) {
            Vector<T> x = GetXAxis() * v.x;
            Vector<T> y = GetYAxis() * v.y;
            Vector<T> z = GetZAxis() * v.z;
            Vector<T> w = GetWAxis() * v.w;
            Vector<T> p(
                x.x + x.y + x.z + x.w,
                y.x + y.y + y.z + y.w,
                z.x + z.y + z.z + z.w,
                w.x + w.y + w.z + w.w);

            return p;
        }

        Vector<T> LeftMultiply(Vector<T> v) {
            Vector<T> x = GetRow1() * v.x;
            Vector<T> y = GetRow2() * v.y;
            Vector<T> z = GetRow3() * v.z;
            Vector<T> w = GetRow4() * v.w;
            Vector<T> p(
                x.x + x.y + x.z + x.w,
                y.x + y.y + y.z + y.w,
                z.x + z.y + z.z + z.w,
                w.x + w.y + w.z + w.w);

            return p;
        }

        /**
         * @brief Convert matrix to direction
         *
         * @param v
         * @return constexpr Vector<T>
         */
        Vector<T> Direction(Vector<T> const& v) {
            return Vector<T>(m[0] * v.x + m[1] * v.y + m[2] * v.z, m[4] * v.x + m[5] * v.y + m[6] * v.z, m[8] * v.x + m[9] * v.y + m[10] * v.z, 1.0f);
        }

        /**
         * @brief Inverse of matrix
         *
         * @return constexpr Matrix4<T>
         */
        Matrix4<T> Inverse() {
            Matrix4<T> inv;

            inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
            inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
            inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
            inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
            inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
            inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
            inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
            inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
            inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
            inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
            inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
            inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
            inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
            inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
            inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
            inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

            const T det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];

            inv /= det;

            return inv;
        }

        /**
         * @brief Perspective projection using field of view
         *
         * @param fov in degrees
         * @param width
         * @param height
         * @param near
         * @param far
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> PerspectiveFOV(T fov, T width, T height, T near, T far) {
            Matrix4<T> mat;

            const T f = static_cast<T>(1) / std::tan(fov / static_cast<T>(2));

            mat.m[0] = f * (height / width);
            mat.m[5] = f;
            mat.m[10] = -(far + near) / (far - near);
            mat.m[11] = -static_cast<T>(1);
            mat.m[14] = -(static_cast<T>(2) * far * near) / (far - near);
            mat.m[15] = 1.0f;

            return mat;
        }

        /**
         * @brief Perspective projection for unsymmetrical view
         *
         * @param right
         * @param left
         * @param top
         * @param bottom
         * @param near
         * @param far
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> Perspective(T right, T left, T top, T bottom, T near, T far) {
            Matrix4<T> mat;

            mat.m[0] = (static_cast<T>(2) * near) / (right - left);
            mat.m[2] = (right + left) / (right - left);
            mat.m[5] = (static_cast<T>(2) * near) / (top - bottom);
            mat.m[6] = (top + bottom) / (top - bottom);
            mat.m[10] = -(far + near) / (far - near);
            mat.m[11] = (static_cast<T>(-2) * far * near) / (far - near);
            mat.m[14] = static_cast<T>(-1);

            return mat;
        }

        /**
         * @brief Perspective projection for symmetrical view
         *
         * @param right
         * @param top
         * @param near
         * @param far
         * @return constexpr Matrix4<T>
         */
        static Matrix4<T> PerspectiveSymmetrical(T right, T top, T near, T far) {
            Matrix4<T> mat;

            mat.m[0] = near / right;
            mat.m[5] = near / top;
            mat.m[10] = -(far + near) / (far - near);
            mat.m[11] = (static_cast<T>(-2) * far * near) / (far - near);
            mat.m[14] = static_cast<T>(-1);

            return mat;
        }

        /**
         * @brief Orthographic projection for unsymmetrical view
         *
         * @param right
         * @param left
         * @param top
         * @param bottom
         * @param near
         * @param far
         * @return constexpr Matrix<T>
         */
        static Matrix4<T> Orthographic(T right, T left, T top, T bottom, T near, T far) {
            Matrix4<T> mat;

            mat.m[0] = static_cast<T>(2) / (right - left);
            mat.m[3] = -((right + left) / (right - left));
            mat.m[5] = static_cast<T>(2) / (top - bottom);
            mat.m[7] = -((top + bottom) / (top - bottom));
            mat.m[10] = static_cast<T>(-2) / (far - near);
            mat.m[11] = -((far + near) / (far - near));
            mat.m[15] = static_cast<T>(1);

            return mat;
        }

        /**
         * @brief Orthographic projection for symmetrical view
         *
         * @param right
         * @param top
         * @param near
         * @param far
         * @return constexpr Matrix<T>
         */
        static Matrix4<T> OrthographicSymmertical(T right, T top, T near, T far) {
            Matrix4<T> mat;

            mat.m[0] = static_cast<T>(1) / right;
            mat.m[5] = static_cast<T>(1) / top;
            mat.m[10] = static_cast<T>(-2) / (far - near);
            mat.m[11] = -((far + near) / (far - near));
            mat.m[15] = static_cast<T>(1);

            return mat;
        }

        /**
         * @brief Look at matrix
         *
         * @param eye camera position
         * @param at on what camera looking
         * @param up up direction
         * @return Matrix4<T>
         */
        static Matrix4<T> LookAt(Vector<T> eye, Vector<T> at, Vector<T> up) {
            Vector<T> z_axis = Vector<T>::Normalize(at - eye);
            Vector<T> x_axis = up.Cross(z_axis).Normalize();
            Vector<T> y_axis = z_axis.Cross(x_axis);

            Matrix4<T> mat = Matrix4<T>::Identity();

            mat.m[0] = x_axis.x;
            mat.m[1] = y_axis.x;
            mat.m[2] = z_axis.x;
            mat.m[4] = x_axis.y;
            mat.m[5] = y_axis.y;
            mat.m[6] = z_axis.y;
            mat.m[8] = x_axis.z;
            mat.m[9] = y_axis.z;
            mat.m[10] = z_axis.z;
            mat.m[12] = -x_axis.Dot(eye);
            mat.m[13] = -y_axis.Dot(eye);
            mat.m[14] = -z_axis.Dot(eye);
            mat.m[15] = 1.0f;

            return mat;
        }
    };

    /*template<typename T>
    class Quaterion {
    private:
        Vector<T> q;

    public:
        void FromPoint(Vector<T> const& v, RVec) {
            p.x = cos
        }
    };*/

    /**
     * @brief Point transform class
     *
     */
    class Transform {
    private:
        Vector<real> mPosition;
        Vector<real> mScale;
        Vector<real> mRotation;
        Matrix4<real> mTransformMatrix = Matrix4<float>::Identity();

        /**
         * @brief Recalculatin entire matrix
         *
         */
        void __RecalculateMatrix() {
            mTransformMatrix = Matrix4<real>::Scale(mScale) * Matrix4<real>::RotateX(mRotation.x) * Matrix4<real>::RotateY(mRotation.y) * Matrix4<real>::RotateZ(mRotation.z) * Matrix4<real>::Translate(mPosition);
        }

    public:
        Transform() {
            mScale = Vector<real>(1.0, 1.0, 1.0);

            __RecalculateMatrix();
        }

        Matrix4<real>* operator()() { return &mTransformMatrix; }

        /**
         * @brief Set the Position
         *
         * @param v
         */
        void SetPosition(Vector<real> const& v) {
            mPosition = v;

            __RecalculateMatrix();
        }

        /**
         * @brief Set the Scale
         *
         * @param v
         */
        void SetScale(Vector<real> const& v) {
            mScale = v;

            __RecalculateMatrix();
        }

        /**
         * @brief Set the Rotation
         *
         * @param v
         */
        void SetRotation(Vector<real> const& v) {
            mRotation = v;

            __RecalculateMatrix();
        }

        /**
         * @brief Get the Transform Matrix
         *
         * @return Matrix4<real>
         */
        Matrix4<real> GetTransform() {
            return mTransformMatrix;
        }

        /**
         * @brief Get the Position
         *
         * @return Vector<real>
         */
        Vector<real> GetPosition() {
            return mPosition;
        }

        /**
         * @brief Get the Scale
         *
         * @return Vector<real>
         */
        Vector<real> GetScale() {
            return mScale;
        }

        /**
         * @brief Get the Rotation
         *
         * @return Vector<real>
         */
        Vector<real> GetRotation() {
            return mRotation;
        }
    };

    template<class T>
    inline std::ostream& operator<<(std::ostream& out, const Vector<T>& vec) {
        out << "X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << " W: " << vec.w;

        return out;
    }

    template<class T>
    inline std::ostream& operator<<(std::ostream& out, const Matrix3<T>& m) {
        out << m.m[0] << ", " << m.m[1] << ", " << m.m[2] << "\n"
            << m.m[3] << ", " << m.m[4] << ", " << m.m[5] << "\n"
            << m.m[6] << ", " << m.m[7] << ", " << m.m[8];

        return out;
    }

    template<class T>
    inline std::ostream& operator<<(std::ostream& out, const Matrix4<T>& m) {
        out << m.m[0] << ", " << m.m[1] << ", " << m.m[2] << ", " << m.m[3] << "\n"
            << m.m[4] << ", " << m.m[5] << ", " << m.m[6] << ", " << m.m[7] << "\n"
            << m.m[8] << ", " << m.m[9] << ", " << m.m[10] << ", " << m.m[11] << "\n"
            << m.m[12] << ", " << m.m[13] << ", " << m.m[14] << ", " << m.m[15];

        return out;
    }

    typedef Matrix4<float> FMat;
    typedef Matrix4<double> DMat;
    typedef Matrix4<real> RMat;
}

#endif