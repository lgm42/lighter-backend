#pragma once

class IComponent
{
public:
	IComponent() {}
	virtual ~IComponent() {}
	virtual void setup(void) = 0;
	virtual void handle(void) = 0;
};