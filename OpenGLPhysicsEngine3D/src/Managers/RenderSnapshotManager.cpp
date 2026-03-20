#include "RenderSnapshotManager.h"

RenderSnapshotManager::RenderSnapshotManager()
{
}

void RenderSnapshotManager::ClearPreviousSnapshots()
{
	m_Snapshots.clear();
}

void RenderSnapshotManager::SetSnapshot(unsigned int id, const ObjectProperties& objectProperties)
{
	m_Snapshots[id] = objectProperties;
}

ObjectProperties* RenderSnapshotManager::GetSnapshotByEntityId(unsigned int id)
{
	auto it = m_Snapshots.find(id);
	if (it == m_Snapshots.end())
		return nullptr;
	return &it->second;
}
