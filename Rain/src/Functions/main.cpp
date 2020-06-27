#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "../Any/App.h"

namespace ii887522::Rain
{
	static int main()
	{
		App{ }.eventLoop();
		return EXIT_SUCCESS;
	}
}

#ifdef _CONSOLE

int main()
{
	return ii887522::Rain::main();
}

#endif

#ifdef _WINDOWS

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	return ii887522::Rain::main();
}

#endif