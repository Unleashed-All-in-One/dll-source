#pragma once
struct CSoundPlayerCri
{
	void* vftable /*VFT*/;
	int m_Field004;
	int m_Field008;
	int m_Field00C;
	int m_Field010;
	int m_Field014;
	int m_Field018;
	float m_Volume;
};

class SoundController
{
public:
	static float GetBGMVolume(int in_BgmIndex)
	{
		Sonic::CGameDocument* gameDocument = Sonic::CGameDocument::GetInstance().get().get();
		hh::vector<Sonic::SBGMData>* audioData = &gameDocument->m_pMember->m_AudioData;
		Hedgehog::Sound::CSoundHandleBgm* bgm = audioData->at(in_BgmIndex).m_spSoundHandleBGM.get();
		CSoundPlayerCri* soundPlayer = (CSoundPlayerCri*)bgm->GetSoundPlayer();
		return soundPlayer->m_Volume;
	}
	static void SetBGMVolume(float in_Volume)
	{
		Sonic::CGameDocument* gameDocument = Sonic::CGameDocument::GetInstance().get().get();
		hh::vector<Sonic::SBGMData>* audioData = &gameDocument->m_pMember->m_AudioData;

		for (int i = 0; i < audioData->size(); ++i)
		{
			printf(audioData->at(i).m_SongName.c_str());
			Hedgehog::Sound::CSoundHandleBgm* bgm = audioData->at(i).m_spSoundHandleBGM.get();

			CSoundPlayerCri* soundPlayer = (CSoundPlayerCri*)bgm->GetSoundPlayer();

			FUNCTION_PTR(void*, __thiscall, SoundPlayerSetVolume, 0x00765850, void* _SoundPlayer, float _Volume);
			SoundPlayerSetVolume(soundPlayer, in_Volume);
		}

	}
};