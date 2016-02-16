# ProtocolAnalyser
A small GUI program built in C++ using Win32 API, created with the intention of gathering data in regards to the comparable efficiency of UDP and TCP datagrams.

The sending/receiving of data is all done asynchronously and is all event driven thanks to Winsocket's WSAAsyncSelect model.

The program allows the user to:
* Select to be a client or server.
* Enter on client side the specifics of the data to be sent (protocol, packet size and amount of packets to send)
* Specify port for client and server, and specify servers IP address on client side
* Gather statistics in regards to data being transfered on server side

# How To Run

After cloning or downloading, compile and run the ProtocolAnalyser.exe application.

# How To Compile

Clone or download the files onto your Windows system.

Open up the ProtocolAnalyser.sln file in Visual Studio and run the program. Everything should already be linked.

# User Manual (Purpose, Usage, Design & Testing)

The user manual document for the program is the UserManual.pdf. That is where you will find the design documents (state diagrams and pseudocode) for this project, as well as the testing document.

# Analysis Report

The analysis report on the findings of the program can be found in AnalysisReport.dpf. The findings, however, are incorrect. Essentially, the timing mechanism used does not take the TCP three-way handshake into account, so the data being compared is the raw data being sent from TCP after the handshake in comparison to UDP. We are essentially dropping the initial overhead from all TCP data.

On top of this, the UDP side could not reliably receiving the end packet stating it had finished reading, since the implementation of the buffer system was poor and amounted to near 50% packet loss at higher amounts. Because of this, UDP has a timeout system to determine if the client has finished sending, which offset the UDP statistics (since it would waste time once UDP has finished waiting to timeout).

Those two issues, paired together with going the WSAAsyncSelect model of event driven over completion routine (which caused lower efficient readings in general), resulted in the analysis report not being quite accurate.

Nontheless, the analysis report is created with the findings from this program, despite the issues with implementation.
