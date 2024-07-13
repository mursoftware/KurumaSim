
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
IUnknown*				SoundManager::m_EQ{};





SoundManager::SoundManager()
{
	m_Instance = this;



	(void)CoInitializeEx( NULL, COINIT_MULTITHREADED );

	XAudio2Create( &m_Xaudio, 0 );

	m_Xaudio->CreateMasteringVoice( &m_MasteringVoice );




	//m_MasteringVoice->SetVolume( 0.0f );


	XAUDIO2_EFFECT_DESCRIPTOR desc[2];

	//CreateFX(__uuidof(::FXReverb), &m_Reverb);
	XAudio2CreateReverb(&m_Reverb);
	assert(m_Reverb);

	desc[1].InitialState = TRUE;
	desc[1].OutputChannels = 2;
	desc[1].pEffect = m_Reverb;


	CreateFX(__uuidof(::FXEQ), &m_EQ);
	assert(m_EQ);

	desc[0].InitialState = TRUE;
	desc[0].OutputChannels = 2;
	desc[0].pEffect = m_EQ;



	XAUDIO2_EFFECT_CHAIN chain;
	chain.pEffectDescriptors = desc;
	chain.EffectCount = 2;

	//m_MasteringVoice->SetEffectChain(&chain);


	

	FXEQ_PARAMETERS eqPrameters{};
	eqPrameters.FrequencyCenter0 = 10.0f; 
	eqPrameters.Gain0 = 1.0f;             
	eqPrameters.Bandwidth0 = 2.0f;        

	eqPrameters.FrequencyCenter1 = 160.0f;
	eqPrameters.Gain1 = 1.0f;             
	eqPrameters.Bandwidth1 = 0.0f;        

	eqPrameters.FrequencyCenter2 = 350.0f;
	eqPrameters.Gain2 = 1.0f;             
	eqPrameters.Bandwidth2 = 0.0f;        

	eqPrameters.FrequencyCenter3 = 22000.0f;
	eqPrameters.Gain3 = 1.0f;             
	eqPrameters.Bandwidth3 = 3.0f;        

	//m_MasteringVoice->SetEffectParameters(0, &eqPrameters, sizeof(FXEQ_PARAMETERS));




	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = 1;
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
	reverbParameters.WetDryMix = 0.0f;

	//m_MasteringVoice->SetEffectParameters(1, &reverbParameters, sizeof(XAUDIO2FX_REVERB_PARAMETERS));



}



SoundManager::~SoundManager()
{


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




	SOUND *sound = new SOUND{};

	strcpy_s( sound->FileName, FileName );

	char path[260]{};
	strcpy( path, "Asset\\" );
	strcat( path, FileName );



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



	for (int j = 0; j < SOUND_SOURCE_MAX; j++)
	{
		m_Xaudio->CreateSourceVoice(&sound->SourceVoice[j], &wfx);
	}


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
			XAUDIO2_BUFFER bufinfo;

			memset(&bufinfo,0x00,sizeof(bufinfo));
			bufinfo.AudioBytes = sound->Length;
			bufinfo.pAudioData = sound->SoundData;
			bufinfo.PlayBegin = 0;
			bufinfo.PlayLength = sound->PlayLength;

			if( Loop )
			{
				bufinfo.LoopBegin = 0;
				bufinfo.LoopLength = sound->PlayLength;
				bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			sound->SourceVoice[j]->SubmitSourceBuffer( &bufinfo, NULL );


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




void SoundManager::SetEQ(const char* FileName, FXEQ_PARAMETERS EqPrametersLo, FXEQ_PARAMETERS EqPrametersHi)
{

	SOUND* sound = LoadData(FileName);


	CreateFX(__uuidof(::FXEQ), &sound->EffectHi);
	assert(sound->EffectHi);

	CreateFX(__uuidof(::FXEQ), &sound->EffectLo);
	assert(sound->EffectLo);


	XAUDIO2_EFFECT_DESCRIPTOR desc[2];
	desc[0].InitialState = TRUE;
	desc[0].OutputChannels = 2;
	desc[0].pEffect = sound->EffectHi;

	desc[1].InitialState = TRUE;
	desc[1].OutputChannels = 2;
	desc[1].pEffect = sound->EffectLo;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.pEffectDescriptors = desc;
	chain.EffectCount = 1;


	for (int j = 0; j < SOUND_SOURCE_MAX; j++)
	{
		sound->SourceVoice[j]->SetEffectChain(&chain);
		sound->SourceVoice[j]->SetEffectParameters(0, &EqPrametersLo, sizeof(FXEQ_PARAMETERS));
		//sound->SourceVoice[j]->SetEffectParameters(1, &EqPrametersHi, sizeof(FXEQ_PARAMETERS));
	}

}


