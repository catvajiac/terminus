# terminus
homemade BBS.

### TODO:

#### General
  - [x] Begin protocol (branch: `server-client`)

#### Server
  - [x] Set serverside representation of sessions
  - [ ] Set serverside fork + exec
  - [ ] Create menu application to list, choose, run (by fork + exec), and come back to list for the programs.
  - [ ] Close stdin/stdout for user processes, and open appropriate files
  - [ ] Redirect information to and from client to process

#### Client
  - [x] Get terminal size
  - [ ] Initial connection, save session id.
  - [ ] Buffer keypresses, send to server occasionally
  - [ ] Display output

#### Applications
  - [ ] Make a few simple applications, particularly some showing off the ability to use the terminal size.  Maybe centering, or something.

#### Future
It would be awesome if we could do these at some point:
  - [ ] Allow processes to somehow flush output to the client?  Is this even possible?


*History of Computing*

*Project 1*

*Catalina Vajiac, Conor Nailos, Timothy Burchfield*
