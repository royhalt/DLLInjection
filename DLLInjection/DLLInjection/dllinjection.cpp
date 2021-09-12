#include<iostream>
#include<Windows.h>
using namespace std;
//void inject(const char* file_name, DWORD PID)
int main()
{
	
	char dllPath[_MAX_PATH];
	const char* file_name = "Dll.dll";
	DWORD PID;
	cin >> PID;
	cin >> dllPath;
	//GetWindowThreadProcessId(FindWindow(NULL, file), &PID);
	

	//Lấy handle
	HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (!hHandle) cout << "Process Error";

	//Lấy đương dẫn tới file dll
	#//if (!GetFullPathName(file_name, _MAX_PATH, dllPath, NULL)) cout << "error";
	
	//Cấp phát vùng nhớ
	LPVOID dllPathAddress = VirtualAllocEx(hHandle, 0, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!dllPathAddress) cout << "Allocate error";

	//Viết đường dẫn dll vào 
	WriteProcessMemory(hHandle, dllPathAddress, dllPath, strlen(dllPath), NULL);

	//Lấy địa chỉ LoadLibraryA và chuyển đến thực thi
	LPVOID loadLibraryAddress = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");

	//Bắt đầu thread thực thi tại LoaLibraryA
	HANDLE rThread = CreateRemoteThread(hHandle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, dllPathAddress, 0, NULL);
	if (!rThread) cout << "Remote error";

	//Đợi đến khi hoàn thành
	WaitForSingleObject(rThread, INFINITE);
	std::cout << "Dll path allocated at: " << std::hex << dllPath<< std::endl;
	
	/*
	// lấy 'giá trị trả về' của hàm LoadLibraryA nếu cần
	DWORD exit_code;
	GetExitCodeThread(rThread, &exit_code);

	// giải phóng handle của thread vừa mới được inject...
	CloseHandle(rThread);*/
	
	// ...và bộ nhớ được cấp phát cho đường dẫn DLL đấy
	VirtualFreeEx(hHandle, dllPathAddress, 0, MEM_RELEASE);
	
	// ...và handle cho tiến trình mục tiêu đấy
	CloseHandle(hHandle);

	//return (HANDLE)exit_code;
	return 0;
}
