#pragma once


#include <xaudio2.h>

static const int SAMPLE_RATE = 44100;
static const int BUFFER_SAMPLES = 2048;
static const int NUM_BUFFERS = 2;
static const int MAX_CYLINDER = 12;
static const int SPEC_WIDTH = 256;   // 時間方向（横）
//static const int SPEC_HEIGHT = 256;  // 周波数方向（縦）


class Exhaust
{
    bool m_Active = false;
    float m_Time = 0.0f;
    float m_OpenTime = 0.0f;
    float m_Throttle = 0.0f;
    float m_NoiseRatio = 0.2f;

public:
    bool IsActive() const { return m_Active; }
    void Start(float OpenTime, float Throttle, float NoiseRatio);
    float Compute();
};

class ExhaustPipe
{
    bool m_Enabled = true;
    std::vector<float> m_DelayLine;
    int m_WriteIndex = 0;
    float m_LengthMeters = 0.5f;
    float m_LengthInSamples = 0.0f;
    float m_Reflection = -0.9f;
    float m_LpState = 0.0f;
    float m_Cutoff = 1.0f;
    float m_Mix = 0.5f;
    float m_SoundSpeed = 343.0f;

public:
    ExhaustPipe(float LengthMeters, float Reflection, float Cutoff, float Mix);

    float Process(float Input, float soundSpeed);
    void DrawDebugUI(const char* Name);
};

class EngineSound
{
	IXAudio2SourceVoice* m_PSourceVoice = nullptr;
	Exhaust m_Exhaust[MAX_CYLINDER];

	float m_Rpm = 800.0f;
	float m_Throttle = 0.1f;

	int m_NumCylinders = 6;

	float m_ValveOpenRatio = 0.5f;
	float m_VvtRatio = 0.0f;
    float m_PhaseOffset[MAX_CYLINDER] = {0.3f, 0.25f, 0.1f, 0.35f, 0.2f, 0.15f};
	float m_PhaseNoise = 0.0f;
	float m_Gain = 2.0f;

	float m_Phase = 0.0;
	double m_CylTime = 0.0;
	int m_CylStep = 0;
	float m_Jitter = 0.0;
	float m_PrevRpm = 800.0f;
	float m_NoiseRatio = 0.2f;

	std::vector<short> m_AudioMemory[NUM_BUFFERS];
	int m_BufferIndex = 0;
	std::atomic<bool> m_Running = true;

	ExhaustPipe m_P1{ 0.9f, -0.8f, 0.05f, 0.8f };
	ExhaustPipe m_P2{ 1.8f, -0.8f, 0.01f, 0.8f };
    ExhaustPipe m_P3{ 0.6f, -0.8f, 0.05f, 0.8f };

	std::vector<short> m_VisBuffer;
	std::thread* m_AudioThread = nullptr;

	std::vector<float> m_Spectrogram;
	int m_SpecWritePos = 0;

	void Run();

public:
    EngineSound();

    bool Initialize(IXAudio2* XAudio2);
    void SetState(float Rpm, float Throttle);
    void SetVolume(float Volume);
    void SetPitch(float Pitch);
    void DrawDebug();
    void Uninitialize();
};