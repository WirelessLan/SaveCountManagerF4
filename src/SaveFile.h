#pragma once

namespace SCM {
	class SaveFile {
	public:
		enum SAVE_TYPE : std::uint16_t {
			kRegular,
			kAuto,
			kQuick,
			kExit
		};

		SaveFile() {}
		SaveFile(const std::string& saveName) {
			this->saveName = saveName;
			separateSaveName();
		}

		bool operator < (const SaveFile& sfi) const {
			return (number < sfi.number);
		}

		std::string saveName;

		SAVE_TYPE type;
		std::uint32_t number;
		std::string id;
		std::uint8_t unk1;
		std::string playerName;
		std::string location;
		std::string	unk2;
		std::string time;
		std::uint32_t unk3;
		std::uint32_t unk4;

	private:
		void separateSaveName() {
			this->cIdx = 0;
			getSaveType();
			getSaveNumber();
			getSaveId();
			getUnk1();
			getPlayerName();
			getLocation();
			getUnk2();
			getTime();
			getUnk3();
			getUnk4();
		}

		char getNextChar() {
			return this->saveName[this->cIdx++];
		}

		void getNext() {
			std::uint32_t ii;
			for (ii = 0; this->cIdx < this->saveName.length(); ii++) {
				if (this->saveName[this->cIdx] == '_') {
					this->cIdx++;
					break;
				}
				buf[ii] = getNextChar();
			}
			buf[ii] = 0;
		}

		std::string getNextString() {
			getNext();
			return std::string(buf);
		}

		std::uint32_t getNextInt() {
			getNext();
			return atoi(buf);
		}

		void getSaveType() {
			char saveType[30] = { 0 };
			for (std::uint32_t ii = 0; this->cIdx < this->saveName.length(); ii++) {
				if (this->saveName[this->cIdx] >= '0' && this->saveName[this->cIdx] <= '9')
					break;
				saveType[ii] = getNextChar();
			}

			if (strcmp(saveType, "Save") == 0) this->type = SAVE_TYPE::kRegular;
			else if (strcmp(saveType, "Autosave") == 0) this->type = SAVE_TYPE::kAuto;
			else if (strcmp(saveType, "Quicksave") == 0) this->type = SAVE_TYPE::kQuick;
			else if (strcmp(saveType, "Exitsave") == 0)	this->type = SAVE_TYPE::kExit;
			else
				logger::warn(FMT_STRING("Unknown save type: {}"), saveType);
		}

		void getSaveNumber() {
			this->number = getNextInt();
		}

		void getSaveId() {
			char saveId[10] = { 0 };
			for (std::uint32_t ii = 0; ii < 8; ii++)
				saveId[ii] = getNextChar();

			this->id = std::string(saveId);
		}

		void getUnk1() {
			this->unk1 = getNextChar();
		}

		void getPlayerName() {
			this->playerName = getNextString();
		}

		void getLocation() {
			this->location = getNextString();
		}

		void getUnk2() {
			this->unk2 = getNextString();
		}

		void getTime() {
			this->time = getNextString();
		}

		void getUnk3() {
			this->unk3 = getNextInt();
		}

		void getUnk4() {
			this->unk4 = getNextInt();
		}

		std::uint32_t cIdx;
		char buf[300];
	};
}
