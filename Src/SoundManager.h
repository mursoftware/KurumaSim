


#include <xaudio2.h>
#include <XAPOFX.h>

#pragma comment(lib, "XAudio2.lib")


#include <map>
#include <string>


const int SOUND_SOURCE_MAX = 10;


struct SOUND
{
	char					FileName[260];

	BYTE*					SoundData;
	IXAudio2SourceVoice*	SourceVoice[ SOUND_SOURCE_MAX ];
	int						Length;
	int						PlayLength;

};


class SoundManager
{
	private:

		static SoundManager*			m_Instance;

		static IXAudio2*				m_Xaudio;
		static IXAudio2MasteringVoice*	m_MasteringVoice;

		static std::unordered_map<std::string, SOUND*>		m_BufferList;

		static IUnknown*				m_Reverb;

	public:
		static SoundManager* GetInstance() { return m_Instance; }

		SoundManager();
		~SoundManager();

		void StopAll(){}
		void SetMasterVolume( float volume ){}

		void Load( const char* FileName );
		int Play( const char* FileName, bool Loop = false, float Volume = 1.0f );
		void Stop( const char* FileName, int Index);
		void SetPitch( const char* FileName, int Index, float pitch );
		//void SetPan(const char* FileName, int Index, float pan);
		void SetVolume( const char* FileName, int Index, float volume );

		SOUND* LoadData( const char* FileName );

};