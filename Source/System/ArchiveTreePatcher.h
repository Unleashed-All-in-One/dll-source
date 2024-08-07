#pragma once
namespace SUC::System
{
	class ArchiveTreePatcher
	{
	public:
		struct ArchiveDependency
		{
			std::string m_archive;
			std::vector<std::string> m_dependencies;

			ArchiveDependency() {};
			ArchiveDependency(std::string _archive, std::vector<std::string> _dependencies)
				: m_archive(_archive)
				, m_dependencies(_dependencies)
			{}
		};
		static void RegisterHooks();
		static std::vector<ArchiveDependency> s_ArchiveDependencies;
		static std::vector<std::string> s_LanguageArchives;
	};
}