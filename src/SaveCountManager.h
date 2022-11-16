#pragma once 

#include <ShlObj_core.h>
#include <regex>

#include "SaveFile.h"

namespace SCM {
	class SaveCountManager {
	public:
		SaveCountManager(const std::string& saveName) {
			logger::info(FMT_STRING("New save file name: {}"), saveName.c_str());

			this->sf = SaveFile(saveName);
			logger::info(FMT_STRING("type[{}] number[{}] id[{}] unk1[{}] playerName[{}] location[{}] unk2[{}] time[{}] unk3[{}] unk4[{}]"),
				this->sf.type, this->sf.number, this->sf.id.c_str(), this->sf.unk1, this->sf.playerName.c_str(),
				this->sf.location.c_str(), this->sf.unk2.c_str(), this->sf.time.c_str(), this->sf.unk3, this->sf.unk4);
		}

		void DeleteOldSaves(uint32_t uMaxSaveCnt, bool bPreserveFirstSave) {
			if (this->sf.type != SaveFile::kRegular)
				return;

			std::map<uint32_t, std::vector<std::filesystem::path>> sv_map = getSaveMap();
			if ((uint32_t)sv_map.size() < uMaxSaveCnt)
				return;

			uint32_t map_indx = 0;
			for (auto iter : sv_map) {
				if (map_indx >= sv_map.size() - uMaxSaveCnt)
					break;

				if (bPreserveFirstSave && map_indx == 0) {
					map_indx++;
					continue;
				}

				for (auto p_iter : iter.second) {
					if (!remove(p_iter)) {
						logger::critical(FMT_STRING("Failed to remove file: {}"), p_iter.string());
						continue;
					}
				}

				map_indx++;
			}
		}

	private:
		std::string getSavePath() {
			static std::string savePath;

			if (!savePath.empty())
				return savePath;

			char path[MAX_PATH];
			if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
				logger::warn("Failed to get document path!");
				return savePath;
			}

			savePath = std::string(path) + "\\My Games\\Fallout4\\";

			RE::INISettingCollection* iniCollection = RE::INISettingCollection::GetSingleton();
			RE::Setting* localSavePathSetting = nullptr;
			for (auto it : iniCollection->settings) {
				if (it->GetKey().compare("SLocalSavePath:General"sv) == 0) {
					localSavePathSetting = it;
					break;
				}
			}

			if (localSavePathSetting && localSavePathSetting->GetType() == RE::Setting::SETTING_TYPE::kString)
				savePath += localSavePathSetting->GetString();
			else
				savePath += "Saves\\";

			return savePath;
		}

		std::map<uint32_t, std::vector<std::filesystem::path>> getSaveMap() {
			std::map<uint32_t, std::vector<std::filesystem::path>> retMap;

			std::string saveDir = getSavePath();
			if (saveDir.empty())
				return retMap;

			const std::regex filter("Save.*_" + this->sf.id + ".*");
			const std::filesystem::directory_iterator dir_iter(saveDir);
			for (auto iter : dir_iter) {
				if (!std::filesystem::is_regular_file(iter.status()))
					continue;

				if (!std::regex_match(iter.path().filename().string(), filter))
					continue;

				SaveFile save = SaveFile(iter.path().filename().string());
				auto num_iter = retMap.find(save.number);
				if (num_iter == retMap.end())
					retMap.insert(std::make_pair(save.number, std::vector<std::filesystem::path>{ iter.path() }));
				else
					num_iter->second.push_back(iter.path().string());
			}

			return retMap;
		}

		SaveFile sf;
	};
}
