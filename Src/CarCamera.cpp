#include "Pch.h"
#include "Main.h"
#include "GameManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "CarCamera.h"
#include "Car.h"
#include "Field.h"

#include "GameScene.h"




CarCamera::CarCamera()
{
	m_Position = Vector3{ 10.0f, 0.0f, 0.0f };
	m_Target = Vector3{ 0.0f, 0.5f, 0.0f };
	m_Rotation = Vector3{ 0.0f, 1.0f, 0.0f };
	m_TargetRotation = m_Rotation;
	m_Distance = 5.0f;
	m_TargetDistance = m_Distance;

	m_DronePosition = Vector3{ 10.0f, 0.0f, 0.0f };
	m_DroneRotationSpeed = Vector3{ 0.0f, 0.0f, 0.0f };


	m_AutoExposure = false;
	m_Exposure = 15.0f;
	m_Gamma = 2.2f;
	m_Vignette = 0.4f;
	m_LensFlare = 0.2f;
	m_TemporalRatio = 0.8f;
	m_MotionBlurCount = 4;
	m_MotionBlur = 0.5f;



	{
		std::string path;
		path = ".\\Setting.ini";

		m_AntiAliasng = GetPrivateProfileInt("RENDER", "ANTI_ALIASING", 0, path.c_str());
	}

}






void CarCamera::Update()
{
	GameManager* gameManager = GameManager::GetInstance();
	InputManager* inputManager = InputManager::GetInstance();



	bool changeViewMode = false;
	if (inputManager->GetButtonTrigger(8) || inputManager->GetKeyTrigger('W'))
	{
		m_ViewMode++;
		if (m_ViewMode > 5)
			m_ViewMode = 0;

		changeViewMode = true;
	}
	if (inputManager->GetButtonTrigger(9) || inputManager->GetKeyTrigger('S'))
	{
		m_ViewMode--;
		if (m_ViewMode < 0)
			m_ViewMode = 5;

		changeViewMode = true;
	}


	if (changeViewMode)
	{

		if (m_ViewMode == 1)
		{
			m_FocalBlur = 0.0f;
			m_FocalLength = 16.0f;
			m_Exposure = -0.8f;
		}
		else
		{
			m_FocalBlur = 50.0f;
			m_FocalLength = 35.0f;
			m_Exposure = -0.5f;
		}

	}

	if (m_ViewMode == 4)
		m_FocalBlur = 300.0f;



	GameScene* scene = (GameScene*)gameManager->GetScene();
	Car* car = scene->GetGameObject<Car>();
	Vector3 playerPosition = car->GetPosition();
	//	Vector3 playerRotation = player->getRotation();

	Vector3 playerDirection = car->GetDirection();
	Vector3 playerRight = car->GetRight();
	//float playerSpeed = player->GetSpeed();
	Vector3 playerVelocity = car->GetVelocity();
	playerVelocity.Y = 0.0f;



	if (m_ViewMode == 0)
	{
		//Third Person View


		m_Up = Vector3(0.0f, 1.0f, 0.0f);


		m_Target = playerPosition;


		Vector3 vec, nvec, pos;
		float len;

		playerPosition.Y += 1.3f;

		vec = playerPosition - m_Position;
		len = vec.Length();
		nvec = vec / len;

		//if( len > 5.5f && !rot )
		{
			m_Position += nvec * (len - 6.5f);
		}/*
		else
		{
			pos.X = playerPosition.X + cosf( Rotation.Y ) * 5.0f;
			pos.Z = playerPosition.Z + sinf( Rotation.Y ) * 5.0f;
			pos.Y = playerPosition.Y;
			Position -= (Position - pos) * 0.1f;
		}*/

		if (m_Position.Y < playerPosition.Y)
			m_Position.Y = playerPosition.Y;

		m_Target.X = playerPosition.X;// -cosf(playerRotation.Y + Rotation.Y) * 2.0f;
		m_Target.Z = playerPosition.Z;// -sinf(playerRotation.Y + Rotation.Y) * 2.0f;
		//Target.Y = -sinf( playerRotation.X ) * 5.0f + 1.0;
		m_Target.Y = 0.5;


		m_Target = playerPosition;// +playerDirection * playerSpeed * 10.0f;
		m_Target.Y = m_Position.Y - 1.0f;
	}
	else if (m_ViewMode == 1)
	{
		//First Person View

		m_Up = car->GetUp();

		Vector3 driverPosition = car->GetDriverPosition();
		m_Position = playerPosition + playerRight * driverPosition.X + playerDirection * driverPosition.Z + m_Up * driverPosition.Y;


		Vector3 velocity = m_Position - m_OldPosition;
		Vector3 acc = velocity - m_OldVelocity;

		m_OldVelocity = velocity;
		m_OldPosition = m_Position;

		//m_Position += -acc * 20.0f;

		m_Target = m_Position + playerDirection;

	}
	else if (m_ViewMode == 2)
	{
		//around view

		if (InputManager::GetInstance()->GetKeyPress(VK_RBUTTON))
		{
			POINT mouseMove = InputManager::GetInstance()->GetMouseMove();

			m_TargetRotation.Y += mouseMove.x * 0.002f;
			m_TargetRotation.X += mouseMove.y * 0.002f;

		}

		m_TargetRotation.Y -= inputManager->GetAxisRight().X * 0.01f;
		m_TargetRotation.X += inputManager->GetAxisRight().Y * 0.01f;

		m_Rotation += (m_TargetRotation - m_Rotation) * 0.05f;



		if (InputManager::GetInstance()->GetKeyPress(VK_MBUTTON))
		{
			POINT mouseMove = InputManager::GetInstance()->GetMouseMove();
			m_TargetDistance += mouseMove.y * 0.02f;
		}
		m_Distance += (m_TargetDistance - m_Distance) * 0.05f;


		m_Up = car->GetUp();

		m_Target = Vector3(0.0f, 0.8f, 0.0f);
		m_Position.X = m_Target.X + cosf(m_Rotation.X) * sinf(m_Rotation.Y) * m_Distance;
		m_Position.Z = m_Target.Z + cosf(m_Rotation.X) * cosf(m_Rotation.Y) * m_Distance;
		m_Position.Y = m_Target.Y + sinf(m_Rotation.X) * m_Distance;

	}
	else if (m_ViewMode == 3)
	{
		//Tire View

		m_Up = car->GetUp();

		m_Position = playerPosition + playerRight * 1.2f + playerDirection * -2.6f + m_Up * -0.1f;
		m_Target = m_Position + playerDirection;

	}
	else if (m_ViewMode == 4)
	{
		//replay camera


		m_Up = Vector3(0.0f, 1.0f, 0.0f);
		m_Target += (playerPosition + playerVelocity * 0.3f - m_Target) * 0.01f;

		float minLength = 10000.0;


		Vector3 oldPosition = m_Position;

		for (auto location : m_Location)
		{
			float length = (playerPosition - location).Length();
			if (length < minLength)
			{
				m_Position = location;
				minLength = length;
			}
		}

		if ((oldPosition - m_Position).LengthSq() > 1.0f)
			changeViewMode = true;

		m_FocalLength = minLength * 5.0f;

		m_FocalLength = min(m_FocalLength, 300.0f);
		m_FocalLength = max(m_FocalLength, 35.0f);
	}
	else if (m_ViewMode == 5)
	{
		//drone view

		m_Up = Vector3(0.0f, 1.0f, 0.0f);

		m_DroneRotationSpeed.Y += inputManager->GetAxisRight().X * 0.00005f;
		m_DroneRotationSpeed.X += inputManager->GetAxisRight().Y * 0.00005f;
		m_DroneRotationSpeed *= 0.99f;
		m_Rotation += m_DroneRotationSpeed;


		m_DronePosition.Y -= inputManager->GetTriggerLeft() * 0.005f;
		m_DronePosition.Y += inputManager->GetTriggerRight() * 0.005f;

		m_DronePosition += Vector3(-sinf(m_Rotation.Y), 0.0f, -cosf(m_Rotation.Y)) * inputManager->GetAxisLeft().X * 0.01f;
		m_DronePosition += Vector3(cosf(m_Rotation.Y), 0.0f, -sinf(m_Rotation.Y)) * inputManager->GetAxisLeft().Y * 0.01f;


		m_Position += ((playerPosition + m_DronePosition) - m_Position) * 0.005f;


		m_Target = m_Position 
			+ Vector3(cosf(m_Rotation.Y) * cosf(m_Rotation.X), 
					sinf(m_Rotation.X), 
					-sinf(m_Rotation.Y) * cosf(m_Rotation.X));

	}


/*
	if (changeViewMode)
	{
		PreDraw();
	}
*/

}



Vector2 g_Halton[] =
{
	{0.000000f, 0.000000f},
	{0.500000f, 0.333333f},
	{0.250000f, 0.666667f},
	{0.750000f, 0.111111f},
	{0.125000f, 0.444444f},
	{0.625000f, 0.777778f},
	{0.375000f, 0.222222f},
	{0.875000f, 0.555556f},
	{0.062500f, 0.888889f},
	{0.562500f, 0.037037f},
};


float Halton(int index, int radix)
{
	float result = 0.0f;
	float fraction = 1.0f / float(radix);


	while (index > 0)
	{
		result += float(index % radix) * fraction;

		index /= radix;
		fraction /= float(radix);
	}

	return result;
}



void CarCamera::PreDraw()
{
	float ox = (float)rand() / RAND_MAX * 0.001f;
	float oy = (float)rand() / RAND_MAX * 0.001f;
	float oz = (float)rand() / RAND_MAX * 0.001f;



	m_OldViewMatrix = m_ViewMatrix;
	m_ViewMatrix = Matrix44::LookAt(m_Position, m_Target, m_Up);




	m_DrawPosition = m_Position;

	m_OldProjectionMatrix = m_ProjectionMatrix;


	float width = (float)RenderManager::GetInstance()->GetBackBufferWidth();
	float height = (float)RenderManager::GetInstance()->GetBackBufferHeight();

	float sswidth = (float)RenderManager::GetInstance()->GetSSBufferWidth();
	float ssheight = (float)RenderManager::GetInstance()->GetSSBufferHeight();

	float fov = atan2(24.0f, m_FocalLength);
	float aspect = (float)width / height;
	m_ProjectionMatrix = Matrix44::PerspectiveFov(fov, aspect, 0.5f, 800.0f);



	float dx{}, dy{};


	switch (m_AntiAliasng)
	{
		case 0://None
		{

			break;
		}

		case 1://FXAA
		{

			break;
		}



		case 2://TAA
		{
			static int haltonIndex;
			haltonIndex = (haltonIndex + 1) % 32;
			m_TaaFrame = haltonIndex;

			dx = (Halton(haltonIndex, 2) - 0.5f) * 2.0f / sswidth;
			dy = (Halton(haltonIndex, 3) - 0.5f) * 2.0f / ssheight;

			break;
		}


		case 3://TSR
		{
			static int haltonIndex;
			haltonIndex = (haltonIndex + 1) % 32;

			dx = (Halton(haltonIndex / 4, 2) - 0.5f) * 2.0f / width;
			dy = (Halton(haltonIndex / 4, 3) - 0.5f) * 2.0f / height;

			m_TaaFrame = (m_TaaFrame + 1) % 4;
			dx += -(m_TaaFrame % 2 - 0.5f) * 2.0f / width;
			dy += (m_TaaFrame / 2 - 0.5f) * 2.0f / height;

			break;
		}


		default:
			break;
	}



	m_ProjectionMatrix *= Matrix44::TranslateXYZ(dx, dy, 0.0f);

	
}




void CarCamera::Draw()
{
	GameManager* gameManager = GameManager::GetInstance();
	GameScene* scene = (GameScene*)gameManager->GetScene();
	Car* car = scene->GetGameObject<Car>();
	Vector3 playerPosition = car->GetPosition();




	CAMERA_CONSTANT constant{};
	constant.CameraPosition = { m_DrawPosition.X, m_DrawPosition.Y, m_DrawPosition.Z, 0.0f };	

	constant.Exposure = std::powf(2.0f, -m_Exposure) * 10000.0;//Luminance unit in shader is 1/10000 nit
	constant.WhiteBalance = m_WhiteBalance;
	constant.Gamma = m_Gamma;
	constant.AutoExposure = m_AutoExposure;

	constant.LensFlare = m_LensFlare;

	constant.Vignette = m_Vignette;

	constant.FocalLength = m_FocalLength / 1000.0f;
	constant.FocalDistance = (playerPosition - m_Position).Length();
	constant.FocalBlur = m_FocalBlur;

	constant.MotionBlur = m_MotionBlur;
	constant.MotionBlurCount = m_MotionBlurCount;

	constant.AntiAliasing = m_AntiAliasng;
	constant.TemporalRatio = m_TemporalRatio;
	constant.TemporalFrame = m_TaaFrame;

	constant.SSBufferSize.X = (float)RenderManager::GetInstance()->GetSSBufferWidth();
	constant.SSBufferSize.Y = (float)RenderManager::GetInstance()->GetSSBufferHeight();
	//constant.SSRatio = RenderManager::GetInstance()->GetSSRatio();

	RenderManager::GetInstance()->SetConstant(1, &constant, sizeof(constant));


	//m_TSSCount++;

	//if (m_TSSCount > 64)
	//	m_TSSCount = 64;


}



void CarCamera::DrawDebug()
{
	ImGui::Begin("Camera");

	//ImGui::DragFloat3("Rotation", (float*)&m_Rotation, 0.01f);
	//ImGui::SliderFloat("Distance", &m_Distance, 1.0f, 10.0f, "%.1f m");
	ImGui::SliderFloat("FocalLength", &m_FocalLength, 10.0f, 200.0f, "%.0f mm");
	ImGui::SliderFloat("FocalBlur", &m_FocalBlur, 0.0f, 1000.0f, "%.0f");

	ImGui::Checkbox("AutoExposure", &m_AutoExposure);
	if(m_AutoExposure)
		ImGui::SliderFloat("Compensation", &m_Exposure, -5.0f, 5.0f, "%.2f EV");
	else
		ImGui::SliderFloat("Exposure", &m_Exposure, 1.0f, 20.0f, "%.2f EV");

	ImGui::SliderFloat("WhiteBalance", &m_WhiteBalance, 1000.0f, 10000.0f, "%.0f K");
	ImGui::SliderFloat("Gamma", &m_Gamma, 1.0f, 3.0f, "%.2f");
	ImGui::SliderFloat("LensFlare", &m_LensFlare, 0.0f, 1.0f, "%.1f");
	ImGui::SliderFloat("MotionBlur", &m_MotionBlur, 0.0f, 2.0f, "%.1f");
	ImGui::SliderInt("MotionBlurCount", &m_MotionBlurCount, 1, 10);
	ImGui::SliderFloat("Vignette", &m_Vignette, 0.0f, 1.0f, "%.2f");
	//ImGui::Checkbox("ACES Film", &m_ACESFilmEnable);
	//ImGui::Checkbox("FXAA", &m_FXAAEnable);

	const char* items[] = { "None", "FXAA", "TAA", "TSR" };
	ImGui::Combo("AntiAliasing", &m_AntiAliasng, items, IM_ARRAYSIZE(items));
	ImGui::SliderFloat("TemporalRatio", &m_TemporalRatio, 0.0f, 1.0f, "%.2f");



	ImGui::End();

}

void CarCamera::SetLoacation(Field* CameraField)
{
	Model* model = CameraField->GetModel();
	std::vector<Vector3>* cameraPositionList = model->GetPositionList();

	for (auto cameraPosition : *cameraPositionList)
	{
		m_Location.push_back(cameraPosition);
	}

}




