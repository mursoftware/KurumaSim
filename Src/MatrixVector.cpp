

#include "Pch.h"

#include <math.h>
#include "MatrixVector.h"




Vector3::operator Vector3D() const
{
	Vector3D vector;

	vector.X = (double)X;
	vector.Y = (double)Y;
	vector.Z = (double)Z;

	return vector;
}

Vector3::Vector3( float nx, float ny, float nz )
{
	X = nx;
	Y = ny;
	Z = nz;
}

Vector3& Vector3::operator=( const Vector3 &Vector )
{
	X = Vector.X;
	Y = Vector.Y;
	Z = Vector.Z;

	return *this;
}
Vector3& Vector3::operator=(const Vector3D& Vector)
{
	X = (float)Vector.X;
	Y = (float)Vector.Y;
	Z = (float)Vector.Z;

	return *this;
}


Vector3 Vector3::operator+( const Vector3 &Vector ) const
{
	Vector3 vector;

	vector.X = X + Vector.X;
	vector.Y = Y + Vector.Y;
	vector.Z = Z + Vector.Z;

	return vector;
}

Vector3 Vector3::operator+() const
{
	Vector3 vector;

	vector.X = X;
	vector.Y = Y;
	vector.Z = Z;

	return vector;
}

Vector3& Vector3::operator+=( const Vector3 &Vector )
{
	X += Vector.X;
	Y += Vector.Y;
	Z += Vector.Z;

	return *this;
}

Vector3 Vector3::operator-( const Vector3 &Vector ) const
{
	Vector3 vector;

	vector.X = X - Vector.X;
	vector.Y = Y - Vector.Y;
	vector.Z = Z - Vector.Z;

	return vector;
}

Vector3 Vector3::operator-() const
{
	Vector3 vector;

	vector.X = -X;
	vector.Y = -Y;
	vector.Z = -Z;

	return vector;
}

Vector3& Vector3::operator-=( const Vector3 &Vector )
{
	X -= Vector.X;
	Y -= Vector.Y;
	Z -= Vector.Z;

	return *this;
}

Vector3 Vector3::operator*( float f ) const
{
	Vector3 vector;

	vector.X = X * f;
	vector.Y = Y * f;
	vector.Z = Z * f;

	return vector;
}

Vector3& Vector3::operator*=( float f )
{

	X *= f;
	Y *= f;
	Z *= f;

	return *this;
}

Vector3 Vector3::operator/( float f ) const
{
	Vector3 vector;

	vector.X = X / f;
	vector.Y = Y / f;
	vector.Z = Z / f;

	return vector;
}

Vector3& Vector3::operator/=( float f )
{
	X /= f;
	Y /= f;
	Z /= f;

	return *this;
}


Vector3 Vector3::operator*(const Vector3 &Vector) const
{
	Vector3 vector;

	vector.X = X * Vector.X;
	vector.Y = Y * Vector.Y;
	vector.Z = Z * Vector.Z;

	return vector;
}

Vector3& Vector3::operator*=(const Vector3 &Vector)
{
	X *= Vector.X;
	Y *= Vector.Y;
	Z *= Vector.Z;

	return *this;
}


Vector3 Vector3::operator/(const Vector3 &Vector) const
{
	Vector3 vector;

	vector.X = X / Vector.X;
	vector.Y = Y / Vector.Y;
	vector.Z = Z / Vector.Z;

	return vector;
}

Vector3& Vector3::operator/=(const Vector3 &Vector)
{
	X /= Vector.X;
	Y /= Vector.Y;
	Z /= Vector.Z;

	return *this;
}



float Vector3::Length()  const
{
	return sqrtf( X * X + Y * Y + Z * Z );
}
float Vector3::LengthSq()  const
{
	return ( X * X + Y * Y + Z * Z );
}

void Vector3::Normalize()
{
	*this = *this / Length();
}


Vector3 Vector3::Cross( const Vector3 &Vector1, const Vector3 &Vector2 )
{
	Vector3 vector;

	vector.X = Vector1.Y * Vector2.Z - Vector1.Z * Vector2.Y;
	vector.Y = Vector1.Z * Vector2.X - Vector1.X * Vector2.Z;
	vector.Z = Vector1.X * Vector2.Y - Vector1.Y * Vector2.X;

	return vector;
}

float Vector3::Dot( const Vector3 &Vector1, const Vector3 &Vector2 )
{
	return Vector1.X * Vector2.X + 
			Vector1.Y * Vector2.Y + 
			Vector1.Z * Vector2.Z;
}



Vector4 Vector3::Transform( const Matrix44& Matrix, const Vector3& Vector )
{
	Vector4 vec4;

	vec4.X =  Vector.X * Matrix.M[0][0]
			+ Vector.Y * Matrix.M[1][0]
			+ Vector.Z * Matrix.M[2][0]
			+			 Matrix.M[3][0];

	vec4.Y =  Vector.X * Matrix.M[0][1]
			+ Vector.Y * Matrix.M[1][1]
			+ Vector.Z * Matrix.M[2][1]
			+			 Matrix.M[3][1];

	vec4.Z =  Vector.X * Matrix.M[0][2]
			+ Vector.Y * Matrix.M[1][2]
			+ Vector.Z * Matrix.M[2][2]
			+			 Matrix.M[3][2];

	vec4.W =  Vector.X * Matrix.M[0][3]
			+ Vector.Y * Matrix.M[1][3]
			+ Vector.Z * Matrix.M[2][3]
			+			 Matrix.M[3][3];

	return Vector4( vec4.X, vec4.Y, vec4.Z, vec4.W);
}


Vector3 Vector3::Transform(const Matrix33& Matrix, const Vector3& Vector)
{
	Vector3 vec3;

	vec3.X = Vector.X * Matrix.M[0][0]
		+ Vector.Y * Matrix.M[1][0]
		+ Vector.Z * Matrix.M[2][0];

	vec3.Y = Vector.X * Matrix.M[0][1]
		+ Vector.Y * Matrix.M[1][1]
		+ Vector.Z * Matrix.M[2][1];

	vec3.Z = Vector.X * Matrix.M[0][2]
		+ Vector.Y * Matrix.M[1][2]
		+ Vector.Z * Matrix.M[2][2];


	return vec3;
}




Vector3 Vector3::TransformCoord( const Matrix44& Matrix, const Vector3& Vector )
{
	Vector4 vec4;

	vec4.X =  Vector.X * Matrix.M[0][0]
			+ Vector.Y * Matrix.M[1][0]
			+ Vector.Z * Matrix.M[2][0]
			+			 Matrix.M[3][0];

	vec4.Y =  Vector.X * Matrix.M[0][1]
			+ Vector.Y * Matrix.M[1][1]
			+ Vector.Z * Matrix.M[2][1]
			+			 Matrix.M[3][1];

	vec4.Z =  Vector.X * Matrix.M[0][2]
			+ Vector.Y * Matrix.M[1][2]
			+ Vector.Z * Matrix.M[2][2]
			+			 Matrix.M[3][2];

	vec4.W =  Vector.X * Matrix.M[0][3]
			+ Vector.Y * Matrix.M[1][3]
			+ Vector.Z * Matrix.M[2][3]
			+			 Matrix.M[3][3];

	return Vector3( vec4.X, vec4.Y, vec4.Z ) / vec4.W;
}



Vector3 Vector3::TransformNormal( const Matrix44& Matrix, const Vector3& Vector )
{
	Vector4 vec4;

	vec4.X =  Vector.X * Matrix.M[0][0]
			+ Vector.Y * Matrix.M[1][0]
			+ Vector.Z * Matrix.M[2][0];

	vec4.Y =  Vector.X * Matrix.M[0][1]
			+ Vector.Y * Matrix.M[1][1]
			+ Vector.Z * Matrix.M[2][1];

	vec4.Z =  Vector.X * Matrix.M[0][2]
			+ Vector.Y * Matrix.M[1][2]
			+ Vector.Z * Matrix.M[2][2];

	vec4.W =  Vector.X * Matrix.M[0][3]
			+ Vector.Y * Matrix.M[1][3]
			+ Vector.Z * Matrix.M[2][3]
			+			 Matrix.M[3][3];

	return Vector3( vec4.X, vec4.Y, vec4.Z ) / vec4.W;
}


Vector3 Vector3::RotationQuaternion(const Quaternion& q)
{

	float len = sqrtf(q.X*q.X + q.Y*q.Y + q.Z*q.Z);
	float angle = 2.0f * asinf(len);

	Vector3 vec3;
	if (len * angle == 0.0f)
	{
		vec3.X = 0.0f;
		vec3.Y = 0.0f;
		vec3.Z = 0.0f;
	}
	else
	{
		vec3.X = q.X / len * angle;
		vec3.Y = q.Y / len * angle;
		vec3.Z = q.Z / len * angle;
	}

	return vec3;
}









Vector3D::operator Vector3() const
{
	Vector3 vector;

	vector.X = (float)X;
	vector.Y = (float)Y;
	vector.Z = (float)Z;

	return vector;
}



Vector3D::Vector3D(double nx, double ny, double nz)
{
	X = nx;
	Y = ny;
	Z = nz;
}

Vector3D& Vector3D::operator=(const Vector3D &Vector)
{
	X = Vector.X;
	Y = Vector.Y;
	Z = Vector.Z;

	return *this;
}

Vector3D& Vector3D::operator=(const Vector3& Vector)
{
	X = (double)Vector.X;
	Y = (double)Vector.Y;
	Z = (double)Vector.Z;

	return *this;
}


Vector3D Vector3D::operator+(const Vector3D &Vector) const
{
	Vector3D vector;

	vector.X = X + Vector.X;
	vector.Y = Y + Vector.Y;
	vector.Z = Z + Vector.Z;

	return vector;
}

Vector3D Vector3D::operator+() const
{
	Vector3D vector;

	vector.X = X;
	vector.Y = Y;
	vector.Z = Z;

	return vector;
}

Vector3D& Vector3D::operator+=(const Vector3D &Vector)
{
	X += Vector.X;
	Y += Vector.Y;
	Z += Vector.Z;

	return *this;
}

Vector3D Vector3D::operator-(const Vector3D &Vector) const
{
	Vector3D vector;

	vector.X = X - Vector.X;
	vector.Y = Y - Vector.Y;
	vector.Z = Z - Vector.Z;

	return vector;
}

Vector3D Vector3D::operator-() const
{
	Vector3D vector;

	vector.X = -X;
	vector.Y = -Y;
	vector.Z = -Z;

	return vector;
}

Vector3D& Vector3D::operator-=(const Vector3D &Vector)
{
	X -= Vector.X;
	Y -= Vector.Y;
	Z -= Vector.Z;

	return *this;
}

Vector3D Vector3D::operator*(double f) const
{
	Vector3D vector;

	vector.X = X * f;
	vector.Y = Y * f;
	vector.Z = Z * f;

	return vector;
}

Vector3D& Vector3D::operator*=(double f)
{

	X *= f;
	Y *= f;
	Z *= f;

	return *this;
}


Vector3D Vector3D::operator/(double f) const
{
	Vector3D vector;

	vector.X = X / f;
	vector.Y = Y / f;
	vector.Z = Z / f;

	return vector;
}

Vector3D& Vector3D::operator/=(double f)
{
	X /= f;
	Y /= f;
	Z /= f;

	return *this;
}


Vector3D Vector3D::operator*(const Vector3D &Vector) const
{
	Vector3D vector;

	vector.X = X * Vector.X;
	vector.Y = Y * Vector.Y;
	vector.Z = Z * Vector.Z;

	return vector;
}

Vector3D& Vector3D::operator*=(const Vector3D &Vector)
{
	X *= Vector.X;
	Y *= Vector.Y;
	Z *= Vector.Z;

	return *this;
}


Vector3D Vector3D::operator/(const Vector3D &Vector) const
{
	Vector3D vector;

	vector.X = X / Vector.X;
	vector.Y = Y / Vector.Y;
	vector.Z = Z / Vector.Z;

	return vector;
}

Vector3D& Vector3D::operator/=(const Vector3D &Vector)
{
	X /= Vector.X;
	Y /= Vector.Y;
	Z /= Vector.Z;

	return *this;
}



double Vector3D::Length()  const
{
	return sqrt(X * X + Y * Y + Z * Z);
}
double Vector3D::LengthSq()  const
{
	return (X * X + Y * Y + Z * Z);
}

void Vector3D::Normalize()
{
	*this = *this / Length();
}


Vector3D Vector3D::Cross(const Vector3D &Vector1, const Vector3D &Vector2)
{
	Vector3D vector;

	vector.X = Vector1.Y * Vector2.Z - Vector1.Z * Vector2.Y;
	vector.Y = Vector1.Z * Vector2.X - Vector1.X * Vector2.Z;
	vector.Z = Vector1.X * Vector2.Y - Vector1.Y * Vector2.X;

	return vector;
}

double Vector3D::Dot(const Vector3D &Vector1, const Vector3D &Vector2)
{
	return Vector1.X * Vector2.X +
		Vector1.Y * Vector2.Y +
		Vector1.Z * Vector2.Z;
}



Vector4 Vector3D::Transform(const Matrix44& Matrix, const Vector3D& Vector)
{
	Vector4 vec4;

	vec4.X = (float)(Vector.X * Matrix.M[0][0]
					+ Vector.Y * Matrix.M[1][0]
					+ Vector.Z * Matrix.M[2][0]
					+ Matrix.M[3][0]);

	vec4.Y = (float)(Vector.X * Matrix.M[0][1]
					+ Vector.Y * Matrix.M[1][1]
					+ Vector.Z * Matrix.M[2][1]
					+ Matrix.M[3][1]);

	vec4.Z = (float)(Vector.X * Matrix.M[0][2]
					+ Vector.Y * Matrix.M[1][2]
					+ Vector.Z * Matrix.M[2][2]
					+ Matrix.M[3][2]);

	vec4.W = (float)(Vector.X * Matrix.M[0][3]
					+ Vector.Y * Matrix.M[1][3]
					+ Vector.Z * Matrix.M[2][3]
					+ Matrix.M[3][3]);

	return Vector4(vec4.X, vec4.Y, vec4.Z, vec4.W);
}


Vector3D Vector3D::Transform(const Matrix33& Matrix, const Vector3D& Vector)
{
	Vector3D vec3;

	vec3.X = (float)(Vector.X * Matrix.M[0][0]
					+ Vector.Y * Matrix.M[1][0]
					+ Vector.Z * Matrix.M[2][0]);

	vec3.Y = (float)(Vector.X * Matrix.M[0][1]
					+ Vector.Y * Matrix.M[1][1]
					+ Vector.Z * Matrix.M[2][1]);

	vec3.Z = (float)(Vector.X * Matrix.M[0][2]
					+ Vector.Y * Matrix.M[1][2]
					+ Vector.Z * Matrix.M[2][2]);


	return vec3;
}




Vector3D Vector3D::TransformCoord(const Matrix44& Matrix, const Vector3D& Vector)
{
	Vector4 vec4;

	vec4.X = (float)(Vector.X * Matrix.M[0][0]
					+ Vector.Y * Matrix.M[1][0]
					+ Vector.Z * Matrix.M[2][0]
					+ Matrix.M[3][0]);

	vec4.Y = (float)(Vector.X * Matrix.M[0][1]
					+ Vector.Y * Matrix.M[1][1]
					+ Vector.Z * Matrix.M[2][1]
					+ Matrix.M[3][1]);

	vec4.Z = (float)(Vector.X * Matrix.M[0][2]
					+ Vector.Y * Matrix.M[1][2]
					+ Vector.Z * Matrix.M[2][2]
					+ Matrix.M[3][2]);

	vec4.W = (float)(Vector.X * Matrix.M[0][3]
					+ Vector.Y * Matrix.M[1][3]
					+ Vector.Z * Matrix.M[2][3]
					+ Matrix.M[3][3]);

	return Vector3D(vec4.X, vec4.Y, vec4.Z) / vec4.W;
}



Vector3D Vector3D::TransformNormal(const Matrix44& Matrix, const Vector3D& Vector)
{
	Vector4 vec4;

	vec4.X = (float)(Vector.X * Matrix.M[0][0]
					+ Vector.Y * Matrix.M[1][0]
					+ Vector.Z * Matrix.M[2][0]);

	vec4.Y = (float)(Vector.X * Matrix.M[0][1]
					+ Vector.Y * Matrix.M[1][1]
					+ Vector.Z * Matrix.M[2][1]);

	vec4.Z = (float)(Vector.X * Matrix.M[0][2]
					+ Vector.Y * Matrix.M[1][2]
					+ Vector.Z * Matrix.M[2][2]);

	vec4.W = (float)(Vector.X * Matrix.M[0][3]
					+ Vector.Y * Matrix.M[1][3]
					+ Vector.Z * Matrix.M[2][3]
					+ Matrix.M[3][3]);

	return Vector3D(vec4.X, vec4.Y, vec4.Z) / vec4.W;
}


Vector3D Vector3D::RotationQuaternion(const Quaternion& q)
{

	double len = sqrt(q.X*q.X + q.Y*q.Y + q.Z*q.Z);
	double angle = 2.0 * asin(len);

	Vector3D vec3;
	if (len * angle == 0.0f)
	{
		vec3.X = 0.0;
		vec3.Y = 0.0;
		vec3.Z = 0.0;
	}
	else
	{
		vec3.X = q.X / len * angle;
		vec3.Y = q.Y / len * angle;
		vec3.Z = q.Z / len * angle;
	}

	return vec3;
}
















Quaternion& Quaternion::operator=(const Quaternion &q)
{
	X = q.X;
	Y = q.Y;
	Z = q.Z;
	W = q.W;

	return *this;
}

Quaternion Quaternion::operator+(const Quaternion &q) const
{
	Quaternion oq;

	oq.X = X + q.X;
	oq.Y = Y + q.Y;
	oq.Z = Z + q.Z;
	oq.W = W + q.W;

	return oq;
}

Quaternion Quaternion::operator+() const
{
	Quaternion q;

	q.X = X;
	q.Y = Y;
	q.Z = Z;
	q.W = W;

	return q;
}

Quaternion& Quaternion::operator+=(const Quaternion &q)
{
	X += q.X;
	Y += q.Y;
	Z += q.Z;
	W += q.W;

	return *this;
}

Quaternion Quaternion::operator-(const Quaternion &q) const
{
	Quaternion oq;

	oq.X = X - q.X;
	oq.Y = Y - q.Y;
	oq.Z = Z - q.Z;
	oq.W = W - q.W;

	return oq;
}

Quaternion Quaternion::operator-() const
{
	Quaternion oq;

	oq.X = -X;
	oq.Y = -Y;
	oq.Z = -Z;
	oq.W = -W;

	return oq;
}

Quaternion& Quaternion::operator-=(const Quaternion &q)
{
	X -= q.X;
	Y -= q.Y;
	Z -= q.Z;
	W -= q.W;

	return *this;
}

Quaternion Quaternion::operator*(float f) const
{
	Quaternion oq;

	oq.X = X * f;
	oq.Y = Y * f;
	oq.Z = Z * f;
	oq.W = W * f;

	return oq;
}

Quaternion& Quaternion::operator*=(float f)
{

	X *= f;
	Y *= f;
	Z *= f;
	W *= f;

	return *this;
}

Quaternion Quaternion::operator/(float f) const
{
	Quaternion oq;

	oq.X = X / f;
	oq.Y = Y / f;
	oq.Z = Z / f;
	oq.W = W / f;

	return oq;
}

Quaternion& Quaternion::operator/=(float f)
{
	X /= f;
	Y /= f;
	Z /= f;
	W /= f;

	return *this;
}



Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion oq;

	oq.X = W * q.X + X * q.W + Y * q.Z - Z * q.Y;
	oq.Y = W * q.Y + Y * q.W + Z * q.X - X * q.Z;
	oq.Z = W * q.Z + Z * q.W + X * q.Y - Y * q.X;
	oq.W = W * q.W - X * q.X - Y * q.Y - Z * q.Z;

	return oq;
}



Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	Quaternion oq;

	oq.X = W * q.X + X * q.W + Y * q.Z - Z * q.Y;
	oq.Y = W * q.Y + Y * q.W + Z * q.X - X * q.Z;
	oq.Z = W * q.Z + Z * q.W + X * q.Y - Y * q.X;
	oq.W = W * q.W - X * q.X - Y * q.Y - Z * q.Z;

	*this = oq;

	return *this;
}



float Quaternion::Length()
{
	return sqrtf(X * X + Y * Y + Z * Z + W * W);
}

void Quaternion::Normalize()
{
	*this = *this / Length();
}


void Quaternion::Inverse()
{
	float len = Length();

	X /= -len;
	Y /= -len;
	Z /= -len;
	W /= len;
}




Quaternion Quaternion::Identity()
{
	Quaternion oq;

	oq.X = 0.0f;
	oq.Y = 0.0f;
	oq.Z = 0.0f;
	oq.W = 1.0f;

	return oq;
}


Quaternion Quaternion::Multiply(const Quaternion& q2, const Quaternion& q1)
{
	Quaternion oq;

	oq.X = q1.W * q2.X + q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y;
	oq.Y = q1.W * q2.Y + q1.Y * q2.W + q1.Z * q2.X - q1.X * q2.Z;
	oq.Z = q1.W * q2.Z + q1.Z * q2.W + q1.X * q2.Y - q1.Y * q2.X;
	oq.W = q1.W * q2.W - q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z;

	return oq;
}

Quaternion Quaternion::RotationAxis(const Vector3& v)
{
	Quaternion oq;
	float len = v.Length();

	if (len == 0.0f)
	{
		oq = Quaternion::Identity();
		return oq;
	}

	float s = sinf(len * 0.5f);
	Vector3 vn = v / len;
	oq.X = vn.X * s;
	oq.Y = vn.Y * s;
	oq.Z = vn.Z * s;
	oq.W = cosf(len * 0.5f);

	return oq;
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float dot = Dot(q1, q2);

	if (dot > 0.9999999f)
	{
		return q1;
	}


	float theta = acosf(dot);

	if (theta < 0.0)
	{
		theta = -theta;
	}


	float st = sinf(theta);

	float sut = sinf(theta * t);
	float sout = sinf(theta * (1.0f - t));

	float coeff1 = sout / st;
	float coeff2 = sut / st;


	Quaternion oq;
	oq.X = coeff1 * q1.X + coeff2 * q2.X;
	oq.Y = coeff1 * q1.Y + coeff2 * q2.Y;
	oq.Z = coeff1 * q1.Z + coeff2 * q2.Z;
	oq.W = coeff1 * q1.W + coeff2 * q2.W;

	return oq;
}

float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W;
}


Matrix33 Matrix33::Identity()
{
	Matrix33 matrix;

	matrix.M[0][0] = 1.0f;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = 1.0f;
	matrix.M[1][2] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f;

	return matrix;
}



Matrix44 Matrix44::Identity()
{
	Matrix44 matrix;

	matrix.M[0][0] = 1.0f;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = 1.0f;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = 0.0f;
	matrix.M[3][1] = 0.0f;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;
}


Matrix44 Matrix44::FromAxis(const Vector3& VX, const Vector3& VY, const Vector3& VZ)
{
	Matrix44 matrix;

	matrix.M[0][0] = VX.X;
	matrix.M[0][1] = VX.Y;
	matrix.M[0][2] = VX.Z;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = VY.X;
	matrix.M[1][1] = VY.Y;
	matrix.M[1][2] = VY.Z;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = VZ.X;
	matrix.M[2][1] = VZ.Y;
	matrix.M[2][2] = VZ.Z;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = 0.0f;
	matrix.M[3][1] = 0.0f;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;

}


Matrix44 Matrix44::RotationQuaternion(const Quaternion& q)
{


	float n = q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W;
	float s = (n > 0.0f) ? (2.0f / n) : 0.0f;

	float xs = q.X * s, ys = q.Y * s, zs = q.Z * s;
	float wx = q.W * xs, wy = q.W * ys, wz = q.W * zs;
	float xx = q.X * xs, xy = q.X * ys, xz = q.X * zs;
	float yy = q.Y * ys, yz = q.Y * zs, zz = q.Z * zs;

	Matrix44 matrix;
	matrix = Matrix44::Identity();
	matrix.M[0][0] = 1.0f - (yy + zz);	matrix.M[1][0] = xy - wz;         matrix.M[2][0] = xz + wy;
	matrix.M[0][1] = xy + wz;			matrix.M[1][1] = 1.0f - (xx + zz); matrix.M[2][1] = yz - wx;
	matrix.M[0][2] = xz - wy;			matrix.M[1][2] = yz + wx;         matrix.M[2][2] = 1.0f - (xx + yy);

	return matrix;

}



Matrix44 Matrix44::operator*(const Matrix44& Matrix) const
{
	Matrix44 matrix;

	matrix.M[0][0] = M[0][0] * Matrix.M[0][0]
		+ M[0][1] * Matrix.M[1][0]
		+ M[0][2] * Matrix.M[2][0]
		+ M[0][3] * Matrix.M[3][0];

	matrix.M[0][1] = M[0][0] * Matrix.M[0][1]
		+ M[0][1] * Matrix.M[1][1]
		+ M[0][2] * Matrix.M[2][1]
		+ M[0][3] * Matrix.M[3][1];

	matrix.M[0][2] = M[0][0] * Matrix.M[0][2]
		+ M[0][1] * Matrix.M[1][2]
		+ M[0][2] * Matrix.M[2][2]
		+ M[0][3] * Matrix.M[3][2];

	matrix.M[0][3] = M[0][0] * Matrix.M[0][3]
		+ M[0][1] * Matrix.M[1][3]
		+ M[0][2] * Matrix.M[2][3]
		+ M[0][3] * Matrix.M[3][3];


	matrix.M[1][0] = M[1][0] * Matrix.M[0][0]
		+ M[1][1] * Matrix.M[1][0]
		+ M[1][2] * Matrix.M[2][0]
		+ M[1][3] * Matrix.M[3][0];

	matrix.M[1][1] = M[1][0] * Matrix.M[0][1]
		+ M[1][1] * Matrix.M[1][1]
		+ M[1][2] * Matrix.M[2][1]
		+ M[1][3] * Matrix.M[3][1];

	matrix.M[1][2] = M[1][0] * Matrix.M[0][2]
		+ M[1][1] * Matrix.M[1][2]
		+ M[1][2] * Matrix.M[2][2]
		+ M[1][3] * Matrix.M[3][2];

	matrix.M[1][3] = M[1][0] * Matrix.M[0][3]
		+ M[1][1] * Matrix.M[1][3]
		+ M[1][2] * Matrix.M[2][3]
		+ M[1][3] * Matrix.M[3][3];


	matrix.M[2][0] = M[2][0] * Matrix.M[0][0]
		+ M[2][1] * Matrix.M[1][0]
		+ M[2][2] * Matrix.M[2][0]
		+ M[2][3] * Matrix.M[3][0];

	matrix.M[2][1] = M[2][0] * Matrix.M[0][1]
		+ M[2][1] * Matrix.M[1][1]
		+ M[2][2] * Matrix.M[2][1]
		+ M[2][3] * Matrix.M[3][1];

	matrix.M[2][2] = M[2][0] * Matrix.M[0][2]
		+ M[2][1] * Matrix.M[1][2]
		+ M[2][2] * Matrix.M[2][2]
		+ M[2][3] * Matrix.M[3][2];

	matrix.M[2][3] = M[2][0] * Matrix.M[0][3]
		+ M[2][1] * Matrix.M[1][3]
		+ M[2][2] * Matrix.M[2][3]
		+ M[2][3] * Matrix.M[3][3];


	matrix.M[3][0] = M[3][0] * Matrix.M[0][0]
		+ M[3][1] * Matrix.M[1][0]
		+ M[3][2] * Matrix.M[2][0]
		+ M[3][3] * Matrix.M[3][0];

	matrix.M[3][1] = M[3][0] * Matrix.M[0][1]
		+ M[3][1] * Matrix.M[1][1]
		+ M[3][2] * Matrix.M[2][1]
		+ M[3][3] * Matrix.M[3][1];

	matrix.M[3][2] = M[3][0] * Matrix.M[0][2]
		+ M[3][1] * Matrix.M[1][2]
		+ M[3][2] * Matrix.M[2][2]
		+ M[3][3] * Matrix.M[3][2];

	matrix.M[3][3] = M[3][0] * Matrix.M[0][3]
		+ M[3][1] * Matrix.M[1][3]
		+ M[3][2] * Matrix.M[2][3]
		+ M[3][3] * Matrix.M[3][3];

	return matrix;
}

Matrix44& Matrix44::operator*=(const Matrix44& Matrix)
{
	Matrix44 matrix;

	matrix.M[0][0] = M[0][0] * Matrix.M[0][0]
		+ M[0][1] * Matrix.M[1][0]
		+ M[0][2] * Matrix.M[2][0]
		+ M[0][3] * Matrix.M[3][0];

	matrix.M[0][1] = M[0][0] * Matrix.M[0][1]
		+ M[0][1] * Matrix.M[1][1]
		+ M[0][2] * Matrix.M[2][1]
		+ M[0][3] * Matrix.M[3][1];

	matrix.M[0][2] = M[0][0] * Matrix.M[0][2]
		+ M[0][1] * Matrix.M[1][2]
		+ M[0][2] * Matrix.M[2][2]
		+ M[0][3] * Matrix.M[3][2];

	matrix.M[0][3] = M[0][0] * Matrix.M[0][3]
		+ M[0][1] * Matrix.M[1][3]
		+ M[0][2] * Matrix.M[2][3]
		+ M[0][3] * Matrix.M[3][3];


	matrix.M[1][0] = M[1][0] * Matrix.M[0][0]
		+ M[1][1] * Matrix.M[1][0]
		+ M[1][2] * Matrix.M[2][0]
		+ M[1][3] * Matrix.M[3][0];

	matrix.M[1][1] = M[1][0] * Matrix.M[0][1]
		+ M[1][1] * Matrix.M[1][1]
		+ M[1][2] * Matrix.M[2][1]
		+ M[1][3] * Matrix.M[3][1];

	matrix.M[1][2] = M[1][0] * Matrix.M[0][2]
		+ M[1][1] * Matrix.M[1][2]
		+ M[1][2] * Matrix.M[2][2]
		+ M[1][3] * Matrix.M[3][2];

	matrix.M[1][3] = M[1][0] * Matrix.M[0][3]
		+ M[1][1] * Matrix.M[1][3]
		+ M[1][2] * Matrix.M[2][3]
		+ M[1][3] * Matrix.M[3][3];


	matrix.M[2][0] = M[2][0] * Matrix.M[0][0]
		+ M[2][1] * Matrix.M[1][0]
		+ M[2][2] * Matrix.M[2][0]
		+ M[2][3] * Matrix.M[3][0];

	matrix.M[2][1] = M[2][0] * Matrix.M[0][1]
		+ M[2][1] * Matrix.M[1][1]
		+ M[2][2] * Matrix.M[2][1]
		+ M[2][3] * Matrix.M[3][1];

	matrix.M[2][2] = M[2][0] * Matrix.M[0][2]
		+ M[2][1] * Matrix.M[1][2]
		+ M[2][2] * Matrix.M[2][2]
		+ M[2][3] * Matrix.M[3][2];

	matrix.M[2][3] = M[2][0] * Matrix.M[0][3]
		+ M[2][1] * Matrix.M[1][3]
		+ M[2][2] * Matrix.M[2][3]
		+ M[2][3] * Matrix.M[3][3];


	matrix.M[3][0] = M[3][0] * Matrix.M[0][0]
		+ M[3][1] * Matrix.M[1][0]
		+ M[3][2] * Matrix.M[2][0]
		+ M[3][3] * Matrix.M[3][0];

	matrix.M[3][1] = M[3][0] * Matrix.M[0][1]
		+ M[3][1] * Matrix.M[1][1]
		+ M[3][2] * Matrix.M[2][1]
		+ M[3][3] * Matrix.M[3][1];

	matrix.M[3][2] = M[3][0] * Matrix.M[0][2]
		+ M[3][1] * Matrix.M[1][2]
		+ M[3][2] * Matrix.M[2][2]
		+ M[3][3] * Matrix.M[3][2];

	matrix.M[3][3] = M[3][0] * Matrix.M[0][3]
		+ M[3][1] * Matrix.M[1][3]
		+ M[3][2] * Matrix.M[2][3]
		+ M[3][3] * Matrix.M[3][3];

	*this = matrix;

	return *this;
}


Matrix44 Matrix44::Multiply( const Matrix44& Matrix1, const Matrix44& Matrix2 )
{
	Matrix44 matrix;

	matrix.M[0][0] =  Matrix1.M[0][0] * Matrix2.M[0][0]
					+ Matrix1.M[0][1] * Matrix2.M[1][0]
					+ Matrix1.M[0][2] * Matrix2.M[2][0]
					+ Matrix1.M[0][3] * Matrix2.M[3][0];

	matrix.M[0][1] =  Matrix1.M[0][0] * Matrix2.M[0][1]
					+ Matrix1.M[0][1] * Matrix2.M[1][1]
					+ Matrix1.M[0][2] * Matrix2.M[2][1]
					+ Matrix1.M[0][3] * Matrix2.M[3][1];

	matrix.M[0][2] =  Matrix1.M[0][0] * Matrix2.M[0][2]
					+ Matrix1.M[0][1] * Matrix2.M[1][2]
					+ Matrix1.M[0][2] * Matrix2.M[2][2]
					+ Matrix1.M[0][3] * Matrix2.M[3][2];

	matrix.M[0][3] =  Matrix1.M[0][0] * Matrix2.M[0][3]
					+ Matrix1.M[0][1] * Matrix2.M[1][3]
					+ Matrix1.M[0][2] * Matrix2.M[2][3]
					+ Matrix1.M[0][3] * Matrix2.M[3][3];


	matrix.M[1][0] =  Matrix1.M[1][0] * Matrix2.M[0][0]
					+ Matrix1.M[1][1] * Matrix2.M[1][0]
					+ Matrix1.M[1][2] * Matrix2.M[2][0]
					+ Matrix1.M[1][3] * Matrix2.M[3][0];

	matrix.M[1][1] =  Matrix1.M[1][0] * Matrix2.M[0][1]
					+ Matrix1.M[1][1] * Matrix2.M[1][1]
					+ Matrix1.M[1][2] * Matrix2.M[2][1]
					+ Matrix1.M[1][3] * Matrix2.M[3][1];

	matrix.M[1][2] =  Matrix1.M[1][0] * Matrix2.M[0][2]
					+ Matrix1.M[1][1] * Matrix2.M[1][2]
					+ Matrix1.M[1][2] * Matrix2.M[2][2]
					+ Matrix1.M[1][3] * Matrix2.M[3][2];

	matrix.M[1][3] =  Matrix1.M[1][0] * Matrix2.M[0][3]
					+ Matrix1.M[1][1] * Matrix2.M[1][3]
					+ Matrix1.M[1][2] * Matrix2.M[2][3]
					+ Matrix1.M[1][3] * Matrix2.M[3][3];


	matrix.M[2][0] =  Matrix1.M[2][0] * Matrix2.M[0][0]
					+ Matrix1.M[2][1] * Matrix2.M[1][0]
					+ Matrix1.M[2][2] * Matrix2.M[2][0]
					+ Matrix1.M[2][3] * Matrix2.M[3][0];

	matrix.M[2][1] =  Matrix1.M[2][0] * Matrix2.M[0][1]
					+ Matrix1.M[2][1] * Matrix2.M[1][1]
					+ Matrix1.M[2][2] * Matrix2.M[2][1]
					+ Matrix1.M[2][3] * Matrix2.M[3][1];

	matrix.M[2][2] =  Matrix1.M[2][0] * Matrix2.M[0][2]
					+ Matrix1.M[2][1] * Matrix2.M[1][2]
					+ Matrix1.M[2][2] * Matrix2.M[2][2]
					+ Matrix1.M[2][3] * Matrix2.M[3][2];

	matrix.M[2][3] =  Matrix1.M[2][0] * Matrix2.M[0][3]
					+ Matrix1.M[2][1] * Matrix2.M[1][3]
					+ Matrix1.M[2][2] * Matrix2.M[2][3]
					+ Matrix1.M[2][3] * Matrix2.M[3][3];


	matrix.M[3][0] =  Matrix1.M[3][0] * Matrix2.M[0][0]
					+ Matrix1.M[3][1] * Matrix2.M[1][0]
					+ Matrix1.M[3][2] * Matrix2.M[2][0]
					+ Matrix1.M[3][3] * Matrix2.M[3][0];

	matrix.M[3][1] =  Matrix1.M[3][0] * Matrix2.M[0][1]
					+ Matrix1.M[3][1] * Matrix2.M[1][1]
					+ Matrix1.M[3][2] * Matrix2.M[2][1]
					+ Matrix1.M[3][3] * Matrix2.M[3][1];

	matrix.M[3][2] =  Matrix1.M[3][0] * Matrix2.M[0][2]
					+ Matrix1.M[3][1] * Matrix2.M[1][2]
					+ Matrix1.M[3][2] * Matrix2.M[2][2]
					+ Matrix1.M[3][3] * Matrix2.M[3][2];

	matrix.M[3][3] =  Matrix1.M[3][0] * Matrix2.M[0][3]
					+ Matrix1.M[3][1] * Matrix2.M[1][3]
					+ Matrix1.M[3][2] * Matrix2.M[2][3]
					+ Matrix1.M[3][3] * Matrix2.M[3][3];

	return matrix;
}


Matrix44 Matrix44::Scale( float X, float Y, float Z )
{
	Matrix44 matrix;

	matrix.M[0][0] = X;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = Y;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = Z;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = 0.0f;
	matrix.M[3][1] = 0.0f;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;
}


Matrix44 Matrix44::TranslateXYZ( float X, float Y, float Z )
{
	Matrix44 matrix;

	matrix.M[0][0] = 1.0f;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = 1.0f;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = X;
	matrix.M[3][1] = Y;
	matrix.M[3][2] = Z;
	matrix.M[3][3] = 1.0f;

	return matrix;
}


Matrix44 Matrix44::RotationXYZ( float X, float Y, float Z )
{
	Matrix44 matrixX, matrixY, matrixZ, matrix;
	float sinx, cosx, siny, cosy, sinz, cosz;


	sinx = sinf( X );
	cosx = cosf( X );

	siny = sinf( Y );
	cosy = cosf( Y );

	sinz = sinf( Z );
	cosz = cosf( Z );


	matrixX.M[0][0] = 1.0f;
	matrixX.M[0][1] = 0.0f;
	matrixX.M[0][2] = 0.0f;
	matrixX.M[0][3] = 0.0f;

	matrixX.M[1][0] = 0.0f;
	matrixX.M[1][1] = cosx;
	matrixX.M[1][2] = -sinx;
	matrixX.M[1][3] = 0.0f;

	matrixX.M[2][0] = 0.0f;
	matrixX.M[2][1] = sinx;
	matrixX.M[2][2] = cosx;
	matrixX.M[2][3] = 0.0f;

	matrixX.M[3][0] = 0.0f;
	matrixX.M[3][1] = 0.0f;
	matrixX.M[3][2] = 0.0f;
	matrixX.M[3][3] = 1.0f;



	matrixY.M[0][0] = cosy;
	matrixY.M[0][1] = 0.0f;
	matrixY.M[0][2] = siny;
	matrixY.M[0][3] = 0.0f;

	matrixY.M[1][0] = 0.0f;
	matrixY.M[1][1] = 1.0f;
	matrixY.M[1][2] = 0.0f;
	matrixY.M[1][3] = 0.0f;

	matrixY.M[2][0] = -siny;
	matrixY.M[2][1] = 0.0f;
	matrixY.M[2][2] = cosy;
	matrixY.M[2][3] = 0.0f;

	matrixY.M[3][0] = 0.0f;
	matrixY.M[3][1] = 0.0f;
	matrixY.M[3][2] = 0.0f;
	matrixY.M[3][3] = 1.0f;


	matrix = Matrix44::Multiply( matrixX, matrixY );


	matrixZ.M[0][0] = cosz;
	matrixZ.M[0][1] = -sinz;
	matrixZ.M[0][2] = 0.0f;
	matrixZ.M[0][3] = 0.0f;

	matrixZ.M[1][0] = sinz;
	matrixZ.M[1][1] = cosz;
	matrixZ.M[1][2] = 0.0f;
	matrixZ.M[1][3] = 0.0f;

	matrixZ.M[2][0] = 0.0f;
	matrixZ.M[2][1] = 0.0f;
	matrixZ.M[2][2] = 1.0f;
	matrixZ.M[2][3] = 0.0f;

	matrixZ.M[3][0] = 0.0f;
	matrixZ.M[3][1] = 0.0f;
	matrixZ.M[3][2] = 0.0f;
	matrixZ.M[3][3] = 1.0f;

	matrix = Matrix44::Multiply( matrix, matrixZ );

	return matrix;
}



Matrix44 Matrix44::LookAt( const Vector3& Position, const Vector3& Target, const Vector3& UpVector )
{
	Vector3 vx, vy, vz;

	vz = (Vector3)Target - Position;
	vz.Normalize();

	vx = Vector3::Cross( UpVector, vz );
	vx.Normalize();

    vy = Vector3::Cross( vz, vx );


	Matrix44 matrix;

	matrix.M[0][0] = vx.X;
	matrix.M[0][1] = vy.X;
	matrix.M[0][2] = vz.X;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = vx.Y;
	matrix.M[1][1] = vy.Y;
	matrix.M[1][2] = vz.Y;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = vx.Z;
	matrix.M[2][1] = vy.Z;
	matrix.M[2][2] = vz.Z;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = -Vector3::Dot( vx, Position );
	matrix.M[3][1] = -Vector3::Dot( vy, Position );
	matrix.M[3][2] = -Vector3::Dot( vz, Position );
	matrix.M[3][3] = 1.0f;

	return matrix;

}


Matrix44 Matrix44::PerspectiveFov( float FOV, float Aspect, float NearZ, float FarZ )
{
	Matrix44 matrix;
	float t = 1.0f / tanf( FOV * 0.5f );

	matrix.M[0][0] = t / Aspect;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = t;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = FarZ / (FarZ - NearZ);
	matrix.M[2][3] = 1.0f;

	matrix.M[3][0] = 0.0f;
	matrix.M[3][1] = 0.0f;
	matrix.M[3][2] = -(FarZ * NearZ) / (FarZ - NearZ);
	matrix.M[3][3] = 0.0f;

	return matrix;
}



Matrix44 Matrix44::Ortho(float X, float Y, float Width, float Height)
{
	Matrix44 matrix;

	matrix.M[0][0] = 2.0f / Width;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = -2.0f / Height;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = -(Width + X * 2.0f) / Width;
	matrix.M[3][1] = (Height + Y * 2.0f) / Height;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;
}

Matrix44 Matrix44::OrthoCenter(float Width, float Height, float Depth)
{
	Matrix44 matrix;

	matrix.M[0][0] = 2.0f / Width;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = 2.0f / Height;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f / Depth;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = 0.0f;
	matrix.M[3][1] = 0.0f;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;
}


Matrix44 Matrix44::Viewport( float Width, float Height )
{
	Matrix44 matrix;

	matrix.M[0][0] = Width / 2;
	matrix.M[0][1] = 0.0f;
	matrix.M[0][2] = 0.0f;
	matrix.M[0][3] = 0.0f;

	matrix.M[1][0] = 0.0f;
	matrix.M[1][1] = -Height / 2;
	matrix.M[1][2] = 0.0f;
	matrix.M[1][3] = 0.0f;

	matrix.M[2][0] = 0.0f;
	matrix.M[2][1] = 0.0f;
	matrix.M[2][2] = 1.0f;
	matrix.M[2][3] = 0.0f;

	matrix.M[3][0] = Width / 2;
	matrix.M[3][1] = Height / 2;
	matrix.M[3][2] = 0.0f;
	matrix.M[3][3] = 1.0f;

	return matrix;

}


Matrix44 Matrix44::Transpose( const Matrix44& Matrix1 )
{
	Matrix44 matrix;

	matrix.M[0][0] = Matrix1.M[0][0];
	matrix.M[0][1] = Matrix1.M[1][0];
	matrix.M[0][2] = Matrix1.M[2][0];
	matrix.M[0][3] = Matrix1.M[3][0];

	matrix.M[1][0] = Matrix1.M[0][1];
	matrix.M[1][1] = Matrix1.M[1][1];
	matrix.M[1][2] = Matrix1.M[2][1];
	matrix.M[1][3] = Matrix1.M[3][1];

	matrix.M[2][0] = Matrix1.M[0][2];
	matrix.M[2][1] = Matrix1.M[1][2];
	matrix.M[2][2] = Matrix1.M[2][2];
	matrix.M[2][3] = Matrix1.M[3][2];

	matrix.M[3][0] = Matrix1.M[0][3];
	matrix.M[3][1] = Matrix1.M[1][3];
	matrix.M[3][2] = Matrix1.M[2][3];
	matrix.M[3][3] = Matrix1.M[3][3];

	return matrix;

}


Matrix44 Matrix44::Inverse(const Matrix44& Matrix1)
{
	float det = (Matrix1.M[0][0]*Matrix1.M[1][1] - Matrix1.M[1][0] * Matrix1.M[0][1])*(Matrix1.M[2][2]*Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[2][3])
			- (Matrix1.M[0][0]*Matrix1.M[2][1] - Matrix1.M[2][0] * Matrix1.M[0][1])*(Matrix1.M[1][2]*Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[1][3])
			+ (Matrix1.M[0][0]*Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[0][1])*(Matrix1.M[1][2]*Matrix1.M[2][3] - Matrix1.M[2][2] * Matrix1.M[1][3])
			+ (Matrix1.M[1][0]*Matrix1.M[2][1] - Matrix1.M[2][0] * Matrix1.M[1][1])*(Matrix1.M[0][2]*Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[0][3])
			- (Matrix1.M[1][0]*Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[1][1])*(Matrix1.M[0][2]*Matrix1.M[2][3] - Matrix1.M[2][2] * Matrix1.M[0][3])
			+ (Matrix1.M[2][0]*Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[2][1])*(Matrix1.M[0][2]*Matrix1.M[1][3] - Matrix1.M[1][2] * Matrix1.M[0][3]);

	det = 1.0f / det;

	Matrix44 matrix;

	matrix.M[0][0] = (Matrix1.M[1][1] * (Matrix1.M[2][2] * Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[2][3]) + Matrix1.M[2][1] * (Matrix1.M[3][2] * Matrix1.M[1][3] - Matrix1.M[1][2] * Matrix1.M[3][3]) + Matrix1.M[3][1] * (Matrix1.M[1][2] * Matrix1.M[2][3] - Matrix1.M[2][2] * Matrix1.M[1][3])) * det;
	matrix.M[1][0] = (Matrix1.M[1][2] * (Matrix1.M[2][0] * Matrix1.M[3][3] - Matrix1.M[3][0] * Matrix1.M[2][3]) + Matrix1.M[2][2] * (Matrix1.M[3][0] * Matrix1.M[1][3] - Matrix1.M[1][0] * Matrix1.M[3][3]) + Matrix1.M[3][2] * (Matrix1.M[1][0] * Matrix1.M[2][3] - Matrix1.M[2][0] * Matrix1.M[1][3])) * det;
	matrix.M[2][0] = (Matrix1.M[1][3] * (Matrix1.M[2][0] * Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[2][1]) + Matrix1.M[2][3] * (Matrix1.M[3][0] * Matrix1.M[1][1] - Matrix1.M[1][0] * Matrix1.M[3][1]) + Matrix1.M[3][3] * (Matrix1.M[1][0] * Matrix1.M[2][1] - Matrix1.M[2][0] * Matrix1.M[1][1])) * det;
	matrix.M[3][0] = (Matrix1.M[1][0] * (Matrix1.M[3][1] * Matrix1.M[2][2] - Matrix1.M[2][1] * Matrix1.M[3][2]) + Matrix1.M[2][0] * (Matrix1.M[1][1] * Matrix1.M[3][2] - Matrix1.M[3][1] * Matrix1.M[1][2]) + Matrix1.M[3][0] * (Matrix1.M[2][1] * Matrix1.M[1][2] - Matrix1.M[1][1] * Matrix1.M[2][2])) * det;

	matrix.M[0][1] = (Matrix1.M[2][1] * (Matrix1.M[0][2] * Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[0][3]) + Matrix1.M[3][1] * (Matrix1.M[2][2] * Matrix1.M[0][3] - Matrix1.M[0][2] * Matrix1.M[2][3]) + Matrix1.M[0][1] * (Matrix1.M[3][2] * Matrix1.M[2][3] - Matrix1.M[2][2] * Matrix1.M[3][3])) * det;
	matrix.M[1][1] = (Matrix1.M[2][2] * (Matrix1.M[0][0] * Matrix1.M[3][3] - Matrix1.M[3][0] * Matrix1.M[0][3]) + Matrix1.M[3][2] * (Matrix1.M[2][0] * Matrix1.M[0][3] - Matrix1.M[0][0] * Matrix1.M[2][3]) + Matrix1.M[0][2] * (Matrix1.M[3][0] * Matrix1.M[2][3] - Matrix1.M[2][0] * Matrix1.M[3][3])) * det;
	matrix.M[2][1] = (Matrix1.M[2][3] * (Matrix1.M[0][0] * Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[0][1]) + Matrix1.M[3][3] * (Matrix1.M[2][0] * Matrix1.M[0][1] - Matrix1.M[0][0] * Matrix1.M[2][1]) + Matrix1.M[0][3] * (Matrix1.M[3][0] * Matrix1.M[2][1] - Matrix1.M[2][0] * Matrix1.M[3][1])) * det;
	matrix.M[3][1] = (Matrix1.M[2][0] * (Matrix1.M[3][1] * Matrix1.M[0][2] - Matrix1.M[0][1] * Matrix1.M[3][2]) + Matrix1.M[3][0] * (Matrix1.M[0][1] * Matrix1.M[2][2] - Matrix1.M[2][1] * Matrix1.M[0][2]) + Matrix1.M[0][0] * (Matrix1.M[2][1] * Matrix1.M[3][2] - Matrix1.M[3][1] * Matrix1.M[2][2])) * det;

	matrix.M[0][2] = (Matrix1.M[3][1] * (Matrix1.M[0][2] * Matrix1.M[1][3] - Matrix1.M[1][2] * Matrix1.M[0][3]) + Matrix1.M[0][1] * (Matrix1.M[1][2] * Matrix1.M[3][3] - Matrix1.M[3][2] * Matrix1.M[1][3]) + Matrix1.M[1][1] * (Matrix1.M[3][2] * Matrix1.M[0][3] - Matrix1.M[0][2] * Matrix1.M[3][3])) * det;
	matrix.M[1][2] = (Matrix1.M[3][2] * (Matrix1.M[0][0] * Matrix1.M[1][3] - Matrix1.M[1][0] * Matrix1.M[0][3]) + Matrix1.M[0][2] * (Matrix1.M[1][0] * Matrix1.M[3][3] - Matrix1.M[3][0] * Matrix1.M[1][3]) + Matrix1.M[1][2] * (Matrix1.M[3][0] * Matrix1.M[0][3] - Matrix1.M[0][0] * Matrix1.M[3][3])) * det;
	matrix.M[2][2] = (Matrix1.M[3][3] * (Matrix1.M[0][0] * Matrix1.M[1][1] - Matrix1.M[1][0] * Matrix1.M[0][1]) + Matrix1.M[0][3] * (Matrix1.M[1][0] * Matrix1.M[3][1] - Matrix1.M[3][0] * Matrix1.M[1][1]) + Matrix1.M[1][3] * (Matrix1.M[3][0] * Matrix1.M[0][1] - Matrix1.M[0][0] * Matrix1.M[3][1])) * det;
	matrix.M[3][2] = (Matrix1.M[3][0] * (Matrix1.M[1][1] * Matrix1.M[0][2] - Matrix1.M[0][1] * Matrix1.M[1][2]) + Matrix1.M[0][0] * (Matrix1.M[3][1] * Matrix1.M[1][2] - Matrix1.M[1][1] * Matrix1.M[3][2]) + Matrix1.M[1][0] * (Matrix1.M[0][1] * Matrix1.M[3][2] - Matrix1.M[3][1] * Matrix1.M[0][2])) * det;

	matrix.M[0][3] = (Matrix1.M[0][1] * (Matrix1.M[2][2] * Matrix1.M[1][3] - Matrix1.M[1][2] * Matrix1.M[2][3]) + Matrix1.M[1][1] * (Matrix1.M[0][2] * Matrix1.M[2][3] - Matrix1.M[2][2] * Matrix1.M[0][3]) + Matrix1.M[2][1] * (Matrix1.M[1][2] * Matrix1.M[0][3] - Matrix1.M[0][2] * Matrix1.M[1][3])) * det;
	matrix.M[1][3] = (Matrix1.M[0][2] * (Matrix1.M[2][0] * Matrix1.M[1][3] - Matrix1.M[1][0] * Matrix1.M[2][3]) + Matrix1.M[1][2] * (Matrix1.M[0][0] * Matrix1.M[2][3] - Matrix1.M[2][0] * Matrix1.M[0][3]) + Matrix1.M[2][2] * (Matrix1.M[1][0] * Matrix1.M[0][3] - Matrix1.M[0][0] * Matrix1.M[1][3])) * det;
	matrix.M[2][3] = (Matrix1.M[0][3] * (Matrix1.M[2][0] * Matrix1.M[1][1] - Matrix1.M[1][0] * Matrix1.M[2][1]) + Matrix1.M[1][3] * (Matrix1.M[0][0] * Matrix1.M[2][1] - Matrix1.M[2][0] * Matrix1.M[0][1]) + Matrix1.M[2][3] * (Matrix1.M[1][0] * Matrix1.M[0][1] - Matrix1.M[0][0] * Matrix1.M[1][1])) * det;
	matrix.M[3][3] = (Matrix1.M[0][0] * (Matrix1.M[1][1] * Matrix1.M[2][2] - Matrix1.M[2][1] * Matrix1.M[1][2]) + Matrix1.M[1][0] * (Matrix1.M[2][1] * Matrix1.M[0][2] - Matrix1.M[0][1] * Matrix1.M[2][2]) + Matrix1.M[2][0] * (Matrix1.M[0][1] * Matrix1.M[1][2] - Matrix1.M[1][1] * Matrix1.M[0][2])) * det;

	return matrix;

}
