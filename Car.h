
#include "GameObject.h"

#include "RigidBody.h"
#include "BodyRB.h"
#include "Engine.h"
#include "Clutch.h"
#include "Transmission.h"
#include "Differential.h"
#include "TireRB.h"
#include "Brake.h"
#include "Suspension.h"
#include "Meter.h"




struct INPUT_STATE
{
	//Vector2 AxisL;
	//Vector2 AxisR;
	float	Steering;
	float	Accel;
	float	Brake;
	bool	ShiftUp;
	bool	ShiftDown;
	bool	ShiftUpRelease;
	bool	ShiftDownRelease;
};




class Car : public GameObject
{
	protected:


		BodyRB			m_BodyRB;

		Engine			m_Engine;
		Clutch			m_Clutch;
		Transmission	m_Transmission;
		Differential	m_DifferentialCenter;
		Differential	m_DifferentialFront;
		Differential	m_DifferentialRear;

		TireRB		m_TireFLRB;
		TireRB		m_TireFRRB;
		TireRB		m_TireRLRB;
		TireRB		m_TireRRRB;

		Brake		m_BrakeFL;
		Brake		m_BrakeFR;
		Brake		m_BrakeRL;
		Brake		m_BrakeRR;

		Suspension	m_SuspensionFL;
		Suspension	m_SuspensionFR;
		Suspension	m_SuspensionRL;
		Suspension	m_SuspensionRR;

		Meter		m_Meter;


		float		m_Throttle{};
		float		m_ClutchRatio{};
		float		m_SteeringAngle{};
		float		m_Brake{};

		bool		m_ShitUp{};
		bool		m_ShitDown{};



		bool			m_SoundEnable{};

		int				m_SoundIndexBrake{};
		//int				m_SoundIndexEngine2000{};
		//int				m_SoundIndexEngine2000Ex{};
		int				m_SoundIndexEngine4000{};
		int				m_SoundIndexEngine4000Ex{};
		//int				m_SoundIndexEngine6000{};
		//int				m_SoundIndexEngine6000Ex{};
		int				m_SoundIndexEngineNoise{};

		int				m_SoundIndexEngineRedNoise{};

		int				m_SoundIndexGear{};


		bool			m_ButtonLeft{};
		bool			m_ButtonRight{};
		bool			m_ButtonDown{};
		bool			m_ButtonUp{};



		Vector3			m_DriverPosition;


		bool								m_Replay{};
		int									m_ReplayFrame{};
		static std::vector<INPUT_STATE>		m_InputHistory;


		bool			m_JackUp{};
		int				m_Frame{};


		int				m_SmokeInterval{};

		int				m_LodBias{};

		float			m_KeybordSteering{};

		float			m_SteeringGearRatio{};



	public:
		Car();
		~Car();

		void InitPositionRotation(Vector3 Position, float Rotation);

		void Update( bool Control, bool Input, float dt );
		void PreDraw() override;
		void DrawDepth(class Camera* DrawCamera, int LodLevel=4);
		void Draw(class Camera* DrawCamera, int LodLevel=0);
		void DrawDebug() override;
		//void DrawShadow(class Camera* DrawCamera);
		void DrawUI() override;

		Vector3 GetPosition(){ return m_BodyRB.GetPosition(); }
		Vector3 GetDirection() { return m_BodyRB.GetMatrix().Front(); }
		Vector3 GetRight() { return m_BodyRB.GetMatrix().Right(); }
		Vector3 GetUp() { return m_BodyRB.GetMatrix().Up(); }
		Matrix44 GetRotationMatrix() { return m_BodyRB.GetMatrix(); }
		Vector3 GetVelocity() { return m_BodyRB.GetVelocity(); }



		Vector3 GetDriverPosition() { return m_DriverPosition; }


		void SetReplay(bool Replay);
		void SetSoundEnable(bool SoundEnable) { m_SoundEnable = SoundEnable; }
		void SetJackUp(bool JackUp) { m_JackUp = JackUp; }


		BodyRB* GetBodyRB(){ return &m_BodyRB; }
		TireRB* GetTireFLRB(){ return &m_TireFLRB; }
		TireRB* GetTireRLRB(){ return &m_TireRLRB; }
		TireRB* GetTireRRRB(){ return &m_TireRRRB; }
		TireRB* GetTireFRRB(){ return &m_TireFRRB; }
};