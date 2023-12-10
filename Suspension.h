#pragma once



class Suspension
{
private:

	class RigidBody* m_Body{};
	class RigidBody* m_Tire{};
	class Suspension* m_SuspensionOther{};



	float		m_ScaleX{};

	Vector3		m_UpperMountOffset;
	Vector3		m_LowerMountOffset;
	float		m_TireAxisLength{};
	float		m_LowerArmLength{};
	float		m_StrutLength{};
	float		m_SpringNaturalLength{};

	Vector3		m_UpperMountPos;
	Vector3		m_LowerArmBodyPos;
	Vector3		m_LowerArmTirePos;
	Vector3		m_TireAxisBodyPos;

	float		m_CamberAngle{};
	float		m_ToeAngle{};

	float		m_Stroke{};
	float		m_SpringRatio{};	//(N/m)
	float		m_DampingRatioExt{};//(N/m/s)
	float		m_DampingRatioRet{};//(N/m/s)

	float		m_Stabilizer{};

	float		m_SteeringAngle{};
	float		m_SpringLength{};


	//Model*		m_Model;

public:

	void SetRigidBody(RigidBody* Body, RigidBody* Tire, Suspension* SuspensionOther)
	{
		m_Body = Body;
		m_Tire = Tire;
		m_SuspensionOther = SuspensionOther;
	}
	void Load(const char* FileName, const char* PartName, float ScaleX);
	void Update(float dt);
	void UpdateGeometry(float dt);
	void DrawDebug(const char* DebugName);
	void PreDraw();
	void Draw();

	void SetSteeringAngle(float SteeringAngle) { m_SteeringAngle = SteeringAngle; }

	float GetSpringLength() { return m_SpringLength; }
};

