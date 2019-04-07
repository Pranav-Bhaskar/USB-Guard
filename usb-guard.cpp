#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<vector>

void showHelp(){
	std::cout<<"\nUsage: USB-Guard [OPTION]\n"
	<<"Checks if any unknown usb device is not connected.\n\n"
	<<"  -e\t\tEnables the USB-Guard.\n"
	<<"  -d\t\tDisables the USB-Guard.\n"
	<<"  -s\t\tPrints the current USB-Guard status(Enabled/Disabled) with the list\n  \t\tof whitelisted devices.\n"
	<<"  -i\t\tResets the whitelisted devices list with the currently connected devices.\n"
	<<"  -h\t\tDisplays this help menu.\n"
	<<"  -a\t\tAppends the whitelisted devices list with the currently connected devices.\n"
	<<"  -c\t\tConfigures the action to be taken when a usb device is inserted.\n"
	<<"  -x\t\tChecks if any unknown device is connected or not and takes actions respectively.\n"
	<<"\nNOTE: Few options are required to be executed as a privileged user.\n\n";
}

std::string cutter(std::string& line){
	std::string temp;
	bool isID = false;
	for(int i=1;i<line.size();++i){
		if(isID){
			temp.push_back(line[i]);
			continue;
		}
		if(line[i] == 'D' && line[i-1] == 'I'){
			isID = true;
			++i;
		}
	}
	return temp;
}

std::vector<std::string> spliter(std::string& line){
	std::string temp;
	std::vector<std::string> vec;
	bool isPID = false;
	for(int i=0;i<line.size();++i){
		if(isPID == false){
			if(line[i] == ':'){
				isPID = true;
				vec.push_back(temp);
				temp.clear();
				continue;
			}
			temp.push_back(line[i]);
			continue;
		}
		if(line[i] == ' '){
			isPID = false;
			vec.push_back(temp);
			temp.clear();
			continue;
		}
		temp.push_back(line[i]);
	}
	vec.push_back(temp);
	return vec;
}

void removeLockFile(){
	system("rm -f /usr/share/USB-Guard/usb-guard.lock");
}

void createLockFile(){
	removeLockFile();
	system("chmod 400 /usr/share/USB-Guard/usb-guard.lock>>/usr/share/USB-Guard/usb-guard.lock");
}

bool exsists(){
	std::ifstream f("/usr/share/USB-Guard/usb-guard.lock");
	return f.good();
}

void printWhiteList(){
	std::ifstream f;
	f.open("/usr/share/USB-Guard/white.list");
	std::string line;
	std::vector<std::string> data;
	std::cout<<"List of whitelisted devices :\n";
	if(!f.good())
		return;
	while(f){
		getline(f, line);
		if(f.eof())
			continue;
		data = spliter(line);
		std::cout<<"Vendor ID: "<<data[0]<<" Product ID: "<<data[1]<<" Name: "<<data[2]<<std::endl;
	}
	f.close();
}

void printStatus(){
	std::cout<<"Status : ";
	if(exsists())
		std::cout<<"Enabled\n";
	else
		std::cout<<"Disabled\n";
	printWhiteList();
}

void changeConfigFile(){
	int opt;
	do{
		std::cout<<"\nChoose Editor\n1. Nano\n2. Vi\n3. Gedit\nEnter Option : ";
		std::cin>>opt;
	}while(opt>3||opt<1);
	switch(opt){
		case 1:system("nano /usr/share/USB-Guard/script.sh");
			break;
		case 2:system("vi /usr/share/USB-Guard/script.sh");
			break;
		case 3:system("gedit /usr/share/USB-Guard/script.sh");
			break;
	}
	std::cout<<"\nEdit Saved.\n";
}

std::vector<std::string> readWhiteFile(){
	std::ifstream f;
	f.open("/usr/share/USB-Guard/white.list");
	std::string line;
	std::vector<std::string> data;
	if(!f.good())
		return data;
	while(f){
		getline(f, line);
		if(f.eof())
			continue;
		data.push_back(line);
	}
	f.close();
	return data;
}
void appendWhileList(){
	std::ifstream f;
	std::vector<std::string> data = readWhiteFile();
	system("lsusb > /usr/share/USB-Guard/temp.data");
	f.open("/usr/share/USB-Guard/temp.data");
	std::vector<std::string> vec;
	std::string line;
	bool exsis;
	while(f){
		getline(f, line);
		if(f.eof())
			continue;
		line = cutter(line);
		exsis = false;
		for(int i=0;i<data.size();++i){
			if(line == data[i]){
				exsis = true;
				break;
			}			
		}
		if(!exsis)
			vec.push_back(line);
	}
	f.close();
	std::ofstream of;
	of.open("/usr/share/USB-Guard/white.list", std::ios::out | std::ios::app);
	for(int i=0;i<vec.size();++i){
		of << vec[i] << std::endl;
	}
	of.close();
	system("rm -f /usr/share/USB-Guard/temp.data");
}

void createWhiteList(){
	std::ifstream f;
	system("lsusb > /usr/share/USB-Guard/temp.data");
	f.open("/usr/share/USB-Guard/temp.data");
	std::vector<std::string> vec;
	std::string line;
	bool exsis;
	while(f){
		getline(f, line);
		if(f.eof())
			continue;
		line = cutter(line);
		vec.push_back(line);
	}
	f.close();
	std::ofstream of;
	of.open("/usr/share/USB-Guard/white.list", std::ios::out);
	for(int i=0;i<vec.size();++i){
		of << vec[i] << std::endl;
	}
	of.close();
	system("rm -f /usr/share/USB-Guard/temp.data");
}

void executeFile(){
	std::ifstream f;
	std::vector<std::string> data = readWhiteFile();
	system("lsusb > /usr/share/USB-Guard/temp.data");
	f.open("/usr/share/USB-Guard/temp.data");
	std::string line;
	bool exsis;
	bool known = true;
	while(f){
		getline(f, line);
		if(f.eof())
			continue;
		line = cutter(line);
		exsis = false;
		for(int i=0;i<data.size();++i){
			if(line == data[i]){
				exsis = true;
				break;
			}			
		}
		if(!exsis){
			known = false;
			break;
		}
	}
	f.close();
	system("rm -f /usr/share/USB-Guard/temp.data");
	if(!known)
		system("bash /usr/share/USB-Guard/script.sh");
}

int main(int argc,const char* argv[]){
	if(argc == 2){
		if(strcmp(argv[1], "-d") == 0){
			removeLockFile();
		}
		else if(strcmp(argv[1], "-e") == 0){
			createLockFile();
		}
		else if(strcmp(argv[1], "-s") == 0){
			printStatus();
		}
		else if(strcmp(argv[1], "-i") == 0){
			createWhiteList();
		}
		else if(strcmp(argv[1], "-a") == 0){
			appendWhileList();
		}
		else if(strcmp(argv[1], "-c") == 0){
			changeConfigFile();
		}
		else if(strcmp(argv[1], "-x") == 0){
			executeFile();
		}
		else if(strcmp(argv[1], "-h") == 0){
			showHelp();
		}
		else{
			std::cout<<"\nError : Argument not recognized.\nDisplaying Help : \n";
			showHelp();
		}
	}
	else{
		std::cout<<"\nError : Argument not recognized.\nDisplaying Help : \n";
		showHelp();
	}
	std::cout<<"Done.\n";
	return 0;
}
