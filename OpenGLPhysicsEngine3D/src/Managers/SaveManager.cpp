#include "SaveManager.h"

#include <windows.h>
#include <commdlg.h>
#include <fstream>

#include "Application/Globals.h"

SaveManager::SaveManager(PhysicsWorld& physicsWorld)
	: m_PhysicsWorld(physicsWorld)
{
}

void SaveManager::Save()
{
	std::string filepath = GetFilePathWithExplorer(SaveActions::Save);

	if (filepath == "")
		return;

	std::fstream file;
	file.open(filepath, std::ios::out | std::ios::binary);

	for (auto& entity : m_PhysicsWorld->GetEntities()) {
		EntityTypes type = entity.second.GetType();
		ObjectProperties props = entity.second.GetProperties();

		file.write((char*)&type, sizeof(type));
		file.write((char*)&props, sizeof(props));
	}

	file.close();
}

void SaveManager::Load()
{
	std::string filepath = GetFilePathWithExplorer(SaveActions::Load);

	if (filepath == "")
		return;

	std::fstream file;
	file.open(filepath, std::ios::in | std::ios::binary);

	Entity entity;
	std::size_t expectedEntitySize = sizeof(EntityTypes) + sizeof(ObjectProperties);

	file.seekg(0, std::ios::end);
	auto fileSize = file.tellg();

	if (fileSize == -1 || fileSize % expectedEntitySize != 0)
		throw "File is broken!";

	file.seekg(0, std::ios::beg);

	m_PhysicsWorld->ClearAll();

	while (file.tellg() < fileSize) {
		EntityTypes type;
		ObjectProperties props;

		file.read((char*)&type, sizeof(type));
		file.read((char*)&props, sizeof(props));

		m_PhysicsWorld->AddEntity(type, props);
	}

	file.close();
}

std::string SaveManager::GetFilePathWithExplorer(SaveActions action)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Scene Files\0*.scene\0All Files\0*.*\0";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	switch (action) {
		case SaveActions::Save:
			if (GetSaveFileNameA(&ofn))
				return std::string(ofn.lpstrFile);
			break;
		case SaveActions::Load:
			if (GetOpenFileNameA(&ofn))
				return std::string(ofn.lpstrFile);
			break;
		default:
			return "";
	}

	return "";
}
