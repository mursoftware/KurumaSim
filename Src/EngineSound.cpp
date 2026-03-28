
#include "Pch.h"
#include "EngineSound.h"
#include <cmath>
#include <chrono>
#include <complex>


const unsigned char roseusColormap[256][3] = {
    { 1,   1,   1 },
    { 1,   2,   2 },
    { 2,   2,   2 },
    { 2,   3,   3 },
    { 2,   3,   4 },
    { 2,   4,   5 },
    { 2,   5,   6 },
    { 3,   6,   7 },
    { 3,   7,   8 },
    { 3,   8,  10 },
    { 3,   9,  12 },
    { 3,  10,  14 },
    { 3,  12,  16 },
    { 3,  13,  17 },
    { 3,  14,  19 },
    { 2,  15,  21 },
    { 2,  16,  23 },
    { 2,  17,  25 },
    { 2,  18,  27 },
    { 2,  19,  30 },
    { 1,  20,  32 },
    { 1,  21,  34 },
    { 1,  22,  36 },
    { 1,  23,  38 },
    { 1,  24,  40 },
    { 0,  25,  43 },
    { 0,  26,  45 },
    { 0,  27,  47 },
    { 0,  27,  50 },
    { 0,  28,  52 },
    { 0,  29,  54 },
    { 0,  30,  57 },
    { 0,  30,  59 },
    { 1,  31,  62 },
    { 1,  32,  64 },
    { 1,  32,  67 },
    { 2,  33,  69 },
    { 3,  33,  72 },
    { 4,  34,  74 },
    { 5,  35,  77 },
    { 6,  35,  79 },
    { 8,  35,  82 },
    { 9,  36,  84 },
    { 11,  36,  86 },
    { 13,  37,  89 },
    { 15,  37,  91 },
    { 17,  37,  94 },
    { 19,  37,  96 },
    { 21,  38,  99 },
    { 23,  38, 101 },
    { 25,  38, 104 },
    { 27,  38, 106 },
    { 29,  38, 108 },
    { 31,  38, 111 },
    { 33,  38, 113 },
    { 35,  38, 115 },
    { 38,  38, 118 },
    { 40,  38, 120 },
    { 42,  38, 122 },
    { 44,  38, 124 },
    { 46,  38, 126 },
    { 49,  38, 128 },
    { 51,  38, 130 },
    { 53,  37, 132 },
    { 55,  37, 134 },
    { 58,  37, 136 },
    { 60,  36, 138 },
    { 62,  36, 139 },
    { 65,  36, 141 },
    { 67,  35, 143 },
    { 69,  35, 144 },
    { 72,  35, 146 },
    { 74,  34, 147 },
    { 76,  34, 149 },
    { 79,  33, 150 },
    { 81,  33, 151 },
    { 84,  32, 152 },
    { 86,  32, 153 },
    { 88,  31, 154 },
    { 91,  31, 155 },
    { 93,  30, 156 },
    { 95,  29, 157 },
    { 98,  29, 158 },
    { 100,  28, 159 },
    { 103,  28, 159 },
    { 105,  27, 160 },
    { 107,  27, 160 },
    { 110,  26, 161 },
    { 112,  26, 161 },
    { 114,  25, 161 },
    { 117,  25, 162 },
    { 119,  24, 162 },
    { 121,  24, 162 },
    { 124,  23, 162 },
    { 126,  23, 162 },
    { 128,  23, 162 },
    { 131,  22, 161 },
    { 133,  22, 161 },
    { 135,  22, 161 },
    { 137,  22, 161 },
    { 140,  22, 160 },
    { 142,  22, 160 },
    { 144,  22, 159 },
    { 146,  22, 159 },
    { 148,  22, 158 },
    { 150,  22, 157 },
    { 153,  22, 157 },
    { 155,  23, 156 },
    { 157,  23, 155 },
    { 159,  23, 154 },
    { 161,  24, 153 },
    { 163,  24, 152 },
    { 165,  25, 151 },
    { 167,  26, 150 },
    { 169,  26, 149 },
    { 171,  27, 148 },
    { 173,  28, 147 },
    { 175,  29, 146 },
    { 177,  29, 145 },
    { 179,  30, 144 },
    { 181,  31, 142 },
    { 183,  32, 141 },
    { 184,  33, 140 },
    { 186,  34, 139 },
    { 188,  35, 137 },
    { 190,  36, 136 },
    { 192,  37, 135 },
    { 193,  39, 133 },
    { 195,  40, 132 },
    { 197,  41, 130 },
    { 198,  42, 129 },
    { 200,  43, 128 },
    { 202,  45, 126 },
    { 203,  46, 125 },
    { 205,  47, 123 },
    { 206,  48, 122 },
    { 208,  50, 120 },
    { 209,  51, 119 },
    { 211,  52, 117 },
    { 212,  54, 116 },
    { 214,  55, 114 },
    { 215,  57, 113 },
    { 217,  58, 111 },
    { 218,  60, 110 },
    { 219,  61, 109 },
    { 221,  63, 107 },
    { 222,  64, 106 },
    { 223,  66, 104 },
    { 225,  67, 103 },
    { 226,  69, 101 },
    { 227,  70, 100 },
    { 228,  72,  99 },
    { 229,  73,  97 },
    { 230,  75,  96 },
    { 231,  77,  94 },
    { 233,  78,  93 },
    { 234,  80,  92 },
    { 235,  82,  91 },
    { 236,  83,  89 },
    { 237,  85,  88 },
    { 237,  87,  87 },
    { 238,  89,  86 },
    { 239,  90,  84 },
    { 240,  92,  83 },
    { 241,  94,  82 },
    { 242,  96,  81 },
    { 242,  97,  80 },
    { 243,  99,  79 },
    { 244, 101,  78 },
    { 245, 103,  77 },
    { 245, 105,  76 },
    { 246, 107,  75 },
    { 246, 108,  74 },
    { 247, 110,  74 },
    { 248, 112,  73 },
    { 248, 114,  72 },
    { 248, 116,  72 },
    { 249, 118,  71 },
    { 249, 120,  71 },
    { 250, 122,  70 },
    { 250, 124,  70 },
    { 250, 126,  70 },
    { 251, 128,  70 },
    { 251, 130,  69 },
    { 251, 132,  70 },
    { 251, 134,  70 },
    { 251, 136,  70 },
    { 252, 138,  70 },
    { 252, 140,  70 },
    { 252, 142,  71 },
    { 252, 144,  72 },
    { 252, 146,  72 },
    { 252, 148,  73 },
    { 252, 150,  74 },
    { 251, 152,  75 },
    { 251, 154,  76 },
    { 251, 156,  77 },
    { 251, 158,  78 },
    { 251, 160,  80 },
    { 251, 162,  81 },
    { 250, 164,  83 },
    { 250, 166,  85 },
    { 250, 168,  87 },
    { 249, 170,  88 },
    { 249, 172,  90 },
    { 248, 174,  93 },
    { 248, 176,  95 },
    { 248, 178,  97 },
    { 247, 180,  99 },
    { 247, 182, 102 },
    { 246, 184, 104 },
    { 246, 186, 107 },
    { 245, 188, 110 },
    { 244, 190, 112 },
    { 244, 192, 115 },
    { 243, 194, 118 },
    { 243, 195, 121 },
    { 242, 197, 124 },
    { 242, 199, 127 },
    { 241, 201, 131 },
    { 240, 203, 134 },
    { 240, 205, 137 },
    { 239, 207, 140 },
    { 239, 208, 144 },
    { 238, 210, 147 },
    { 238, 212, 151 },
    { 237, 213, 154 },
    { 237, 215, 158 },
    { 236, 217, 161 },
    { 236, 218, 165 },
    { 236, 220, 169 },
    { 236, 222, 172 },
    { 235, 223, 176 },
    { 235, 225, 180 },
    { 235, 226, 183 },
    { 235, 228, 187 },
    { 235, 229, 191 },
    { 235, 230, 194 },
    { 236, 232, 198 },
    { 236, 233, 201 },
    { 236, 234, 205 },
    { 237, 236, 208 },
    { 237, 237, 212 },
    { 238, 238, 215 },
    { 239, 239, 219 },
    { 240, 240, 222 },
    { 241, 242, 225 },
    { 242, 243, 228 },
    { 243, 244, 231 },
    { 244, 245, 234 },
    { 246, 246, 237 },
    { 247, 247, 240 },
    { 249, 248, 242 },
    { 251, 249, 245 },
    { 253, 250, 247 },
    { 254, 251, 249 },
};






using Complex = std::complex<float>;

void BitReverseCopy(const std::vector<Complex>& Src, std::vector<Complex>& Dst)
{
    int n = (int)Src.size();
    int levels = 0;
    for (int temp = n; temp > 1; temp >>= 1) levels++;
    for (int i = 0; i < n; i++)
    {
        int x = i, y = 0;
        for (int j = 0; j < levels; j++)
        {
            y = (y << 1) | (x & 1);
            x >>= 1;
        }
        Dst[y] = Src[i];
    }
}

void PerformFFT(std::vector<Complex>& Data)
{
    int n = (int)Data.size();
    std::vector<Complex> buffer(n);
    BitReverseCopy(Data, buffer);
    Data = buffer;
    for (int size = 2; size <= n; size *= 2)
    {
        int halfSize = size / 2;
        float angle = -2.0f * PI / size;
        Complex wStep(cosf(angle), sinf(angle));
        for (int i = 0; i < n; i += size)
        {
            Complex w(1.0f, 0.0f);
            for (int j = 0; j < halfSize; j++)
            {
                Complex u = Data[i + j];
                Complex v = Data[i + j + halfSize] * w;
                Data[i + j] = u + v;
                Data[i + j + halfSize] = u - v;
                w *= wStep;
            }
        }
    }
}





void Exhaust::Start(float OpenTime, float Throttle, float NoiseRatio)
{
    m_Active = true;
    m_OpenTime = OpenTime;
    m_Throttle = Throttle;
    m_NoiseRatio = NoiseRatio;
    m_Time = 0.0f;
}

float Exhaust::Compute()
{
    if (m_Time >= m_OpenTime)
    {
        m_Active = false;
        return 0.0f;
    }
    float t = m_Time / m_OpenTime;
    //float pressure = t * (1.0f - t) * (0.5f + m_Throttle*0.5f) / m_OpenTime * 0.01f;
    float pressure = 20.0f * t * std::exp(-8.0f * t) * (0.5f + m_Throttle*0.5f) / m_OpenTime * 0.02f;

    //pressure *= pressure;
    //float noise = GenerateRedNoise() * pressure * m_NoiseRatio * 0.0f;
	float noise = (rand() / (float)RAND_MAX - 0.5f) * pressure * m_NoiseRatio * 1.0f;
    m_Time += 1.0f / SAMPLE_RATE;
    return pressure + noise;
}





ExhaustPipe::ExhaustPipe(bool enabled, float LengthMeters, float Reflection, float Cutoff, float Mix)
{
	m_Enabled = enabled;
    m_LengthMeters = LengthMeters;
    m_LengthInSamples = (LengthMeters * 2.0f / m_SoundSpeed) * SAMPLE_RATE;
    m_DelayLine.resize((size_t)m_LengthInSamples + 1, 0.0f);

    m_Reflection = Reflection;
    m_Cutoff = Cutoff;
    m_Mix = Mix;
}

float ExhaustPipe::Process(float Input, float soundSpeed)
{
    if (!m_Enabled) return Input;
    if (m_DelayLine.empty()) return Input;

    m_SoundSpeed = soundSpeed;
    m_LengthInSamples = (m_LengthMeters * 2.0f / soundSpeed) * SAMPLE_RATE;

    int readIndex = (m_WriteIndex - (int)m_LengthInSamples + (int)m_DelayLine.size()) % (int)m_DelayLine.size();
    float delayedSample = m_DelayLine[readIndex];
    m_LpState = m_LpState + m_Cutoff * (1.0f + soundSpeed * 0.002f) * (delayedSample - m_LpState);
    float feedback = m_LpState * m_Reflection;
    if (!std::isfinite(feedback)) feedback = 0.0f;
    if (!std::isfinite(m_LpState)) m_LpState = 0.0f;
    m_DelayLine[m_WriteIndex] = Input + feedback;
    m_WriteIndex = (m_WriteIndex + 1) % (int)m_DelayLine.size();
    return Input * (1.0f - m_Mix) + feedback * m_Mix;
}

void ExhaustPipe::DrawDebugUI(const char* Name)
{
    if (ImGui::CollapsingHeader(Name))
    {
        ImGui::Checkbox((std::string("Enabled##") + Name).c_str(), &m_Enabled);

        bool change = ImGui::SliderFloat((std::string("Length##") + Name).c_str(), &m_LengthMeters, 0.05f, 4.0f);
        if (change)
        {
            m_LengthInSamples = (m_LengthMeters * 2.0f / m_SoundSpeed) * SAMPLE_RATE;
            m_DelayLine.resize((size_t)m_LengthInSamples + 1, 0.0f);
            m_WriteIndex = 0;
        }

        ImGui::SliderFloat((std::string("Reflection##") + Name).c_str(), &m_Reflection, -1.0f, 1.0f);
        ImGui::SliderFloat((std::string("Cutoff##") + Name).c_str(), &m_Cutoff, 0.001f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat((std::string("Mix##") + Name).c_str(), &m_Mix, 0.0f, 1.0f);
    }
}












EngineSound::EngineSound()
{
    m_VisBuffer.resize(BUFFER_SAMPLES, 0);

    float dB = 20.0f * log10f(0.0f + 1e-12f);
    m_Spectrogram.resize(SPEC_WIDTH * BUFFER_SAMPLES, dB);
}

bool EngineSound::Initialize(IXAudio2* XAudio2)
{
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, SAMPLE_RATE, SAMPLE_RATE * 2, 2, 16, 0 };
    if (FAILED(XAudio2->CreateSourceVoice(&m_PSourceVoice, &wfx))) return false;

    m_PSourceVoice->Start();

    for (int i = 0; i < NUM_BUFFERS; i++)
        m_AudioMemory[i].resize(BUFFER_SAMPLES);

    m_AudioThread = new std::thread(&EngineSound::Run, this);

    return true;
}

void EngineSound::SetState(float Rpm, float Throttle)
{
    m_Rpm = Rpm;
    m_Throttle = Throttle;
}

void EngineSound::Run()
{
    while (m_Running)
    {
        if (!m_PSourceVoice)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        XAUDIO2_VOICE_STATE state;
        m_PSourceVoice->GetState(&state);

        while (state.BuffersQueued < NUM_BUFFERS && m_Running)
        {
            float rpmStep = (m_Rpm - m_PrevRpm) / BUFFER_SAMPLES;
            float processingRpm = m_PrevRpm;


            for (int i = 0; i < BUFFER_SAMPLES; i++)
            {
                processingRpm += rpmStep;

                float sample = 0.0f;

                m_CylTime += 1.0 / SAMPLE_RATE;

                if (m_CylTime > 1.0 / (processingRpm / 60.0) * 2.0 / m_NumCylinders + m_PhaseOffset[m_CylStep] / (800.0 + processingRpm * 0.0f))
                {
                    for (auto& e : m_Exhaust)
                    {
                        if (!e.IsActive())
                        {
                            float valveOpenTime = 1.0f / (processingRpm / 60.0f) * 2.0f / 4.0f * (m_ValveOpenRatio + processingRpm * m_VvtRatio * 0.0000f);
                            e.Start(valveOpenTime, m_Throttle * (1.0f + (rand() / (float)RAND_MAX - 0.5f) * 0.1f), m_NoiseRatio);
                            break;
                        }
                    }

                    m_CylTime -= 1.0 / (processingRpm / 60.0) * 2.0 / m_NumCylinders;

                    m_CylStep++;

                    if (m_CylStep >= m_NumCylinders)
                    {
                        m_CylStep = 0;
                    }
                }

                for (auto& e : m_Exhaust)
                    if (e.IsActive()) sample += e.Compute();

                //sample += (rand() / (float)RAND_MAX - 0.5f) * processingRpm * (0.5f + 0.5f * m_Throttle) * m_NoiseRatio * 0.001f;

				float soundSpeed = 343.0f;// + m_Rpm * 0.01f;
                sample = m_P2.Process(sample, soundSpeed);
                sample = m_P3.Process(sample, soundSpeed);
                sample = m_P1.Process(sample, soundSpeed);

                m_AudioMemory[m_BufferIndex][i] = (short)(std::tanh(sample * m_Gain) * 32767.0f);
            }



            m_PrevRpm = processingRpm;
            m_VisBuffer = m_AudioMemory[m_BufferIndex];

            XAUDIO2_BUFFER buf = {};
            buf.AudioBytes = BUFFER_SAMPLES * sizeof(short);
            buf.pAudioData = (BYTE*)m_AudioMemory[m_BufferIndex].data();
            m_PSourceVoice->SubmitSourceBuffer(&buf);
            m_BufferIndex = (m_BufferIndex + 1) % NUM_BUFFERS;
            m_PSourceVoice->GetState(&state);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void EngineSound::DrawDebug()
{
    ImGui::Begin("Engine Sound Parameter");

    ImGui::SliderFloat("Gain", &m_Gain, 0.0f, 10.0f);

    ImGui::SliderInt("Cylinders", &m_NumCylinders, 1, MAX_CYLINDER);
    ImGui::SliderFloat("ValveOpenRatio", &m_ValveOpenRatio, 0.0f, 2.0f);
    ImGui::SliderFloat("VVT", &m_VvtRatio, 0.0f, 1.0f);
    ImGui::SliderFloat("Noise", &m_NoiseRatio, 0.0f, 1.0f);


    ImGui::SliderFloat("PhaseNoise", &m_PhaseNoise, 0.0f, 1.0f);


    if (ImGui::CollapsingHeader("Exhaust Manifold"))
    {
        for(int i = 0; i < m_NumCylinders; i++)
            ImGui::SliderFloat(("Length" + std::to_string(i)).c_str(), &m_PhaseOffset[i], 0.0f, 1.0f);
    }

    m_P1.DrawDebugUI("Pipe1");
    m_P2.DrawDebugUI("Pipe2");
    m_P3.DrawDebugUI("Pipe3");

    ImGui::End();

    ImGui::Begin("Engine Sound");

    std::vector<short> localBuf = m_VisBuffer;

    if (localBuf.size() == BUFFER_SAMPLES)
    {
        static float wave[BUFFER_SAMPLES];
        static float spec[BUFFER_SAMPLES];
        std::vector<Complex> fftD(BUFFER_SAMPLES);

        for (int i = 0; i < BUFFER_SAMPLES; i++)
        {
            wave[i] = (float)localBuf[i] / 32768.0f;
            fftD[i] = Complex(wave[i] * (0.5f * (1.0f - cosf(2.0f * PI * i / (BUFFER_SAMPLES - 1)))), 0.0f);
        }

        PerformFFT(fftD);

        for (int i = 0; i < BUFFER_SAMPLES / 2; i++)
            spec[i] = sqrtf(std::abs(fftD[i]));

        for (int y = 0; y < BUFFER_SAMPLES; y++)
        {
            float amp = spec[y];
            float dB = 20.0f * log10f(amp + 1e-12f);
			m_Spectrogram[y * SPEC_WIDTH + m_SpecWritePos] = dB;
        }

        m_SpecWritePos = (m_SpecWritePos + 1) % SPEC_WIDTH;


        ImGui::PlotLines("Waveform", wave, BUFFER_SAMPLES / 4, 0, NULL, -1.0f, 1.0f, ImVec2(0, 100));
        ImGui::PlotHistogram("Spectrum", spec, BUFFER_SAMPLES / 2 / 4, 0, NULL, 0.0f, 10.0f, ImVec2(0, 100));
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Audio Data Available");
        ImGui::Text("Buffer Size: %d", (int)localBuf.size());
    }




    ImVec2 size(256, 256);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();



    float freqsToLabel[] = { 100.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };

    auto hzToMel = [](float hz)->float { return 2595.0f * log10f(1.0f + hz / 700.0f); };
    auto melToHz = [](float mel)->float { return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f); };
    float melMax = hzToMel((float)SAMPLE_RATE * 0.5f);

    for (float fTarget : freqsToLabel)
    {
        float mel = hzToMel(fTarget);
        float displayRowF = mel / melMax * (float)(256 - 1);

        if (displayRowF >= 0.0f && displayRowF < 256)
        {
            float py = p.y + (float)(256 - 1 - displayRowF) * (size.y / 256);

            dl->AddLine(ImVec2(p.x, py), ImVec2(p.x + size.x, py), ImColor(255, 255, 255, 100));

            char buf[32];
            if (fTarget >= 1000.0f) sprintf(buf, "%.1fkHz", fTarget / 1000.0f);
            else sprintf(buf, "%.0fHz", fTarget);

            dl->AddText(ImVec2(p.x + size.x + 5, py - 7), ImColor(255, 255, 255, 255), buf);
        }
    }



    for (int x = 0; x < SPEC_WIDTH; x++)
    {
        int xx = (m_SpecWritePos + x) % SPEC_WIDTH;
        for (int y = 0; y < 256; y++)
        {
            float mel = (float)y / (float)(256 - 1) * melMax;
            float hz = melToHz(mel);
            float binF = hz * (float)BUFFER_SAMPLES / (float)SAMPLE_RATE;
            int srcY = (int)roundf(binF);
            if (srcY < 0) srcY = 0;
            if (srcY >= BUFFER_SAMPLES) srcY = BUFFER_SAMPLES - 1;

            float v = (m_Spectrogram[srcY * SPEC_WIDTH + xx] + 20.0f) / 40.0f;
            v = max(0.0f, min(v, 1.0f));

            unsigned char r = roseusColormap[(unsigned char)(v * 255)][0];
            unsigned char g = roseusColormap[(unsigned char)(v * 255)][1];
            unsigned char b = roseusColormap[(unsigned char)(v * 255)][2];

            ImU32 col = ImColor(r, g, b);

            float px = p.x + (float)x * (size.x / SPEC_WIDTH);
            float py = p.y + (float)(256 - 1 - y) * (size.y / 256);

            dl->AddRectFilled(
                ImVec2(px, py),
                ImVec2(px + size.x / SPEC_WIDTH, py + size.y / 256),
                col
            );
        }
    }

    ImGui::Dummy(size);

    ImGui::End();

}

void EngineSound::Uninitialize()
{
    m_Running = false;

    if (m_AudioThread)
    {
        m_AudioThread->join();
        delete m_AudioThread;
        m_AudioThread = nullptr;
    }

    if (m_PSourceVoice)
    {
        m_PSourceVoice->Stop();
        m_PSourceVoice->DestroyVoice();
        m_PSourceVoice = nullptr;
    }
}


void EngineSound::SetVolume(float Volume)
{
    if (m_PSourceVoice)
		m_PSourceVoice->SetVolume(Volume);
}

void EngineSound::SetPitch(float Pitch)
{
    if (m_PSourceVoice)
        m_PSourceVoice->SetFrequencyRatio(Pitch);
}
