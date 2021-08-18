#include <iostream>
#include "ftp_client.hpp"

class myclass
{
public:
	void mymethod()
	{
		std::cout << "mymethod()\n";
	}

	static int callback(off64_t xfered, void* arg)
	{
		static_cast<myclass*>(arg)->mymethod();
		std::cout << "myclass::callback(), xfered: " << xfered << "\n";
		return 1;
	}
};

int main_ftp_client()
{
	rola::ftplib* ftp = new rola::ftplib();
	ftp->Connect("192.168.253.137");
	ftp->Login("anonymous", "");
	ftp->Dir(NULL, "/");
	std::cout << ftp->LastResponse() << "\n";

	char buf[1024];
	std::cout << "SysType(): " << ftp->SysType(buf, 1024) 
		<< ", buf: " << buf << "\n";

	//std::cout << "Mkdir(): " << ftp->Mkdir("new folder") << "\n";
	//std::cout << "Chdir(): " << ftp->Chdir("new folder") << "\n";

	//std::cout << "Cdup(): " << ftp->Cdup() << "\n";
	//std::cout << "Rmdir(): " << ftp->Rmdir("new folder") << "\n";

	//std::cout << "Pwd(): " << ftp->Pwd(buf, 1024) << ", buf: " << buf << "\n";

	//std::cout << "Nlst(): " << ftp->Nlst("floosrootlist.txt", "/") << "\n";
	//std::cout << "Nlst(): " << ftp->Nlst("currentlist.txt", ".") << "\n";

	//std::cout << "Dir(): " << ftp->Dir("floosrootlistall.txt", "/") << "\n";
	//std::cout << "Dir(): " << ftp->Dir("currentlistall.txt", ".") << "\n";

	//int filesize = 0;
	//std::cout << "Text Size(): " << ftp->Size("1.txt", &filesize, rola::ftplib::ascii) << ", size(ascii): " << filesize << "\n";
	//std::cout << "Text Size(): " << ftp->Size("1.txt", &filesize, rola::ftplib::image) << ", size(image): " << filesize << "\n";
	//std::cout << "Exe Size(): " << ftp->Size("2.exe", &filesize, rola::ftplib::ascii) << ", size(ascii): " << filesize << "\n";
	//std::cout << "Exe Size(): " << ftp->Size("2.exe", &filesize, rola::ftplib::image) << ", size(image): " << filesize << "\n";

	//std::cout << "ModDate(): " << ftp->ModDate("2.exe", buf, 1024) << ", date: " << buf << "\n";

	//myclass mc;
	//ftp->SetCallbackArg(&mc);
	//ftp->SetCallbackBytes(1024);
	//ftp->SetCallbackXferFunction(myclass::callback);

	std::cout << "Get(): " << ftp->Get("postman.linux", "postman.gz", rola::ftplib::image) << "\n";

	////std::cout << "Put(): " << ftp->Put("2copy.exe", "2copy.exe", rola::ftplib::image) << "\n";

	//std::cout << "Rename(): " << ftp->Rename("1.txt", "11.txt") << "\n";
	//std::cout << "Delete(): " << ftp->Delete("1.txt") << "\n";

	//std::cout << "Quit(): " << ftp->Quit() << "\n";

	std::cout << "ftp_client_main successful\n";
	return 0;
}