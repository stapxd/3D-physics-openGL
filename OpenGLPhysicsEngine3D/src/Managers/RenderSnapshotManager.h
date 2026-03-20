#pragma once

#include <unordered_map>

#include "Structures/ObjectProperties.h"

class RenderSnapshotManager
{
public:
	RenderSnapshotManager();

	void ClearPreviousSnapshots();
	void SetSnapshot(unsigned int id, const ObjectProperties& objectProperties);
	ObjectProperties* GetSnapshotByEntityId(unsigned int id);

private:
	std::unordered_map<unsigned int, ObjectProperties> m_Snapshots;
};

