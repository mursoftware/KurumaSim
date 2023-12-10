#pragma once


class DriveTrain
{

protected:

	DriveTrain* m_InputDriveTrain1{};
	DriveTrain* m_InputDriveTrain2{};
	DriveTrain* m_OutputDriveTrain1{};
	DriveTrain* m_OutputDriveTrain2{};


public:


	void SetDrivetrain(DriveTrain* InputDrivetrain1, DriveTrain* InputDrivetrain2, DriveTrain* OutputDrivetrain1, DriveTrain* OutputDrivetrain2)
	{
		m_InputDriveTrain1 = InputDrivetrain1;
		m_InputDriveTrain2 = InputDrivetrain2;
		m_OutputDriveTrain1 = OutputDrivetrain1;
		m_OutputDriveTrain2 = OutputDrivetrain2;
	}

	virtual float GetInputTorque1() { return 0.0f; }
	virtual float GetInputAngularSpeed1() { return 0.0f; }

	virtual float GetInputTorque2() { return 0.0f; }
	virtual float GetInputAngularSpeed2() { return 0.0f; }

	virtual float GetOutputTorque1() { return 0.0f; }
	virtual float GetOutputAngularSpeed1() { return 0.0f; }

	virtual float GetOutputTorque2() { return 0.0f; }
	virtual float GetOutputAngularSpeed2() { return 0.0f; }

	virtual void  SetOutputAngularSpeed1(float AngularSpeed) {}

};