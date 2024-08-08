#pragma once
namespace SUC::System
{
	enum StageRank
	{
		Rank_E = -1,
		Rank_D,
		Rank_C,
		Rank_B,
		Rank_A,
		Rank_S,
	};
	class StageSaveData
	{
	public:
		std::string stageID;
		int score;
		int moonMedalMax;
		int sunMedalMax;
		int sunMedalCount;
		int moonMedalCount;
		int ringsAcquired;
		float timeInSeconds;
		StageRank rank;
	};
	class KeyBool
	{
	public:
		std::string keyName;
		bool keyValue;
	};
	class KeyFloat
	{
	public:
		std::string keyName;
		float keyValue;
	};
	class SaveObject
	{
	public:
		int m_Lives;
		int m_RingsAcquired;
		int m_TotalScore;
		int m_MoonMedalAcquired;
		int m_SunMedalAcquired;
		std::vector<StageSaveData*> m_StageData;
		std::vector<KeyBool*> m_KeysBool;
		std::vector<KeyFloat*> m_KeysFloat;
		int GetStageDataIndexFromID(std::string stageID)
		{
			//oh linq... i miss you...
			for (size_t i = 0; i < m_StageData.size(); i++)
			{
				if (stageID == m_StageData.at(i)->stageID)
				{
					return i;
				}
			}
			return -1;
		}
		bool HasStageBeenPlayedBeforeFromID(std::string stageID)
		{
			//oh linq... i miss you...
			for (size_t i = 0; i < m_StageData.size(); i++)
			{
				if (stageID == m_StageData.at(i)->stageID)
				{
					return true;
				}
			}
			return false;
		}
		bool GetSaveBoolKeyValue(std::string keyName)
		{
			for (size_t i = 0; i < m_KeysBool.size(); i++)
			{
				if (keyName == m_KeysBool.at(i)->keyName)
				{
					return m_KeysBool.at(i)->keyValue;
				}
			}
			return false;
		}
		float GetSaveFloatKeyValue(std::string keyName)
		{
			for (size_t i = 0; i < m_KeysFloat.size(); i++)
			{
				if (keyName == m_KeysFloat.at(i)->keyName)
				{
					return m_KeysFloat.at(i)->keyValue;
				}
			}
			return -1;
		}
	};
	class SaveManager
	{
	private:

		static SaveObject* save;
	public:
		static void GenerationsSave();
		static SaveObject* GetCurrentSave(bool returnNewIfNull = true)
		{
			if (save == nullptr)
			{
				LoadFromDisk(returnNewIfNull);
			}
			return save;
		}
		static bool SaveToDisk()
		{
			if (save == nullptr)
			{
				LoadFromDisk();
				if (save == nullptr)
				{
					printf("[SonicUnleashedConversion] Save data not found, creating...");
					save = new SaveObject();
				}
			}
			Json::Value root;
			root["lives"] = save->m_Lives;
			root["ringsAcquired"] = save->m_RingsAcquired;
			root["totalScore"] = save->m_TotalScore;
			root["moonMedalAcquired"] = save->m_MoonMedalAcquired;
			root["sunMedalAcquired"] = save->m_SunMedalAcquired;

			Json::Value stageData(Json::arrayValue);
			for (const auto& stage : save->m_StageData)
			{
				Json::Value stageObj;
				stageObj["stageID"] = stage->stageID;
				stageObj["score"] = stage->score;
				stageObj["moonMedalMax"] = stage->moonMedalMax;
				stageObj["sunMedalMax"] = stage->sunMedalMax;
				stageObj["sunMedalCount"] = stage->sunMedalCount;
				stageObj["moonMedalCount"] = stage->moonMedalCount;
				stageObj["ringsAcquired"] = stage->ringsAcquired;
				stageObj["timeInSeconds"] = stage->timeInSeconds;

				switch (stage->rank) {
				case S:
					stageObj["rank"] = "S";
					break;
				case A:
					stageObj["rank"] = "A";
					break;
				case B:
					stageObj["rank"] = "B";
					break;
				case C:
					stageObj["rank"] = "C";
					break;
				case D:
					stageObj["rank"] = "D";
					break;
				case E:
					stageObj["rank"] = "E";
					break;
				}

				stageData.append(stageObj);
			}

			Json::Value keysData(Json::arrayValue);
			for (const auto& keys : save->m_KeysBool)
			{
				Json::Value keysObj;
				keysObj["keyName"] = keys->keyName;
				keysObj["keyValue"] = keys->keyValue;
				keysData.append(keysObj);
			}
			root["stageData"] = stageData;
			root["keyDataBool"] = keysData;

			std::ofstream outputFile(SAVE_FILE_NAME);
			if (outputFile.is_open())
			{
				// Write JSON to file
				outputFile << root;
				outputFile.close();
			}
			else
			{
				printf("\n[SonicUnleashedConversion] Unable to open save file for writing, is the system on ReadOnly?.");
				return false;
			}
			return true;
		}
		static bool LoadFromDisk(bool returnNewIfNull = true)
		{
			std::ifstream inputFile(SAVE_FILE_NAME);
			if (!inputFile.is_open())
			{
				printf("\n[SonicUnleashedConversion] Error whilst loading save file, file may not exist.");
				if (save == nullptr && returnNewIfNull)
					save = new SaveObject();
				return false;
			}

			Json::Value root;
			inputFile >> root;
			if (save == nullptr)
				save = new SaveObject();
			save->m_Lives = root["lives"].asInt();
			save->m_RingsAcquired = root["ringsAcquired"].asInt();
			save->m_TotalScore = root["totalScore"].asInt();
			save->m_MoonMedalAcquired = root["moonMedalAcquired"].asInt();
			save->m_SunMedalAcquired = root["sunMedalAcquired"].asInt();

			Json::Value stageData = root["stageData"];
			for (const auto& stage : stageData) {
				StageSaveData stageSaveData;
				stageSaveData.score = stage["score"].asInt();
				stageSaveData.stageID = stage["stageID"].asCString();
				stageSaveData.moonMedalMax = stage["moonMedalMax"].asInt();
				stageSaveData.sunMedalMax = stage["sunMedalMax"].asInt();
				stageSaveData.sunMedalCount = stage["sunMedalCount"].asInt();
				stageSaveData.moonMedalCount = stage["moonMedalCount"].asInt();
				stageSaveData.ringsAcquired = stage["ringsAcquired"].asInt();
				stageSaveData.timeInSeconds = stage["timeInSeconds"].asFloat();

				std::string rankStr = stage["rank"].asString();
				if (rankStr == "S")
				{
					stageSaveData.rank = Rank_S;
				}
				else if (rankStr == "A")
				{
					stageSaveData.rank = Rank_A;
				}
				else if (rankStr == "B")
				{
					stageSaveData.rank = Rank_B;
				}
				else if (rankStr == "C")
				{
					stageSaveData.rank = Rank_C;
				}
				else if (rankStr == "D")
				{
					stageSaveData.rank = Rank_D;
				}
				else if (rankStr == "E")
				{
					stageSaveData.rank = Rank_E;
				}
				else
				{
					stageSaveData.rank = Rank_E;
				}

				save->m_StageData.push_back(&stageSaveData);
			}
			Json::Value keysData = root["keyDataBool"];
			for (const auto& keyData : keysData) {

				KeyBool keyBoolNew;
				keyBoolNew.keyName = keyData["keyName"].asString();
				keyBoolNew.keyValue = keyData["keyValue"].asBool();
			}

			inputFile.close();
			return true;
		}
		static bool DeleteSave(bool resetPointer = true)
		{
			if (resetPointer)
				save = new SaveObject();
			if (std::remove(SAVE_FILE_NAME) != 0)
			{
				printf("\n[SonicUnleashedConversion] Error whilst deleting save file, file may not exist.");
				return false;
			}
			else
			{
				printf("\n[SonicUnleashedConversion] Save file deleted successfully.");
				return true;
			}
		}
		static void RegisterHooks();
	};
}