
#include "Pch.h"
#include "InputManager.h"

#include "GameManager.h"
#include "RigidBody.h"
#include "Car.h"
#include "GameScene.h"
//#include "Field.h"
#include "CarCamera.h"
#include "SmokeManager.h"
//#include "Enemy.h"

#include "Setting.h"


std::vector<INPUT_STATE>		Car::m_InputHistory;



Car::Car()
{
	m_SoundEnable = true;



	//パラメータ読み込み
	{
		std::string carname = "DB06";
		std::string path;
		path = "Asset\\Car\\" + carname + "\\data.ini";



		m_DriverPosition = GetPrivateProfileVector3("BODY", "CAMERA_POSITON", path.c_str());		


		m_BodyRB.Load(path.c_str(), "BODY");
		m_Engine.Load(path.c_str(), "ENGINE");
		m_Clutch.Load(path.c_str(), "CLUTCH");
		m_Transmission.Load(path.c_str(), "TRANSMISSION");

		m_DifferentialCenter.Load(path.c_str(), "DIFFERENTIAL_CENTER");
		m_DifferentialFront.Load(path.c_str(), "DIFFERENTIAL_FRONT");
		m_DifferentialRear.Load(path.c_str(), "DIFFERENTIAL_REAR");

		m_TireFLRB.Load(path.c_str(), "TIRE_FRONT", -1.0f);
		m_TireFRRB.Load(path.c_str(), "TIRE_FRONT", 1.0f);
		m_TireRLRB.Load(path.c_str(), "TIRE_REAR", -1.0f);
		m_TireRRRB.Load(path.c_str(), "TIRE_REAR", 1.0f);

		m_BrakeFL.Load(path.c_str(), "BRAKE_FRONT");
		m_BrakeFR.Load(path.c_str(), "BRAKE_FRONT");
		m_BrakeRL.Load(path.c_str(), "BRAKE_REAR");
		m_BrakeRR.Load(path.c_str(), "BRAKE_REAR");

		m_SuspensionFL.Load(path.c_str(), "SUSPENSION_FRONT", -1.0f);
		m_SuspensionFR.Load(path.c_str(), "SUSPENSION_FRONT", 1.0f);
		m_SuspensionRL.Load(path.c_str(), "SUSPENSION_REAR", -1.0f);
		m_SuspensionRR.Load(path.c_str(), "SUSPENSION_REAR", 1.0f);



		m_Engine.SetDrivetrain(nullptr, nullptr, &m_Clutch, nullptr);
		m_Clutch.SetDrivetrain(&m_Engine, nullptr, &m_Transmission, nullptr);
		m_Transmission.SetDrivetrain(&m_Clutch, nullptr, &m_DifferentialCenter, nullptr);


		m_DifferentialCenter.SetDrivetrain(&m_Transmission, nullptr, &m_DifferentialFront, &m_DifferentialRear);
		m_DifferentialFront.SetDrivetrain(&m_DifferentialCenter, nullptr, &m_TireFLRB, &m_TireFRRB);
		m_DifferentialRear.SetDrivetrain(nullptr, &m_DifferentialCenter, &m_TireRLRB, &m_TireRRRB);


		m_TireFLRB.SetDrivetrain(&m_DifferentialFront, nullptr, nullptr, nullptr);
		m_TireFRRB.SetDrivetrain(nullptr, &m_DifferentialFront, nullptr, nullptr);

		m_TireRLRB.SetDrivetrain(&m_DifferentialRear, nullptr, nullptr, nullptr);
		m_TireRRRB.SetDrivetrain(nullptr, &m_DifferentialRear, nullptr, nullptr);


		m_BrakeFL.SetRigidBody(&m_BodyRB, &m_TireFLRB);
		m_BrakeFR.SetRigidBody(&m_BodyRB, &m_TireFRRB);

		m_BrakeRL.SetRigidBody(&m_BodyRB, &m_TireRLRB);
		m_BrakeRR.SetRigidBody(&m_BodyRB, &m_TireRRRB);


		m_SuspensionFL.SetRigidBody(&m_BodyRB, &m_TireFLRB, &m_SuspensionFR);
		m_SuspensionFR.SetRigidBody(&m_BodyRB, &m_TireFRRB, &m_SuspensionFL);

		m_SuspensionRL.SetRigidBody(&m_BodyRB, &m_TireRLRB, &m_SuspensionRR);
		m_SuspensionRR.SetRigidBody(&m_BodyRB, &m_TireRRRB, &m_SuspensionRL);

	}







	{
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		m_BodyRB.Init(position, Quaternion::Identity());

		m_TireFLRB.Init(position, Quaternion::Identity());
		m_TireFLRB.SetBodyRB(&m_BodyRB);

		m_TireFRRB.Init(position, Quaternion::Identity());
		m_TireFRRB.SetBodyRB(&m_BodyRB);

		m_TireRLRB.Init(position, Quaternion::Identity());
		m_TireRLRB.SetBodyRB(&m_BodyRB);

		m_TireRRRB.Init(position, Quaternion::Identity());
		m_TireRRRB.SetBodyRB(&m_BodyRB);
	}



	{
		m_ClutchRatio = 1.0f;
		m_LodBias = 0;

		m_SteeringGearRatio = 20.0f;
	}


	if(m_SoundEnable)
	{			
		SoundManager* soundManager = SoundManager::GetInstance();

		//m_SoundIndexEngine2000 = soundManager->Play("sound\\2000.wav", true, 0.0f );
		//m_SoundIndexEngine2000Ex = soundManager->Play("sound\\2000ex.wav", true, 0.0f);
		//m_SoundIndexEngine4000 = soundManager->Play("sound\\4000.wav", true, 0.0f);
		m_SoundIndexEngine4000Ex = soundManager->Play("sound\\4000ex.wav", true, 0.0f);
		//m_SoundIndexEngine6000 = soundManager->Play("sound\\6000.wav", true, 0.0f);
		//m_SoundIndexEngine6000Ex = soundManager->Play("sound\\6000ex.wav", true, 0.0f);

		m_SoundIndexEngineNoise = soundManager->Play("sound\\noise.wav", true, 0.0f);

		m_SoundIndexEngineRedNoise = soundManager->Play("sound\\rednoise.wav", true, 0.0f);


		m_SoundIndexGear = soundManager->Play("sound\\gear.wav", true, 0.0f);


		m_SoundIndexBrake = soundManager->Play( "sound\\brake.wav", true, 0.0f );

	}

}



Car::~Car()
{
	//if( m_SoundEnable )
	{
		SoundManager* soundManager = SoundManager::GetInstance();

		//soundManager->Stop("sound\\2000.wav", m_SoundIndexEngine2000);
		//soundManager->Stop("sound\\2000ex.wav", m_SoundIndexEngine2000Ex);
		//soundManager->Stop("sound\\4000.wav", m_SoundIndexEngine4000);
		soundManager->Stop("sound\\4000ex.wav", m_SoundIndexEngine4000Ex);
		//soundManager->Stop("sound\\6000.wav", m_SoundIndexEngine6000);
		//soundManager->Stop("sound\\6000ex.wav", m_SoundIndexEngine6000Ex);


		soundManager->Stop("sound\\noise.wav", m_SoundIndexEngineNoise);
		soundManager->Stop("sound\\rednoise.wav", m_SoundIndexEngineRedNoise);

		soundManager->Stop("sound\\gear.wav", m_SoundIndexGear);


		soundManager->Stop( "sound\\brake.wav", m_SoundIndexBrake);
	}

}

void Car::InitPositionRotation(Vector3 Position, float Rotation)
{
	m_BodyRB.Init(Position, Quaternion::RotationAxis(Vector3(0.0f, Rotation, 0.0f)));
	m_TireFLRB.Init(Position, Quaternion::RotationAxis(Vector3(0.0f, Rotation, 0.0f)));
	m_TireFRRB.Init(Position, Quaternion::RotationAxis(Vector3(0.0f, Rotation, 0.0f)));
	m_TireRLRB.Init(Position, Quaternion::RotationAxis(Vector3(0.0f, Rotation, 0.0f)));
	m_TireRRRB.Init(Position, Quaternion::RotationAxis(Vector3(0.0f, Rotation, 0.0f)));
}




void Car::SetReplay(bool Replay)
{ 
	m_Replay = Replay;
	m_ReplayFrame = 0;

	if (!m_Replay)
		m_InputHistory.clear();
}


void Car::Update( bool Control, bool Input, float dt )
{

	InputManager* inputManager = InputManager::GetInstance();

	SoundManager* soundManager = SoundManager::GetInstance();
	GameManager* gameManager = GameManager::GetInstance();
	GameScene* scene = (GameScene*)gameManager->GetScene();








	INPUT_STATE inputState{};

	if(m_Replay && m_ReplayFrame < m_InputHistory.size())
	{

		inputState = m_InputHistory[m_ReplayFrame];
		m_ReplayFrame++;
	}
	else
	{
		//inputState.AxisL = inputManager->GetAxisLeft();
		//inputState.AxisR = inputManager->GetAxisRight();
		
		inputState.Steering = inputManager->GetAxisLeft().X;

		inputState.Accel = max(inputManager->GetAxisRight().Y, 0.0f);
		inputState.Brake = max(-inputManager->GetAxisRight().Y, 0.0f);

		inputState.Accel += inputManager->GetTriggerRight();
		inputState.Brake += inputManager->GetTriggerLeft();

		inputState.Accel = min(inputState.Accel, 1.0f);
		inputState.Brake = min(inputState.Brake, 1.0f);

		inputState.ShiftUp = inputManager->GetButtonPress(5) || inputManager->GetButtonPress(0);
		inputState.ShiftDown = inputManager->GetButtonPress(4) || inputManager->GetButtonPress(2);
		inputState.ShiftUpRelease = inputManager->GetButtonRelease(5) || inputManager->GetButtonRelease(0);
		inputState.ShiftDownRelease = inputManager->GetButtonRelease(4) || inputManager->GetButtonRelease(2);

		if (inputManager->GetKeyPress(VK_LEFT))
		{
			if (m_KeybordSteering > -1.0)
			{
				m_KeybordSteering -= 0.008f;
				if (m_KeybordSteering < -1.0)
					m_KeybordSteering = -1.0;
			}
		}
		else if (inputManager->GetKeyPress(VK_RIGHT))
		{
			if (m_KeybordSteering < 1.0)
			{
				m_KeybordSteering += 0.008f;
				if (m_KeybordSteering > 1.0)
					m_KeybordSteering = 1.0;
			}
		}
		else
		{
			if (m_KeybordSteering > 0.0)
			{
				m_KeybordSteering -= 0.008f;
				if (m_KeybordSteering < 0.0)
					m_KeybordSteering = 0.0;
			}
			else if (m_KeybordSteering < 0.0)
			{
				m_KeybordSteering += 0.008f;
				if (m_KeybordSteering > 0.0)
					m_KeybordSteering = 0.0;
			}
		}


		inputState.Steering += m_KeybordSteering;



		if (inputManager->GetKeyPress(VK_UP))
			inputState.Accel += 1.0f;
		if (inputManager->GetKeyPress(VK_DOWN))
			inputState.Brake += 1.0f;

		if (inputManager->GetKeyTrigger('Z'))
			inputState.ShiftDownRelease |= true;
		if (inputManager->GetKeyTrigger('X'))
			inputState.ShiftUpRelease |= true;


		m_InputHistory.push_back(inputState);
	}


	//入力

	if( Input )
	{
		//アクセル・ブレーキ
		m_Throttle = inputState.Accel;
		m_Brake = inputState.Brake;

		if(m_Brake > 0.0f)
			m_BodyRB.SetBrakeLamp(true);
		else
			m_BodyRB.SetBrakeLamp(false);


		if (m_Throttle < 0.05f)
			m_Throttle = 0.05f;



		//ステアリング
		m_SteeringAngle = -inputState.Steering * 450.0f / 360.0f * 2.0f * PI / m_SteeringGearRatio;



		//シフト
		int gear = m_Transmission.GetGear();
		int gearCount = m_Transmission.GetGearCount();

		
		//MT
		{
			if (inputState.ShiftDownRelease && gear > -1)
			{
				gear--;
				m_ShitDown = true;
			}

			if (inputState.ShiftUpRelease && gear < gearCount)
			{
				gear++;
				m_ShitUp = true;
			}
		}
	


		float rpm = m_Engine.GetOutputAngularSpeed1() * 60.0f / 2.0f / PI;

		if (rpm < 1000.0f)
			gear = 0;


		m_Transmission.SetGear(gear);

	}


	//自動クラッチ
	{
		if (inputState.ShiftDown && m_Transmission.GetGear() > 1)
		{
			m_ClutchRatio -= 15.0f * dt;
			m_Throttle = 1.0f - m_ClutchRatio;
		}
		else if (inputState.ShiftUp && m_Transmission.GetGear() < m_Transmission.GetGearCount())
		{
			m_ClutchRatio -= 15.0f * dt;
			m_Throttle = m_ClutchRatio;

			if (m_Throttle < 0.1f)
				m_Throttle = 0.1f;
		}
		else
		{
			m_ClutchRatio += 15.0f * dt;

			if (m_ShitDown)
				m_Throttle = 1.0f - m_ClutchRatio;

			if (m_ShitUp)
				m_Throttle = m_ClutchRatio;
		}


		if (m_ClutchRatio < 0.0f)
			m_ClutchRatio = 0.0f;
		if (m_ClutchRatio > 1.0f)
		{
			m_ClutchRatio = 1.0f;
			m_ShitDown = false;
			m_ShitUp = false;
		}

	}











	//ドライブトレイン
	{

		m_Engine.SetThrottle(m_Throttle);
		m_Engine.Update(dt);

		m_Clutch.SetClutchRatio(m_Transmission.GetGearRatio());
		m_Clutch.SetClutch(m_ClutchRatio);
		m_Clutch.Update(dt);

		m_Transmission.Update(dt);

		m_DifferentialCenter.Update(dt);

		m_DifferentialFront.Update(dt);
		m_DifferentialRear.Update(dt);


	}


	//重力
	{
		Vector3 g = Vector3(0.0f, -9.80665f, 0.0f);
		m_BodyRB.AddForceWorld(g*m_BodyRB.GetMass(), dt);
		m_TireFLRB.AddForceWorld(g*m_TireFLRB.GetMass(), dt);
		m_TireFRRB.AddForceWorld(g*m_TireFRRB.GetMass(), dt);
		m_TireRLRB.AddForceWorld(g*m_TireRLRB.GetMass(), dt);
		m_TireRRRB.AddForceWorld(g*m_TireRRRB.GetMass(), dt);
	}


	if (m_JackUp || m_Frame < 60)
	{
		m_Brake = 1.0f;
	}


	//ブレーキ
	{
		m_BrakeFL.SetRatio(m_Brake);
		m_BrakeFR.SetRatio(m_Brake);
		m_BrakeRL.SetRatio(m_Brake);
		m_BrakeRR.SetRatio(m_Brake);

		m_BrakeFL.Update(dt);
		m_BrakeFR.Update(dt);
		m_BrakeRL.Update(dt);
		m_BrakeRR.Update(dt);
	}





	//サスペンション
	{
		m_SuspensionFL.Update(dt);
		m_SuspensionFR.Update(dt);
		m_SuspensionRL.Update(dt);
		m_SuspensionRR.Update(dt);

	}




	//タイヤ接地・更新
	{
		m_TireFLRB.Update(dt);
		m_TireFRRB.Update(dt);
		m_TireRLRB.Update(dt);
		m_TireRRRB.Update(dt);
	}




	//ボディ更新
	{

		//ジャッキアップ
		if (m_JackUp || m_Frame < 60)
		{

		}
		else
		{
			m_BodyRB.Update(dt);
		}
	}

	m_Frame++;



	//サスペンションジオメトリ
	{

		m_SuspensionFL.SetSteeringAngle(m_SteeringAngle);
		m_SuspensionFL.UpdateGeometry(dt);

		m_SuspensionFR.SetSteeringAngle(m_SteeringAngle);
		m_SuspensionFR.UpdateGeometry(dt);

		m_SuspensionRL.UpdateGeometry(dt);

		m_SuspensionRR.UpdateGeometry(dt);

	}




	//変位→速度→力・トルク・ステアリングトルク算出
	{
		Vector3 force, torque, torqueUp;
		float steeringTorqueL, steeringTorqueR;
		Matrix44 matrix;
		matrix = m_BodyRB.GetMatrix();

		m_BodyRB.CalcVelocity(dt, &force, &torque);


		m_TireFLRB.CalcVelocity(dt, &force, &torque);
		m_BodyRB.AddForceWorld(m_TireFLRB.GetPosition(), -force, dt);

		steeringTorqueL = Vector3::Dot(torque, matrix.Up());
		torqueUp = matrix.Up() * steeringTorqueL;
		torque -= torqueUp;
		m_BodyRB.AddTorqueWorld(m_TireFLRB.GetPosition(), -torque, dt);


		m_TireFRRB.CalcVelocity(dt, &force, &torque);
		m_BodyRB.AddForceWorld(m_TireFRRB.GetPosition(), -force, dt);

		steeringTorqueR = Vector3::Dot(torque, matrix.Up());
		torqueUp = matrix.Up() * steeringTorqueR;
		torque -= torqueUp;
		m_BodyRB.AddTorqueWorld(m_TireFRRB.GetPosition(), -torque, dt);


		inputManager->SetTorque((steeringTorqueL + steeringTorqueR) / m_SteeringGearRatio);


		Vector3 torqueL, torqueR;

		m_TireRLRB.CalcVelocity(dt, &force, &torqueL);
		m_BodyRB.AddForceWorld(m_TireRLRB.GetPosition(), -force, dt);
		m_BodyRB.AddTorqueWorld(m_TireRLRB.GetPosition(), -torqueL, dt);


		m_TireRRRB.CalcVelocity(dt, &force, &torqueR);
		m_BodyRB.AddForceWorld(m_TireRRRB.GetPosition(), -force, dt);
		m_BodyRB.AddTorqueWorld(m_TireRRRB.GetPosition(), -torqueR, dt);

	}



	//タイヤスモーク
	m_SmokeInterval++;
	if(m_SmokeInterval == 10)
	{
		m_SmokeInterval = 0;

		SmokeManager* smokeManager = scene->GetGameObject<SmokeManager>();
		if (smokeManager)
		{
			float srip;
			Vector3 position;

			srip = m_TireFLRB.GetSrip();
			position = m_TireFLRB.GetPosition();
			if (srip > 0.1f)
			{
				smokeManager->Create(position, min(srip * 1.0f, 1.0f));
			}

			srip = m_TireFRRB.GetSrip();
			position = m_TireFRRB.GetPosition();
			if (srip > 0.1f)
			{
				smokeManager->Create(position, min(srip * 1.0f, 1.0f));
			}

			srip = m_TireRLRB.GetSrip();
			position = m_TireRLRB.GetPosition();
			if (srip > 0.1f)
			{
				smokeManager->Create(position, min(srip * 1.0f, 1.0f));
			}

			srip = m_TireRRRB.GetSrip();
			position = m_TireRRRB.GetPosition();
			if (srip > 0.1f)
			{
				smokeManager->Create(position, min(srip * 1.0f, 1.0f));
			}
		}
	}



	if (m_SoundEnable)
	{
		Camera* camera = scene->GetCurrentCamera();
		Vector3 cameraDirection = camera->GetPosition() - m_BodyRB.GetPosition();
		float distance = cameraDirection.Length();
		cameraDirection.Normalize();

		float volume = 50.0f / distance;
		volume = min(volume, 4.0f);

		float pitch = 1.0f;

		if (camera->GetViewMode() == 4)
		{
			Vector3 carVelocity = m_BodyRB.GetVelocity();
			float cvd = Vector3::Dot(carVelocity, cameraDirection);
			pitch = 340.29f / (340.29f - cvd);
		}


		float rpm = m_Engine.GetOutputAngularSpeed1() * 60.0f / 2.0f / PI;

		//if (rpm < 3000.0f)
		//{
		//	soundManager->SetPitch("sound\\2000.wav", m_SoundIndexEngine2000, rpm / 2000.0f * pitch);
		//	soundManager->SetVolume("sound\\2000.wav", m_SoundIndexEngine2000, (m_Engine.GetOutputAngularSpeed1() * 0.0005f) * m_Throttle * 0.2f * volume);

		//	soundManager->SetPitch("sound\\2000ex.wav", m_SoundIndexEngine2000Ex, rpm / 2000.0f * pitch);
		//	soundManager->SetVolume("sound\\2000ex.wav", m_SoundIndexEngine2000Ex, (m_Engine.GetOutputAngularSpeed1() * 0.0005f + 0.5f) * 0.1f * volume);

		//	soundManager->SetVolume("sound\\4000.wav", m_SoundIndexEngine4000, 0.0f);
		//	soundManager->SetVolume("sound\\4000ex.wav", m_SoundIndexEngine4000Ex, 0.0f);

		//	soundManager->SetVolume("sound\\6000.wav", m_SoundIndexEngine6000, 0.0f);
		//	soundManager->SetVolume("sound\\6000ex.wav", m_SoundIndexEngine6000Ex, 0.0f);
		//}
		//else if(rpm < 5000.0f)
		{
			//soundManager->SetPitch("sound\\4000.wav", m_SoundIndexEngine4000, rpm / 4000.0f * pitch);
			//soundManager->SetVolume("sound\\4000.wav", m_SoundIndexEngine4000, (m_Engine.GetOutputAngularSpeed1() * 0.0005f) * (1.0f - m_Engine.GetThrottle()) * 0.3f * volume);

			soundManager->SetPitch("sound\\4000ex.wav", m_SoundIndexEngine4000Ex, rpm / 4000.0f * pitch);
			soundManager->SetVolume("sound\\4000ex.wav", m_SoundIndexEngine4000Ex, rpm / 50000.0f * volume);

			//soundManager->SetVolume("sound\\2000.wav", m_SoundIndexEngine2000, 0.0f);
			//soundManager->SetVolume("sound\\2000ex.wav", m_SoundIndexEngine2000Ex, 0.0f);

			//soundManager->SetVolume("sound\\6000.wav", m_SoundIndexEngine6000, 0.0f);
			//soundManager->SetVolume("sound\\6000ex.wav", m_SoundIndexEngine6000Ex, 0.0f);
		}
		//else
		//{
		//	soundManager->SetPitch("sound\\6000.wav", m_SoundIndexEngine6000, rpm / 6000.0f * pitch);
		//	soundManager->SetVolume("sound\\6000.wav", m_SoundIndexEngine6000, (m_Engine.GetOutputAngularSpeed1() * 0.0005f) * m_Throttle * 0.2f * volume);

		//	soundManager->SetPitch("sound\\6000ex.wav", m_SoundIndexEngine6000Ex, rpm / 6000.0f * pitch);
		//	soundManager->SetVolume("sound\\6000ex.wav", m_SoundIndexEngine6000Ex, (m_Engine.GetOutputAngularSpeed1() * 0.0005f + 0.5f) * 0.1f * volume);

		//	soundManager->SetVolume("sound\\2000.wav", m_SoundIndexEngine2000, 0.0f);
		//	soundManager->SetVolume("sound\\2000ex.wav", m_SoundIndexEngine2000Ex, 0.0f);

		//	soundManager->SetVolume("sound\\4000.wav", m_SoundIndexEngine4000, 0.0f);
		//	soundManager->SetVolume("sound\\4000ex.wav", m_SoundIndexEngine4000Ex, 0.0f);
		//}

		//soundManager->SetPitch("sound\\noise.wav", m_SoundIndexEngineNoise, 0.9f + rpm / 20000.0f);
		soundManager->SetVolume("sound\\noise.wav", m_SoundIndexEngineNoise, rpm * rpm / 200000000.0f * (0.5f + m_Engine.GetThrottle() * 0.5f) * volume);

		soundManager->SetVolume("sound\\rednoise.wav", m_SoundIndexEngineRedNoise, m_TireFLRB.GetInputAngularSpeed1() * 0.003f);



		soundManager->SetPitch("sound\\gear.wav", m_SoundIndexGear, m_TireRLRB.GetInputAngularSpeed1() * 0.005f * pitch);
		soundManager->SetVolume("sound\\gear.wav", m_SoundIndexGear, m_TireRLRB.GetInputAngularSpeed1() * 0.0002f * volume);



		float sripL = (m_TireFLRB.GetSrip() + m_TireRLRB.GetSrip()) / 2.0f;
		float sripR = (m_TireFRRB.GetSrip() + m_TireRRRB.GetSrip()) / 2.0f;

		soundManager->SetPitch("sound\\brake.wav", m_SoundIndexBrake, 0.5f * pitch);
		soundManager->SetVolume("sound\\brake.wav", m_SoundIndexBrake, (sripL + sripR) * 1.0f * volume);

	}




}




void Car::DrawDebug()
{

	ImGui::Begin("CarDebug");

		ImGui::SliderInt("LodBias", &m_LodBias, -5, 5);


		ImGui::SliderFloat("SteeringGearRatio", &m_SteeringGearRatio, 0.0f, 50.0f, "%.1f");



		m_BodyRB.DrawDebug();
		m_Engine.DrawDebug();
		m_Clutch.DrawDebug();
		m_Transmission.DrawDebug();

		m_DifferentialCenter.DrawDebug("DifferentialCenter");
		m_DifferentialFront.DrawDebug("DifferentialFront");
		m_DifferentialRear.DrawDebug("DifferentialRear");

		m_SuspensionFL.DrawDebug("SuspensionF");
		m_SuspensionFR.DrawDebug("SuspensionF");
		m_SuspensionRL.DrawDebug("SuspensionR");
		m_SuspensionRR.DrawDebug("SuspensionR");

		m_BrakeFL.DrawDebug("BrakeF");
		m_BrakeFR.DrawDebug("BrakeF");
		m_BrakeRL.DrawDebug("BrakeR");
		m_BrakeRR.DrawDebug("BrakeR");

	ImGui::End();






	m_TireFLRB.DrawDebug("TireFL");
	m_TireFRRB.DrawDebug("TireFR");
	m_TireRLRB.DrawDebug("TireRL");
	m_TireRRRB.DrawDebug("TireRR");







	ImGui::Begin("Car");

		char buf[32];
		float rpm = m_Engine.GetOutputAngularSpeed1() * 30.0f / PI;

		ImVec4 color[4] =
		{
			{1.0f, 1.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f}
		};

		for (int i = 0; i < 4; i++)
		{
			ImGui::SameLine();

			if (rpm > 4000 + i * 500)
				ImGui::PushStyleColor(ImGuiCol_Button, color[i]);
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Button(" ");
			ImGui::PopStyleColor(1);
		}


		sprintf(buf, "%.0f RPM", rpm);
		ImGui::ProgressBar(rpm / 40000.0f, ImVec2(0, 16), buf);


		int gear = m_Transmission.GetGear();
		sprintf(buf, "Gear %d", gear);
		ImGui::ProgressBar(gear / 10.0f, ImVec2(0, 0), buf);




		float speedkmph;
		speedkmph = m_BodyRB.GetVelocity().Length() / 4000.0f * 60.0f * 60.0f;
		speedkmph = abs(speedkmph);
		sprintf(buf, "%.0f km/h", speedkmph);
		ImGui::ProgressBar(speedkmph / 300.0f, ImVec2(0, 16), buf);

	ImGui::End();



	ImGui::Begin("Controller");

		sprintf(buf, "SterlingAngle %.1f", -m_SteeringAngle);
		ImGui::ProgressBar(-m_SteeringAngle + 0.5f, ImVec2(0, 0), buf);

		sprintf(buf, "Throttle %.2f", m_Throttle);
		ImGui::ProgressBar(m_Throttle, ImVec2(0, 0), buf);
		sprintf(buf, "Brake %.2f", m_Brake);
		ImGui::ProgressBar(m_Brake, ImVec2(0, 0), buf);

	ImGui::End();

}






void Car::PreDraw()
{

	m_BodyRB.PreDraw();

	m_TireFLRB.PreDraw();
	m_TireFRRB.PreDraw();
	m_TireRLRB.PreDraw();
	m_TireRRRB.PreDraw();

	m_BrakeFL.PreDraw();
	m_BrakeFR.PreDraw();
	m_BrakeRL.PreDraw();
	m_BrakeRR.PreDraw();

/*
	m_SuspensionFL.PreDraw();
	m_SuspensionFR.PreDraw();
	m_SuspensionRL.PreDraw();
	m_SuspensionRR.PreDraw();
*/
}




void Car::DrawDepth(Camera* DrawCamera, int LodLevel)
{
	LodLevel += m_LodBias;
	LodLevel = max(LodLevel, 0);
	LodLevel = min(LodLevel, 4);

	m_BodyRB.Draw(DrawCamera, LodLevel);

	m_TireFLRB.Draw(DrawCamera, LodLevel);
	m_TireFRRB.Draw(DrawCamera, LodLevel);
	m_TireRLRB.Draw(DrawCamera, LodLevel);
	m_TireRRRB.Draw(DrawCamera, LodLevel);
/*
	m_SuspensionFL.Draw();
	m_SuspensionFR.Draw();
	m_SuspensionRL.Draw();
	m_SuspensionRR.Draw();
*/
}



void Car::Draw(Camera* DrawCamera, int LodLevel)
{
	RenderManager* render = RenderManager::GetInstance();
	render->SetPipelineState("Car");


	LodLevel += m_LodBias;
	LodLevel = max(LodLevel, 0);
	LodLevel = min(LodLevel, 4);


	m_BodyRB.Draw(DrawCamera, LodLevel);
	
	m_TireFLRB.Draw(DrawCamera, LodLevel);
	m_TireFRRB.Draw(DrawCamera, LodLevel);
	m_TireRLRB.Draw(DrawCamera, LodLevel);
	m_TireRRRB.Draw(DrawCamera, LodLevel);

	m_BrakeFL.Draw(DrawCamera, LodLevel);
	m_BrakeFR.Draw(DrawCamera, LodLevel);
	m_BrakeRL.Draw(DrawCamera, LodLevel);
	m_BrakeRR.Draw(DrawCamera, LodLevel);

/*	
	m_SuspensionFL.Draw();
	m_SuspensionFR.Draw();
	m_SuspensionRL.Draw();
	m_SuspensionRR.Draw();
*/
}



//
//void Car::DrawShadow(Camera* DrawCamera)
//{
//	RenderManager* render = RenderManager::GetInstance();
//	render->SetPipelineState("Shadow");
//
//	m_BodyRB.DrawShadow(DrawCamera);
//
//	//m_TireFLRB.DrawShadow(DrawCamera);
//	//m_TireFRRB.DrawShadow(DrawCamera);
//	//m_TireRLRB.DrawShadow(DrawCamera);
//	//m_TireRRRB.DrawShadow(DrawCamera);
//
//}






void Car::DrawUI()
{



	RenderManager* render = RenderManager::GetInstance();
	render->SetPipelineState("Ui");

	m_Meter.SetRPM(m_Engine.GetOutputAngularSpeed1());
	m_Meter.SetSpeed(m_BodyRB.GetVelocity().Length());
	m_Meter.SetGear(m_Transmission.GetGear());

	m_Meter.DrawUI();


}


