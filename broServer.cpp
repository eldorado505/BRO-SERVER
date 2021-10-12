#ifdef _WIN32
#include<windows.h>
#include<winsock2.h>
#endif // _WIN32
#ifdef linux
#include<arpa/inet.h>
#include<sys/socket.h>
#endif // linux
#include<iostream>
#include<string>
#include<map>
#include<forward_list>
using namespace std;

//Amit (The Bro Programmer)

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



class validator {
private:
	validator() {}
public:
	static bool isValidMIMEType(string &mimeType) {
		// do nothing
		return true;
	}
	static bool isValidPath(string &path) {
		// do nothing
		return true;
	}
	static bool isValidURL(string &url) {
		// do nothing
		return true;
	}
};

class Request {
private:
	char *method, *httpVersion, *requestURI;
public:
	Request(char *method, char *requestURI, char *httpVersion) {
		this->method = method;
		this->requestURI = requestURI;
		this->httpVersion = httpVersion;
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
	string staticResourseFolder;
public:

	Bro() {
		// not yet decided
	}

	~Bro() {
		// not yet decided
	}

	void setStaticResourcesFolder(string staticResourseFolder) {
		if (validator::isValidPath(staticResourseFolder)) {
			this->staticResourseFolder = staticResourseFolder;
		}
	}

	void get(string url, void (*callBack)(Request &, Response &)) {
		if (validator::isValidURL(url)) {
			urlMappings[url] = {__GET__, callBack};
		}
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

			auto urlMappingsIterator = urlMappings.find(requestURI);
			if (urlMappingsIterator == urlMappings.end()) {
				// Error 404
				HTTPErrorStatusUtility::sendNotFoundError(clientSocketDescriptor, requestURI);
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
			Request request(method, requestURI, httpVersion);
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

// Bobby [The Web Application Developer] - user of web server.
int main() {
	Bro bro;
	bro.setStaticResourcesFolder("whatever");
	bro.get("/", [](Request & request, Response & response)-> void {
		const char *html = R""""(
		<!DOCTYPE HTML>
		<html lang='en'>
		<head>
		<meta charset='utf-8'>
		<title>welocme</title>
		</head>
		<body>
		<h1>welcome</hl>
		<h3>Administrator</h3>
		<a href='getCustomers'>Customers List</a>
		</body>
		</html>
		)"""";
		response.setContentType("text/html"); //setting html MIME
		response << html;
	});
	bro.get("/getCustomers", [](Request & request, Response & response)-> void {
		const char *html = R""""(
		<!DOCTYPE HTML>
		<html lang='en'>
		<head>
		<meta charset='utf-8'>
		<title>whatever</title>
		</head>
		<body>
		<h1>List Of Customers</h1>
		<ul>
		<li>Ramesh</li>
		<li>Suresh</li>
		<li>Mohan</li>
		</ul>
		<a href='/'>Home</a>
		</body>
		</html>
		)"""";
		response.setContentType("text/html"); //setting html MIME
		response << html;
	});
	bro.listen(6060, [](Error & error)->void {
		if (error.hasError()) {
			cout << error.getError() << endl;
			return;
		}
		cout << "Bro Web Server is ready to accept the request on port 6060" << endl;
	});
	return 0;
}

