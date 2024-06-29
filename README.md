# Yapp_ChatApp
This is a chat application designed by Group No. 4 of BSCS - 2B (Semester 2)
Chat App README

Introduction

Welcome to the Chat App, a simple console-based chat application that allows users to communicate with each other in real-time. This app is designed to demonstrate the basics of socket programming and file handling in C++.

*Features*

Console-based chat interface
User authentication using username and password
Real-time messaging between users
File handling for storing and retrieving user credentials
Getting Started

Compile and run the server program (server.cpp) to start the chat server.
Compile and run the client program (client.cpp) to connect to the chat server.
Enter your username and password to authenticate with the server.
Once authenticated, you can start chatting with other users.

*Usage*

To send a message, simply type your message and press enter.
To quit the chat, type "quit" and press enter.

*File Handling*

The app uses a file called "users.txt" to store user credentials.
Each line in the file represents a user, with the format "username:password".
The app reads and writes to this file to authenticate users and store new user credentials.

*Known Issues*

This app is a simple demonstration of chat functionality and may not be suitable for production use.
There is no error handling for cases where the user enters invalid input.
The app does not support multiple chat rooms or advanced features like file transfer or video conferencing.
License

This app is licensed under the MIT License. See LICENSE.txt for details.

*Contributing*

If you'd like to contribute to this app, please fork the repository and submit a pull request with your changes.
