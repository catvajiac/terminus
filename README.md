# terminus
homemade BBS.

### TODO:

#### General
  - [x] Begin protocol (branch: `server-client`)

#### Server
  - [x] Set serverside representation of sessions
  - [x] Set serverside fork + exec
  - [x] Create menu application to list, choose, run (by fork + exec), and come back to list for the programs.
  - [x] Close stdin/stdout for user processes, and open appropriate files
  - [x] Redirect information to and from client to process

#### Client
  - [x] Get terminal size
  - [ ] Initial connection, save session id.
  - [ ] Buffer keypresses, send to server occasionally
  - [ ] Display output
  - [ ] If the response completely filled the buffer, make another request immediately following.

#### Applications
  - [ ] Make a few simple applications, particularly some showing off the ability to use the terminal size.  Maybe centering, or something.


Tim is a cool squid

*History of Computing*

*Project 1*

*Catalina Vajiac, Conor Nailos, Timothy Burchfield*
