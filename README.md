Snapshot
========
CS425/ECE428 Distributed Systems, MP1

 - Mark Kennedy - kenned31
 - Jordan Ebel  - ebel1

Usage
=====
Compile: From within src/ directory, "make".  Also, "make clean".

Run: ./run num_processes num_snapshots random_number_seed

Resulting log files are stored in the records/ directory.

Information and Algorithm
==========================
This project uses the Chandy Lamport Snapshot algorithm to reliably record the state of a variable number of processes in the distributed system.  Each process stores its own number of money and widgets.  Messages are sent over reliable, one way, unicast channels and contain a random transfer of money and widgets.  On receipt of a marker, the process records the current state of its money and widgets to a log file, then begins recording the state of its incoming channels.  The process will then broadcast its own marker.  After receiving a marker from another process, the process stops recording the state of the corresponding incoming channel and records the messages in the channel to the log file.  The program terminates when the set number of snapshots have been recorded.

Technical Details
==================
This project uses the fork() system call to create processes to mimic nodes in a distributed system.  Each process has both read and write threads, which concurrently send out new messages and process incoming messages after set delays.  The threads are implemented using pthreads.  On receipt of a message, the message is placed in a holding queue and is processed after a delay, to mimic delay in the channel.  Each process has one incoming port to receive messages on, and outgoing ports to every other process.  The processes use UDP sockets and packets to send messages to the incoming port of other processes.  There is also one snapshot thread that periodically sends out marker messages to all processes, thereby initiating the snapshot algorithm.

To Do
=====
 - Check for correctness:
    - Observed bug: Message from 1 to 2 can be sent after 1's snapshot but before 2's snapshot.  Result is message not recorded in 1's channel but 2's invariants already updated.  Looks like invariants lost.
    - Result of incorrect implementation of snapshot algorithm.  Every process must send broadcast marker after receiving first marker

Change Log
==========
 - 2/19 : Initial project set up
 - 2/22 : Reorganized project.  Networking setup complete.  Fork and communication between 2 processes complete.
 - 2/22 : Fork variable amount of processes but pthreads don't work as expected in them. Shift to just using pthreads.
 - 2/23 : Fixing pthread problem, will be up soon.
 - 2/23 : Fixed fork and pthread bug.
 - 2/23 : Added clear_ports.sh, a script that opens up all ports if not binding correctly.
 - 2/23 : Read and Write threads working with Lamport timestamps and valid invariant passing
 - 2/24 : Vector timestamps implemented
 - 2/24 : Snapshot Markers sent ever 5 seconds
 - 2/24 : Messages are now put in virtual queues and processed 1 second after being received
 - 2/27 : Updated Makefile to create directories when needed.  Currently imperfect solution (suppresses "Nothing to be done" messages) but works
 - 2/27 : Support for file input and output. Implemented snapshot recording.
 - 2/27 : Added search-all script.  I think it fills the requirements but it might need to be changed (it is just grep repackaged...)
 - 2/28 : Randomized message contents and recipients
 - 2/28 : Changed message format to support 4 bytes per number instead of 1 in messages (to solve overflow bug)

