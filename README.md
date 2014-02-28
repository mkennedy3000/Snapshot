Snapshot
========

CS425/ECE428 Distributed Systems, MP1

 - Mark Kennedy - kenned31
 - Jordan Ebel  - ebel1

To Do
=====
 - Random sending of money and widgets
 - Check for correctness

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

