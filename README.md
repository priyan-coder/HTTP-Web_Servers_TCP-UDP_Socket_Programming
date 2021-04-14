# EE4210_CA2

The codebase written in C, implements two servers that serve requests for HTTP/1.1. 

The first server (s1) uses TCP as the transport protocol while the second server (s2) uses UDP instead. 

1. Both the servers are concurrent i.e. they are able to serve multiple HTTP requests simultaneously.
2. For s1: The HTTP response is a web-page having a form with one text input. After the text is entered, s1 updates the web-page by displaying the entered text.
3. For s2: The HTTP response is a web-page displaying the text “EE-4210: Continuous assessment”.
4. Non-standard ports are used.
5. The HTTP headers are constructed as a string in this implementation without the use of any external libraries/API.


To test out the TCP Server: 

1. Compile by:  gcc s1_server.c - lm - o s1
2. Exe as:  ./s1
3. Head over to : http://localhost:1200/
4. Type in any input into the field displayed, hit ENTER and the server should return a response that displays your input.


To test out the UDP Server: 

1. Compile by:   gcc s2_server.c - lm - o s2
2. Exe as:  ./s2
3. You can use software such as netcat(nc) to send requests to the server at port 1600. Alternatively you could write your own simple client for testing. 
4. Server returns a HTTP response of a web-page displaying the text “EE-4210: Continuous assessment”

NOTE: Since web browsers use TCP by default, you will not be able to use your usual web browser for connecting to a UDP server.
