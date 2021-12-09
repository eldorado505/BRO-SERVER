#include<bits/stdc++.h>
#ifdef _WIN32
#include<windows.h>
#include<winsock2.h>
#endif // _WIN32
#ifdef linux
#include<arpa/inet.h>
#include<sys/socket.h>
#endif // linux
#include<sys/stat.h>
#include<string>
#include<map>
#include<forward_list>
using namespace std;

//Amit (The Bro Programmer)
class BroUtility {
private:
	BroUtility() {};
public:
	static isHexChar(char ch) {
		if (ch >= 48 && ch <= 57) return true;
		if (ch >= 'a' && ch <= 'z') return true;
		if (ch >= 'A' && ch <= 'Z') return true;
		return false;
	}

	static void decode(char *encodedString, char *decodedString) {
		char *ptr = encodedString;
		int i = 0;
		while (*ptr != '\0') {
			if (*ptr == '+') {
				decodedString[i] = ' ';
				i++;
				ptr++;
				continue;
			}
			if (*ptr != '%') {
				decodedString[i] = *ptr;
				ptr++;
				i++;
				continue;
			}
			ptr++;
			if (isHexChar(*ptr) && isHexChar(*(ptr + 1))) {
				int x;
				sscanf(ptr, "%2x", &x);
				decodedString[i] = x;
				ptr += 2;
				i++;
			}
			else {
				i = 0;
				break;
			}
		}
		decodedString[i] = '\0';
	}

	static void loadMIMETypes(map<string, string> &mimeTypesMap) {
		FILE *f = fopen("/bro-data/mime.types", "r");
		if (f == NULL) return;
		char *mimeType, *extension;
		char line[200];
		while (1) {
			fgets(line, 200, f);
			if (feof(f)) break;
			if (line[0] == '#') continue;
			int x = strlen(line) - 1;
			while (x >= 0 && (line[x] == '\r' || line[x] == '\n')) {
				line[x] = '\0';
				x--;
			}
			if (x == -1) continue;
			mimeType = &line[0];
			for (x = 0; line[x] != '\t'; x++);
			line[x] = '\0';
			x++;
			while (line[x] == '\t') x++;
			while (1) {
				extension = &line[x];
				while (line[x] != '\0' && line[x] != ' ') x++;
				if (line[x] == '\0') {
					mimeTypesMap[string(extension)] = string(mimeType);
					break;
				}
				else {
					line[x] = '\0';
					mimeTypesMap[string(extension)] = string(mimeType);
					x++;
				}
				// cout << extension << " -> " << mimeType << endl;
			}
		}
		cout << endl << "Successfully Loaded the mime types" << endl;
	}
};

class Error {
private:
	string error;
public:
	Error(string error) {
		this->error = error;
	}
	bool hasError() {
		return this->error.length() > 0;
	}
	string getError() {
		return this->error;
	}
};

class StringUtility {
private:
	StringUtility() {}
public:
	static void toLowerCase(char *str) {
		while (*str) {
			if (*str >= 65 && *str <= 90) *str += 32;
			str++;
		}
	}
};

class HTTPErrorStatusUtility {
private:
	HTTPErrorStatusUtility() {}
public:
	static void sendBadRequestError(int clientSocketDescriptor) {
		// Do later
	}

	static void sendHTTPVersionNotSupportedError(int clientSocketDescriptor, char *httpVersion) {
		// Do later
	}

	static void sendNotFoundError(int clientSocketDescriptor, char *requestURI) {
		char content[1000];
		char header[200];
		char response[1200];
		sprintf(content, "<!DOCTYPE HTML><html lang='en'><head><title>404 Error</title></head><body>Reuested Resources [%s] Not Found</body>", requestURI);
		int contentLength = strlen(content);
		sprintf(header, "HTTP/1.1 404 Not Found\r\nContent-Type:text/html\nContent-Length:%d\nConnection:close\r\n\r\n", contentLength);
		strcpy(response, header);
		strcat(response, content);
		send(clientSocketDescriptor, response, strlen(response), 0);
	}

	static void sendMethodNotAllowedError(int clientSocketDescriptor, char *method, char *requestURI) {
		// Do later
	}
};

class FileSystemUtility {
private:
	FileSystemUtility() {};
public:
	static bool fileExists(const char *path) {
		struct stat s;
		int x = stat(path, &s);
		if (x == -1) return false;
		if (s.st_mode & S_IFDIR) return false;
		return true;
	}
	static bool directoryExists(const char *path) {
		struct stat s;
		int x = stat(path, &s);
		if (x == -1) return false;
		if (s.st_mode & S_IFDIR) return true;
		return false;
	}
	static string getFileExtension(const char *path) {
		int x = strlen(path) - 1;
		while (x >= 0 && path[x] != '.') x--;
		if (x < 0) return "html";
		return string(path + x + 1);
	}
};

class validator {
private:
	validator() {}
public:
	static bool isValidMIMEType(string &mimeType) {
		// do nothing
		return true;
	}
	static bool isValidPath(string &path) {
		return FileSystemUtility::directoryExists(path.c_str());
	}
	static bool isValidURL(string &url) {
		// do nothing
		return true;
	}
};

class Request {
private:
	char *method, *httpVersion, *requestURI;
	map<string, string> dataMap;
	void createDataMap(char *str, map<string, string> &dataMap) {
		char *ptr1 = str, *ptr2 = str;
		// cout << str << endl;
		while (1) {
			// extract (key,value) and put into dataMap
			while (*ptr2 != '\0' && *ptr2 != '=') ptr2++;
			if (*ptr2 == '\0') return;
			*ptr2 = '\0';
			int keyLength = ptr2 - ptr1;
			char *decoded = new char[keyLength + 1];
			BroUtility::decode(ptr1, decoded);
			string key = string(decoded);
			delete []decoded;
			ptr2++;
			ptr1 = ptr2;
			while (*ptr2 != '\0' && *ptr2 != '&') ptr2++;
			if (*ptr2 == '\0') {
				int valueLength = ptr2 - ptr1;
				char *decoded = new char[valueLength + 1];
				BroUtility::decode(ptr1, decoded);
				string value = string(decoded);
				dataMap[key] = value;
				delete []decoded;
				// cout << key << " " << string(ptr1) << endl;
				break;
			}
			else {
				*ptr2 = '\0';
				int valueLength = ptr2 - ptr1;
				char *decoded = new char[valueLength + 1];
				BroUtility::decode(ptr1, decoded);
				string value = string(decoded);
				dataMap[key] = value;
				delete []decoded;
				// cout << key << " " << string(ptr1) << endl;
				ptr2++;
				ptr1 = ptr2;
			}
		} // infinite loop ends
	}
public:
	Request(char *method, char *requestURI, char *httpVersion, char *dataInRequest) {
		this->method = method;
		this->requestURI = requestURI;
		this->httpVersion = httpVersion;
		if (dataInRequest != NULL && strcmp(this->method, "get") == 0) {
			createDataMap(dataInRequest, dataMap);
		}
	}
	string operator[](string key) {
		auto iterator = dataMap.find(key);
		if (iterator != dataMap.end()) return iterator->second;
		return "";
	}
	friend class Bro;
};

class Response {
private:
	forward_list<string> content;
	forward_list<string>::iterator contentIterator;
	unsigned long long contentLength;
	string contentType;
public:
	Response() {
		this->contentLength = 0;
		this->contentIterator = this->content.before_begin();
	}
	~Response() {
		// not yet decided
	}
	void setContentType(string contentType) {
		if (validator::isValidMIMEType(contentType)) {
			this->contentType = contentType;
		}
	}
	Response & operator<<(string content) {
		this->contentLength += content.length();
		this->contentIterator = this->content.insert_after(this->contentIterator, content);
		return *this;
	}
	friend class HTTPResponseUtility;
};



class HTTPResponseUtility {
private:
	HTTPResponseUtility() {}
public:
	static void sendResponse(int clientSocketDescriptor, Response &response) {
		char header[200];
		unsigned long long contentLength = response.contentLength;
		sprintf(header, "HTTP/1.1 200 ok\r\nContent-Type:text/html\nContent-Length:%d\nConnection:close\r\n\r\n", contentLength);
		send(clientSocketDescriptor, header, strlen(header), 0);
		auto contentIterator = response.content.begin();
		while (contentIterator != response.content.end()) {
			string str = *contentIterator;
			send(clientSocketDescriptor, str.c_str(), str.length(), 0);
			contentIterator++;
		}
	}
};

enum __request_method__ {
	__GET__, __POST__, __PUT__, __DELETE__, __CONNECT__, __HEAD__, __OPTIONS__, __TRACE__
};

typedef struct __url__mapping {
	__request_method__ requestMethod;
	void (*mappedFunction)(Request&, Response&);
} URLMapping;


class Bro {
private:
	map<string, URLMapping> urlMappings;
	string staticResourseFolderpath;
	map<string, string> mimeTypes;
public:

	Bro() {
		BroUtility::loadMIMETypes(mimeTypes);
	}

	~Bro() {
		// not yet decided
	}

	void setStaticResourcesFolder(string staticResourseFolderpath) {
		if (validator::isValidPath(staticResourseFolderpath)) {
			this->staticResourseFolderpath = staticResourseFolderpath;
		}
		else {
			string exception = "Invalid Static Resource Folder path : " + staticResourseFolderpath;
			throw exception;
		}
	}

	void get(string url, void (*callBack)(Request &, Response &)) {
		if (validator::isValidURL(url)) {
			urlMappings[url] = {__GET__, callBack};
		}
	}

	void post(string url, void (*callBack)(Request &, Response &)) {
		if (validator::isValidURL(url)) {
			urlMappings[url] = {__POST__, callBack};
		}
	}

	bool serverStaticResource(int clientSocketDescriptor, const char *requestURI) {
		if (this->staticResourseFolderpath.length() == 0) return false;
		if (!FileSystemUtility::directoryExists(this->staticResourseFolderpath.c_str())) return false;
		string resourcePath = this->staticResourseFolderpath + requestURI;
		if (!FileSystemUtility::fileExists(resourcePath.c_str())) return false;
		// File Exists..
		//Serve the file
		FILE *f = fopen(resourcePath.c_str(), "rb");
		if (f == NULL) return false;
		fseek(f, 0, SEEK_END);
		long long int fileSize = ftell(f);
		if (fileSize == 0) {
			fclose(f);
			return false;
		}
		rewind(f); // Move internal pointer to start of file
		string extension = FileSystemUtility::getFileExtension(resourcePath.c_str());
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		auto mimeTypesIterator = mimeTypes.find(extension);
		string mimeType;
		if (mimeTypesIterator != mimeTypes.end()) mimeType = mimeTypesIterator->second;
		else mimeType = "text/html";
		char header[200];
		sprintf(header, "HTTP/1.1 200 ok\r\nContent-Type:%s\nContent-Length:%ld\nConnection:close\r\n\r\n", mimeType.c_str(), fileSize);
		send(clientSocketDescriptor, header, strlen(header), 0);
		// serve file now
		long long bytesLeftToRead = fileSize;
		long long bytesToRead = 4096;
		char buffer[4096];
		while (bytesLeftToRead > 0) {
			if (bytesLeftToRead < bytesToRead) bytesToRead = bytesLeftToRead;
			fread(buffer, bytesToRead, 1, f);
			if (feof(f)) break; // we never reach to this condition
			send(clientSocketDescriptor, buffer, bytesToRead, 0);
			bytesLeftToRead -= bytesToRead;
		}
		fclose(f);
		return true;
	}

	void listen(int portNumber, void (*callBack)(Error &)) {
#ifdef _WIN32
		WSADATA wsaData;
		WORD ver = MAKEWORD(1, 1);
		WSAStartup(ver, &wsaData);
#endif
		int serverSocketDescriptor, requestLength;
		char requestBuffer[4097];

		serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocketDescriptor < 0) { // unable to create
#ifdef _WIN32
			WSACleanup();
#endif
			Error error("unable to create server socket");
			callBack(error);
			return;
		}

		struct sockaddr_in serverSocketInfo; // server sock info
		serverSocketInfo.sin_family = AF_INET;
		serverSocketInfo.sin_port = htons(portNumber);
		serverSocketInfo.sin_addr.s_addr = htonl(INADDR_ANY);

		int successCode = bind(serverSocketDescriptor, (SOCKADDR*) &serverSocketInfo, sizeof(serverSocketInfo));
		if (successCode < 0) { // unable to bind
			closesocket(serverSocketDescriptor);
#ifdef _WIN32
			WSACleanup();
#endif
			Error error("unable to bind to given port number");
			callBack(error);
			return ;
		}
		successCode =::listen(serverSocketDescriptor, 10);
		if (successCode < 0) {
			closesocket(serverSocketDescriptor);
#ifdef _WIN32
			WSACleanup();
#endif
			Error error("unable to accept client connections");
			callBack(error);
			return;
		}
		Error error(""); // Ready
		callBack(error);
		sockaddr_in clientSocketInfo;
#ifdef _WIN32
		int len = sizeof(clientSocketInfo);
#endif // _WIN32
#ifdef linux
		socklen_t len = sizeof(clientSocketInfo);
#endif // linux
		int clientSocketDescriptor;
		while (1) {
			clientSocketDescriptor = accept(serverSocketDescriptor, (SOCKADDR*)&clientSocketInfo, &len);
			if (clientSocketDescriptor < 0) {
				// not yet decided
			}
			requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0); // no of bytes received
			if (requestLength == 0 || requestLength == -1) {
				closesocket(clientSocketDescriptor);
				continue;
			}
			requestBuffer[requestLength] = '\0';

			// code to parse the first line of request
			// first line must contain Request_Method URI HTTP_Version
			char *method, *requestURI, *httpVersion;
			int i = 0;


			method = requestBuffer;
			while (requestBuffer[i] && requestBuffer[i] != ' ') i++;
			if (requestBuffer[i] == '\0') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}
			requestBuffer[i] = '\0';
			i++;
			if (requestBuffer[i] == '\0' || requestBuffer[i] == ' ') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}
			StringUtility::toLowerCase(method);
			if (!(strcmp(method, "get") == 0 ||
			        strcmp(method, "post") == 0 ||
			        strcmp(method, "put") == 0 ||
			        strcmp(method, "delete") == 0 ||
			        strcmp(method, "connect") == 0 ||
			        strcmp(method, "options") == 0 ||
			        strcmp(method, "head") == 0 ||
			        strcmp(method, "trace") == 0)) {
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}

			requestURI = requestBuffer + i;
			while (requestBuffer[i] && requestBuffer[i] != ' ') i++;
			if (requestBuffer[i] == '\0') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}
			requestBuffer[i] = '\0';
			i++;
			if (requestBuffer[i] == '\0' || requestBuffer[i] == ' ') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}

			httpVersion = requestBuffer + i;
			while (requestBuffer[i] && requestBuffer[i] != '\r' && requestBuffer[i] != '\n') i++;
			if (requestBuffer[i] == '\0') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}
			if (requestBuffer[i] == '\r' && requestBuffer[i + 1] != '\n') {
				// Wrong Format (Error 400)
				HTTPErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				closesocket(clientSocketDescriptor);
				continue;
			}
			if (requestBuffer[i] == '\r') {
				requestBuffer[i] = '\0';
				i += 2;
			}
			else {
				requestBuffer[i] = '\0';
				i++;
			}
			StringUtility::toLowerCase(httpVersion);
			if (strcmp(httpVersion, "http/1.1")) {
				//Error
				HTTPErrorStatusUtility::sendHTTPVersionNotSupportedError(clientSocketDescriptor, httpVersion);
				closesocket(clientSocketDescriptor);
				continue;
			}
			i = 0;
			char *dataInRequest = NULL;
			while (requestURI[i] != '\0' && requestURI[i] != '?') i++;
			if (requestURI[i] == '?') {
				requestURI[i] = '\0';
				dataInRequest = requestURI + i + 1;
			}
			auto urlMappingsIterator = urlMappings.find(requestURI);
			cout << "Method : " << method << " " << "RequestURI : " << requestURI << endl;
			if (urlMappingsIterator == urlMappings.end()) {
				if (!serverStaticResource(clientSocketDescriptor, requestURI)) {
					// Error 404
					HTTPErrorStatusUtility::sendNotFoundError(clientSocketDescriptor, requestURI);
				}
				closesocket(clientSocketDescriptor);
				continue;
			}
			URLMapping urlMapping = urlMappingsIterator->second;
			if (urlMapping.requestMethod == __GET__ && strcmp(method, "get")) {
				// Error 405
				HTTPErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor, method, requestURI);
				closesocket(clientSocketDescriptor);
				continue;
			}

			// Code to parse the header & then body if exists starts here
			// Code to parse the header & then body if exists ends here
			Request request(method, requestURI, httpVersion, dataInRequest);
			Response response;
			urlMapping.mappedFunction(request, response);
			HTTPResponseUtility::sendResponse(clientSocketDescriptor, response);
			closesocket(clientSocketDescriptor);
		}
#ifdef _WIN32
		closesocket(serverSocketDescriptor);
		WSACleanup();
#endif
	}

};


