#include "Pch.h"
#include "Main.h"
#include "InputManager.h"


//#define USE_LOGITECH_STEERING_WHEEL_LIB
//https://gaming.logicool.co.jp/innovation/developer-lab.html


#ifdef USE_LOGITECH_STEERING_WHEEL_LIB

	#define DIRECTINPUT_VERSION 0x0800
	#include "LogitechSteeringWheelLib.h"
	#pragma comment(lib, "LogitechSteeringWheelLib.lib")

#endif // USE_LOGITECH_STEERING_WHEEL_LIB



InputManager* InputManager::m_Instance{};



InputManager::InputManager()
{
	m_Instance = this;


#ifdef USE_LOGITECH_STEERING_WHEEL_LIB
	bool ret = LogiSteeringInitializeWithWindow(TRUE, GetWindow());
#endif // USE_LOGITECH_STEERING_WHEEL_LIB


	m_TorqureRatio = 0.1f;
}



InputManager::~InputManager()
{
#ifdef USE_LOGITECH_STEERING_WHEEL_LIB
	LogiSteeringShutdown();
#endif // USE_LOGITECH_STEERING_WHEEL_LIB
}




void InputManager::Update()
{
	memcpy(m_OldKeyState, m_KeyState, 256);
	(void)GetKeyboardState(m_KeyState);


	m_OldMousePosition = m_MousePosition;
	GetCursorPos(&m_MousePosition);




	memcpy(m_ButtonOld, m_Button, sizeof(m_Button));


	XINPUT_STATE state{};
	DWORD ret = XInputGetState(0, &state);

	if (ret == ERROR_SUCCESS)
	{
		m_AxisLeft.Y = (float)state.Gamepad.sThumbLY / SHRT_MAX;

		m_AxisRight.X = (float)state.Gamepad.sThumbRX / SHRT_MAX;
		m_AxisRight.Y = (float)state.Gamepad.sThumbRY / SHRT_MAX;

		m_TriggerLeft = (float)state.Gamepad.bLeftTrigger / 255;
		m_TriggerRight = (float)state.Gamepad.bRightTrigger / 255;


		float gamma = 0.5f;
		float stea, gammastea;
		stea = (float)state.Gamepad.sThumbLX / SHRT_MAX;
		gammastea = powf(fabs(stea), 1.0f / gamma);
		if (stea < 0.0f)
			gammastea *= -1.0f;


		if (m_AxisLeft.X < gammastea)
		{
			m_AxisLeft.X += 0.008f;
			if (m_AxisLeft.X > gammastea)
				m_AxisLeft.X = gammastea;
		}
		else if (m_AxisLeft.X > gammastea)
		{
			m_AxisLeft.X -= 0.008f;
			if (m_AxisLeft.X < gammastea)
				m_AxisLeft.X = gammastea;
		}




		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			m_Button[0] = true;
		else
			m_Button[0] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			m_Button[1] = true;
		else
			m_Button[1] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			m_Button[2] = true;
		else
			m_Button[2] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			m_Button[3] = true;
		else
			m_Button[3] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			m_Button[4] = true;
		else
			m_Button[4] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			m_Button[5] = true;
		else
			m_Button[5] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			m_Button[6] = true;
		else
			m_Button[6] = false;


		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
			m_Button[7] = true;
		else
			m_Button[7] = false;




		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			m_Button[8] = true;
		else
			m_Button[8] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			m_Button[9] = true;
		else
			m_Button[9] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			m_Button[10] = true;
		else
			m_Button[10] = false;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			m_Button[11] = true;
		else
			m_Button[11] = false;


	}





	if (m_TorqueCount != 0)
	{
		m_TorqueAverage += (m_Torque / m_TorqueCount - m_TorqueAverage) * 0.1f;
		m_Torque = 0.0f;
		m_TorqueCount = 0;
	}



#ifdef USE_LOGITECH_STEERING_WHEEL_LIB
	if (LogiUpdate())
	{
		DIJOYSTATE2* handle = LogiGetState(0);

		if (handle)
		{

			m_AxisLeft.X = (float)handle->lX / SHRT_MAX;
			m_AxisRight.Y = -(float)handle->rglSlider[0] / SHRT_MAX;


			if (handle->rgbButtons[5] & 0x80 || handle->rgbButtons[13] & 0x80)
				m_Button[4] = true;
			else
				m_Button[4] = false;

			if (handle->rgbButtons[4] & 0x80 || handle->rgbButtons[12] & 0x80)
				m_Button[5] = true;
			else
				m_Button[5] = false;

			if (handle->rgbButtons[9] & 0x80)
				m_Button[7] = true;
			else
				m_Button[7] = false;

			if (handle->rgdwPOV[0] == 0)
				m_Button[8] = true;
			else
				m_Button[8] = false;

			if (handle->rgbButtons[0] & 0x80)
				m_Button[0] = true;
			else
				m_Button[0] = false;

			if (handle->rgbButtons[2] & 0x80)
				m_Button[2] = true;
			else
				m_Button[2] = false;


			//LogiPlayConstantForce(0,10);
			//LogiPlayDamperForce(0, -30);

			LogiPlayConstantForce(0, (int)(m_TorqueAverage / 2.5f * 100.0f * m_TorqureRatio));

		}
	}
#endif // USE_LOGITECH_STEERING_WHEEL_LIB


}



void InputManager::DrawDebug()
{

	//ImGui::Begin("Controller");

	//char buf[32]{};
	//sprintf(buf, "SterlingAngle %.1f", m_AxisLeft.X * 450.0f);
	//ImGui::ProgressBar(m_AxisLeft.X / 2.0f + 0.5f, ImVec2(0, 0), buf);
	//sprintf(buf, "Torque %.0f Nm", -m_TorqueAverage);
	//ImGui::ProgressBar(-m_TorqueAverage / 100.0f + 0.5f, ImVec2(0, 0), buf);

	//ImGui::SliderFloat("TorqueRatio", &m_TorqureRatio, 0.0f, 1.0f, "%.2f");


	//float throttle{}, brake{};
	//if (m_AxisRight.Y > 0.0f)
	//{
	//	throttle = m_AxisRight.Y;
	//	brake = 0.0f;
	//}
	//else
	//{
	//	throttle = 0.0f;
	//	brake = -m_AxisRight.Y;
	//}

	//sprintf(buf, "Throttle %.2f", throttle);
	//ImGui::ProgressBar(throttle, ImVec2(0, 0), buf);
	//sprintf(buf, "Brake %.2f", brake);
	//ImGui::ProgressBar(brake, ImVec2(0, 0), buf);

	//ImGui::End();


}


bool InputManager::GetKeyPress(BYTE KeyCode)
{
	return (m_KeyState[KeyCode] & 0x80);
}

bool InputManager::GetKeyTrigger(BYTE KeyCode)
{
	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
}




POINT InputManager::GetMouseMove()
{
	POINT mouseMove{};

	mouseMove.x = m_MousePosition.x - m_OldMousePosition.x;
	mouseMove.y = m_MousePosition.y - m_OldMousePosition.y;

	return mouseMove;
}




void InputManager::SetTorque(float Torque)
{
	m_Torque += Torque;
	m_TorqueCount++;
}