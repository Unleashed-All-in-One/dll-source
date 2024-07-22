#include "MicroSequencer.h"

#include <rapidxml/rapidxml_utils.hpp>

std::string MicroSequencer::m_sequenceExtension = ".seq.xml";
SequenceUnit* parseSequenceUnit(rapidxml::xml_node<>* sequenceUnitNode)
{
    SequenceUnit newSequenceUnit = SequenceUnit();
    auto typeValue = sequenceUnitNode->first_node("type")->value();
    if (typeValue == "SetFlag")
        newSequenceUnit.m_type = SequenceUnit::SetFlag;
    if (typeValue == "AutoSave")
        newSequenceUnit.m_type = SequenceUnit::AutoSave;
    if (typeValue == "WaitStageEnd")
        newSequenceUnit.m_type = SequenceUnit::WaitStageEnd;
    if (typeValue == "ChangeStage")
        newSequenceUnit.m_type = SequenceUnit::ChangeStage;
    if (typeValue == "PlayMovie")
        newSequenceUnit.m_type = SequenceUnit::PlayMovie;
    if (typeValue == "RegisterClearFlag")
        newSequenceUnit.m_type = SequenceUnit::RegisterClearFlag;

    auto paramNode = sequenceUnitNode->first_node("param");
    for (rapidxml::xml_node<>* paramChildNode = paramNode->first_node(); paramChildNode; paramChildNode = paramChildNode->next_sibling())
    {
		switch (newSequenceUnit.m_type)
		{
		case SequenceUnit::SetFlag:
		{
			newSequenceUnit.param->setFlagParameter = new SetFlagParameter(std::string(paramChildNode->first_node("FlagName")->value()), std::string(paramChildNode->first_node("FlagValue")->value()));
			break;
		}
		case SequenceUnit::AutoSave:
		{
			newSequenceUnit.param->autoSaveParameter = new AutoSaveParameter();
			break;
		}
		case SequenceUnit::WaitStageEnd:
		{
			newSequenceUnit.param->waitStageEndParameter = new WaitStageEndParameter(std::string(paramChildNode->first_node("WhenTerminate")->value()), std::string(paramChildNode->first_node("WhenAbort")->value()));
			break;
		}
		case SequenceUnit::ChangeStage:
		{
			std::string stageType = paramChildNode->first_node("StageType") != nullptr ? std::string(paramChildNode->first_node("StageType")->value()) : "Action";
			std::string archiveName = paramChildNode->first_node("ArchiveName") != nullptr ? std::string(paramChildNode->first_node("ArchiveName")->value()) : "ghz100";
			std::string countryName = paramChildNode->first_node("CountryName") != nullptr ? std::string(paramChildNode->first_node("CountryName")->value()) : "Mykonos";
			newSequenceUnit.param->changeStageParameter = new ChangeStageParameter(stageType, archiveName, countryName);
			///theres a lot of missing data, add parsing for it later
			
			break;
		}
		case SequenceUnit::PlayMovie:
		{
			std::string comment = paramChildNode->first_node("Comment") != nullptr ? std::string(paramChildNode->first_node("Comment")->value()) : "";
			newSequenceUnit.param->playMovieParameter = new PlayMovieParameter(std::string(paramChildNode->first_node("FileName")->value()), std::string(paramChildNode->first_node("SceneName")->value()), comment);
			break;
		}
		}
		//paramChildNode->
    }
    return &newSequenceUnit;
}
IfUnit* parseIfNode(rapidxml::xml_node<>* ifNode)
{
    std::string operation = ifNode->first_attribute("operation")->value();
    SIfStatementUnit::SSwitchCaseOperation operationParsed;
    if (operation == "E")
        operationParsed = SIfStatementUnit::SSwitchCaseOperation::Equal;
    if (operation == "NE")
        operationParsed = SIfStatementUnit::SSwitchCaseOperation::NotEqual;


    SequenceUnit* unit = nullptr;
    for (rapidxml::xml_node<>* sequenceUnit = ifNode->first_node("SequenceUnit"); sequenceUnit; sequenceUnit = sequenceUnit->next_sibling("SequenceUnit")) 
    {
        unit=parseSequenceUnit(sequenceUnit);
    }
    auto ifunit = new IfUnit(std::string(ifNode->first_attribute("flag")->value()), operationParsed, std::string(ifNode->first_attribute("value")->value()), unit);
    return ifunit;
}
void parseCaseNode(rapidxml::xml_node<>* caseNode)
{
    std::cout << "<Case flag=\"" << caseNode->first_attribute("flag")->value() << "\" operation=\"" << caseNode->first_attribute("operation")->value() << "\" value=\"" << caseNode->first_attribute("value")->value() << "\">" << std::endl;
    for (rapidxml::xml_node<>* childNode = caseNode->first_node(); childNode; childNode = childNode->next_sibling())
    {
        if (std::string(childNode->name()) == "SequenceUnit") 
        {
            parseSequenceUnit(childNode);
        }
        else if (std::string(childNode->name()) == "If") 
        {
            parseIfNode(childNode);
        }
    }
}
void parseSwitchNode(rapidxml::xml_node<>* switchNode)
{
    for (rapidxml::xml_node<>* childNode = switchNode->first_node(); childNode; childNode = childNode->next_sibling()) 
    {
        if (std::string(childNode->name()) == "Case") 
        {
            parseCaseNode(childNode);
        }
        else if (std::string(childNode->name()) == "Default") 
        {
            for (rapidxml::xml_node<>* sequenceUnit = childNode->first_node("SequenceUnit"); sequenceUnit; sequenceUnit = sequenceUnit->next_sibling("SequenceUnit")) 
            {
                parseSequenceUnit(sequenceUnit);
            }            
        }
    }
}

void MicroSequencer::ReadFileFromFilename(std::string in_FileName)
{
	rapidxml::file<> xmlFile((in_FileName + m_sequenceExtension).c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	// Access the root node
	rapidxml::xml_node<>* root = doc.first_node("MicroSequence");
	for (rapidxml::xml_node<>* sequenceUnit = root->first_node("SequenceUnit"); sequenceUnit; sequenceUnit = sequenceUnit->next_sibling("SequenceUnit")) 
	{
		
	}
}
