
#include "Pch.h"
#include "SoundManager.h"


#include <mmsystem.h>
#pragma comment ( lib, "winmm.lib" )

#include <xaudio2fx.h>

SoundManager* SoundManager::m_Instance{};

std::unordered_map<std::string, SOUND*>	SoundManager::m_BufferList;
IXAudio2*				SoundManager::m_Xaudio{};
IXAudio2MasteringVoice* SoundManager::m_MasteringVoice{};

IUnknown*				SoundManager::m_Reverb{};





SoundManager::SoundManager()
{
	m_Instance = this;



	// COM������
	(void)CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// XAudio����
	XAudio2Create( &m_Xaudio, 0 );

	// �}�X�^�����O�{�C�X����
	m_Xaudio->CreateMasteringVoice( &m_MasteringVoice );




	//m_MasteringVoice->SetVolume( 0.0f );



	// ���o�[�u�G�t�F�N�g�ݒ�
	//CreateFX(__uuidof(::FXReverb), &m_Reverb);
	XAudio2CreateReverb(&m_Reverb);
	assert(m_Reverb);

	XAUDIO2_EFFECT_DESCRIPTOR desc;
	desc.InitialState = TRUE;
	desc.OutputChannels = 2;      // �o�̓`�����l����
	desc.pEffect = m_Reverb; // �G�t�F�N�g�ւ̃|�C���^

	XAUDIO2_EFFECT_CHAIN chain;
	chain.pEffectDescriptors = &desc; // Descriptor�ւ̃|�C���^�A�����ڑ�����ꍇ�͔z��̐擪
	chain.EffectCount = 1;     // Descriptor����������̂�

	m_MasteringVoice->SetEffectChain(&chain);

/*
	FXREVERB_PARAMETERS param;
	param.Diffusion = 0.9f; // �U�z�ʁi�g�U�ʁH�j
	param.RoomSize = 0.5f;  // �������Ă���{�݂̑傫��������
	m_MasteringVoice->SetEffectParameters(0, &param, sizeof(FXREVERB_PARAMETERS));
*/


/*
	IUnknown* pXAPO;
	HRESULT hr = XAudio2CreateReverb(&pXAPO);


	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 1;
	descriptor.pEffect = pXAPO;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;


	m_MasteringVoice->SetEffectChain(&chain);
*/


	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = 0;
	reverbParameters.ReverbDelay = 0;
	reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParameters.WetDryMix = 30.0f;

	m_MasteringVoice->SetEffectParameters(0, &reverbParameters, sizeof(XAUDIO2FX_REVERB_PARAMETERS));


}



SoundManager::~SoundManager()
{


	//�o�b�t�@�폜
	for( std::pair<std::string, SOUND*> pair : m_BufferList )
	{
		for (int j = 0; j < SOUND_SOURCE_MAX; j++)
		{
			pair.second->SourceVoice[j]->DestroyVoice();
		}

		assert(pair.second->SoundData);
		assert(pair.second);

		delete pair.second->SoundData;
		delete pair.second;
	}

	m_BufferList.clear();



	//m_Reverb->Release();



	m_MasteringVoice->DestroyVoice();

	m_Xaudio->Release();

	CoUninitialize();

}




SOUND* SoundManager::LoadData( const char *FileName )
{


	if (m_BufferList.count(FileName))
		return m_BufferList[FileName];




	//�T�E���h�\���̐���
	SOUND *sound = new SOUND{};

	//�t�@�C�����ݒ�
	strcpy_s( sound->FileName, FileName );

	//�o�b�t�@�쐬
	char path[260]{};
	strcpy( path, "Asset\\" );
	strcat( path, FileName );



	// �T�E���h�f�[�^�Ǎ�
	WAVEFORMATEX wfx{};

	{
		HMMIO hmmio{};
		MMIOINFO mmioinfo{};
		MMCKINFO riffchunkinfo{};
		MMCKINFO datachunkinfo{};
		MMCKINFO mmckinfo{};
		UINT32 buflen{};
		LONG readlen{};


		hmmio = mmioOpen((LPSTR)path, &mmioinfo, MMIO_READ);
		assert(hmmio);

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf{};
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		sound->SoundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)sound->SoundData, buflen);


		sound->Length = readlen;
		sound->PlayLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);
	}

	// �T�E���h�\�[�X����
	for (int j = 0; j < SOUND_SOURCE_MAX; j++)
	{
		m_Xaudio->CreateSourceVoice(&sound->SourceVoice[j], &wfx);
	}


	//���X�g�ǉ�
	m_BufferList[FileName] = sound;


	return sound;
	
}


void SoundManager::Load( const char* FileName )
{
	LoadData( FileName );
}




int SoundManager::Play( const char *FileName, bool Loop, float Volume )
{

	SOUND *sound = LoadData( FileName );


	for( int j = 0; j < SOUND_SOURCE_MAX; j++ )
	{
		XAUDIO2_VOICE_STATE state;

		sound->SourceVoice[j]->GetState( &state );

		if( state.BuffersQueued == 0 )
		{
			// �o�b�t�@�ݒ�
			XAUDIO2_BUFFER bufinfo;

			memset(&bufinfo,0x00,sizeof(bufinfo));
			bufinfo.AudioBytes = sound->Length;
			bufinfo.pAudioData = sound->SoundData;
			bufinfo.PlayBegin = 0;
			bufinfo.PlayLength = sound->PlayLength;

			// ���[�v�ݒ�
			if( Loop )
			{
				bufinfo.LoopBegin = 0;
				bufinfo.LoopLength = sound->PlayLength;
				bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			sound->SourceVoice[j]->SubmitSourceBuffer( &bufinfo, NULL );


			// �Đ�
			sound->SourceVoice[j]->Start();
			sound->SourceVoice[j]->SetVolume( Volume );

			return j;
		}
	}

	assert(0);
	return -1;
}



void SoundManager::Stop( const char *FileName, int Index)
{

	SOUND *sound = m_BufferList[FileName];

	sound->SourceVoice[Index]->Stop();
	sound->SourceVoice[Index]->FlushSourceBuffers();

}



void SoundManager::SetPitch( const char *FileName, int Index, float Pitch )
{
	SOUND *sound = m_BufferList[FileName];

	sound->SourceVoice[Index]->SetFrequencyRatio( Pitch );

}



void SoundManager::SetVolume( const char *FileName, int Index, float Volume )
{
	SOUND *sound = m_BufferList[FileName];

	Volume = min(Volume, 2.0f);
	sound->SourceVoice[Index]->SetVolume( Volume );
}
