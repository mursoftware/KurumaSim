#pragma once


#define PI 3.141592f


struct Matrix44;
struct Matrix33;
struct Vector4;
struct Quaternion;
struct Vector3D;




struct Vector2
{
	float X{};
	float Y{};

	Vector2(){}

	Vector2( float Xn, float Yn )
	{
		X = Xn;
		Y = Yn;
	}
};




struct Vector3
{
	float X{};
	float Y{};
	float Z{};

	Vector3(){}
	Vector3( float nx, float ny, float nz );

	operator Vector3D() const;

	Vector3 &operator=( const Vector3& Vector );
	Vector3 &operator=( const Vector3D& Vector );


	Vector3 operator+( const Vector3& Vector ) const;
    Vector3 operator+() const;
	Vector3 &operator+=( const Vector3& Vector );

	Vector3 operator-( const Vector3& Vector ) const;
	Vector3 operator-() const;
	Vector3 &operator-=( const Vector3& Vector );

	Vector3 operator*( float f ) const;
	Vector3 &operator*=( float f );

	Vector3 operator/( float f ) const;
	Vector3 &operator/=( float f );

	Vector3 operator*(const Vector3& Vector) const;
	Vector3 &operator*=(const Vector3& Vector );

	Vector3 operator/(const Vector3& Vector) const;
	Vector3 &operator/=(const Vector3& Vector);

	void Normalize();
	float Length() const;
	float LengthSq() const;

	static Vector3 Cross( const Vector3& Vector1, const Vector3& Vector2 );
	static float Dot( const Vector3& Vector1, const Vector3& Vector2 );


	static Vector4 Transform( const Matrix44& Matrix, const Vector3& Vector );
	static Vector3 Transform( const Matrix33& Matrix, const Vector3& Vector );
	static Vector3 TransformCoord( const Matrix44& Matrix, const Vector3& Vector );
	static Vector3 TransformNormal( const Matrix44& Matrix, const Vector3& Vector );
	static Vector3 RotationQuaternion(const Quaternion& q);

};




struct Vector3D
{
	double X{};
	double Y{};
	double Z{};

	Vector3D() {}
	Vector3D(double nx, double ny, double nz);

	operator Vector3() const;

	Vector3D &operator=(const Vector3D& Vector);
	Vector3D &operator=(const Vector3& Vector);

	Vector3D operator+(const Vector3D& Vector) const;
	Vector3D operator+() const;
	Vector3D &operator+=(const Vector3D& Vector);

	Vector3D operator-(const Vector3D& Vector) const;
	Vector3D operator-() const;
	Vector3D &operator-=(const Vector3D& Vector);

	Vector3D operator*(double f) const;
	Vector3D &operator*=(double f);

	Vector3D operator/(double f) const;
	Vector3D &operator/=(double f);

	Vector3D operator*(const Vector3D& Vector) const;
	Vector3D &operator*=(const Vector3D& Vector);

	Vector3D operator/(const Vector3D& Vector) const;
	Vector3D &operator/=(const Vector3D& Vector);

	void Normalize();
	double Length() const;
	double LengthSq() const;

	static Vector3D Cross(const Vector3D& Vector1, const Vector3D& Vector2);
	static double Dot(const Vector3D& Vector1, const Vector3D& Vector2);


	static Vector4 Transform(const Matrix44& Matrix, const Vector3D& Vector);
	static Vector3D Transform(const Matrix33& Matrix, const Vector3D& Vector);
	static Vector3D TransformCoord(const Matrix44& Matrix, const Vector3D& Vector);
	static Vector3D TransformNormal(const Matrix44& Matrix, const Vector3D& Vector);
	static Vector3D RotationQuaternion(const Quaternion& q);

};



struct Vector4
{
	float X{};
	float Y{};
	float Z{};
	float W{};

	Vector4(){}

	Vector4( float X, float Y, float Z, float W )
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->W = W;
	}
};




struct Quaternion
{
	float X{};
	float Y{};
	float Z{};
	float W{};

	Quaternion() {}

	Quaternion(float X, float Y, float Z, float W)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->W = W;
	}

	Quaternion &operator=(const Quaternion& q);

	Quaternion operator+(const Quaternion& q) const;
	Quaternion operator+() const;
	Quaternion &operator+=(const Quaternion& q);

	Quaternion operator-(const Quaternion& q) const;
	Quaternion operator-() const;
	Quaternion &operator-=(const Quaternion& q);

	Quaternion operator*(float f) const;
	Quaternion &operator*=(float f);

	Quaternion operator/(float f) const;
	Quaternion &operator/=(float f);

	float Length();
	void Normalize();
	void Inverse();

	static Quaternion Identity();
	static Quaternion Multiply(const Quaternion& q1, const Quaternion& q2);
	static Quaternion RotationAxis(const Vector3& q1);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
	static float Dot(const Quaternion& q1, const Quaternion& q2);

};



struct Matrix33
{
	float M[3][3]{};


	static Matrix33 Identity();



};


struct Matrix44
{
	float M[4][4]{};

	
	static Matrix44 Identity();
	
	static Matrix44 Viewport( float Width, float Height );

	static Matrix44 PerspectiveFov( float FOV, float Aspect, float NearZ, float FarZ );
	static Matrix44 Ortho( float X, float Y, float Width, float Height);
	static Matrix44 OrthoCenter( float Width, float Height, float Depth);

	static Matrix44 LookAt( const Vector3& Position, const Vector3& Target, const Vector3& UpVector );

	static Matrix44 Scale( float X, float Y, float Z );
	static Matrix44 RotationXYZ( float X, float Y, float Z );
	static Matrix44 TranslateXYZ( float X, float Y, float Z );
	static Matrix44 FromAxis(const Vector3& VX, const Vector3& VY, const Vector3& VZ );

	
	static Matrix44 Multiply( const Matrix44& Matrix1, const Matrix44& Matrix2 );
	static Matrix44 Transpose( const Matrix44& Matrix );
	static Matrix44 Inverse(const Matrix44& Matrix);

	static Matrix44 RotationQuaternion(const Quaternion& q);

	Matrix44 operator*(const Matrix44& Matrix) const;
	Matrix44& operator*=(const Matrix44& Matrix);



	Vector3 Right() { return Vector3(M[0][0], M[0][1], M[0][2]); }
	Vector3 Up()	{ return Vector3(M[1][0], M[1][1], M[1][2]); }
	Vector3 Front() { return Vector3(M[2][0], M[2][1], M[2][2]); }
	Vector3 Position() { return Vector3(M[3][0], M[3][1], M[3][2]); }

};
