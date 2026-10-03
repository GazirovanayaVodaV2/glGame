#pragma once

class Idrawable {
public:
	virtual void draw(float alpha) {};
	bool canDraw() { return true; }
	void tryDraw(float alpha) { if (canDraw()) draw(alpha); }
	virtual void SaveStateForInterpolation() = 0;
};