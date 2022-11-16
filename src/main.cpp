#include "SaveCountManager.h"

uint32_t uMaxSaveCnt = 20;
bool bPreserveFirstSave = false;

std::string GetINIOption(const char* section, const char* key) {
	std::string	result;
	char resultBuf[256] = { 0 };

	static const std::string& configPath = "Data\\F4SE\\Plugins\\" + std::string(Version::PROJECT) + ".ini";
	GetPrivateProfileStringA(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());
	return resultBuf;
}

void ReadINI() {
	std::string uMaxSaveCnt_value = GetINIOption("Settings", "uMaxSaveCnt");
	if (!uMaxSaveCnt_value.empty() && std::stoul(uMaxSaveCnt_value)) {
		uMaxSaveCnt = std::stoul(uMaxSaveCnt_value);
		logger::info(FMT_STRING("uMaxSaveCnt: {}"), uMaxSaveCnt);
	}

	std::string bPreserveFirstSave_value = GetINIOption("Settings", "bPreserveFirstSave");
	if (!bPreserveFirstSave_value.empty() && std::stoul(bPreserveFirstSave_value)) {
		bPreserveFirstSave = std::stoul(bPreserveFirstSave_value);
		logger::info(FMT_STRING("bPreserveFirstSave: {}"), bPreserveFirstSave);
	}
}

void OnF4SEMessage(F4SE::MessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SE::MessagingInterface::kPostSaveGame:
		SCM::SaveCountManager scm((const char*)msg->data);
		scm.DeleteOldSaves(uMaxSaveCnt, bPreserveFirstSave);
		break;
	}
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface * a_f4se, F4SE::PluginInfo * a_info) {
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_f4se->IsEditor()) {
		logger::critical("loaded in editor"sv);
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_10_162) {
		logger::critical(FMT_STRING("unsupported runtime v{}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface * a_f4se) {
	F4SE::Init(a_f4se);

	ReadINI();

	const F4SE::MessagingInterface* message = F4SE::GetMessagingInterface();
	if (message)
		message->RegisterListener(OnF4SEMessage);

	return true;
}
