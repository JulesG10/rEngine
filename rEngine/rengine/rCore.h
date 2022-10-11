#pragma once
#include "stdafx.h"


class rCore
{
public:
	rCore();
	int Start(char*);

	friend void* WindowLoadThread(void* data);
protected:
	void InitDefault(int w = 0, int h = 0, const char* title = "");

	int virtual Init(std::string args);
	void virtual UpdateFrame();
	void virtual LoadingFrame();
	void virtual Load();
	void virtual UnLoad();

private:
	bool loading = true;
};

static void* WindowLoadThread(void* data);

