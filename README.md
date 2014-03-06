Snapshot
========
CS425/ECE428 Distributed Systems, MP1

 - Mark Kennedy - kenned31
 - Jordan Ebel  - ebel1

Usage
=====
Compile: From within 'src/' directory, 'make'.  Also, 'make clean'.

Run: 
    
    ./run num_processes num_snapshots random_number_seed

Search script:

    ./search-all snapshot_number

Resulting log files are stored in the 'records/' directory.

Information and Algorithm
==========================
This project uses the Chandy Lamport Snapshot algorithm to reliably record the state of a variable number of processes in the distributed system.  Each process stores its own number of money and widgets.  Messages are sent over reliable, one way, unicast channels and contain a random transfer of money and widgets.  On receipt of a marker, the process records the current state of its money and widgets to a log file, then begins recording the state of its incoming channels.  The process will then broadcast its own marker.  After receiving a marker from another process, the process stops recording the state of the corresponding incoming channel and records the messages in the channel to the log file.  The program terminates when the set number of snapshots have been recorded.

Technical Details
==================
This project uses the fork() system call to create processes to mimic nodes in a distributed system.  Each process has both read and write threads, which concurrently send out new messages and process incoming messages after set delays.  The threads are implemented using pthreads.  On receipt of a message, the message is placed in a holding queue and is processed after a delay, to mimic delay in the channel.  Each process has one incoming port to receive messages on, and outgoing ports to every other process.  The processes use UDP sockets and packets to send messages to the incoming port of other processes.  There is also one snapshot thread that periodically sends out marker messages to all processes, thereby initiating the snapshot algorithm.

