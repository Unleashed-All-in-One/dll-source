
namespace SUC::System
{
    std::vector<ArchiveTreePatcher::ArchiveDependency> ArchiveTreePatcher::s_ArchiveDependencies = {};
    std::vector<std::string> ArchiveTreePatcher::s_LanguageArchives = {};

    HOOK(bool, __stdcall, ArchiveTreePatcher_ParseArchiveTree, 0xD4C8E0, void* A1, char* pData, const size_t size, void* pDatabase)
    {
        std::string str;
        {
            std::stringstream stream;

            for (ArchiveTreePatcher::ArchiveDependency const& node : ArchiveTreePatcher::s_ArchiveDependencies)
            {
                stream << "  <Node>\n";
                stream << "    <Name>" << node.m_archive << "</Name>\n";
                stream << "    <Archive>" << node.m_archive << "</Archive>\n";
                stream << "    <Order>" << 0 << "</Order>\n";
                stream << "    <DefAppend>" << node.m_archive << "</DefAppend>\n";

                for (std::string const& dependency : node.m_dependencies)
                {
                    stream << "    <Node>\n";
                    stream << "      <Name>" << dependency << "</Name>\n";
                    stream << "      <Archive>" << dependency << "</Archive>\n";
                    stream << "      <Order>" << 0 << "</Order>\n";
                    stream << "    </Node>\n";
                }

                stream << "  </Node>\n";
            }

            str = stream.str();
        }

        const size_t newSize = size + str.size();
        const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);
        memcpy(pBuffer.get(), pData, size);

        char* pInsertionPos = strstr(pBuffer.get(), "<Include>");

        memmove(pInsertionPos + str.size(), pInsertionPos, size - (size_t)(pInsertionPos - pBuffer.get()));
        memcpy(pInsertionPos, str.c_str(), str.size());

        bool result;
        {
            result = originalArchiveTreePatcher_ParseArchiveTree(A1, pBuffer.get(), newSize, pDatabase);
        }

        return result;
    }

    boost::shared_ptr<hh::db::CRawData>* __fastcall ArchiveTreePatcher_GetRawDataImpl
    (
        hh::db::CDatabase* This,
        void* Edx,
        boost::shared_ptr<hh::db::CRawData>& rawData,
        const hh::base::CSharedString& name,
        const bool flag)
    {
        hh::db::fpCDatabaseGetRawData(This, rawData, name, flag);
        if (name != "LanguageTree.xml" || !rawData || !rawData->m_spData)
            return &rawData;

        std::string str;
        {
            std::stringstream stream;
            for (std::string const& archive : ArchiveTreePatcher::s_LanguageArchives)
            {
                stream << "<Archive>" << archive << "</Archive>\n";
            }
            str = stream.str();
        }

        const char* const appendData = str.c_str();
        const size_t appendDataSize = strlen(appendData);

        const size_t newSize = rawData->m_DataSize + appendDataSize;
        const boost::shared_ptr<uint8_t[]> buffer = boost::make_shared<uint8_t[]>(newSize);
        memcpy(buffer.get(), rawData->m_spData.get(), rawData->m_DataSize);

        char* insertionPos = strstr((char*)buffer.get(), "</Language>");

        memmove(insertionPos + appendDataSize, insertionPos, rawData->m_DataSize - (size_t)(insertionPos - (char*)buffer.get()));
        memcpy(insertionPos, appendData, appendDataSize);

        rawData = boost::make_shared<hh::db::CRawData>();
        rawData->m_Flags = hh::db::eDatabaseDataFlags_IsMadeAll;
        rawData->m_spData = buffer;
        rawData->m_DataSize = newSize;

        return &rawData;
    }

    void ArchiveTreePatcher::RegisterHooks()
    {
        std::vector<std::string> hubs = SUC::Project::GetAllWhiteWorld();
        for (size_t i = 0; i < SUC::Project::s_AdditionalArchiveTree.data.size(); i++)
        {
            s_ArchiveDependencies.push_back(SUC::Project::s_AdditionalArchiveTree.data[i]);
        }
        for (size_t i = 0; i < hubs.size(); i++)
        {
            s_ArchiveDependencies.push_back(ArchiveDependency("SonicPam", { hubs[i] }));
            s_ArchiveDependencies.push_back(ArchiveDependency("pam000", { hubs[i] }));
            s_ArchiveDependencies.push_back(ArchiveDependency("StageGate", { hubs[i] }));
        }

        s_ArchiveDependencies.push_back(ArchiveDependency("UIFade", { "SystemCommon" }));
        s_ArchiveDependencies.push_back(ArchiveDependency("TailsEVRoot", { "ev041", "m1_03_1" }));
        s_ArchiveDependencies.push_back(ArchiveDependency("SonicEVRoot", { "ev041" }));
        s_ArchiveDependencies.push_back(ArchiveDependency("ObjectPhysicsUnleashed", { "cmn200" }));
        s_ArchiveDependencies.push_back(ArchiveDependency("HoloskaStomp", { "cmn200" }));
        s_ArchiveDependencies.push_back(ArchiveDependency("ObjectPhysicsUnleashed", { "cmn100" }));

        if (!s_ArchiveDependencies.empty())
        {
            INSTALL_HOOK(ArchiveTreePatcher_ParseArchiveTree);
        }

        if (!s_LanguageArchives.empty())
        {
            WRITE_CALL(0xD4C4C1, ArchiveTreePatcher_GetRawDataImpl);
        }
    }

}
