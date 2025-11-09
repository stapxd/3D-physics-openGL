#pragma once
class PhysicsBody
{
public:
	PhysicsBody(bool isStatic) {
		SetStatic(isStatic);
	}

	void SetStatic(bool value) { m_IsStatic = value; }
	bool GetStatic() const { return m_IsStatic; }
private:
	bool m_IsStatic;
};

