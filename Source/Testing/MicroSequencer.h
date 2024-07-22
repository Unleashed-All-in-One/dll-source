#pragma once

class ISequenceUnitParameterContainer
{
	
};
class SetFlagParameter : public ISequenceUnitParameterContainer
{
public:
	std::string m_flagName;
	std::string  m_flagValue;
	SetFlagParameter(const std::string& in_flagName, const std::string&  in_flagValue)
	{
		m_flagName = in_flagName;
		m_flagValue = in_flagValue;
	}
};
class MicroSequenceParameter : public ISequenceUnitParameterContainer
{
public:
	std::string m_fileName;
	MicroSequenceParameter(const std::string& in_fileName)
	{
		m_fileName = in_fileName;
	}
};
class AutoSaveParameter : public ISequenceUnitParameterContainer{};
//im not going to make the SwapDisk parameters for a very obvious reason

class WaitStageEndParameter : public ISequenceUnitParameterContainer
{
public:
	std::string m_whenTerminateSequenceFileName;
	std::string m_whenAbortSequenceFileName;
	WaitStageEndParameter(const std::string& in_sequenceTerminateFileName, const std::string& in_sequenceAbortFileName)
	{
		m_whenTerminateSequenceFileName = in_sequenceTerminateFileName;
		m_whenAbortSequenceFileName = in_sequenceAbortFileName;
	}
};
class ChangeStageParameter : public ISequenceUnitParameterContainer
{
public:
	struct SRankInfo
	{
	public:
		int32_t S, A, B, C, D, E;
	};
	struct SOverwriteTerrainInfo
	{
	public:
		std::string dirName;
	};
	struct SOverWriteSetDataInfo
	{
	public:
		std::string layerName;
		std::string layerFileName;
	};
	std::string m_stageType;
	std::string m_countryName;
	std::string m_archiveName;
	SOverwriteTerrainInfo m_overwriteTerrainInfo;
	std::string m_settingName; //just default to Stage
	bool m_isEvil;
	Hedgehog::Math::CVector m_startPosition;
	float m_startYaw;
	std::vector<SOverWriteSetDataInfo> m_overWriteSetDataInfos;
	int m_missionNo;
	SRankInfo m_rankInfo;
	int m_baseTime; //???
	ChangeStageParameter(const std::string& in_StageType, const std::string& in_ArchiveName, const std::string& in_CountryName)
	{
		m_stageType = in_StageType;
		m_archiveName = in_ArchiveName;
		m_countryName = in_CountryName;
	}

};
class PlayMovieParameter : public ISequenceUnitParameterContainer
{
public:
	std::string m_fileName;
	std::string m_sceneName;
	std::string m_comment;

	PlayMovieParameter(const std::string& in_FileName, const std::string& in_SceneName, const std::string& in_Comment)
	{
		m_fileName = in_FileName;
		m_sceneName = in_SceneName;
		m_comment = in_Comment;
	}
};

class RegisterClearFlagParameter : public ISequenceUnitParameterContainer {};
//not gonna do the other 3 for now


struct SSequenceUnitParameters
{
public:
	SetFlagParameter* setFlagParameter;
	MicroSequenceParameter* microSequenceParameter;
	AutoSaveParameter* autoSaveParameter;
	WaitStageEndParameter* waitStageEndParameter;
	ChangeStageParameter* changeStageParameter;
	PlayMovieParameter* playMovieParameter;
	RegisterClearFlagParameter* registerClearFlagParameter;
	SSequenceUnitParameters()
	{
		
	}
};
class SequenceUnit
{
public:
	enum ESequenceUnitType
	{
		SetFlag,
		MicroSequence,
		AutoSave,
		SwapDisk,
		WaitStageEnd,
		ChangeStage,
		PlayMovie,
		RegisterClearFlag,
		SendTownMessage,
		SendMediaRoomMessage,
		SetPresence
	};
	ESequenceUnitType m_type;
	SSequenceUnitParameters* param;

	SequenceUnit()
	{
		param = new SSequenceUnitParameters();
	}
};
struct SIfStatementUnit
{
public:
	enum SSwitchCaseOperation
	{
		Equal,
		NotEqual
	};
	std::string flag;
	SSwitchCaseOperation operation;
	std::string value;
};
class SwitchCaseUnit
{
public:	
	std::vector<SIfStatementUnit> switchCases;
	SequenceUnit unitToRun;
};
class IfUnit
{
public:
	SIfStatementUnit statement;
	SequenceUnit* unitToRun;

	IfUnit(const std::string& in_Flag, const SIfStatementUnit::SSwitchCaseOperation& in_Operation,const std::string& in_Value, SequenceUnit* in_Unit)
	{
		statement = SIfStatementUnit();
		statement.flag = in_Flag;
		statement.value = in_Value;
		statement.operation = in_Operation;
		unitToRun = in_Unit;
	}
};

class SequenceFile
{
	std::vector<SequenceUnit> sequenceUnits;
	std::vector<SwitchCaseUnit> switchUnits;
	std::vector<IfUnit> ifUnits;
};
class MicroSequencer
{
public:
	SequenceFile m_currentlyLoadedFile;
	static std::string m_entryPointFileName/* = "Opening"*/;
	static std::string m_sequenceExtension /*= ".seq.xml"*/;
	static void ReadStart();
	static void ReadFileFromFilename(std::string in_FileName);
};