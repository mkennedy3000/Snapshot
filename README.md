Snapshot
========

CS425/ECE428 Distributed Systems, MP1

 - Mark Kennedy
 - Jordan Ebel

To Do
=====
 - Implement Snapshots (Record current state and all channels being processed)
 - File I/O (output correct data to files)
 - Random sending of money and widgets
 - Search All utility (possibly in python?)

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

